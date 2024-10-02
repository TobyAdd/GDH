#include "replayEngine.hpp"
#include "hacks.hpp"
#include "gui.hpp"
#include "recorder.hpp"
#include <shlobj.h>

ReplayEngine engine;
SpamBot spamBot;
StraightFly straightFly;

bool ReplayEngine::containsRussianLetters(const std::filesystem::path& p) {
    auto pathStr = p.u8string();
    for (char c : pathStr) {
        if ((unsigned char)c >= 0xD0 && (unsigned char)c <= 0xD1) {
            return true;
        }
    }
    return false;
}

unsigned ReplayEngine::get_frame() {
    auto pl = GameManager::sharedState()->getPlayLayer();
    if (!pl) {
        return 0;
    }

    return (engine.version_engine == 1)
        ? static_cast<unsigned>(pl->m_gameState.m_levelTime * hacks::tps_value)
        : (engine.version_engine == 2)
            ? static_cast<unsigned>(pl->m_gameState.m_currentProgress)
            : 0;
}


void ReplayEngine::handle_recording(GJBaseGameLayer* self, bool player) {
    if (!player && !self->m_gameState.m_isDualMode)
        return;

    unsigned frame = get_frame();

    bool frameExists = std::find_if(replay.begin(), replay.end(), [&](const auto &data)
                                    { return data.frame == frame && data.player == player; }) != replay.end();

    if (frameExists)
    {
        return;
    }

    replay.push_back({frame,
                      player ? self->m_player1->m_position.x : self->m_player2->m_position.x,
                      player ? self->m_player1->m_position.y : self->m_player2->m_position.y,
                      player ? self->m_player1->getRotation() : self->m_player2->getRotation(),
                      player ? self->m_player1->m_yVelocity : self->m_player2->m_yVelocity,
                      player});
}

void ReplayEngine::handle_recording2(bool hold, int button, bool player) {
    // auto pl = PlayLayer::get();

    // if (pl && !replay2.empty() && replay2.back().hold == hold)
    //     return;

    replay2.push_back({get_frame(), hold, button, player});
}

void ReplayEngine::handle_playing(GJBaseGameLayer* self)
{
    unsigned frame = get_frame();

    if (version_engine == 1 && accuracy_fix) {
        while (index < (int)replay.size() && frame >= replay[index].frame)
        {
            if (replay[index].player)
            {
                self->m_player1->m_position.x = replay[index].x;
                self->m_player1->m_position.y = replay[index].y;
                if (rotation_fix)
                    self->m_player1->setRotation(replay[index].rotation);
                self->m_player1->m_yVelocity = replay[index].y_accel;
            }
            else
            {
                self->m_player2->m_position.x = replay[index].x;
                self->m_player2->m_position.y = replay[index].y;
                if (rotation_fix)
                    self->m_player2->setRotation(replay[index].rotation);
                self->m_player2->m_yVelocity = replay[index].y_accel;
            }
            index++;
        }
    }


    while (index2 < (int)replay2.size() && frame >= replay2[index2].frame)
    {
        self->handleButton(replay2[index2].hold, replay2[index2].button, replay2[index2].player);
        index2++; 
    }
}

void ReplayEngine::handle_reseting(PlayLayer* self) {
    int lastCheckpointFrame = get_frame();

    if (mode == state::record) {
        remove_actions(lastCheckpointFrame);

        if (!practice_fix && !replay2.empty() && replay2.back().hold) {
            handle_recording2(false, replay2.back().button, replay2.back().player);
        }
    }
    else if (mode == state::play) {
        index = 0;
        index2 = 0;
    }
}

std::string ReplayEngine::save(std::string name)
{
    if (replay2.empty())
        return "Replay doesn't have actions";

    std::ofstream file(hacks::folderMacroPath / std::string(name + ".re"), std::ios::binary);

    file.write(reinterpret_cast<char *>(&hacks::tps_value), sizeof(hacks::tps_value));

    unsigned replay_size = replay.size();
    unsigned replay2_size = replay2.size();

    file.write(reinterpret_cast<char *>(&replay_size), sizeof(replay_size));
    file.write(reinterpret_cast<char *>(&replay2_size), sizeof(replay2_size));

    file.write(reinterpret_cast<char *>(&replay[0]), sizeof(replay_data) * replay_size);
    file.write(reinterpret_cast<char *>(&replay2[0]), sizeof(replay_data2) * replay2_size);

    file.close();
    return "Replay saved";
}

