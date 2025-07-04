#include "gui.hpp"
#include <imgui/imgui_stdlib.h>
#include <font.hpp>
#include "config.hpp"
#include "json.hpp"
#include "hacks.hpp"
#include "memory.hpp"
#include "labels.hpp"
#include "replayEngine.hpp"
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
        ImGui::GetIO().Inverted = !inverted;
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
            search_text = "";
            settings_openned = false;
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
                ImGui::GetIO().Inverted = !inverted;
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
            if (ImGuiH::Checkbox("##tps_enabled", &tps_enabled, m_scale)) {
                config.set<bool>("tps_enabled", tps_enabled);
                hacks.TPSBypass_Init(tps_enabled);
            }                
            
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
                int resumeTimer = config.get<int>("resumeTimer_value", 2);
                if (ImGui::DragInt("##ResumerTimer", &resumeTimer, 1, 2, INT_MAX, "Resume Timer: %d"))
                    config.set<int>("resumeTimer_value", resumeTimer);    

                ImGui::Text("Higher resume timer means a smoother start but a delay (adjust as needed)");
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
                ImGui::InputTextWithHint("##replay_name", "Enter the macro name", &engine.replay_name);

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

                if (settings_openned) {
                    if (first_time_re_settings) {
                        first_time_re_settings = false;
                        ImGui::SetNextWindowSize({800 * m_scale, 540 * m_scale});
                    }                
                }

                if (ImGui::BeginPopupModal("Replay Engine More Settings", &settings_openned) && ImGui::BeginTabBar("Engine Tabs")) {
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

                    if (ImGui::BeginTabItem("Editor")) {
                        static int selected_index = 0;
                        static int selected_frame_type = 1;
                        static int selected_player = 0;
                        static int prev_frame_type = selected_frame_type;
                        static int prev_player = selected_player;
                    
                        const char* frame_types[] = { "Physic Frames", "Input Frames" };
                        const char* player_labels[] = { "Player 1", "Player 2" };
                    
                        bool isPlayer1 = (selected_player == 0);
                        
                        if (prev_frame_type != selected_frame_type || prev_player != selected_player) {
                            selected_index = 0;
                            prev_frame_type = selected_frame_type;
                            prev_player = selected_player;
                        }
                    
                        if (selected_frame_type == 0) {
                            auto& frames = engine.getPhysicFrames(isPlayer1);
                            
                            if (frames.empty()) {
                                selected_index = -1;
                            } else if (selected_index >= frames.size()) {
                                selected_index = frames.size() - 1;
                            }
                    
                            if (ImGui::BeginTable("PhysicTable", 5, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY,
                                ImVec2(ImGui::GetContentRegionAvail().x - 200 * m_scale, 0))) {
                    
                                ImGui::TableSetupColumn("Frame");
                                ImGui::TableSetupColumn("X");
                                ImGui::TableSetupColumn("Y");
                                ImGui::TableSetupColumn("Rotation");
                                ImGui::TableSetupColumn("Y Accel");
                                ImGui::TableHeadersRow();
                    
                                ImGuiListClipper clipper;
                                clipper.Begin(frames.size());
                                while (clipper.Step()) {
                                    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
                                        const auto& frame = frames[i];
                                        ImGui::TableNextRow();
                    
                                        bool is_selected = (selected_index == i);
                                        ImGui::TableSetColumnIndex(0);
                                        std::string frame_str = std::to_string(frame.frame) + "##" + std::to_string(i);
                                        if (ImGui::Selectable(frame_str.c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns)) {
                                            selected_index = i;
                                        }
                    
                                        ImGui::TableSetColumnIndex(1); ImGui::Text("%f", frame.x);
                                        ImGui::TableSetColumnIndex(2); ImGui::Text("%f", frame.y);
                                        ImGui::TableSetColumnIndex(3); ImGui::Text("%f", frame.rotation);
                                        ImGui::TableSetColumnIndex(4); ImGui::Text("%f", frame.y_accel);
                                    }
                                }
                    
                                ImGui::EndTable();
                            }
                        } else {
                            auto& frames = engine.getInputFrames(isPlayer1);
                            
                            if (frames.empty()) {
                                selected_index = -1;
                            } else if (selected_index >= frames.size()) {
                                selected_index = frames.size() - 1;
                            }
                    
                            if (ImGui::BeginTable("InputTable", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY,
                                ImVec2(ImGui::GetContentRegionAvail().x - 200 * m_scale, 0))) {
                    
                                ImGui::TableSetupColumn("Frame");
                                ImGui::TableSetupColumn("Button");
                                ImGui::TableSetupColumn("Action");
                                ImGui::TableHeadersRow();
                    
                                ImGuiListClipper clipper;
                                clipper.Begin(frames.size());
                                while (clipper.Step()) {
                                    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
                                        const auto& frame = frames[i];
                                        ImGui::TableNextRow();
                    
                                        bool is_selected = (selected_index == i);
                                        ImGui::TableSetColumnIndex(0);

                                        std::string frame_str = std::to_string(frame.frame) + "##" + std::to_string(i);
                                        if (ImGui::Selectable(frame_str.c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns)) {
                                            selected_index = i;
                                        }
                    
                                        ImGui::TableSetColumnIndex(1); ImGui::Text((frame.button == 1) ? "Up" : (frame.button == 2) ? "Left" : 
                                            (frame.button == 3) ? "Right" : "Unknown");
                                        ImGui::TableSetColumnIndex(2); ImGui::Text("%s", frame.down ? "Push" : "Release");
                                    }
                                }
                    
                                ImGui::EndTable();
                            }
                        }
                    
                        ImGui::SameLine();
                    
                        if (ImGui::BeginChild("##EditorSettings", {0, ImGui::GetContentRegionAvail().y})) {
                            ImGui::Text("Frame Type:");
                            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                            ImGui::Combo("##Frame Type", &selected_frame_type, frame_types, IM_ARRAYSIZE(frame_types));
                    
                            ImGui::Text("Player:");
                            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                            ImGui::Combo("##Player", &selected_player, player_labels, IM_ARRAYSIZE(player_labels));
                            
                            ImGui::Separator();
                            ImGui::Text("Actions:");
                            
                            if (selected_frame_type == 0) {
                                auto& frames = engine.getPhysicFrames(isPlayer1);
                                
                                if (ImGuiH::Button("Add Action", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                                    replay_data new_frame;
                                    
                                    new_frame.x = 0.0f;
                                    new_frame.y = 0.0f;
                                    new_frame.rotation = 0.0f;
                                    new_frame.y_accel = 0.0f;
                                    new_frame.player = isPlayer1;
                                    
                                    if (!frames.empty() && selected_index >= 0 && selected_index < frames.size()) {
                                        new_frame.frame = frames[selected_index].frame + 1;
                                        
                                        frames.insert(frames.begin() + selected_index + 1, new_frame);
                                        selected_index++;
                                    } else {
                                        new_frame.frame = 0;
                                        frames.push_back(new_frame);
                                        selected_index = frames.size() - 1;
                                    }
                                }
                                
                                if (!frames.empty() && selected_index >= 0 && selected_index < frames.size()) {
                                    ImGui::BeginDisabled(frames.size() < 1);
                                    if (ImGuiH::Button("Remove Action", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                                        frames.erase(frames.begin() + selected_index);
                                        
                                        if (frames.empty()) {
                                            selected_index = -1;
                                        } else if (selected_index >= frames.size()) {
                                            selected_index = frames.size() - 1;
                                        }
                                    }
                                    ImGui::EndDisabled();
                                    
                                    ImGui::BeginDisabled(selected_index <= 0);
                                    if (ImGuiH::Button("Move Up", ImVec2((ImGui::GetContentRegionAvail().x / 2.0f) - 2.0f, 0))) {
                                        if (selected_index > 0) {
                                            std::swap(frames[selected_index], frames[selected_index - 1]);
                                            selected_index--;
                                        }
                                    }
                                    ImGui::EndDisabled();
                                    
                                    ImGui::SameLine();
                                    
                                    ImGui::BeginDisabled(selected_index >= frames.size() - 1);
                                    if (ImGuiH::Button("Move Down", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                                        if (selected_index < frames.size() - 1) {
                                            std::swap(frames[selected_index], frames[selected_index + 1]);
                                            selected_index++;
                                        }
                                    }
                                    ImGui::EndDisabled();
                                    
                                    ImGui::Separator();
                                }
                                
                                if (!frames.empty() && selected_index >= 0 && selected_index < frames.size()) {
                                    auto& selected_frame = frames[selected_index];
                                                                
                                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                                    int frame_cast = static_cast<int>(selected_frame.frame);
                                    if (ImGui::DragInt("Frame##Drag", &frame_cast, 1.f, 0, INT_MAX, "Frame: %i")) {
                                        selected_frame.frame = static_cast<unsigned int>(frame_cast);
                                    }
                                
                                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                                    ImGui::DragFloat("X##Drag", &selected_frame.x, 0.001f, -FLT_MAX, FLT_MAX, "X Position: %f");
                                
                                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                                    ImGui::DragFloat("Y##Drag", &selected_frame.y, 0.001f, -FLT_MAX, FLT_MAX, "Y Position: %f");
                                
                                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                                    ImGui::DragFloat("Rotation##Drag", &selected_frame.rotation, 0.001f, -FLT_MAX, FLT_MAX, "Rotation: %f");
                                
                                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                                    float yAccel_cast = static_cast<float>(selected_frame.y_accel);
                                    if (ImGui::DragFloat("YAccel##Drag", &yAccel_cast, 0.001f, -FLT_MAX, FLT_MAX, "Y Accel: %f")) {
                                        selected_frame.y_accel = static_cast<double>(yAccel_cast);
                                    }
                                } else {
                                    ImGui::TextColored(ImColor(255, 128, 128).Value, "No physic frames");
                                }
                            } else if (selected_frame_type == 1) {
                                auto& frames = engine.getInputFrames(isPlayer1);
                                
                                if (ImGuiH::Button("Add Action", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                                    replay_data2 new_frame;
                                    
                                    new_frame.button = 1;
                                    new_frame.down = false;
                                    new_frame.isPlayer1 = isPlayer1;
                                    
                                    if (!frames.empty() && selected_index >= 0 && selected_index < frames.size()) {
                                        new_frame.frame = frames[selected_index].frame + 1;
                                        
                                        frames.insert(frames.begin() + selected_index + 1, new_frame);
                                        selected_index++;
                                    } else {
                                        new_frame.frame = 0;
                                        frames.push_back(new_frame);
                                        selected_index = frames.size() - 1;
                                    }
                                }
                                
                                if (!frames.empty() && selected_index >= 0 && selected_index < frames.size()) {
                                    ImGui::BeginDisabled(frames.size() < 1);
                                    if (ImGuiH::Button("Remove Action", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                                        frames.erase(frames.begin() + selected_index);
                                        
                                        if (frames.empty()) {
                                            selected_index = -1;
                                        } else if (selected_index >= frames.size()) {
                                            selected_index = frames.size() - 1;
                                        }
                                    }
                                    ImGui::EndDisabled();
                                    
                                    ImGui::BeginDisabled(selected_index <= 0);
                                    if (ImGuiH::Button("Move Up", ImVec2((ImGui::GetContentRegionAvail().x / 2.0f) - 2.0f, 0))) {
                                        if (selected_index > 0) {
                                            std::swap(frames[selected_index], frames[selected_index - 1]);
                                            selected_index--;
                                        }
                                    }
                                    ImGui::EndDisabled();
                                    
                                    ImGui::SameLine();
                                    
                                    ImGui::BeginDisabled(selected_index >= frames.size() - 1);
                                    if (ImGuiH::Button("Move Down", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                                        if (selected_index < frames.size() - 1) {
                                            std::swap(frames[selected_index], frames[selected_index + 1]);
                                            selected_index++;
                                        }
                                    }
                                    ImGui::EndDisabled();
                                    
                                    ImGui::Separator();
                                }
                                
                                if (!frames.empty() && selected_index >= 0 && selected_index < frames.size()) {
                                    auto& selected_frame = frames[selected_index];
                                
                                    int frame_cast = static_cast<int>(selected_frame.frame);
                                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                                    if (ImGui::DragInt("Frame##Drag", &frame_cast, 1, 0, INT_MAX, "Frame: %i")) {
                                        selected_frame.frame = static_cast<unsigned int>(frame_cast);
                                    }

                                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                                    ImGui::DragInt("Button##Drag", &selected_frame.button, 1, 1, 3, "Button: %i");

                                    ImGuiH::Checkbox("Push", &selected_frame.down, m_scale);                                
                                } else {
                                    ImGui::TextColored(ImColor(255, 128, 128).Value, "No input frames");
                                }
                            }
                    
                            ImGui::EndChild();
                        }
                    
                        ImGui::EndTabItem();
                    }  
                    
                    ImGui::EndTabBar();
                    ImGui::EndPopup();
                }                            
                                

                if (ImGui::MenuItem("More Settings")) {
                    settings_openned = true;
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
                "Replay Engine State",
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
                else if (selected_label_type == 4) text = "{fps} FPS";
                else if (selected_label_type == 5) text = "{progress:2f}";
                else if (selected_label_type == 6) text = "Attempt {attempt}";
                else if (selected_label_type == 7) text = "ColoredCPS({cps}/{cpsHigh}/{clicks})";
                else if (selected_label_type == 8) text = "{levelName}{byLevelCreator} ({levelId})";
                else if (selected_label_type == 9) text = "ColoredDeath({noclipAccuracy})";
                else if (selected_label_type == 10) text = "ColoredDeath({deaths} Deaths)";
                else if (selected_label_type == 11) text = "{startposCurrentIDX}/{startposAllIDX}";
                else if (selected_label_type == 12) text = "{testmode}";
                else if (selected_label_type == 13) text = "{re_state}";
                
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
                    if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("LBLMOVE")) {
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
    first_time_re_settings = true;
    settings_openned = false;

    bool inverted = config.get("gui_inverted", false);
    ApplyGuiColors(inverted);
    ApplyColor(themes[config.get<int>("gui_color_index", 0)]);
    ApplyStyle(m_scale);
    ImGuiIO &io = ImGui::GetIO();
    io.Inverted = inverted;
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
