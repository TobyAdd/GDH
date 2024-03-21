#include "gui.hpp"
#include "data.hpp"
#include "content.hpp"
#include "memory.hpp"
#include "hooks.hpp"
#include "speedhackAudio.hpp"
#include "replayEngine.hpp"
#include "labels.hpp"

bool gui::show = false;
bool gui::inited = false;
bool gui::license_accepted = false;

char search_str[128];

float gui::scalePercentage = 1.f;
bool need_unload = false;
bool need_rescale = false;

bool ricon_enabled = false;
float ricon_coef = 0.25f;
float ricon_shift = 5.0f;
float ricon_saturation = 1.0f, ricon_brightness = 1.0f;

std::chrono::steady_clock::time_point animationStartTime;
bool isAnimating = false;
bool isFadingIn = false;
int anim_durr = 100;

bool gui::onlyRE = false;

void ShitoCursor() {
    auto pl = gd::GameManager::sharedState()->getPlayLayer();
    if (pl && !hooks::pause_menu && !gd::GameManager::sharedState()->getGameVariable("0024")) {
        cocos2d::CCEGLView::sharedOpenGLView()->showCursor(gui::show);
    }
}

void animateAlpha(int ms)
{
    ImGuiStyle& style = ImGui::GetStyle();

    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> diff = currentTime - animationStartTime;
    float elapsed = diff.count();

    float time = ms / 1000.0f;
    if (elapsed >= time)
    {
        style.Alpha = isFadingIn ? 1.0f : 0.0f;
        isAnimating = false;

        if (!isFadingIn)
        {
            gui::show = !gui::show;
            ShitoCursor();
        }

        return;
    }

    float t = elapsed / time;
    float alpha = isFadingIn ? 0.0f + t: 1.0f - t;
    style.Alpha = alpha;
}