std::string ReplayEngine::load(std::string name)
{
    if (!replay2.empty())
        return "Please clear replay before loading another";

    std::ifstream file(hacks::folderMacroPath / std::string(name + ".re"), std::ios::binary);
    if (!file)
        return "Replay doesn't exist";

    file.read(reinterpret_cast<char *>(&hacks::tps_value), sizeof(hacks::tps_value));
    hacks::update_framerate(hacks::tps_value);

    unsigned replay_size = 0;
    unsigned replay2_size = 0;

    file.read(reinterpret_cast<char *>(&replay_size), sizeof(replay_size));
    file.read(reinterpret_cast<char *>(&replay2_size), sizeof(replay2_size));

    replay.resize(replay_size);
    replay2.resize(replay2_size);

    file.read(reinterpret_cast<char *>(&replay[0]), sizeof(replay_data) * replay_size);
    file.read(reinterpret_cast<char *>(&replay2[0]), sizeof(replay_data2) * replay2_size);

    file.close();
    return "Replay loaded";
}

std::string ReplayEngine::save2(std::string name)
{
    if (replay2.empty())
        return "Replay doesn't have actions";

    std::ofstream file(hacks::folderMacroPath / std::string(name + ".re2"), std::ios::binary);
    if (!file)
        return "Failed to open file for writing";

    const std::string header = "RE2";

    file.write(header.c_str(), header.size());

    unsigned replay2_size = replay2.size();

    file.write(reinterpret_cast<char *>(&replay2_size), sizeof(replay2_size));
    file.write(reinterpret_cast<char *>(&replay2[0]), sizeof(replay_data2) * replay2_size);

    file.close();
    return "Replay saved";
}


std::string ReplayEngine::load2(std::string name)
{
    if (!replay2.empty())
        return "Please clear replay before loading another";

    std::ifstream file(hacks::folderMacroPath / std::string(name + ".re2"), std::ios::binary);
    if (!file)
        return "Replay doesn't exist";

    const std::string expected_header = "RE2";
    char file_header[4] = {0};

    file.read(file_header, expected_header.size());

    if (std::string(file_header) != expected_header)
        return "Invalid replay format";

    hacks::update_framerate(240.f);

    unsigned replay2_size = 0;
    file.read(reinterpret_cast<char *>(&replay2_size), sizeof(replay2_size));

    replay2.resize(replay2_size);
    file.read(reinterpret_cast<char *>(&replay2[0]), sizeof(replay_data2) * replay2_size);

    file.close();
    return "Replay loaded";
}

void ReplayEngine::remove_actions(unsigned frame)
{
    if (version_engine == 1) {
        auto check = [&](replay_data &action) -> bool
        {
            return action.frame > frame;
        };
        replay.erase(remove_if(replay.begin(), replay.end(), check), replay.end());
    }

    auto check2 = [&](replay_data2 &action) -> bool
    {
        return (version_engine == 1) ? action.frame >= frame : action.frame > frame;
    };
    replay2.erase(remove_if(replay2.begin(), replay2.end(), check2), replay2.end());
}

std::vector<std::filesystem::path> replay_list;
void openSelectReplay(const char *str_id) {
    replay_list.clear();
    for (const auto &entry : std::filesystem::directory_iterator(hacks::folderMacroPath)) {
        std::string replay = entry.path().filename().string();
        if (engine.version_engine == 1 && replay.size() >= 3 && replay.substr(replay.size() - 3) == ".re") {
            replay_list.push_back(entry);
        }
        else if (engine.version_engine == 2 && replay.size() >= 4 && replay.substr(replay.size() - 4) == ".re2") {
            replay_list.push_back(entry);
        }
    }
    ImGui::OpenPopup(str_id);
}

void ReplayEngine::openReplayMultishit() {
    ImGui::BeginChild("Select Replay##3", {400 * gui::scale, 300 * gui::scale});
    for (int i = 0; i < (int)replay_list.size(); i++)
    {
        if (ImGui::Button(replay_list[i].filename().replace_extension().string().c_str(), {ImGui::GetContentRegionAvail().x, NULL}))
        {
            replay_name = replay_list[i].filename().replace_extension().string();
            ImGui::CloseCurrentPopup();
        }
    }
    ImGui::EndChild();

    if (ImGui::Button("Open Folder", {ImGui::GetContentRegionAvail().x, NULL})) {
        ShellExecuteW(nullptr, L"open", L"explorer", hacks::folderMacroPath.wstring().c_str(), nullptr, SW_SHOWDEFAULT);
    }
    
    if (ImGui::Button("Close", {ImGui::GetContentRegionAvail().x, NULL})) {
        ImGui::CloseCurrentPopup();
    }
}

