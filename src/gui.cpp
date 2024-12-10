#include "gui.hpp"
#include <imgui/imgui_stdlib.h>
#include <font.hpp>
#include "config.hpp"
#include <matjson.hpp>
#include "hacks.hpp"
#include "memory.hpp"
#include "labels.hpp"
#include "replayEngine.hpp"
#include "recorder.hpp"
#ifdef GEODE_IS_WINDOWS
#include <subprocess.hpp>
#endif

std::chrono::steady_clock::time_point animationStartTime;
bool isAnimating = false;
bool isFadingIn = false;

std::string search_text;

int selected_label_corner = 0;
int selected_label_type = 0;
std::string selected_label_text;

void Gui::updateCursorState() {
    bool canShowInLevel = true;
    if (auto* playLayer = PlayLayer::get()) {
        canShowInLevel = playLayer->m_hasCompletedLevel ||
                         playLayer->m_isPaused ||
                         GameManager::sharedState()->getGameVariable("0024");
    }
    if (m_show || canShowInLevel)
        PlatformToolbox::showCursor();
    else
        PlatformToolbox::hideCursor();
}

void Gui::animateAlpha()
{
    ImGuiStyle& style = ImGui::GetStyle();

    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> diff = currentTime - animationStartTime;
    float elapsed = diff.count();

    float time = m_anim_durr / 1000.0f;
    if (elapsed >= time)
    {
        style.Alpha = isFadingIn ? 1.0f : 0.0f;
        isAnimating = false;

        if (!isFadingIn)
        {
            m_show = !m_show;
            Config::get().save(fileDataPath);
            Labels::get().save();
            RGBIcons::get().save();
            updateCursorState();
        }

        return;
    }

    float t = elapsed / time;
    float alpha = isFadingIn ? 0.0f + t: 1.0f - t;
    style.Alpha = alpha;
}