void License() {
    if (!gui::inited) {
        ImGui::SetNextWindowSize({590 * gui::scalePercentage, 390 * gui::scalePercentage});
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));  
        gui::inited = true;
    }    

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {20, 20});
    ImGui::Begin("Welcome", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::Text("Welcome to GDH.");
    ImGui::PopFont();

    ImGui::Text("Please read the license agreement.");

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {10, 10});
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(40, 45, 51).Value);
    ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, ImColor(40, 45, 51).Value);
    ImGui::BeginChild("##LicenseChild", {0, ImGui::GetContentRegionAvail().y - 40 * gui::scalePercentage}, true);
    ImGui::Text("%s", license.c_str());
    ImGui::EndChild();
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    static bool agree = false;
    ImGui::Checkbox("I agree to the license terms", &agree, gui::scalePercentage);
    ImGui::SetCursorPos({ImGui::GetWindowSize().x - 170 * gui::scalePercentage, ImGui::GetWindowSize().y - 45 * gui::scalePercentage});
    if (ImGui::Button(agree ? "Agree" : "Disagree", {150 * gui::scalePercentage, 30 * gui::scalePercentage})) {
        if (agree) {
            gui::license_accepted = true;
            Content::content["license_accepted"] = true;
        } else {
            exit(-1);
        }
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

void TickRainbowIcon() {
    hacks::rgb_icons = ricon_enabled;
    float r1, g1, b1, r2, g2, b2;

    ImGui::ColorConvertHSVtoRGB((float) ImGui::GetTime() * ricon_coef, ricon_saturation, ricon_brightness, r1, g1, b1);
    ImGui::ColorConvertHSVtoRGB((float) ImGui::GetTime() * ricon_coef + ricon_shift, ricon_saturation, ricon_brightness, r2, g2, b2);

    hacks::iconcolor1[0] = r1;
    hacks::iconcolor1[1] = g1;
    hacks::iconcolor1[2] = b1;

    hacks::iconcolor2[0] = r2;
    hacks::iconcolor2[1] = g2;
    hacks::iconcolor2[2] = b2;
}

std::vector<std::string> stretchedWindows;
void gui::RenderMain() {
    // ImGui::ShowDemoWindow();
    SpeedhackAudio::update();
    if (ricon_enabled) TickRainbowIcon();

    if (isAnimating) {
        animateAlpha(anim_durr);
    }

    if (!show) return;
    
    if (license_accepted) {
        if (onlyRE) {
            engine.renderFull();
            return;
        }


        for (auto &item : Content::content["Hacks"].items()) {
            std::string windowName = item.key();

            if (std::find(stretchedWindows.begin(), stretchedWindows.end(), windowName) == stretchedWindows.end())
            {
                ImVec2 windowSize = ImVec2(float(item.value()["windowSize"]["width"]),
                                        float(item.value()["windowSize"]["height"]));
                ImVec2 windowPos = ImVec2(float(item.value()["windowPosition"]["x"]),
                                        float(item.value()["windowPosition"]["y"]));

                if (need_rescale) {
                    windowSize = ImVec2(float(item.value()["windowSize"]["orig_width"]) * scalePercentage,
                                            float(item.value()["windowSize"]["orig_height"]) * scalePercentage);

                    windowPos = ImVec2(float(item.value()["windowPosition"]["orig_x"]) * scalePercentage,
                                            float(item.value()["windowPosition"]["orig_y"]) * scalePercentage);
                }

                ImGui::SetNextWindowSize(windowSize);
                ImGui::SetNextWindowPos(windowPos);

                stretchedWindows.push_back(windowName);
            }   
            else {
                need_rescale = false;
            }

            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
            ImGui::Begin(windowName.c_str(), 0, ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoNavInputs);
            ImGui::PopFont();

            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);

            if (!ImGui::IsWindowCollapsed()) {
                item.value()["windowSize"]["width"] = ImGui::GetWindowSize().x;
                item.value()["windowSize"]["height"] = ImGui::GetWindowSize().y;
            }

            item.value()["windowPosition"]["x"] = ImGui::GetWindowPos().x;
            item.value()["windowPosition"]["y"] = ImGui::GetWindowPos().y;


            auto &components = item.value()["components"];
            for (auto &component : components)
            {
                std::string type = component["type"];
                if (type == "checkbox_hack")
                {
                    std::string name = component["name"].get<std::string>();
                    bool enabled = component["enabled"];

                    std::string search_name = name;
                    std::string search_item(search_str);
                    
                    std::transform(search_item.begin(), search_item.end(), search_item.begin(), ::tolower);
                    std::transform(search_name.begin(), search_name.end(), search_name.begin(), ::tolower);

                    bool founded = search_item.empty() ? true : (search_name.find(search_item) != std::string::npos);
                    if (founded) {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255).Value);
                    }
                    else {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImColor(64, 64, 64).Value);
                    }

                    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
                        ImGui::SetKeyboardFocusHere(0);

                    if (ImGui::Checkbox(name.c_str(), &enabled, scalePercentage))
                    {
                        component["enabled"] = enabled;
                        json opcodes = component["opcodes"];
                        for (auto& opcode : opcodes)
                        {
                            std::string addrStr = opcode["address"];
                            std::string bytesStr = enabled ? opcode["on"] : opcode["off"];

                            uintptr_t address;
                            sscanf_s(addrStr.c_str(), "%x", &address);

                            if (address != 0) {
                                DWORD base = (DWORD)GetModuleHandleA(0);
                                if (!opcode["lib"].is_null())
                                {
                                    base = (DWORD)GetModuleHandleA(std::string(opcode["lib"]).c_str());
                                }
                                
                                memory::WriteBytes(base + address, bytesStr);
                            }
                        }
                    }
                    ImGui::PopStyleColor();
                }
                else if (type == "separator")
                {
                    ImGui::Separator();
                }
                else if (type == "search_box")
                {
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    ImGui::InputTextWithHint("##Search", "Search:", search_str, 128);

                    if (ImGui::GetIO().MouseWheel != 0 && ImGui::IsItemActive())
                        ImGui::SetWindowFocus(NULL);
                }
                else if (type == "scale_gui")
                {
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

                    const char* items[] = {"25%", "50%", "75%", "100%", "125%", "150%", "175%", "200%", "250%", "300%", "400%"};
                    int index = component["index"];
                    if (ImGui::Combo("##Menu scale", &index, items, IM_ARRAYSIZE(items))) {
                        component["index"] = index;
                        scalePercentage = float(atof(items[index])) / 100.0f;
                        ImGuiHook::scale = scalePercentage;       
                        ImGui::GetStyle().ScrollbarSize = 14.f * scalePercentage;
                        need_unload = true;
                    }
                }
                else if (type == "checkbox_smart_startpos") {
                    bool enabled = component["enabled"];
                    if (ImGui::Checkbox("Smart Startpos", &enabled, scalePercentage)) {
                        component["enabled"] = enabled;
                        hacks::smart_startpos = enabled;
                    }
                }
                else if (type == "checkbox_discord_rpc") {
                    bool enabled = component["enabled"];
                    if (ImGui::Checkbox("Discord RPC", &enabled, scalePercentage)) {
                        component["enabled"] = enabled;
                        hacks::discord_rpc = enabled;
                    }
                }
                else if (type == "checkbox_auto_song_downloader") {
                    bool enabled = component["enabled"];
                    if (ImGui::Checkbox("Auto Song Download", &enabled, scalePercentage)) {
                        component["enabled"] = enabled;
                        hacks::auto_song_downloader = enabled;
                    }
                }
                else if (type == "checkbox_auto_ldm") {
                    bool enabled = component["enabled"];
                    if (ImGui::Checkbox("Auto LDM", &enabled, scalePercentage)) {
                        component["enabled"] = enabled;
                        hacks::auto_ldm = enabled;
                    }
                }                
                else if (type == "checkbox_auto_pickup_coins") {
                    bool enabled = component["enabled"];
                    if (ImGui::Checkbox("Auto Pickup Coins", &enabled, scalePercentage)) {
                        component["enabled"] = enabled;
                        hacks::auto_pickup_coins = enabled;
                    }
                }                              
                else if (type == "checkbox_auto_practice_mode") {
                    bool enabled = component["enabled"];
                    if (ImGui::Checkbox("Auto Practice Mode", &enabled, scalePercentage)) {
                        component["enabled"] = enabled;
                        hacks::auto_practice_mode = enabled;
                    }
                }
                else if (type == "fps_multiplier") {
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - (35 + 5) * scalePercentage);
                    if (ImGui::DragFloat("##TPS_Value", &hacks::tps, 1, 1, FLT_MAX, "%0.f TPS")) {
                        component["tps_value"] = hacks::tps;
                        hacks::update_framerate();
                        left_over = 0;
                    }
                    ImGui::SameLine();
                    if (ImGui::Checkbox("##TPS Enable", &hacks::tps_enabled, scalePercentage)) {
                        component["tps_enabled"] = hacks::tps_enabled;   
                        left_over = 0;                     
                    }
                }
                else if (type == "speed") {
                    float value = component["value"];
                    bool speedhack_audio_enabled = component["audio"];

                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    if (ImGui::DragFloat("##Speed_Value", &value, 0.01f, 0, FLT_MAX, "%.2fx")) {
                        component["value"] = value;
                        hacks::speed = value;
                        if (speedhack_audio_enabled) {
                            SpeedhackAudio::speed = hacks::speed;
                        }
                    }
                    
                    if (ImGui::Checkbox("Speedhack Audio", &speedhack_audio_enabled, scalePercentage)) {
                        component["audio"] = speedhack_audio_enabled;
                        if (speedhack_audio_enabled) {
                        }
                        else {
                        }
                    }
                }
                else if (type == "checkbox_show_hitboxes") {
                    bool enabled = component["enabled"];
                    if (ImGui::Checkbox("Show Hitboxes", &enabled, scalePercentage)) {
                        component["enabled"] = enabled;
                        hacks::show_hitboxes = enabled;
                        hitboxes_mode::enabled_hitbox(enabled);
                    }
                    ImGui::SameLine();
                    if (ImGui::ArrowButton("##hitboxSettings", ImGuiDir_Right)) {
                        ImGui::OpenPopup("Show Hitboxes Settings");
                    }

                    if (ImGui::BeginPopupModal("Show Hitboxes Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                        bool show_on_death = component["show_on_death"];
                        if (ImGui::Checkbox("Show on death", &show_on_death, scalePercentage)) {
                            component["show_on_death"] = show_on_death;
                            hacks::show_on_death = show_on_death;
                        }

                        bool draw_trail_enabled = component["draw_trail"];
                        if (ImGui::Checkbox("Draw Trail", &draw_trail_enabled, scalePercentage)) {
                            component["draw_trail"] = draw_trail_enabled;
                            hacks::draw_trail = draw_trail_enabled;
                            hitboxes_mode::enabled_draw_trail(draw_trail_enabled);
                        }
                        bool default_circle_hitboxes_size_enabled = component["default_circle_hitboxes_size"];
                        if (ImGui::Checkbox("Default Circle Hitboxes Size", &default_circle_hitboxes_size_enabled, scalePercentage)) {
                            component["default_circle_hitboxes_size"] = default_circle_hitboxes_size_enabled;
                            hacks::default_hitbox_size = default_circle_hitboxes_size_enabled;
                        }
                        int border_alpha = component["border_alpha"];
                        if (ImGui::DragInt("##brdalpha", &border_alpha, 0.01f, 0, 1000, "Border Alpha: %d")) {
                            component["border_alpha"] = border_alpha;
                            hacks::border_alpha = border_alpha;
                        }
                        int fill_alpha = component["fill_alpha"];
                        if (ImGui::DragInt("##fllalpha", &fill_alpha, 0.01f, 0, 1000, "Fill Alpha: %d")) {
                            component["fill_alpha"] = fill_alpha;
                            hacks::fill_alpha = fill_alpha;
                        }
                        float hitboxes_size = component["hitboxes_size"];
                        if (ImGui::DragFloat("##htbxsize", &hitboxes_size, 0.01f, 0, 1000, "Hitboxes Size: %.2f")) {
                            component["hitboxes_size"] = hitboxes_size;
                            hacks::hitbox_size = hitboxes_size;
                        }
                        int circle_border_alpha = component["circle_border_alpha"];
                        if (ImGui::DragInt("##crclbrdalpha", &circle_border_alpha, 0.01f, 0, 1000, "Circle Border Alpha: %d")) {
                            component["circle_border_alpha"] = circle_border_alpha;
                            hacks::circle_border_alpha = circle_border_alpha;
                        }
                        int circle_fill_alpha = component["circle_fill_alpha"];
                        if (ImGui::DragInt("##crclfillalpha", &circle_fill_alpha, 0.01f, 0, 1000, "Circle Fill Alpha: %d")) {
                            component["circle_fill_alpha"] = circle_fill_alpha;
                            hacks::circle_fill_alpha = circle_fill_alpha;
                        }
                        float circle_hitbox_size = component["circle_hitboxes_size"];
                        if (ImGui::DragFloat("##crclhtbxsize", &circle_hitbox_size, 0.01f, 0, 1000, "Circle Hitboxes Size: %.2f")) {
                            component["circle_hitboxes_size"] = circle_hitbox_size;
                            hacks::circle_hitbox_size = circle_hitbox_size;
                        }
                        if (ImGui::Button("Close", {ImGui::GetContentRegionAvail().x, NULL})) {
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                }
                else if (type == "checkbox_layout_mode") {
                    bool enabled = component["enabled"];
                    if (ImGui::Checkbox("Show Layout", &enabled, scalePercentage)) {
                        component["enabled"] = enabled;
                        layout_mode::set_enabled(enabled);
                    }
                }
                else if (type == "checkbox_instant_complete") {
                    bool enabled = component["enabled"];
                    if (ImGui::Checkbox("Instant Complete", &enabled, scalePercentage)) {
                        component["enabled"] = enabled;
                        hacks::instant_complate = enabled;
                    }
                }
                else if (type == "checkbox_hide_pause_menu") {
                    bool enabled = component["enabled"];
                    if (ImGui::Checkbox("Hide Pause Menu", &enabled, scalePercentage)) {
                        component["enabled"] = enabled;
                        hacks::hide_menu = enabled;
                        
                        auto pl = gd::GameManager::sharedState()->getPlayLayer();
                        if (pl && hooks::pause_menu)
                            hooks::pause_menu->setVisible(!enabled);
                    }
                }
                else if (type == "checkbox_respawn_time") {
                    float duration = component["duration"];

                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    if (ImGui::DragFloat("##duration", &duration, 0.01f, 0, FLT_MAX, "Respawn Time: %.2f")) {
                        component["duration"] = duration;
                        hacks::respawnTime = duration;
                    }
                }
                else if (type == "checkbox_startpos_switcher") {
                    bool enabled = component["enabled"];
                    if (ImGui::Checkbox("Startpos Switcher", &enabled, scalePercentage)) {
                        component["enabled"] = enabled;
                        startPosSwitcher::enabled = enabled;
                    }

                    bool use_a_d_enabled = component["use_a_d"];
                    if (ImGui::Checkbox("Use A/D", &use_a_d_enabled, scalePercentage)) {
                        component["use_a_d"] = use_a_d_enabled;
                        startPosSwitcher::use_a_d = use_a_d_enabled;
                    }
                }
                else if (type == "checkbox_rainbow_colors") {
                    ricon_enabled = component["enabled"];
                    ricon_coef = component["coefficent"];
                    ricon_shift = component["shift"];
                    ricon_brightness = component["brightness"];
                    ricon_saturation = component["saturation"];

                    if (ImGui::Checkbox("Rainbow Icon", &ricon_enabled, scalePercentage)) {
                        component["enabled"] = ricon_enabled;
                    }

                    ImGui::SameLine();
                    if (ImGui::ArrowButton("##rainbowSettings", ImGuiDir_Right)) {
                        ImGui::OpenPopup("Rainbow Icon Settings");
                    }

                    if (ImGui::BeginPopupModal("Rainbow Icon Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                        ImGui::DragFloat("##riconBrightness", &ricon_brightness, 0.01f, 0.0f, 1.0f, "Brightness: %.2f");
                        ImGui::DragFloat("##riconSaturation", &ricon_saturation, 0.01f, 0.0f, 1.0f, "Saturation: %.2f");

                        ImGui::DragFloat("##riconCoef", &ricon_coef, 0.01f, 0.0f, 10.0f, "Speed Coefficent: %.2f");
                        ImGui::DragFloat("##riconShift", &ricon_shift, 0.01f, 0.0f, 1.0f, "Secondary Color Shift: %.2f");
                        
                        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(hacks::iconcolor1[0], hacks::iconcolor1[1], hacks::iconcolor1[2], 1.0f));
                        ImGui::BeginChild("##colorExampleP1", {ImGui::GetContentRegionAvail().x / 2, 24 * scalePercentage});
                        ImGui::EndChild();
                        ImGui::PopStyleColor();

                        ImGui::SameLine();

                        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(hacks::iconcolor2[0], hacks::iconcolor2[1], hacks::iconcolor2[2], 1.0f));
                        ImGui::BeginChild("##colorExampleP2", {ImGui::GetContentRegionAvail().x, 24 * scalePercentage});
                        ImGui::EndChild();
                        ImGui::PopStyleColor();

                        component["enabled"] = ricon_enabled;
                        component["coefficent"] = ricon_coef;
                        component["shift"] = ricon_shift;
                        component["brightness"] = ricon_brightness;
                        component["saturation"] = ricon_saturation;
                        if (ImGui::Button("Close", {ImGui::GetContentRegionAvail().x, NULL})) {
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                }
                else if (type == "replay_engine") {
                    engine.render();
                }
                else if (type == "about") {
                    ImGui::Text("GDH v3.0 Betatest Build");
                    ImGui::Text("Special thanks to Qwix and Prevter");
                    if (ImGui::MenuItem("Graphical interface is made using ImGui"))
                        ShellExecuteA(NULL, "open", "https://github.com/ocornut/imgui", NULL, NULL, SW_SHOWNORMAL);

                    if (ImGui::Button("Discord Server", {ImGui::GetContentRegionAvail().x, NULL}))
                        ShellExecuteA(NULL, "open", "https://discord.gg/ahYEz4MAwP", NULL, NULL, SW_SHOWNORMAL);

                    if (ImGui::Button("License", {ImGui::GetContentRegionAvail().x, NULL}))
                        license_accepted = false;
                }
                else if (type == "transition_customizer") {
                    bool enabled = component["enabled"];
                    if (ImGui::Checkbox("Custom Transition", &enabled, scalePercentage)) {
                        component["enabled"] = enabled;
                        hacks::transition_enabled = enabled;
                    }

                    ImGui::SameLine();
                    if (ImGui::ArrowButton("##transition_settings", ImGuiDir_Right)) {
                        ImGui::OpenPopup("Transition Settings");
                    }

                    if (ImGui::BeginPopup("Transition Settings", ImGuiWindowFlags_AlwaysAutoResize)) {
                        const char *e[] = {"Fade", "CrossFade", "Fade Bottom Left", "Fade Down", "Fade Top Right", "Fade Up", "Flip Angular", "Flip X", "Flip Y", "Jump Zoom", "Move In Bottom", "Move In Left", "Move In Right", "Move In Top", "Page Turn", "Progress Horizontal", "Progress In Out", "Progress Out In", "Progress Radial CWW", "Progress Radial CW", "Progress Vertical", "Rotation Zoom", "Shrink Grow", "Slider In Bottom", "Slider In Left", "Slider In Right", "Slider In Top", "Split Columns", "Split Rows", "Titles", "Zoom Flip Angular", "Zoom Flip X", "Zoom Flip Y"};
                        int index = component["index"];
                        ImGui::SetNextItemWidth(150.f * ImGuiHook::scale);
                        if (ImGui::Combo("Transition", &index, e, 33, 5)) {
                            component["index"] = index;
                            hacks::transition_index = index;
                        }

                        float duration = component["duration"];
                        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                        if (ImGui::DragFloat("##Duration", &duration, 0.01f, 0, FLT_MAX, "Duration: %.2f")) {
                            component["duration"] = duration;
                            hacks::transition_duration = duration;
                        }
                        ImGui::EndPopup();
                    }
                }
                else if (type == "checkbox_hide_player") {
                    bool enabled = component["enabled"];
                    if (ImGui::Checkbox("Hide Player", &enabled, scalePercentage)) {
                        component["enabled"] = enabled;
                        hacks::hide_player_enabled = enabled;
                    }

                    ImGui::SameLine();
                    if (ImGui::ArrowButton("##hide_player_settings", ImGuiDir_Right)) {
                        ImGui::OpenPopup("Hide Player Settings");
                    }

                    if (ImGui::BeginPopup("Hide Player Settings", ImGuiWindowFlags_AlwaysAutoResize)) {
                        bool first_player = component["first_player"];
                        if (ImGui::Checkbox("First player", &first_player, scalePercentage)) {
                            component["first_player"] = first_player;
                            hacks::hide_player_p1 = first_player;
                        }

                        bool second_player = component["second_player"];
                        if (ImGui::Checkbox("Second player", &second_player, scalePercentage)) {
                            component["second_player"] = second_player;
                            hacks::hide_player_p2 = second_player;
                        }
                        ImGui::EndPopup();
                    }
                }
                else if (type == "labels") {
                    bool custom_text_enabled = component["custom_text_enabled"];
                    if (ImGui::Checkbox("Custom Text", &custom_text_enabled, gui::scalePercentage)) {
                        component["custom_text_enabled"] = custom_text_enabled;
                        labels::custom_text_enabled = custom_text_enabled;
                    }

                    std::string custom_text = component["custom_text"];
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    if (ImGui::InputText("##Custom Text2", &custom_text)) {
                        component["custom_text"] = custom_text;
                        labels::custom_text = custom_text;
                    }
                    
                    bool time_24hr = component["time_24hr"];
                    if (ImGui::Checkbox("Time (24H)", &time_24hr, gui::scalePercentage)) {
                        component["time_24hr"] = time_24hr;
                        labels::time24_enabled = time_24hr;
                    }

                    ImGui::SameLine();

                    bool time_12hr = component["time_12hr"];
                    if (ImGui::Checkbox("Time (12H)", &time_12hr, gui::scalePercentage)) {
                        component["time_12hr"] = time_12hr;
                        labels::time12_enabled = time_12hr;
                    }
                    
                    bool noclip_accuracy = component["noclip_accuracy"];
                    if (ImGui::Checkbox("Noclip Accuracy", &noclip_accuracy, gui::scalePercentage)) {
                        component["noclip_accuracy"] = noclip_accuracy;
                        labels::noclip_accuracy_enabled = noclip_accuracy;
                    }

                    bool cps_counter = component["cps_counter"];
                    if (ImGui::Checkbox("CPS Counter", &cps_counter, gui::scalePercentage)) {
                        component["cps_counter"] = cps_counter;
                        labels::cps_counter_enabled = cps_counter;
                    }

                    bool death_counter = component["death_counter"];
                    if (ImGui::Checkbox("Death Counter", &death_counter, gui::scalePercentage)) {
                        component["death_counter"] = death_counter;
                        labels::death_enabled = death_counter;
                    }
                    
                    int pos = component["pos"];
                    const char *e[] = {"Top Left", "Bottom Left", "Top Right", "Bottom Right"};
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    if (ImGui::Combo("##Labels Position", &pos, e, 4, 4)) {
                        component["pos"] = pos;
                        labels::pos = pos;
                    }                    
                }
            }
            ImGui::PopFont();
        }
    }
    else {
        License();
    }

    if (need_unload) {
        need_unload = false;
        ImGuiHook::Unload(true);
        need_rescale = true;
        gui::show = true;
    }
}

void gui::Toggle() {
    if (!isAnimating)
    {
        isAnimating = true;
        isFadingIn = !isFadingIn;
        animationStartTime = std::chrono::steady_clock::now();

        if (isFadingIn)
        {
            gui::show = !gui::show;  
            ShitoCursor();         
        }
    }
}

void gui::Unload() {
    // gui::show = false;
    stretchedWindows.clear();    
}