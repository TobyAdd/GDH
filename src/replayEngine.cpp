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
    if (pl)
        return static_cast<unsigned>(from<double>(pl, 0x3C8) * hacks::tps_value);
    return 0;
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
    replay2.push_back({get_frame(), hold, button, player});
}

void ReplayEngine::handle_playing(GJBaseGameLayer* self)
{
    unsigned frame = get_frame();

    if (accuracy_fix) {
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

        if (!replay2.empty() && replay2.back().hold) {
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

    if (!hacks::tps_enabled) {
        hacks::tps_enabled = true;
        imgui_popup::add_popup("TPS Bypass enabled");
    }

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

void ReplayEngine::remove_actions(unsigned frame)
{
    auto check = [&](replay_data &action) -> bool
    {
        return action.frame > frame;
    };
    replay.erase(remove_if(replay.begin(), replay.end(), check), replay.end());

    auto check2 = [&](replay_data2 &action) -> bool
    {
        return action.frame >= frame;
    };
    replay2.erase(remove_if(replay2.begin(), replay2.end(), check2), replay2.end());
}

std::vector<std::filesystem::path> replay_list;
void openSelectReplay(const char *str_id) {
    replay_list.clear();
    for (const auto &entry : std::filesystem::directory_iterator(hacks::folderMacroPath)) {
        std::string replay = entry.path().filename().string();
        if (replay.size() >= 3 && replay.substr(replay.size() - 3) == ".re") {
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
    
    if (ImGui::BeginPopupModal("Replay Engine Settings", &settings_openned)) {
        if (ImGui::BeginTabBar("Replay Engine Settings Tabs")) {
            if (ImGui::BeginTabItem("Settings")) {
                ImGui::Checkbox("Real Time", &engine.real_time, gui::scale);

                ImGui::SameLine();

                ImGui::Checkbox("Frame Advance", &engine.frame_advance, gui::scale);

                ImGui::Checkbox("Accuracy Fix", &engine.accuracy_fix, gui::scale);

                ImGui::SameLine();

                ImGui::Checkbox("Rotation Fix", &engine.rotation_fix, gui::scale);

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
                        if (engine.containsRussianLetters(hacks::folderShowcasesPath)) {
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
                    if (engine.containsRussianLetters(hacks::folderShowcasesPath)) {
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
        if (hacks::tps_enabled) {
            if (engine.frame_advance)
                imgui_popup::add_popup("Frame Advance enabled (that's so you don't say the level was freezing)");

            if (mode != state::record) {
                replay.clear();
                replay2.clear();
            }
            mode = state::record;
        }
        else {
            mode = state::disable;
            imgui_popup::add_popup("Enable TPS Bypass to record the replay");
        }
    }
    
    ImGui::SameLine();

    if (ImGui::RadioButton("Play", &mode_, 2, gui::scale)) {
        if (hacks::tps_enabled) {
            if (engine.frame_advance)
                imgui_popup::add_popup("Frame Advance enabled (that's so you don't say the level was freezing)");

            mode = state::play;
        }
        else {
            mode = state::disable;
            imgui_popup::add_popup("Enable TPS Bypass to playback the replay");
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
        imgui_popup::add_popup(save(replay_name));
    }
    ImGui::SameLine();

    if (ImGui::Button("Load", {ImGui::GetContentRegionAvail().x / 2, NULL})) {
        imgui_popup::add_popup(load(replay_name));
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