std::vector<std::string> stretchedWindows;
void Gui::Render() {
    
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1);
    #ifdef GEODE_IS_ANDROID
    ImGui::Begin("toggle gui");
    if (ImGuiH::Button("toggle")) {
        Toggle();
    }
    ImGui::End();
    #endif
    ImGuiH::Popup::get().render();
    ImGui::PopStyleVar();
    


    if (isAnimating) {
        animateAlpha();
    }
    
    if (!m_show) return;

    auto& hacks = Hacks::get();
    for (auto& win : hacks.m_windows) {
        std::string windowName = win.name;
        if (std::find(stretchedWindows.begin(), stretchedWindows.end(), windowName) == stretchedWindows.end())
        {
            ImVec2 windowSize = ImVec2(win.w, win.h);
            ImVec2 windowPos = ImVec2(win.x, win.y);

            if (m_needRescale) {
                windowSize = ImVec2(win.orig_w * m_scale, win.orig_h * m_scale);
                windowPos = ImVec2(win.orig_x * m_scale, win.orig_y * m_scale);
            }

            ImGui::SetNextWindowSize(windowSize);
            ImGui::SetNextWindowPos(windowPos);

            stretchedWindows.push_back(windowName);
        }

        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
        ImGui::Begin(windowName.c_str(), 0, ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoNavInputs);
        ImGui::PopFont();

        if (!ImGui::IsWindowCollapsed()) {
            auto size = ImGui::GetWindowSize();
            auto pos = ImGui::GetWindowPos();

            win.w = size.x;
            win.h = size.y;
            win.x = pos.x;
            win.y = pos.y;
        }


        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);    

       
        auto &config = Config::get();
        if (windowName == "GDH Settings") {
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::InputTextWithHint("##Search", "Search:", &search_text);

            if (ImGui::GetIO().MouseWheel != 0 && ImGui::IsItemActive())
                ImGui::SetWindowFocus(NULL);

            const char* items[] = {"25%", "50%", "75%", "80%", "85%", "90%", "95%", "100%", "125%", "150%", "175%", "200%", "250%", "300%", "400%"};
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGuiH::Combo("##Menu scale", &m_index_scale, items, IM_ARRAYSIZE(items))) {
                m_scale = float(atof(items[m_index_scale])) / 100.0f;    
                config.set<float>("gui_scale", m_scale);
                config.set<int>("gui_index_scale", m_index_scale);
                m_needRescale = true;
                ImGuiCocos::get().reload();
            }

            int alpha = int(ImGui::GetStyle().Alpha * 255);

            for (int i = 0; i < themes.size(); ++i) {
                const auto& theme = themes[i];
                ImColor button_color(theme.color_bg.r, theme.color_bg.g, theme.color_bg.b, alpha);
                
                if (i == 0)
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.f * m_scale);
                
                if (ImGuiH::CircularButton(fmt::format("COLOR_{}", i).c_str(), 10.f * m_scale, button_color)) {
                    config.set<int>("gui_color_index", i);
                    ApplyColor(theme);
                }

                ImGui::SameLine();
            }

            bool inverted = config.get("gui_inverted", false);
            ImColor inverted_button_color = inverted ? ImColor(255, 255, 255) : ImColor(27, 27, 29, alpha);
            ImColor inverted_button_hover_color = ImColor(64, 64, 64, alpha);

            if (ImGuiH::CircularButton("TOGGLE_INVERT", 10.f * m_scale, inverted_button_color, true, inverted_button_hover_color)) {
                config.set("gui_inverted", !inverted);
                ApplyGuiColors(!inverted);
            }

            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Invert theme (beta)");

            ImGuiH::Checkbox("Keybinds Mode", &hacks.keybinds_mode, m_scale);
        }
        else if (windowName == "Framerate") {
            bool tps_enabled = config.get<bool>("tps_enabled", false);
            float tps_value = config.get<float>("tps_value", 240.f);

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - (35 + 5) * m_scale);
            if (ImGui::DragFloat("##tps_value", &tps_value, 1, 1, FLT_MAX, "%0.f TPS"))
                config.set<float>("tps_value", tps_value);

            ImGui::SameLine();
            if (ImGuiH::Checkbox("##tps_enabled", &tps_enabled, m_scale))
                config.set<bool>("tps_enabled", tps_enabled);
            
            bool speedhack_enabled = config.get<bool>("speedhack_enabled", false);
            float speedhack_value = config.get<float>("speedhack_value", 1.f);

            bool speedhackAudio_enabled = config.get<bool>("speedhackAudio_enabled", false);

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - (35 + 5) * m_scale);
            if (ImGui::DragFloat("##speedhack_value", &speedhack_value, 0.01f, 0, FLT_MAX, "Speed: %.2fx")) 
                config.set<float>("speedhack_value", speedhack_value);

            ImGui::SameLine();
            if (ImGuiH::Checkbox("##speedhack_enabled", &speedhack_enabled, m_scale))
                config.set<bool>("speedhack_enabled", speedhack_enabled);

            if (ImGuiH::Checkbox("Speedhack Audio", &speedhackAudio_enabled, m_scale))
                config.set<bool>("speedhackAudio_enabled", speedhackAudio_enabled);
        }
        else if (windowName == "Variables") {
            static int type_index = 0;
            static int player_index = 0;
            static int creator_index = 0;
            static std::string value;

            const char* types[] = {"Creator", "Player"};
            const char* player[] = {"Attempts", "Jumps", "Normal %", "Position X", "Position Y", "Practice %", "Song ID", "Speed"};
            const char* creator[] = {"Object ID"};

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGuiH::Combo("##TypeVars", &type_index, types, IM_ARRAYSIZE(types));

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            if (type_index == 0)
                ImGuiH::Combo("##CreatorVars", &creator_index, creator, IM_ARRAYSIZE(creator));
            else if (type_index == 1)
                ImGuiH::Combo("##PlayerVars", &player_index, player, IM_ARRAYSIZE(player));

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::InputText("##ValueVar", &value);

            auto handleGetSetCreator = [&](bool isSet) {
                auto EditorUI = EditorUI::get();
                if (!EditorUI) return;

                if (isSet) {
                    EditorUI::get()->m_selectedObjectIndex = std::stoi(value);
                } else {
                    value = std::to_string(EditorUI::get()->m_selectedObjectIndex);
                }
            };

            auto handleGetSetPlayer = [&](bool isSet) {
                auto pl = PlayLayer::get();
                if (!pl) return;

                auto& level = *pl->m_level;
                auto& player1 = *pl->m_player1;

                switch (player_index) {
                    case 0: // Attempts
                        if (isSet) level.m_attempts = std::stoi(value);
                        else value = std::to_string(level.m_attempts);
                        break;
                    case 1: // Jumps
                        if (isSet) level.m_jumps = std::stoi(value);
                        else value = std::to_string(level.m_jumps);
                        break;
                    case 2: // Normal %
                        if (isSet) level.m_normalPercent = std::stoi(value);
                        else value = std::to_string(level.m_normalPercent);
                        break;
                    case 3: // Position X
                        if (isSet) player1.m_position.x = std::stof(value);
                        else value = std::to_string(player1.m_position.x);
                        break;
                    case 4: // Position Y
                        if (isSet) player1.m_position.y = std::stof(value);
                        else value = std::to_string(player1.m_position.y);
                        break;
                    case 5: // Practice %
                        if (isSet) level.m_practicePercent = std::stoi(value);
                        else value = std::to_string(level.m_practicePercent);
                        break;
                    case 6: // Song ID
                        if (isSet) level.m_songID = std::stoi(value);
                        else value = std::to_string(level.m_songID);
                        break;
                    case 7: // Speed
                        if (isSet) player1.m_playerSpeed = std::stof(value);
                        else value = std::to_string(player1.m_playerSpeed);
                        break;
                }
            };

            if (ImGuiH::Button("Get", {ImGui::GetContentRegionAvail().x / 2, NULL})) {
                if (type_index == 0) handleGetSetCreator(false); // Get Creator
                else if (type_index == 1) handleGetSetPlayer(false); // Get Player
            }

            ImGui::SameLine();

            if (ImGuiH::Button("Set", {ImGui::GetContentRegionAvail().x, NULL})) {
                if (type_index == 0) handleGetSetCreator(true); // Set Creator
                else if (type_index == 1) handleGetSetPlayer(true); // Set Player
            }
        }
        else if (windowName == "Replay Engine") {
            auto& engine = ReplayEngine::get();
            static std::vector<std::filesystem::path> replay_list;

            if (ImGui::BeginPopupModal("Select Replay", 0, ImGuiWindowFlags_NoResize)) {
                ImGui::BeginChild("Select Replay##2", {400 * m_scale, 300 * m_scale});
                for (int i = 0; i < (int)replay_list.size(); i++)
                {
                    if (ImGuiH::Button(replay_list[i].filename().string().c_str(), {ImGui::GetContentRegionAvail().x, NULL}))
                    {
                        std::string extension = replay_list[i].filename().extension().string();
                        if (extension != ".re" && extension != ".re2")
                            engine.replay_name = replay_list[i].filename().replace_extension().string();
                        else
                            engine.replay_name = replay_list[i].filename().string();
                        ImGui::CloseCurrentPopup();
                    }
                }
                ImGui::EndChild();

                #ifdef GEODE_IS_WINDOWS
                if (ImGuiH::Button("Open Folder", {ImGui::GetContentRegionAvail().x, NULL})) {
                    geode::utils::file::openFolder(folderMacroPath);
                }
                #endif
                
                if (ImGuiH::Button("Close", {ImGui::GetContentRegionAvail().x, NULL})) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
                
            int mode_ = (int)engine.mode;

            if (ImGuiH::RadioButton("Disable", &mode_, 0, m_scale))
                engine.mode = state::disable;

            ImGui::SameLine();

            if (ImGuiH::RadioButton("Record", &mode_, 1, m_scale))
            {
                bool canRecord = config.get<bool>("tps_enabled", false);
                
                if (canRecord)
                {
                    if (engine.mode != state::record) {
                        engine.clear();
                    }
                    engine.mode = state::record;
                }
                else
                {
                    engine.mode = state::disable;
                    ImGuiH::Popup::get().add_popup("Enable TPS Bypass to record the replay");
                }
            }

            
            ImGui::SameLine();

            if (ImGuiH::RadioButton("Play", &mode_, 2, m_scale)) {
                bool canPlay = config.get<bool>("tps_enabled", false);
                
                if (canPlay) {
                    engine.mode = state::play;
                } else {
                    engine.mode = state::disable;
                    ImGuiH::Popup::get().add_popup("Enable TPS Bypass to playback the replay");
                }
            }

            ImGui::Separator();   

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 35 * m_scale);
            ImGui::InputText("##replay_name", &engine.replay_name);

            ImGui::SameLine();

            if (ImGuiH::ArrowButton("##replay_select", ImGuiDir_Down)) {
                replay_list.clear();
                for (const auto &entry : std::filesystem::directory_iterator(folderMacroPath)) {
                    std::string ext = entry.path().filename().extension().string();
                    if (ext == ".re" || ext == ".re2" || ext == ".re3") {
                        replay_list.push_back(entry);
                    }
                }
                ImGui::OpenPopup("Select Replay");
            }

            if (ImGuiH::Button("Save", {ImGui::GetContentRegionAvail().x / 3, NULL})) {
                ImGuiH::Popup::get().add_popup(engine.save(engine.replay_name));
            }
            ImGui::SameLine();

            if (ImGuiH::Button("Load", {ImGui::GetContentRegionAvail().x / 2, NULL})) {
                ImGuiH::Popup::get().add_popup(engine.load(engine.replay_name));
            }
            ImGui::SameLine();

            if (ImGuiH::Button("Clear", {ImGui::GetContentRegionAvail().x, NULL})) {
                engine.clear();
                ImGuiH::Popup::get().add_popup("Replay has been cleared");
            }

            ImGui::Text("Replay Size: %zu", engine.get_actions_size());
            ImGui::Text("Frame: %i", engine.get_frame());

            ImGui::Separator();

            #ifdef GEODE_IS_WINDOWS
            auto& recorder = Recorder::get();
            auto& recorderAudio = RecorderAudio::get();

            if (recorder.settings_openned) {
                static bool first_time = true;
                if (first_time) {
                    first_time = false;
                    ImGui::SetNextWindowSize({800 * m_scale, 520 * m_scale});
                }                
            }

            if (ImGui::BeginPopupModal("Recorder", &recorder.settings_openned) && ImGui::BeginTabBar("Recorder Tabs")) {
                auto containsRussianLetters = [](const std::filesystem::path& p) -> bool {
                    auto pathStr = p.u8string();
                    for (char c : pathStr) {
                        if ((unsigned char)c >= 0xD0 && (unsigned char)c <= 0xD1) {
                            return true;
                        }
                    }
                    return false;
                };
                
                if (ImGui::BeginTabItem("General")) {
                    if (recorder.ffmpeg_installed) {                        
                        auto pl = PlayLayer::get();
                        if (ImGuiH::Checkbox("Record##Recorder", &recorder.enabled, m_scale)) {
                            ImVec2 displaySize = ImGui::GetIO().DisplaySize;

                            if (containsRussianLetters(recorder.folderShowcasesPath)) {
                                recorder.enabled = false;
                                ImGuiH::Popup::get().add_popup("Invalid path to the showcase folder. Please remove any Cyrillic characters");
                            }
                            else if (pl && pl->m_hasCompletedLevel) {
                                ImGuiH::Popup::get().add_popup("Restart level to start recording");
                                recorder.enabled = false;
                            }
                            else {
                                bool canRecord = (config.get<bool>("tps_enabled", false) && recorder.fps <= config.get<float>("tps_value", 240.f));

                                if (canRecord) {
                                    if (recorder.enabled) {
                                        if (!recorder.advanced_mode) {
                                            recorder.full_cmd = recorder.compile_command();
                                        }

                                        recorder.start(recorder.full_cmd);
                                    }                        
                                    else 
                                        recorder.stop();
                                }
                                else {
                                    recorder.enabled = false;
                                    ImGuiH::Popup::get().add_popup("Recorder FPS is valid and less than or equal to macro FPS");
                                }
                            }
                        }

                        ImGui::SameLine();

                        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                        ImGui::InputText("##videoname", &recorder.video_name);

                        ImGuiH::Checkbox("Advanced Mode", &recorder.advanced_mode, m_scale);

                        if (recorder.advanced_mode) {
                            ImGui::SameLine();
                            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                            ImGui::InputText("##full_args", &recorder.full_cmd);
                            if (ImGuiH::Button("Compile")) {
                                recorder.full_cmd = recorder.compile_command();
                            }

                            ImGui::SameLine();
                            if (ImGuiH::Button("Copy to clipboard")) {
                                ImGui::SetClipboardText(recorder.full_cmd.c_str());
                            }

                            ImGui::SameLine();
                            if (ImGuiH::Button("Paste to from clipboard")) {
                                recorder.full_cmd = (std::string)ImGui::GetClipboardText();
                            }
                        }

                        ImGui::SameLine();

                        ImGui::Spacing();

                        ImGui::Text("Resolution:");
                        ImGui::Separator();

                        ImGui::PushItemWidth(45.f * m_scale);
                        if (ImGui::InputInt("##width", &recorder.width, 0) && recorder.lock_aspect_ratio) {
                            float aspect_ratio = 16.0f / 9.0f;
                            recorder.height = static_cast<int>(recorder.width / aspect_ratio);
                        }
                        ImGui::SameLine(0, 5);

                        ImGui::Text("x");
                        ImGui::SameLine(0, 5);

                        ImGui::PushItemWidth(45.f * m_scale);
                        if (ImGui::InputInt("##height", &recorder.height, 0) && recorder.lock_aspect_ratio) {
                            float aspect_ratio = 16.0f / 9.0f;
                            recorder.width = static_cast<int>(recorder.height * aspect_ratio);
                        }
                        ImGui::SameLine(0, 5);

                        ImGui::Text("@");
                        ImGui::SameLine(0, 5);

                        ImGui::PushItemWidth(35.f * m_scale);
                        ImGui::InputInt("##fps", &recorder.fps, 0);

                        ImGui::SameLine(0, 5);

                        ImGuiH::Checkbox("Lock Aspect Ratio (16:9)", &recorder.lock_aspect_ratio, m_scale);

                        ImGui::Spacing();

                        ImGui::Text("Encoding Settings");
                        ImGui::Separator(); 
                        
                        ImGui::PushItemWidth(50.f * m_scale);
                        ImGui::InputText("Bitrate", &recorder.bitrate);

                        ImGui::SameLine();

                        ImGui::PushItemWidth(80.f * m_scale);
                        ImGui::InputText("Codec", &recorder.codec);

                        ImGui::PushItemWidth(300 * m_scale);
                        ImGui::InputText("Extra Arguments", &recorder.extra_args);

                        ImGui::PushItemWidth(300 * m_scale);
                        ImGui::InputText("VF Args", &recorder.vf_args);

                        if (ImGuiH::Checkbox("vflip", &recorder.vflip, m_scale)) {
                            recorder.compile_vf_args();
                        }

                        if (ImGuiH::Checkbox("Fade in", &recorder.fade_in, m_scale)) {
                            recorder.compile_vf_args();
                        }

                        ImGui::SameLine();

                        ImGui::PushItemWidth(120 * m_scale);
                        if (ImGui::DragFloat("##fade_in_start", &recorder.fade_in_start, 0.01f, 0, FLT_MAX, "Start: %.2fs")) {
                            recorder.compile_vf_args();
                        }

                        ImGui::SameLine();
                        
                        ImGui::PushItemWidth(120 * m_scale);
                        if (ImGui::DragFloat("##fade_in_end", &recorder.fade_in_end, 0.01f, 0, FLT_MAX, "End: %.2fs")) {
                            recorder.compile_vf_args();
                        }

                        ImGuiH::Checkbox("Fade out", &recorder.fade_out, m_scale);
                        ImGui::Text("Note: The length of the fade-out is calculated based on the value of \"Second to Render After\"");

                        ImGuiH::Checkbox("Hide Level Complete", &recorder.hide_level_complete, m_scale);

                        ImGui::Spacing();

                        ImGui::Text("Level Settings");
                        ImGui::Separator();

                        ImGui::PushItemWidth(200 * m_scale);
                        ImGui::InputFloat("Second to Render After", &recorder.after_end_duration, 1);

                        ImGui::Spacing();

                        ImGui::Text("Presets (Thanks WarGack, ElPaan, midixd)");

                        ImGui::Separator();

                        if (ImGuiH::Button("HD"))
                        {
                            recorder.width = 1280;
                            recorder.height = 720;
                            recorder.fps = 60;
                            recorder.bitrate = "25M";
                        }

                        ImGui::SameLine();

                        if (ImGuiH::Button("FULL HD"))
                        {
                            recorder.width = 1920;
                            recorder.height = 1080;
                            recorder.fps = 60;
                            recorder.bitrate = "50M";
                        }

                        ImGui::SameLine();

                        if (ImGuiH::Button("2K"))
                        {
                            recorder.width = 2560;
                            recorder.height = 1440;
                            recorder.fps = 60;
                            recorder.bitrate = "70M";
                        }

                        ImGui::SameLine();

                        if (ImGuiH::Button("4K"))
                        {
                            recorder.width = 3840;
                            recorder.height = 2160;
                            recorder.fps = 60;
                            recorder.bitrate = "80M";
                        }

                        ImGui::SameLine();

                        if (ImGuiH::Button("8K"))
                        {
                            recorder.width = 7680;
                            recorder.height = 4320;
                            recorder.fps = 60;
                            recorder.bitrate = "250M";
                        }
                            
                        if (ImGuiH::Button("CPU x264"))
                        {
                            recorder.codec = "libx264";
                            recorder.extra_args = "-pix_fmt yuv420p -preset ultrafast";
                        }                    
                        
                        ImGui::SameLine();
                        
                        if (ImGuiH::Button("CPU x265"))
                        {
                            recorder.codec = "libx265";
                            recorder.extra_args = "-pix_fmt yuv420p -preset ultrafast";
                        }
                        ImGui::SameLine();
                        if (ImGuiH::Button("CPU AV1 Lossless"))
                        {
                            recorder.codec = "libsvtav1";
                            recorder.extra_args = "-crf 0 -pix_fmt yuv420p";
                        }
                        if (ImGuiH::Button("NVIDIA x264"))
                        {
                            recorder.codec = "h264_nvenc";
                            recorder.extra_args = "-pix_fmt yuv420p -preset p7";
                        }
                        ImGui::SameLine();
                        if (ImGuiH::Button("NVIDIA x265"))
                        {
                            recorder.codec = "hevc_nvenc";
                            recorder.extra_args = "-pix_fmt yuv420p -preset p7";
                        }
                        ImGui::SameLine();
                        
                        if (ImGuiH::Button("NVIDIA AV1"))
                        {
                            recorder.codec = "av1_nvenc";
                            recorder.extra_args = "-pix_fmt yuv420p -preset p7";
                        }
                        
                        if (ImGuiH::Button("AMD x264 Lossless"))
                        {
                            recorder.codec = "h264_amf";
                            recorder.extra_args = "-pix_fmt yuv420p -rc cqp -qp_i 0 -qp_p 0 -qp_b 0";
                        }
                        ImGui::SameLine();
                        if (ImGuiH::Button("AMD x265 Lossless"))
                        {
                            recorder.codec = "hevc_amf";
                            recorder.extra_args = "-pix_fmt yuv420p -rc cqp -qp_i 0 -qp_p 0 -qp_b 0";
                        }
                        if (ImGuiH::Button("Color Fix"))
                        {
                            recorder.compile_vf_args();
                            if (!recorder.vf_args.empty())
                                recorder.vf_args += ",";
                            recorder.vf_args += "scale=out_color_matrix=bt709";
                        }

                        ImGui::Spacing();

                        ImGui::Text("Folders");
                        ImGui::Separator();

                        if (ImGuiH::Button("Open Showcase Folder")) {
                            geode::utils::file::openFolder(recorder.folderShowcasesPath);
                        }

                        ImGui::SameLine();

                        if (ImGuiH::Button("Change folder")) {                           
                            auto result = geode::utils::file::pick(geode::utils::file::PickMode::OpenFolder, {std::nullopt, {}});
                            if (result.isFinished() && !result.getFinishedValue()->isErr()) {
                                recorder.folderShowcasesPath = result.getFinishedValue()->unwrap();
                                config.set<std::filesystem::path>("showcases_path", recorder.folderShowcasesPath);
                            }
                        }

                        ImGui::SameLine();

                        if (ImGuiH::Button("Reset Folder")) {
                            recorder.folderShowcasesPath = folderPath / "Showcases";
                            config.set<std::filesystem::path>("showcases_path", recorder.folderShowcasesPath);
                        }

                        
                        ImGui::Text("Showcase Folder: %s", recorder.folderShowcasesPath.string().c_str());
                    }
                    else {
                        ImGui::Text("Looks like FFmpeg is not installed, here are the instructions:");
                        ImGui::Text("1. Download ffmpeg (archive)");
                        ImGui::SameLine();
                        if (ImGuiH::Button("Download")) {
                            ShellExecuteA(0, "open", "https://github.com/AnimMouse/ffmpeg-autobuild/releases/latest", 0, 0, SW_SHOWNORMAL);
                        }
                        ImGui::Text("2. Extract from archive \"ffmpeg.exe\" file to Geometry Dash folder");
                        ImGui::Text("3. Restart Geometry Dash");
                        if (ImGuiH::Button("Bypass")) recorder.ffmpeg_installed = true;
                    }
                    ImGui::EndTabItem();
                }	
                
                if (ImGui::BeginTabItem("Audio")) {
                    if (ImGui::Checkbox("Record Buffer", &recorderAudio.enabled, m_scale)) {
                        if (containsRussianLetters(recorder.folderShowcasesPath)) {
                            recorder.enabled = false;
                            ImGuiH::Popup::get().add_popup("Invalid path to the showcase folder. Please remove any Cyrillic characters");
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

                    ImGui::Checkbox("Showcase Mode", &recorderAudio.showcase_mode, m_scale);

                    if (recorderAudio.showcase_mode) {
                        ImGui::Spacing();

                        ImGui::Text("Level Settings");
                        ImGui::Separator();

                    
                        ImGui::PushItemWidth(200 * m_scale);
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

                    if (ImGuiH::Button("Open Showcase Folder")) {
                        geode::utils::file::openFolder(recorder.folderShowcasesPath);
                    }
                    ImGui::EndTabItem();
                }
                
                if (ImGui::BeginTabItem("Merge")) {
                    static bool shortest = true;
                    static std::vector<std::filesystem::path> videos;
                    static std::vector<std::filesystem::path> audios;
                    static int index_videos = 0;
                    static int index_audios = 0;

                    ImGui::BeginChild("##VideoSelect", {NULL, 150 * m_scale}, true);
                    for (size_t i = 0; i < videos.size(); i++) {
                        bool is_selected = (index_videos == i);
                        if (ImGui::Selectable(videos[i].filename().string().c_str(), is_selected)) {
                            index_videos = i;
                        }
                    }
                    ImGui::EndChild();

                    ImGui::BeginChild("##AudioSelect", {NULL, 150 * m_scale}, true);
                    for (size_t i = 0; i < audios.size(); i++) {
                        bool is_selected = (index_audios == i);
                        if (ImGui::Selectable(audios[i].filename().string().c_str(), is_selected)) {
                            index_audios = i;
                        }
                    }
                    ImGui::EndChild();

                    if (ImGuiH::Button("Refresh", {ImGui::GetContentRegionAvail().x, NULL})) {
                        videos.clear();
                        audios.clear();
                        for (const auto &entry : std::filesystem::directory_iterator(recorder.folderShowcasesPath)) {
                            if (entry.path().extension() == ".mp4" || 
                                entry.path().extension() == ".mkv" || 
                                entry.path().extension() == ".avi" || 
                                entry.path().extension() == ".mov" || 
                                entry.path().extension() == ".flv" || 
                                entry.path().extension() == ".wmv" || 
                                entry.path().extension() == ".webm" || 
                                entry.path().extension() == ".m4v" || 
                                entry.path().extension() == ".mpeg") {
                                
                                videos.push_back(entry.path().string());
                            }

                            if (entry.path().extension() == ".wav") {
                                audios.push_back(entry.path().string());
                            }
                        }
                    }

                    ImGui::Checkbox("Shortest", &shortest, m_scale);

                    
                    ImGui::SameLine();

                    if (ImGuiH::Button("Merge", {ImGui::GetContentRegionAvail().x, NULL})) {
                        if (videos.empty() || audios.empty()) {
                            
                        } else if (index_videos >= 0 && index_videos < (int)videos.size() && index_audios >= 0 && index_audios < (int)audios.size()) {
                            std::string command2 = "ffmpeg.exe -y -i \"" + videos[index_videos].string() + "\" -i \"" + audios[index_audios].string() + "\" -map 0:v -map 1:a -c:v copy ";
                            if (shortest) {
                                command2 += "-shortest ";
                            }
                            std::filesystem::path video_rel(videos[index_videos]);
                            command2 += fmt::format("\"{}\\audio_{}\"", recorder.folderShowcasesPath, video_rel.filename().string());
                            geode::log::debug("{}", command2);
                            auto process = subprocess::Popen(command2);
                        }
                    }
                    ImGui::EndTabItem();
                }	
                ImGui::EndTabBar();
                ImGui::EndPopup();
            }                            
                            

            if (ImGui::MenuItem("Recorder")) {
                recorder.settings_openned = true;
                recorder.ffmpeg_installed = std::filesystem::exists("ffmpeg.exe");
                ImGui::OpenPopup("Recorder");
            }
            #endif
        }
        else if (windowName == "Variables") { 
            static uintptr_t address = geode::base::get();
            ImGui::Text("%s", fmt::format("{:x} ({:x})", address, address - geode::base::get()).c_str());

            if (ImGuiH::Button("scan"))
                address = memory::PatternScan(address+1, 0, "00 60 6A 48");

            if (ImGuiH::Button("scan2"))
                address = memory::PatternScan(address+1, 0, "80 67 6A 48");

            if (ImGuiH::Button("reset"))
                address = geode::base::get();
        }
        else if (windowName == "Labels") {
            auto &labels = Labels::get();
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x/2);
            if (ImGui::DragFloat("##Label Opacity", &labels.opacity, 0.01f, 0.f, 1.f, "Opacity: %.2f")) config.set<float>("label-opacity", labels.opacity);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGui::DragFloat("##Label Size", &labels.size, 0.01f, 0.f, 1.f, "Size: %.2f")) config.set<float>("label-size", labels.size);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGui::DragFloat("##Label Padding", &labels.padding, 1.f, 0.f, 50.f, "Label Padding: %.1fpx")) config.set<float>("label-padding", labels.padding);
            
            const char *labels_positions[] = {"Top Left", "Top Right", "Top", "Bottom Left", "Bottom Right", "Bottom"};
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGuiH::Combo("##labelspos", &selected_label_corner, labels_positions, 6, 6);

            const char *label_types[] = {
                "Time (24H)",
                "Time (12H)",
                "Session Time",
                "Level Progress",
                "Attempt",
                "CPS Counter",
                "Level Info",
                "Custom Text",
            };
            int label_types_count = sizeof(label_types)/sizeof(label_types[0]);
            
            ImGuiH::Combo("##labeladdtype", &selected_label_type, label_types, label_types_count);
            if (selected_label_type == label_types_count - 1)
                ImGui::InputText("##labelinput", &selected_label_text);

            ImGui::SameLine();
            if (ImGuiH::Button("+")) {
                std::string text;
                if (selected_label_type == label_types_count - 1) text = selected_label_text;
                else if (selected_label_type == 0) text = "{time:24}";
                else if (selected_label_type == 1) text = "{time:12}";
                else if (selected_label_type == 2) text = "Session Time: {sessionTime}";
                else if (selected_label_type == 3) text = "{progress}";
                else if (selected_label_type == 4) text = "Attempt {attempt}";
                else if (selected_label_type == 5) text = "{cps}/{cpsHigh}/{clicks}";
                else if (selected_label_type == 6) text = "{levelName}{byLevelCreator} ({levelId})";
                
                Label l((LabelCorner) (selected_label_corner+1), text);
                labels.add(l);
            }
            
            ImGui::Separator();
            ImGui::BeginChild("Labels");
            ImGui::Spacing();

            if (labels.labels.empty())
                ImGui::TextDisabled("No labels in this corner");
            
            for (size_t index = 0; index < labels.labels.size(); index++) {
                Label& item = labels.labels[index];

                if (static_cast<int>(item.corner-1) != selected_label_corner) continue;
                
                ImGui::PushID(index);

                ImGui::Selectable("  =", false, 0, {20.f, 20.f});
                ImGui::SameLine();
                
                if (ImGui::GetMouseClickedCount(ImGuiMouseButton_Left) >= 2 && ImGui::IsItemHovered()) {
                    labels.remove(index);
                }
                
                if (ImGui::BeginDragDropSource()) {
                    ImGui::TextUnformatted(item.format_text.c_str());
                    ImGui::SetDragDropPayload("LBLMOVE", &index, sizeof(size_t));
                    ImGui::EndDragDropSource();
                }
                
                if (ImGui::BeginDragDropTarget()) {
                    ImGuiDragDropFlags target_flags = 0;
                    if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("LBLMOVE", target_flags)) {
                        size_t move_from = *(size_t*)payload->Data;
                        labels.swap(move_from, index);
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::InputText("##inptext", &item.format_text);
                
                ImGui::PopID();
            }
            ImGui::EndChild();
        }
        else if (windowName == "Shortcuts") {
            if (ImGuiH::Button("Options", {ImGui::GetContentRegionAvail().x, NULL})) {
                auto options_layer = OptionsLayer::create();
                auto scene = cocos2d::CCScene::get();

                if (options_layer && scene) {
                    auto zOrder = scene->getHighestChildZ();
                    scene->addChild(options_layer, zOrder + 1);
                    options_layer->showLayer(false);
                }
            }

            if (ImGuiH::Button("Reset Level", {ImGui::GetContentRegionAvail().x, NULL})) {
                auto pl = PlayLayer::get();
                if (pl) pl->resetLevel();
            }

            if (ImGuiH::Button("Practice Mode", {ImGui::GetContentRegionAvail().x, NULL})) {
                auto pl = PlayLayer::get();
                if (pl) pl->togglePracticeMode(!pl->m_isPracticeMode);
            }

            if (ImGuiH::Button("Reset Volume", {ImGui::GetContentRegionAvail().x, NULL})) {
                auto fmod_engine = FMODAudioEngine::sharedEngine();
                fmod_engine->setBackgroundMusicVolume(0.5f);
                fmod_engine->setEffectsVolume(0.5f);
            }

            if (ImGuiH::Button("Uncomplete Level", {ImGui::GetContentRegionAvail().x, NULL})) {

            }

            if (ImGuiH::Button("Inject DLL", {ImGui::GetContentRegionAvail().x, NULL})) {

            }

            if (ImGuiH::Button("Resources", {ImGui::GetContentRegionAvail().x/2, NULL})) {

            }
            ImGui::SameLine();
            if (ImGuiH::Button("AppData", {ImGui::GetContentRegionAvail().x, NULL})) {

            }
        }
        else {
            for (auto& hck : win.hacks) {
                bool enabled = config.get(hck.config, false);

                std::string search_name = hck.name;
                std::string search_item = search_text;
                
                std::transform(search_item.begin(), search_item.end(), search_item.begin(), ::tolower);
                std::transform(search_name.begin(), search_name.end(), search_name.begin(), ::tolower);

                bool founded = search_item.empty() ? true : (search_name.find(search_item) != std::string::npos);
                ImGui::PushStyleColor(ImGuiCol_Text, founded ? ImGui::GetStyle().Colors[ImGuiCol_Text] : ImColor(64, 64, 64).Value);

                if (ImGuiH::Checkbox(hck.name.c_str(), &enabled, m_scale)) {
                    config.set(hck.config, enabled);
                    if (!hck.game_var.empty())
                        GameManager::get()->setGameVariable(hck.game_var.c_str(), enabled);
                    if (hck.handlerFunc) hck.handlerFunc(enabled);
                }

                if (ImGui::IsItemHovered() && !hck.desc.empty()) {
                    ImGui::SetTooltip("%s", hck.desc.c_str());
                }

                if (hck.handlerCustomWindow) {
                    ImGui::SameLine();
                    if (ImGuiH::ArrowButton(fmt::format("{} Settings", hck.name).c_str(), ImGuiDir_Right)) {
                        ImGui::OpenPopup(fmt::format("{} Settings", hck.name).c_str());
                    }

                    if (ImGui::BeginPopupModal(fmt::format("{} Settings", hck.name).c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                        hck.handlerCustomWindow();

                        if (ImGuiH::Button("Close", {400 * m_scale, NULL})) {
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                }
                
                ImGui::PopStyleColor();
            }
        }

        ImGui::PopFont();
        ImGui::End();
    }
}

void Gui::Init() {
    auto &config = Config::get();

    stretchedWindows.clear();
    ImGuiH::Popup::get().messages.clear();

    m_scale = config.get<float>("gui_scale", 1.f);
    m_index_scale = config.get<int>("gui_index_scale", 7);

    ApplyGuiColors(config.get("gui_inverted", false));
    ApplyColor(themes[config.get<int>("gui_color_index", 0)]);
    ApplyStyle(m_scale);
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = NULL;
    io.Fonts->Clear();
    io.Fonts->AddFontFromMemoryCompressedTTF(roboto_font_data, roboto_font_size, 18.f * m_scale, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    io.Fonts->AddFontFromMemoryCompressedTTF(roboto_font_data, roboto_font_size, 32.f * m_scale, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    io.Fonts->AddFontFromMemoryCompressedTTF(roboto_font_data, roboto_font_size, 20.f * m_scale, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    io.Fonts->Build();
}

void Gui::Toggle() {
    if (!isAnimating)
    {
        isAnimating = true;
        isFadingIn = !isFadingIn;
        animationStartTime = std::chrono::steady_clock::now();

        if (isFadingIn)
        {
            m_show = !m_show;
            updateCursorState();        
        }
    }
}