std::filesystem::path SelectFolder(HWND owner = NULL) {
    WCHAR path[MAX_PATH];

    BROWSEINFOW bi = { 0 };
    bi.lpszTitle = L"Select a directory";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    bi.hwndOwner = owner;

    LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
    if (pidl != 0) {
        if (SHGetPathFromIDListW(pidl, path)) {
            CoTaskMemFree(pidl);
            return std::filesystem::path(path);
        }
        CoTaskMemFree(pidl);
    }

    return std::filesystem::path();
}

void GreenCheckmarkWithText(const char* label, float sz = 16.0f, ImU32 color = IM_COL32(0, 255, 0, ImGui::GetStyle().Alpha * 255))
{
    ImVec2 pos = ImGui::GetCursorScreenPos();

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float thickness = std::max(sz / 5.0f, 1.0f);
    ImVec2 p1 = ImVec2(pos.x + 0.20f * sz, pos.y + 0.55f * sz);
    ImVec2 p2 = ImVec2(pos.x + 0.55f * sz, pos.y + 0.90f * sz);
    ImVec2 p3 = ImVec2(pos.x + 0.85f * sz, pos.y + 0.10f * sz);

    draw_list->AddLine(p1, p2, color, thickness);
    draw_list->AddLine(p2, p3, color, thickness);

    ImGui::SetCursorScreenPos(ImVec2(pos.x + sz + 5.0f, pos.y));

    ImGui::TextUnformatted(label);
}

void RedCrossWithText(const char* label, float sz = 16.0f, ImU32 color = IM_COL32(255, 0, 0, ImGui::GetStyle().Alpha * 255))
{
    ImVec2 pos = ImGui::GetCursorScreenPos();

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float thickness = std::max(sz / 5.0f, 1.0f);
    ImVec2 p1 = ImVec2(pos.x + 0.20f * sz, pos.y + 0.20f * sz);
    ImVec2 p2 = ImVec2(pos.x + 0.80f * sz, pos.y + 0.80f * sz);
    ImVec2 p3 = ImVec2(pos.x + 0.20f * sz, pos.y + 0.80f * sz);
    ImVec2 p4 = ImVec2(pos.x + 0.80f * sz, pos.y + 0.20f * sz);

    draw_list->AddLine(p1, p2, color, thickness);
    draw_list->AddLine(p3, p4, color, thickness);

    ImGui::SetCursorScreenPos(ImVec2(pos.x + sz + 5.0f, pos.y));

    ImGui::TextUnformatted(label);
}


