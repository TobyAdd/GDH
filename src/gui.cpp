#include "gui.hpp"
#include <imgui/imgui_stdlib.h>
#include <font.hpp>
#include "config.hpp"
#include "json.hpp"
#include "hacks.hpp"
#include "memory.hpp"
#include "labels.hpp"
#include "replayEngine.hpp"
#include "recorder.hpp"
#include "utils.hpp"
#ifdef GEODE_IS_WINDOWS
#include <subprocess.hpp>
#include "keyMapping.hpp"
#endif

std::chrono::steady_clock::time_point animationStartTime;
bool isAnimating = false;
bool isFadingIn = false;

std::string search_text;

int selected_label_corner = 0;
int selected_label_type = 0;
std::string selected_label_text;

void Gui::License() {
    auto& config = Config::get();
    if (!m_license_inited) {
        ImGui::SetNextWindowSize({590 * m_scale, 390 * m_scale});
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));  
        m_license_inited = true;
    }    

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {20 * m_scale, 20 * m_scale});
    ImGui::Begin("Welcome", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_Button]);
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::Text("New era of GDH :)");
    ImGui::PopFont();
    ImGui::PopStyleColor();

    ImGui::Text("GDH is an open-source mod menu under the MIT license");

    ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 128, 128).Value);
    ImGui::TextWrapped("Note: GDH is currently in beta testing, so some elements may be unstable/unfinished\n\nIn case of any issues/crashes, please report the problem in the GDH issues section on GitHub");
    ImGui::PopStyleColor();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {10 * m_scale, 10 * m_scale});
    ImGui::BeginChild("##LicenseChild", {0, ImGui::GetContentRegionAvail().y - 40 * m_scale}, true);
    ImGui::Text("Customize a comfortable size for GDH:");

    const char* items[] = {"25%", "50%", "75%", "80%", "85%", "90%", "95%", "100%", "125%", "150%", "175%", "200%", "250%", "300%", "400%"};
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    if (ImGuiH::Combo("##Menu scale", &m_index_scale, items, IM_ARRAYSIZE(items))) {
        m_scale = float(atof(items[m_index_scale])) / 100.0f;    
        config.set<float>("gui_scale", m_scale);
        config.set<int>("gui_index_scale", m_index_scale);
        m_needRescale = true;
        ImGuiCocos::get().reload();
    }

    ImGui::Text("Pick your favorite accent color:");
    
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

    ImGui::Text("Menu Key:");

    #ifdef GEODE_IS_WINDOWS

    renderKeyButton("Menu Key: ", m_toggleKey, true);

    #endif

    ImGui::EndChild();
    ImGui::PopStyleVar();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGuiH::Button("Processed", {ImGui::GetContentRegionAvail().x, 30 * m_scale})) {
        config.set<bool>("license_accepted", true);
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

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

    float time = Config::get().get<int>("gui_anim_durr", 100) / 1000.0f;
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
            Hacks::get().saveKeybinds();
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
    ImGuiH::Popup::get().render();

    ImGui::PopStyleVar();
    
    if (isAnimating) {
        animateAlpha();
    }
    
    if (!m_show) return;

    auto &config = Config::get();
    if (!config.get<bool>("license_accepted", false)) {
        License();
        return;
    }
    
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

            #ifdef GEODE_IS_WINDOWS

            if (ImGui::BeginPopupModal("Keybinds Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::BeginChild("##Keybinds", {420.f * m_scale, 400.f * m_scale});

                bool useKeybindsOnlyInGame = config.get<bool>("use_keybinds_only_in_game", true);
                if (ImGuiH::Checkbox("Use keybinds only in game", &useKeybindsOnlyInGame, m_scale)) {
                    config.set<bool>("use_keybinds_only_in_game", useKeybindsOnlyInGame);
                }

                ImGui::Text("UI");
                ImGui::Separator();
                ImGui::Spacing();

                renderKeyButton("Menu Key: ", m_toggleKey);

                ImGui::Text("Framerate");
                ImGui::Separator();
                ImGui::Spacing();

                renderKeyButton("Speedhack Key: ", m_speedhackKey);
                renderKeyButton("TPS Bypass Key: ", m_tpsKey);

                ImGui::Text("Replay Engine");
                ImGui::Separator();
                ImGui::Spacing();

                renderKeyButton("Disable/Playback Macro: ", m_playbackKey);
                renderKeyButton("Save Macro by Current Level Name: ", m_saveMacroByCurrentNameKey);
                renderKeyButton("Load Macro by Current Level Name: ", m_loadMacroByCurrentNameKey);
                renderKeyButton("Enable Frame Advance + Next Frame: ", m_frameAdvanceEnableKey);
                renderKeyButton("Disable Frame Advance: ", m_frameAdvanceDisableKey);

                ImGui::Text("Shortcuts");
                ImGui::Separator();
                ImGui::Spacing();

                renderKeyButton("Options: ", m_optionsKey);
                renderKeyButton("Reset Level: ", m_resetLevelKey);
                renderKeyButton("Practice Mode: ", m_practiceModeKey);
                renderKeyButton("Reset Volume: ", m_resetVolumeKey);
                renderKeyButton("Uncomplete Level: ", m_uncompleteLevelKey);

                ImGui::Text("Startpos Switcher");
                ImGui::Separator();
                ImGui::Spacing();

                renderKeyButton("Startpos Switcher Left: ", m_startposSwitcherLeftKey);
                renderKeyButton("Startpos Switcher Right: ", m_startposSwitcherRightKey);

                ImGui::EndChild();

                ImGui::Separator();

                ImGui::Text("Tip: To disable the bind, bind it to backspace");
                
                if (ImGuiH::Button("Close", {420 * m_scale, NULL}))
                    ImGui::CloseCurrentPopup();

                ImGui::EndPopup();
            }

            ImGuiH::Checkbox("Keybinds Mode", &m_keybindMode, m_scale);

            if (m_keybindMode && ImGui::IsItemHovered())
                ImGui::SetTooltip("Binds for toggle UI, Speedhack, Replay Engine, and more in the More Keybinds");

            #endif

            if (ImGuiH::Button(m_keybindMode ? "More Keybinds" : "More Settings", {ImGui::GetContentRegionAvail().x, 0})) {
                if (!m_keybindMode)
                    ImGui::OpenPopup("GDH More Settings");
                else
                    ImGui::OpenPopup("Keybinds Settings");
            }

            if (ImGui::BeginPopupModal("GDH More Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                int anim_durr = config.get<int>("gui_anim_durr", 100);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                if (ImGui::DragInt("##gui_anim_durr", &anim_durr, 1, 0, 10000, "Transition Time: %ims")) {
                    config.set<int>("gui_anim_durr", anim_durr);
                }

                if (ImGuiH::Button("Sort Windows", {ImGui::GetContentRegionAvail().x, NULL}))
                    stretchedWindows.clear();

                if (ImGuiH::Button("Close", {400 * m_scale, 0})) {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            #ifdef GEODE_IS_WINDOWS

            static int priorityIndex = 3;
            const char* priorityNames[] = {"Real-time", "High", "Above Normal", "Normal", "Below Normal", "Low"};
            DWORD priorityValues[] = {REALTIME_PRIORITY_CLASS, HIGH_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, 
                                      NORMAL_PRIORITY_CLASS, BELOW_NORMAL_PRIORITY_CLASS, IDLE_PRIORITY_CLASS};

            ImGui::Text("Process Priority");
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGuiH::Combo("##Process Priority", &priorityIndex, priorityNames, IM_ARRAYSIZE(priorityNames))) {
                if (SetPriorityClass(GetCurrentProcess(), priorityValues[priorityIndex])) {
                    ImGuiH::Popup::get().add_popup(fmt::format("Priority changed to: {}", priorityNames[priorityIndex]));
                } else {
                    ImGuiH::Popup::get().add_popup("Failed to change priority!");
                }
            }
            
            #endif

        }
        else if (windowName == "Framerate") {
            bool tps_enabled = config.get<bool>("tps_enabled", false);
            float tps_value = config.get<float>("tps_value", 60.f);;

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - (35 + 5) * m_scale);
            if (ImGui::DragFloat("##tps_value", &tps_value, 1, 1, FLT_MAX, "%0.f TPS"))
                config.set<float>("tps_value", tps_value);

            ImGui::SameLine();
            if (ImGuiH::Checkbox("##tps_enabled", &tps_enabled, m_scale))
                config.set<bool>("tps_enabled", tps_enabled);

            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Multiplies the number of ticks per second, used mainly for botting\n(not recommended for normal use as it ruins the game's performance)");
            
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

            ImGui::SameLine();

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.f * m_scale);
            if (ImGuiH::ArrowButton("##more_tps_settings", ImGuiDir_Down)) {
                ImGui::OpenPopup("More TPS Settings");
            }

            if (ImGui::BeginPopup("More TPS Settings")) {
                bool tps_real_time = config.get<bool>("tps::real_time", true);
                if (ImGuiH::Checkbox("Real Time", &tps_real_time, m_scale)) 
                    config.set<bool>("tps::real_time", tps_real_time);

                int resumeTimer = config.get<int>("resumeTimer_value", 2);
                if (ImGui::DragInt("##ResumerTimer", &resumeTimer, 1, 2, INT_MAX, "Resume Timer: %d"))
                    config.set<int>("resumeTimer_value", resumeTimer);    

                ImGui::Text("Higher resume timer means a smoother start but a delay (adjust as needed)");

                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Allows real-time update of multiplied ticks (may cause lags)");
            }
        }
        else if (windowName == "Variables") {
            static int type_index = 0;
            static int player_index = 0;
            static int creator_index = 0;
            static std::string value;

            const char* types[] = {"Creator", "Player"};
            const char* player[] = {"Attempts", "Jumps", "Normal %", "Position X", "Position Y", "Practice %", "Song ID", "Speed", "Level ID"};
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
                    case 8: // Level ID
                        if (isSet) level.m_levelID = std::stoi(value);
                        else value = std::to_string(level.m_levelID);
                        break;
                }
            };

            if (ImGuiH::Button("Get", {ImGui::GetContentRegionAvail().x / 2, NULL})) {
                if (type_index == 0) handleGetSetCreator(false);
                else if (type_index == 1) handleGetSetPlayer(false);
            }

            ImGui::SameLine();

            if (ImGuiH::Button("Set", {ImGui::GetContentRegionAvail().x, NULL})) {
                if (utilsH::isNumeric(value)) {
                    if (type_index == 0) handleGetSetCreator(true);
                    else if (type_index == 1) handleGetSetPlayer(true);
                }
            }
        }
        else if (windowName == "Replay Engine") {
            static geode::Mod* cbfMod = geode::Loader::get()->getLoadedMod("syzzi.click_between_frames");
            static bool hasCBF = cbfMod != nullptr && cbfMod->isEnabled();

            if (hasCBF && !cbfMod->getSettingValue<bool>("soft-toggle")) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 128, 128).Value);
                ImGui::TextWrapped("Click Between Frames mod is enabled, turn it off to use Replay Engine");
                ImGui::PopStyleColor();
            }
            else {
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

                    if (ImGuiH::Button("Open Folder", {ImGui::GetContentRegionAvail().x, NULL})) {
                        geode::utils::file::openFolder(folderMacroPath);
                    }
                    
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
                    bool canRecord = (!engine.engine_v2 && config.get<bool>("tps_enabled", false)) || (engine.engine_v2 && !config.get<bool>("tps_enabled", false));
                    
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
                        ImGuiH::Popup::get().add_popup(engine.engine_v2 ? "Disable TPS Bypass to record the replay" : "Enable TPS Bypass to record the replay");
                    }
                }

                
                ImGui::SameLine();

                if (ImGuiH::RadioButton("Play", &mode_, 2, m_scale)) {
                    bool canPlay = (!engine.engine_v2 && config.get<bool>("tps_enabled", false)) || (engine.engine_v2 && !config.get<bool>("tps_enabled", false));
                    
                    if (canPlay) {
                        engine.mode = state::play;
                    } else {
                        engine.mode = state::disable;
                        ImGuiH::Popup::get().add_popup(engine.engine_v2 ? "Disable TPS Bypass to playback the replay" : "Enable TPS Bypass to playback the replay");
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
                        if (!engine.engine_v2 ? (ext == ".re" || ext == ".re3") : (ext == ".re21" || ext == ".re2")) {
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

                if (ImGui::IsItemClicked(1) && ImGui::IsItemHovered()) {
                    ImGui::OpenPopup("P1/P2 Load");
                }

                if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                    ImGui::SetTooltip("Right click to load P1/P2 macros");
                }

                if (ImGui::BeginPopup("P1/P2 Load")) {
                    if (ImGuiH::Button("Load P1")) {
                        ImGuiH::Popup::get().add_popup(engine.load(engine.replay_name, true, false));
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::SameLine();

                    if (ImGuiH::Button("Load P2")) {
                        ImGuiH::Popup::get().add_popup(engine.load(engine.replay_name, false, true));
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }         


                ImGui::SameLine();

                if (ImGuiH::Button("Clear", {ImGui::GetContentRegionAvail().x, NULL})) {
                    engine.clear();
                    ImGuiH::Popup::get().add_popup("Replay has been cleared");
                }

                ImGui::Text("Replay Size: %i/%zu", engine.get_current_index(), engine.get_actions_size());
                ImGui::Text("Frame: %i", engine.get_frame());

                ImGui::Separator();

                #ifdef GEODE_IS_WINDOWS
                auto& recorder = Recorder::get();
                auto& recorderAudio = RecorderAudio::get();

                if (recorder.settings_openned) {
                    static bool first_time = true;
                    if (first_time) {
                        first_time = false;
                        ImGui::SetNextWindowSize({800 * m_scale, 540 * m_scale});
                    }                
                }

                if (ImGui::BeginPopupModal("Replay Engine More Settings", &recorder.settings_openned) && ImGui::BeginTabBar("Engine Tabs")) {
                    auto containsRussianLetters = [](const std::filesystem::path& p) -> bool {
                        auto pathStr = p.u8string();
                        for (char c : pathStr) {
                            if ((unsigned char)c >= 0xD0 && (unsigned char)c <= 0xD1) {
                                return true;
                            }
                        }
                        return false;
                    };
                    
                    if (ImGui::BeginTabItem("Settings")) {
                        if (ImGuiH::Checkbox("Engine v2.1 (Beta)", &engine.engine_v2, m_scale))
                            config.set<bool>("engine::v2", engine.engine_v2);

                        if (!engine.engine_v2) {
                            ImGuiH::Checkbox("Accuracy Fix", &engine.accuracy_fix, m_scale);
                            ImGui::SameLine();
                            ImGuiH::Checkbox("Rotation Fix", &engine.rotation_fix, m_scale);
                        }

                        bool practice_fix = config.get<bool>("practice_fix", false);
                        if (ImGuiH::Checkbox("Practice Fix", &practice_fix, m_scale))
                            config.set<bool>("practice_fix", practice_fix);

                        ImGui::EndTabItem();
                    }	

                    if (ImGui::BeginTabItem("Recorder")) {
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
                                    bool fps_enabled = config.get<bool>("tps_enabled", false);
                                    bool check = recorder.fps <= config.get<float>("tps_value", 60.f);
                                    bool check2 = recorder.fps >= 60 && recorder.fps <= 240;

                                    if (engine.engine_v2 ? (!fps_enabled && check2) : (fps_enabled && check)) {
                                        if (recorder.enabled) {
                                            if (recorder.overlay_mode) {
                                                auto size = ImGui::GetIO().DisplaySize;
                                                recorder.width = static_cast<int>(size.x);
                                                recorder.height = static_cast<int>(size.y);
                                            }

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
                                        if (engine.engine_v2) {
                                            if (fps_enabled)
                                                ImGuiH::Popup::get().add_popup("Disable TPS Bypass to start render");
                                            
                                            if (!check2)
                                                ImGuiH::Popup::get().add_popup("Recorder FPS values must be within the range 60 to 240");
                                        }
                                        else {
                                            recorder.enabled = false;
                                            if (!fps_enabled)
                                                ImGuiH::Popup::get().add_popup("Enable TPS Bypass to start render");
                                            
                                            if (!check)
                                                ImGuiH::Popup::get().add_popup("Recorder FPS should not be more than the current TPS");
                                        }
                                        
                                    }
                                }
                            }

                            ImGui::SameLine();

                            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                            ImGui::InputText("##videoname", &recorder.video_name);

                            bool use_flvc = recorder.video_name.ends_with(".flvc");

                            if (use_flvc) {
                                ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 128, 128).Value);
                                ImGui::Text("You are trying to encode a video in Free Lossless Video Codec (FLVC)");
                                ImGui::TextWrapped("- Make sure you have enough disk space: a 1280x720 60 FPS video of 10 seconds takes more than 100 MB (even more at higher resolutions)");
                                ImGui::TextWrapped("- Recommended to record the showcase on HDD (Large file volumes can compromise SSD integrity due to write cycle limitations)");
                                ImGui::PopStyleColor();
                            }


                            if (!use_flvc) {
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
                                ImGui::Spacing();
                            }

                            if (ImGuiH::Checkbox("Overlay Mode", &recorder.overlay_mode, m_scale)) {
                                bool isFullscreen = !GameManager::sharedState()->getGameVariable("0025");
                                if (!isFullscreen) {
                                    recorder.overlay_mode = false;
                                    ImGuiH::Popup::get().add_popup("Please switch to fullscreen to record in overlay mode");
                                }
                            }

                            if (recorder.overlay_mode) {
                                ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 128, 128).Value);
                                ImGui::TextWrapped("Overlay Mode allows you to capture all overlays (like steam or reshade)");
                                ImGui::TextWrapped("In this mode, you can only record in full screen mode (native screen resolution)");
                                ImGui::TextWrapped("- Don't open other overlays like GDH, Steam, Reshade to make a good showcase");
                                ImGui::TextWrapped("- Don't pause while recording the showcase, a frame of the pause menu may be visible");
                                ImGui::PopStyleColor();
                            }


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

                            if (!use_flvc) {                            
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

                            
                            if (ImGuiH::Button("Save")) {
                                nlohmann::json j;
                                j["width"] = recorder.width;
                                j["height"] = recorder.height;
                                j["fps"] = recorder.fps;
                                j["bitrate"] = recorder.bitrate;
                                j["codec"] = recorder.codec;
                                j["extra_args"] = recorder.extra_args;
                                j["vf_args"] = recorder.vf_args;

                                std::ofstream file(folderPath / "recorder_settings.json");
                                if (file.is_open()) {
                                    file << j.dump(4);                            
                                    file.close();

                                    ImGuiH::Popup::get().add_popup("Configuration saved");
                                } else {
                                    ImGuiH::Popup::get().add_popup("Could not open file for writing");
                                }
                            }

                            ImGui::SameLine();

                            if (ImGuiH::Button("Load")) {
                                std::ifstream file(folderPath / "recorder_settings.json");
                                if (file.is_open()) {
                                    std::string file_contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

                                    nlohmann::json j = nlohmann::json::parse(file_contents, nullptr, false);
                                    if (j.is_discarded()) {
                                        ImGuiH::Popup::get().add_popup("JSON parsing error or invalid file");
                                    } else {
                                        recorder.width = j.value("width", recorder.width);
                                        recorder.height = j.value("height", recorder.height);
                                        recorder.fps = j.value("fps", recorder.fps);
                                        recorder.bitrate = j.value("bitrate", recorder.bitrate);
                                        recorder.codec = j.value("codec", recorder.codec);
                                        recorder.extra_args = j.value("extra_args", recorder.extra_args);
                                        recorder.vf_args = j.value("vf_args", recorder.vf_args);

                                        ImGuiH::Popup::get().add_popup("Configuration loaded");
                                    }
                                    file.close();
                                } else {
                                    ImGuiH::Popup::get().add_popup("Could not open file for reading");
                                }
                            }

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

                            if (!use_flvc) {                                
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
                                if (ImGuiH::Button("NVIDIA H264"))
                                {
                                    recorder.codec = "h264_nvenc";
                                    recorder.extra_args = "-pix_fmt yuv420p -preset p7";
                                }
                                ImGui::SameLine();
                                if (ImGuiH::Button("NVIDIA H265"))
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
                                
                                if (ImGuiH::Button("AMD H264 Lossless"))
                                {
                                    recorder.codec = "h264_amf";
                                    recorder.extra_args = "-pix_fmt yuv420p -rc cqp -qp_i 0 -qp_p 0 -qp_b 0";
                                }
                                ImGui::SameLine();
                                if (ImGuiH::Button("AMD H265 Lossless"))
                                {
                                    recorder.codec = "hevc_amf";
                                    recorder.extra_args = "-pix_fmt yuv420p -rc cqp -qp_i 0 -qp_p 0 -qp_b 0";
                                }
                                if (ImGuiH::Button("Color Fix"))
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
                        if (ImGuiH::Checkbox("Record Buffer", &recorderAudio.enabled, m_scale)) {
                            if (containsRussianLetters(recorder.folderShowcasesPath)) {
                                recorder.enabled = false;
                                ImGuiH::Popup::get().add_popup("Invalid path to the showcase folder. Please remove any Cyrillic characters");
                            }
                            else {
                                if (recorderAudio.enabled) {
                                    if (!recorderAudio.showcase_mode) {
                                        recorderAudio.start();
                                        ImGuiH::Popup::get().add_popup("Audio recording started!");
                                    }                                        
                                    else {
                                        ImGuiH::Popup::get().add_popup("Rendering begins when you re-enter the level (don't forget to set up the macro playback!!)");
                                        recorderAudio.first_start = true;
                                    }                                        
                                }
                                else {
                                    recorderAudio.stop();
                                    ImGuiH::Popup::get().add_popup("Audio recording stopped!");
                                }                                    
                            }
                        }

                        ImGuiH::Checkbox("Showcase Mode", &recorderAudio.showcase_mode, m_scale);

                        ImGui::Spacing();

                        ImGui::Text("Level Settings");
                        ImGui::Separator();

                        ImGui::PushItemWidth(120 * m_scale);
                        ImGui::SliderFloat("##MusicRecorder", &recorderAudio.volume_music, 0.f, 1.f, "Music: %.2f");

                        ImGui::SameLine();
                        
                        ImGui::PushItemWidth(120 * m_scale);
                        
                        ImGui::SliderFloat("##SFXRecorder", &recorderAudio.volume_sfx, 0.f, 1.f, "SFX: %.2f");

                        if (recorderAudio.showcase_mode) {
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
                    
                    if (ImGui::BeginTabItem("Merge (Video + Audio)")) {
                        static bool shortest = true;
                        static std::vector<std::filesystem::path> videos;
                        static std::vector<std::filesystem::path> audios;
                        static int index_videos = 0;
                        static int index_audios = 0;
                        static std::string args = "-c:v copy -c:a libfdk_aac -vbr 5 -shortest";

                        auto compile = [&]()
                        {
                            args = "-c:v copy -c:a libfdk_aac -vbr 5";
                            if (shortest) {
                                args += " -shortest";
                            }
                        };

                        ImGui::BeginChild("##VideoSelect", {NULL, 190 * m_scale}, true);
                        for (size_t i = 0; i < videos.size(); i++) {
                            bool is_selected = (index_videos == i);
                            if (ImGui::Selectable(videos[i].filename().string().c_str(), is_selected)) {
                                index_videos = i;
                            }
                        }
                        ImGui::EndChild();

                        ImGui::BeginChild("##AudioSelect", {NULL, 190 * m_scale}, true);
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

                                if (entry.path().extension() == ".wav" ||
                                    entry.path().extension() == ".mp3" ||
                                    entry.path().extension() == ".m4a") {
                                    audios.push_back(entry.path().string());
                                }
                            }
                        }

                        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                        ImGui::InputText("##merge_command", &args);

                        if (ImGuiH::Checkbox("Shortest", &shortest, m_scale)) 
                            compile();
                        
                        ImGui::SameLine();

                        if (ImGuiH::Button("Merge", {ImGui::GetContentRegionAvail().x, NULL})) {
                            if (videos.empty() || audios.empty()) {

                            } else if (index_videos >= 0 && index_videos < (int)videos.size() && index_audios >= 0 && index_audios < (int)audios.size()) {
                                std::string command = "ffmpeg.exe -y -i \"" + videos[index_videos].string() + "\" -i \"" + audios[index_audios].string() + "\" " + args + " ";
                                std::filesystem::path video_rel(videos[index_videos]);
                                command += fmt::format("\"{}\\audio_{}\"", recorder.folderShowcasesPath, video_rel.filename().string());
                                geode::log::debug("{}", command);
                                auto process = subprocess::Popen(command);
                            }
                        }
                        ImGui::EndTabItem();
                    }	

                    ImGui::EndTabBar();
                    ImGui::EndPopup();
                }                            
                                

                if (ImGui::MenuItem("More Settings")) {
                    recorder.settings_openned = true;
                    recorder.ffmpeg_installed = std::filesystem::exists("ffmpeg.exe");
                    ImGui::OpenPopup("Replay Engine More Settings");
                }
                #endif
            }           
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
                "Cheat Indicator",
                "FPS Counter",
                "Level Progress",
                "Attempt",
                "CPS Counter",
                "Level Info",
                "Noclip Accuracy",
                "Death Counter",
                "Startpos Switcher",
                "Testmode",
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
                else if (selected_label_type == 3) {
                    text = "{cheat_indicator}";
                    config.set<bool>("cheat_indicator", true);
                }
                else if (selected_label_type == 4) text = "{fps}";
                else if (selected_label_type == 5) text = "{progress:2f}";
                else if (selected_label_type == 6) text = "Attempt {attempt}";
                else if (selected_label_type == 7) text = "ColoredCPS({cps}/{cpsHigh}/{clicks})";
                else if (selected_label_type == 8) text = "{levelName}{byLevelCreator} ({levelId})";
                else if (selected_label_type == 9) text = "ColoredDeath({noclipAccuracy})";
                else if (selected_label_type == 10) text = "ColoredDeath({deaths} Deaths)";
                else if (selected_label_type == 11) text = "{startposCurrentIDX}/{startposAllIDX}";
                else if (selected_label_type == 12) text = "{testmode}";
                
                Label l((LabelCorner) (selected_label_corner+1), text);
                labels.add(l);
            }
            
            ImGui::Separator();
            ImGui::BeginChild("Labels");
            ImGui::Spacing();

            if (!std::any_of(labels.labels.begin(), labels.labels.end(), 
                            [&](const Label& label) { return static_cast<int>(label.corner - 1) == selected_label_corner; })) {
                ImGui::TextDisabled("No labels in this corner");
            }
                        
            for (size_t index = 0; index < labels.labels.size(); index++) {
                Label& item = labels.labels[index];

                if (static_cast<int>(item.corner-1) != selected_label_corner) continue;
                
                ImGui::PushID(index);

                ImGui::Selectable("  =", false, 0, {20.f * m_scale, 20.f * m_scale});

                if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                    ImGui::SetTooltip("Double click to remove label");
                }

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
                utilsH::UncompleteLevel();
            }

            if (ImGuiH::Button("Inject DLL", {ImGui::GetContentRegionAvail().x, NULL})) {
                #ifdef GEODE_IS_WINDOWS
                geode::utils::file::FilePickOptions::Filter filter = {
                    .description = "Dynamic Link Library",
                    .files = { "*.dll"}
                };

                auto result = geode::utils::file::pick(geode::utils::file::PickMode::OpenFile, {std::nullopt, {filter}});
                if (result.isFinished() && !result.getFinishedValue()->isErr()) {
                    std::filesystem::path path = result.getFinishedValue()->unwrap();
                    HMODULE hModule = LoadLibraryW(path.wstring().c_str());
                    if (hModule) {
                        ImGuiH::Popup::get().add_popup("Injected successfully");
                    }
                    else {
                        ImGuiH::Popup::get().add_popup("Failed to inject");
                    }
                }
                #endif
            }

            if (ImGuiH::Button("Resources", {ImGui::GetContentRegionAvail().x/2, NULL})) {
                std::string path = cocos2d::CCFileUtils::get()->getWritablePath2();
                geode::utils::file::openFolder(path);
            }
            ImGui::SameLine();
            if (ImGuiH::Button("AppData", {ImGui::GetContentRegionAvail().x, NULL})) {
                auto path = geode::dirs::getSaveDir();
                geode::utils::file::openFolder(path);
            }

            if (ImGuiH::Button("GDH AppData", {ImGui::GetContentRegionAvail().x, NULL})) {
                geode::utils::file::openFolder(folderPath);
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


                if (m_keybindMode) {
                    #ifdef GEODE_IS_WINDOWS
                    if (hck.keybind == -1 && !m_waitingForBindKey && m_keyToSet != -1) {
                        hck.keybind = m_keyToSet;
                        m_keyToSet = 0;
                    }

                    if (ImGuiH::Button(hck.keybind != -1 
                        ? fmt::format("{}: {}", 
                            hck.name.length() > 16 ? hck.name.substr(0, 16) : hck.name,
                            KeyMappingUtils::GetNameFromGLFW(hck.keybind)).c_str() 
                        : "Press any key...", {ImGui::GetContentRegionAvail().x, NULL})) 
                    {
                        hck.keybind = -1;
                        m_waitingForBindKey = true;
                    }
                    #endif
                }
                else {
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
                    }
                }

                if (hck.handlerCustomWindow) {
                    if (ImGui::BeginPopupModal(fmt::format("{} Settings", hck.name).c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                        hck.handlerCustomWindow();

                        if (ImGuiH::Button("Close", {400 * m_scale, NULL}))
                            ImGui::CloseCurrentPopup();

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

    m_license_inited = false;

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


void Gui::renderKeyButton(const std::string& label, int& key, bool withoutKeybindsMode) {
    #ifdef GEODE_IS_WINDOWS
    std::string keyStr = label + KeyMappingUtils::GetNameFromGLFW(key);
    if (key == -1) {
        keyStr = "Press any key...";
        if (!m_waitingForBindKey && m_keyToSet != -1) {
            key = m_keyToSet;
            m_keyToSet = 0;
            if (withoutKeybindsMode) m_keybindMode = false;
        }
    }

    if (ImGuiH::Button(keyStr.c_str(), {ImGui::GetContentRegionAvail().x, 0})) {
        key = -1;
        if (withoutKeybindsMode) m_keybindMode = true;
        m_waitingForBindKey = true;
    }
    #endif
};