void ReplayEngine::render() {
    static geode::Mod* cbfMod = geode::Loader::get()->getLoadedMod("syzzi.click_between_frames");
    static bool hasCBF = cbfMod != nullptr && cbfMod->isEnabled();

    if (hasCBF && !cbfMod->getSettingValue<bool>("soft-toggle")) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 128, 128).Value);
        ImGui::TextWrapped("Click Between Frames mod is enabled, turn it off to use Replay Engine");
        ImGui::PopStyleColor();
        return;
    }

    if (ImGui::BeginPopupModal("Select Replay", 0, ImGuiWindowFlags_NoResize)) {
        openReplayMultishit();
        ImGui::EndPopup();
    }

    if (settings_openned) {
        static bool first_time = true;
        if (first_time) {
            first_time = false;
            ImGui::SetNextWindowSize({740 * gui::scale, 500 * gui::scale});
        }        
    }
     
    if (ImGui::BeginPopupModal("Replay Engine Settings", &settings_openned)) {
        if (ImGui::BeginTabBar("Replay Engine Settings Tabs")) {
            if (ImGui::BeginTabItem("Settings")) {
                if (version_engine == 1) {
                    ImGui::Checkbox("Real Time", &real_time, gui::scale);

                    ImGui::SameLine();

                    ImGui::Checkbox("Frame Advance", &frame_advance, gui::scale);

                
                    ImGui::Checkbox("Accuracy Fix", &accuracy_fix, gui::scale);

                    ImGui::SameLine();

                    ImGui::Checkbox("Rotation Fix", &rotation_fix, gui::scale);
                    ImGui::SameLine();
                }

                ImGui::Checkbox("Practice Fix", &practice_fix);

                ImGui::Spacing();
                ImGui::Text("Replay System");
                ImGui::Separator();

                ImGui::RadioButton("Engine v1", &version_engine, 1, gui::scale);

                ImGui::SameLine();

                ImGui::RadioButton("Engine v2", &version_engine, 2, gui::scale);


                ImGui::Spacing();
                ImGui::Text("Replay System differences");
                ImGui::Separator();
                ImGui::Text("Engine v1");
                GreenCheckmarkWithText("More accuracy, physics frames", 16.f * gui::scale);
                GreenCheckmarkWithText("Any TPS value for the macro", 16.f * gui::scale);
                RedCrossWithText("Poor performance", 16.f * gui::scale);
                RedCrossWithText("Large macro size", 16.f * gui::scale);
                RedCrossWithText("Slow video recording", 16.f * gui::scale);

                ImGui::NewLine();

                ImGui::Text("Engine v2");
                GreenCheckmarkWithText("Clear frames, less accurate", 16.f * gui::scale, IM_COL32(255, 255, 0, ImGui::GetStyle().Alpha * 255));
                RedCrossWithText("240 TPS Lock", 16.f * gui::scale);
                GreenCheckmarkWithText("Better performance", 16.f * gui::scale);
                GreenCheckmarkWithText("Small macro size", 16.f * gui::scale);
                GreenCheckmarkWithText("Real-time video recording", 16.f * gui::scale);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Spambot")) {
                if (ImGui::Checkbox("Enable##Spambot", &spamBot.enabled))
                {
                    spamBot.reset_temp();
                }

                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 2);
                ImGui::DragInt("##spamhold", &spamBot.hold, 1, 1, INT_MAX, "Hold: %i");

                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::DragInt("##spamrelease", &spamBot.release, 1, 1, INT_MAX, "Release: %i");

                ImGui::Checkbox("Player 1", &spamBot.player_p1);
                ImGui::SameLine();
                ImGui::Checkbox("Player 2", &spamBot.player_p2);

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Straight Fly Bot")) {
                if (ImGui::Checkbox("Enable##Straight Fly Bot", &straightFly.enabled))
                {
                    auto gjbgl = GJBaseGameLayer::get();
                    straightFly.start(gjbgl);
                }

                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::DragInt("##StraightFlyAcc", &straightFly.accuracy, 1, 0, 100, "Y Accuracy: %i");
                ImGui::Text("Note: Straight Fly Bot works only on first player");

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Recorder (Beta)")) {
                if (recorder.ffmpeg_installed) {
                    if (ImGui::Checkbox("Record", &recorder.enabled, gui::scale)) {
                        if (containsRussianLetters(hacks::folderShowcasesPath)) {
                            recorder.enabled = false;
                            imgui_popup::add_popup("Invalid path to the showcase folder. Please remove any Cyrillic characters");
                        }
                        else {
                            if (recorder.enabled) {
                                if (!recorder.advanced_mode) {
                                    recorder.full_cmd = recorder.compile_command();
                                }

                                recorder.start(recorder.full_cmd);
                            }                        
                            else 
                                recorder.stop();
                        }
                    }

                    ImGui::SameLine();

                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    ImGui::InputText("##videoname", &recorder.video_name);

                    ImGui::Checkbox("Advanced Mode", &recorder.advanced_mode);

                    if (recorder.advanced_mode) {
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                        ImGui::InputText("##full_args", &recorder.full_cmd);
                        if (ImGui::Button("Compile")) {
                            recorder.full_cmd = recorder.compile_command();
                        }

                        ImGui::SameLine();
                        if (ImGui::Button("Copy to clipboard")) {
                            ImGui::SetClipboardText(recorder.full_cmd.c_str());
                        }

                        ImGui::SameLine();
                        if (ImGui::Button("Paste to from clipboard")) {
                            recorder.full_cmd = (std::string)ImGui::GetClipboardText();
                        }
                    }

                    ImGui::SameLine();

                    ImGui::Spacing();

                    ImGui::Text("Resolution:");
                    ImGui::Separator();

                    ImGui::PushItemWidth(45.f * gui::scale);
                    ImGui::InputInt("##width", &recorder.width, 0);
                    ImGui::SameLine(0, 5);

                    ImGui::Text("x");
                    ImGui::SameLine(0, 5);

                    ImGui::PushItemWidth(45.f * gui::scale);
                    ImGui::InputInt("##height", &recorder.height, 0);
                    ImGui::SameLine(0, 5);

                    ImGui::Text("@");
                    ImGui::SameLine(0, 5);

                    ImGui::PushItemWidth(35.f * gui::scale);
                    ImGui::InputInt("##fps", &recorder.fps, 0);

                    ImGui::Spacing();

                    ImGui::Text("Encoding Settings");
                    ImGui::Separator(); 
                    
                    ImGui::PushItemWidth(50.f * gui::scale);
                    ImGui::InputText("Bitrate", &recorder.bitrate);

                    ImGui::SameLine();

                    ImGui::PushItemWidth(80.f * gui::scale);
                    ImGui::InputText("Codec", &recorder.codec);

                    ImGui::PushItemWidth(300 * gui::scale);
                    ImGui::InputText("Extra Arguments", &recorder.extra_args);

                    ImGui::PushItemWidth(300 * gui::scale);
                    ImGui::InputText("VF Args", &recorder.vf_args);

                    if (ImGui::Checkbox("vflip", &recorder.vflip)) {
                        recorder.compile_vf_args();
                    }

                    if (ImGui::Checkbox("Fade in", &recorder.fade_in)) {
                        recorder.compile_vf_args();
                    }

                    ImGui::SameLine();

                    ImGui::PushItemWidth(120 * gui::scale);
                    if (ImGui::DragFloat("##fade_in_start", &recorder.fade_in_start, 0.01f, 0, FLT_MAX, "Start: %.2fs")) {
                        recorder.compile_vf_args();
                    }

                    ImGui::SameLine();
                    
                    ImGui::PushItemWidth(120 * gui::scale);
                    if (ImGui::DragFloat("##fade_in_end", &recorder.fade_in_end, 0.01f, 0, FLT_MAX, "End: %.2fs")) {
                        recorder.compile_vf_args();
                    }

                    ImGui::Checkbox("Fade out", &recorder.fade_out);

                    ImGui::Text("Note: The length of the fade-out is calculated based on the value of \"Second to Render After\" (WIP)");

                    ImGui::Spacing();

                    ImGui::Text("Level Settings");
                    ImGui::Separator();

                    ImGui::PushItemWidth(200 * gui::scale);
                    ImGui::InputFloat("Second to Render After", &recorder.after_end_duration, 1);

                    ImGui::Spacing();

                    ImGui::Text("Presets (%i)", secret);

                    ImGui::Separator();

                    if (ImGui::Button("Save")) {
                        json j;
                        j["width"] = recorder.width;
                        j["height"] = recorder.height;
                        j["fps"] = recorder.fps;
                        j["bitrate"] = recorder.bitrate;
                        j["codec"] = recorder.codec;
                        j["extra_args"] = recorder.extra_args;
                        j["vf_args"] = recorder.vf_args;

                        std::ofstream file(hacks::folderPath / "recorder_settings.json");
                        if (file.is_open()) {
                            file << j.dump(4);                            
                            file.close();

                            imgui_popup::add_popup("Configuration saved");
                        } else {
                            imgui_popup::add_popup("Could not open file for writing");
                        }
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Load")) {
                        std::ifstream file(hacks::folderPath / "recorder_settings.json");
                        if (file.is_open()) {
                            json j = json::parse(file, nullptr, false);
                            if (!j.is_discarded()) {
                                recorder.width = j.value("width", recorder.width);
                                recorder.height = j.value("height", recorder.height);
                                recorder.fps = j.value("fps", recorder.fps);
                                recorder.bitrate = j.value("bitrate", recorder.bitrate);
                                recorder.codec = j.value("codec", recorder.codec);
                                recorder.extra_args = j.value("extra_args", recorder.extra_args);
                                recorder.vf_args = j.value("vf_args", recorder.vf_args);

                                imgui_popup::add_popup("Configuration loaded");
                            } else {
                                imgui_popup::add_popup("json broken?");
                            }

                            file.close();
                        } else {
                            imgui_popup::add_popup("Could not open file for reading");
                        }
                    }

                    if (ImGui::Button("HD"))
                    {
                        recorder.width = 1280;
                        recorder.height = 720;
                        recorder.fps = 60;
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("FULL HD"))
                    {
                        recorder.width = 1920;
                        recorder.height = 1080;
                        recorder.fps = 60;
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("QHD"))
                    {
                        recorder.width = 2560;
                        recorder.height = 1440;
                        recorder.fps = 60;
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("4K"))
                    {
                        recorder.width = 3840;
                        recorder.height = 2160;
                        recorder.fps = 60;
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("8K"))
                    {
                        recorder.width = 7680;
                        recorder.height = 4320;
                        recorder.fps = 60;
                        if (secret > 0)
                            secret--;
                    }

                    if (secret <= 0) {
                        if (ImGui::Button("CPU BGR"))
                        {
                            recorder.codec = "libx264rgb";
                            recorder.extra_args = "-crf 15 -pix_fmt bgr0";
                            recorder.compile_vf_args();
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("CPU YUV420P"))
                        {
                            recorder.codec = "libx265";
                            recorder.extra_args = "-crf 17 -pix_fmt yuv420p";
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("CPU x265"))
                        {
                            recorder.codec = "libx265";
                            recorder.extra_args = "-preset fast -pix_fmt yuv420p -x265-params limit-sao:subme=3:rc-lookahead=40:b-adapt=1:crf=16:bframes=8:psy-rd=2:psy-rdoq=4:aq-mode=3:deblock=-1,-1";
                            recorder.bitrate = "0";
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("CPU x265 Lossless"))
                        {
                            recorder.codec = "libx265";
                            recorder.extra_args = "-preset fast -pix_fmt yuv420p -x265-params limit-sao:subme=3:rc-lookahead=40:b-adapt=1:crf=0:bframes=8:psy-rd=2:psy-rdoq=4:aq-mode=3:deblock=-1,-1";
                            recorder.bitrate = "0";
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("CPU HDR"))
                        {
                            recorder.codec = "libx265";
                            recorder.extra_args = "-pix_fmt yuv420p10le -x265-params hdr-opt=1:repeat-headers=1:colorprim=bt2020:transfer=arib-std-b67:colormatrix=bt2020nc:master-display=G(8500,39850)B(6550,2300)R(35400,14600)WP(15635,16450)L(40000000,50):max-cll=0,0 -crf 12 -preset fast";
                            recorder.compile_vf_args();
                            if (!recorder.vf_args.empty())
                                recorder.vf_args += ",";
                            recorder.vf_args += "colorspace=bt2020:iall=bt709";
                            recorder.bitrate = "0";
                        }

                        if (ImGui::Button("NVIDIA Visually Lossless"))
                        {
                            recorder.codec = "hevc_nvenc";
                            recorder.extra_args = "-preset slow -qp 6 -pix_fmt yuv420p -rc-lookahead 16";
                            recorder.bitrate = "0";
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("NVIDIA True Lossless"))
                        {
                            recorder.codec = "hevc_nvenc";
                            recorder.extra_args = "-preset slow -qp 0 -pix_fmt yuv420p -rc-lookahead 16";
                            recorder.bitrate = "0";
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("NVIDIA Colour Fix"))
                        {
                            recorder.codec = "hevc_nvenc";
                            recorder.extra_args = "-preset slow -qp 16 -pix_fmt bgr0 -rc-lookahead 16";
                            recorder.bitrate = "0";
                        }

                        if (ImGui::Button("AMD x264"))
                        {
                            recorder.codec = "h264_amf";
                            recorder.extra_args = "-pix_fmt yuv420p";
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("AMD x265"))
                        {
                            recorder.codec = "hevc_amf";
                            recorder.extra_args = "-pix_fmt yuv420p -qp_i 16 -qp_p 16";
                            recorder.bitrate = "0";
                        }

                        if (ImGui::Button("Global Colour Fix"))
                        {
                            recorder.compile_vf_args();
                            if (!recorder.vf_args.empty())
                                recorder.vf_args += ",";

                            recorder.vf_args += "colorspace=all=bt709:iall=bt470bg:fast=1";
                        }
                    }

                    ImGui::Spacing();

                    ImGui::Text("Folders");
                    ImGui::Separator();

                    if (ImGui::Button("Open Showcase Folder")) {
                        ShellExecuteW(nullptr, L"open", L"explorer", hacks::folderShowcasesPath.wstring().c_str(), nullptr, SW_SHOWDEFAULT);
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Change folder")) {
                        std::filesystem::path selectedPath = SelectFolder();
                        if (!selectedPath.empty()) {
                            hacks::folderShowcasesPath = selectedPath;
                        }
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Reset Folder")) {
                        hacks::folderShowcasesPath = hacks::folderPath / "Showcases";
                    }

                    
                    ImGui::Text("Showcase Folder: %s", hacks::folderShowcasesPath.string().c_str());
                }
                else {
                    ImGui::Text("Looks like FFmpeg is not installed, here are the instructions:");
                    ImGui::Text("1. Download ffmpeg (archive)");
                    ImGui::SameLine();
                    if (ImGui::Button("Download")) {
                        ShellExecuteA(0, "open", "https://github.com/AnimMouse/ffmpeg-autobuild/releases/latest", 0, 0, SW_SHOWNORMAL);
                    }
                    ImGui::Text("2. Extract from archive \"ffmpeg.exe\" file to Geometry Dash folder");
                    ImGui::Text("3. Restart Geometry Dash");
                    if (ImGui::Button("Bypass")) recorder.ffmpeg_installed = true;
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Audio")) {
                if (ImGui::Checkbox("Record Buffer", &recorderAudio.enabled, gui::scale)) {
                    if (containsRussianLetters(hacks::folderShowcasesPath)) {
                        recorder.enabled = false;
                        imgui_popup::add_popup("Invalid path to the showcase folder. Please remove any Cyrillic characters");
                    }
                    else {
                        if (!recorderAudio.showcase_mode) {
                            if (recorderAudio.enabled)
                                recorderAudio.start();
                            else 
                                recorderAudio.stop();
                        }
                    }
                }

                ImGui::Checkbox("Showcase Mode", &recorderAudio.showcase_mode, gui::scale);

                ImGui::SameLine();

                ImGui::Checkbox("Disable Objects Render", &hacks::disable_render, gui::scale);

                if (recorderAudio.showcase_mode) {
                    ImGui::Spacing();

                    ImGui::Text("Level Settings");
                    ImGui::Separator();

                
                    ImGui::PushItemWidth(200 * gui::scale);
                    ImGui::InputFloat("Second to Render After##2", &recorderAudio.after_end_duration, 1);
                }

                ImGui::Spacing();

                ImGui::Text("Filename");
                ImGui::Separator();

                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::InputText("##audio_filename", &recorderAudio.audio_name);

                ImGui::Spacing();

                ImGui::Text("Folders");
                ImGui::Separator();

                if (ImGui::Button("Open Showcase Folder")) {
                    ShellExecuteW(nullptr, L"open", L"explorer", hacks::folderShowcasesPath.wstring().c_str(), nullptr, SW_SHOWDEFAULT);
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Merge")) {
                static bool shortest = true;
                static std::vector<std::filesystem::path> videos;
                static std::vector<std::filesystem::path> audios;
                static int index_videos = 0;
                static int index_audios = 0;

                ImGui::BeginChild("##VideoSelect", {NULL, 150 * gui::scale}, true);
                for (size_t i = 0; i < videos.size(); i++) {
                    bool is_selected = (index_videos == i);
                    if (ImGui::Selectable(videos[i].filename().string().c_str(), is_selected)) {
                        index_videos = i;
                    }
                }
                ImGui::EndChild();

                ImGui::BeginChild("##AudioSelect", {NULL, 150 * gui::scale}, true);
                for (size_t i = 0; i < audios.size(); i++) {
                    bool is_selected = (index_audios == i);
                    if (ImGui::Selectable(audios[i].filename().string().c_str(), is_selected)) {
                        index_audios = i;
                    }
                }
                ImGui::EndChild();

                if (ImGui::Button("Refresh", {ImGui::GetContentRegionAvail().x, NULL})) {
                    videos.clear();
                    audios.clear();
                    for (const auto &entry : std::filesystem::directory_iterator(hacks::folderShowcasesPath)) {
                        if (entry.path().extension() == ".mp4") {
                            videos.push_back(entry.path().string());
                        }

                        if (entry.path().extension() == ".wav") {
                            audios.push_back(entry.path().string());
                        }
                    }
                }

                ImGui::Checkbox("Shortest", &shortest, gui::scale);

                
                ImGui::SameLine();

                if (ImGui::Button("Merge", {ImGui::GetContentRegionAvail().x, NULL})) {
                    if (videos.empty() || audios.empty()) {
                        
                    } else if (index_videos >= 0 && index_videos < (int)videos.size() && index_audios >= 0 && index_audios < (int)audios.size()) {
                        std::string command2 = "ffmpeg.exe -y -i \"" + videos[index_videos].string() + "\" -i \"" + audios[index_audios].string() + "\" -map 0:v -map 1:a -c:v copy ";
                        if (shortest) {
                            command2 += "-shortest ";
                        }
                        std::filesystem::path video_rel(videos[index_videos]);
                        command2 += fmt::format("\"{}\\audio_{}\"", hacks::folderShowcasesPath, video_rel.filename().string());
                        geode::log::debug("{}", command2);
                        auto process = subprocess::Popen(command2);
                    }
                }


                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("More Settings")) {
                ImGui::Checkbox("Popup Messages", &imgui_popup::enabled);
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::EndPopup();
    }

    int mode_ = (int)mode;

    if (ImGui::RadioButton("Disable", &mode_, 0, gui::scale))
        mode = state::disable;
    ImGui::SameLine();

    if (ImGui::RadioButton("Record", &mode_, 1, gui::scale))
    {
        bool canRecord = (version_engine == 1 && hacks::tps_enabled) || (version_engine == 2 && !hacks::tps_enabled);
        
        if (canRecord)
        {
            if (frame_advance) {
                imgui_popup::add_popup("Frame Advance enabled (that's so you don't say the level was freezing)");
            }

            if (mode != state::record) {
                replay.clear();
                replay2.clear();
            }
            mode = state::record;
        }
        else
        {
            mode = state::disable;
            imgui_popup::add_popup((version_engine == 1) ? "Enable TPS Bypass to record the replay" : "Disable TPS Bypass to record the replay");
        }
    }

    
    ImGui::SameLine();

    if (ImGui::RadioButton("Play", &mode_, 2, gui::scale)) {
        bool canPlay = (version_engine == 1 && hacks::tps_enabled) || (version_engine == 2 && !hacks::tps_enabled);
        
        if (canPlay) {
            if (frame_advance) {
                imgui_popup::add_popup("Frame Advance enabled (that's so you don't say the level was freezing)");
            }

            mode = state::play;
        } else {
            mode = state::disable;
            imgui_popup::add_popup((version_engine == 1) ? "Enable TPS Bypass to playback the replay" : "Disable TPS Bypass to playback the replay");
        }
    }

    ImGui::Separator();   

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 35 * gui::scale);
    ImGui::InputText("##replay_name", &replay_name);

    ImGui::SameLine();

    if (ImGui::ArrowButton("##replay_select", ImGuiDir_Down)) {
        openSelectReplay("Select Replay");
    }

    if (ImGui::Button("Save", {ImGui::GetContentRegionAvail().x / 3, NULL})) {
        if (version_engine == 1)
            imgui_popup::add_popup(save(replay_name));
        else if (version_engine == 2) 
            imgui_popup::add_popup(save2(replay_name));
    }
    ImGui::SameLine();

    if (ImGui::Button("Load", {ImGui::GetContentRegionAvail().x / 2, NULL})) {
        if (version_engine == 1)
            imgui_popup::add_popup(load(replay_name));
        else if (version_engine == 2) 
            imgui_popup::add_popup(load2(replay_name));
    }
    ImGui::SameLine();

    if (ImGui::Button("Clear", {ImGui::GetContentRegionAvail().x, NULL})) {
        replay.clear();
        replay2.clear();
        imgui_popup::add_popup("Replay has been cleared");
    }

    ImGui::Text("Replay Size: %zu", replay2.size());
    ImGui::Text("Frame: %i", get_frame());

    ImGui::Separator();

    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImColor(64, 64, 64).Value);
    if (ImGui::MenuItem("More settings")) {
        settings_openned = true;
        ImGui::OpenPopup("Replay Engine Settings");
    }
    ImGui::PopStyleColor();
}

bool SpamBot::next_frame()
{
    if ((downed && ++release_current >= release) || (!downed && ++hold_current >= hold))
    {
        downed = !downed;
        (downed) ? release_current = 0 : hold_current = 0;
        return true;
    }
    else
    {
        return false;
    }
}

void SpamBot::handle_spambot(GJBaseGameLayer *self)
{
    if (enabled && next_frame())
    {
        bool action = downed;
        if (player_p1)
        {
            self->handleButton(action, 1, true);
        }
        if (player_p2)
        {
            self->handleButton(action, 1, false);
        }
    }
}


void SpamBot::reset_temp()
{
    hold_current = 0;
    release_current = 0;
}

void StraightFly::handle_straightfly(GJBaseGameLayer *self)
{
    if (!enabled)
        return;

    float y = self->m_player1->m_position.y;
    double accel = self->m_player1->m_yVelocity;
    bool holding = self->m_player1->m_jumpBuffered;

    if (start_y == 0)
    {
        start(self);
    }

    if (self->m_player1->m_isUpsideDown)
    {
        float delta_y = y - start_y;
        y = start_y - delta_y;
        accel *= -1;
    }

    if (accel < 0 && y < start_y - accel - accuracy / 100 && !holding)
    {
        self->handleButton(true, 1, true);
    }
    else if (accel > 0 && y > start_y - accel + accuracy / 100 && holding)
    {
        self->handleButton(false, 1, true);
    }
}

void StraightFly::start(GJBaseGameLayer *self)
{
    start_y = self ? self->m_player1->m_position.y : 0.0f;
}