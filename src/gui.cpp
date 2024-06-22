#include "gui.hpp"
//#include "console.hpp"
#include "memory.hpp"
#include "hacks.hpp"
#include "hooks.hpp"
// #include "replayEngine.hpp"
#include "labels.hpp"
#include "license.hpp"
// #include "speedhackAudio.hpp"

bool gui::show = false;
bool gui::inited = false;
bool gui::needRescale = false;
int gui::indexScale = 3;
bool gui::onlyRE = false;
bool gui::license_accepted = false;
float gui::scale = 1.f;

std::chrono::steady_clock::time_point animationStartTime;
bool isAnimating = false;
bool isFadingIn = false;
int anim_durr = 100;

bool needUnloadFont = false;
std::string search_text;

void updateCursorState() {
    bool canShowInLevel = true;
    if (auto* playLayer = PlayLayer::get()) {
        canShowInLevel = playLayer->m_hasCompletedLevel ||
                         playLayer->m_isPaused ||
                         GameManager::sharedState()->getGameVariable("0024");
    }
    if (gui::show || canShowInLevel)
        PlatformToolbox::showCursor();
    else
        PlatformToolbox::hideCursor();
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
            hacks::save(hacks::windows, hacks::fileDataPath);
            updateCursorState();
        }

        return;
    }

    float t = elapsed / time;
    float alpha = isFadingIn ? 0.0f + t: 1.0f - t;
    style.Alpha = alpha;
}

void License() {
    if (!gui::inited) {
        ImGui::SetNextWindowSize({590 * gui::scale, 390 * gui::scale});
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
    ImGui::BeginChild("##LicenseChild", {0, ImGui::GetContentRegionAvail().y - 40 * gui::scale}, true);
    ImGui::Text("%s", license.c_str());
    ImGui::EndChild();
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    static bool agree = false;
    ImGui::Checkbox("I agree to the license terms", &agree, gui::scale);
    
    ImGui::SetCursorPos({ImGui::GetWindowSize().x - 170 * gui::scale, ImGui::GetWindowSize().y - 45 * gui::scale});
    if (ImGui::Button(agree ? "Agree" : "Disagree", {150 * gui::scale, 30 * gui::scale})) {
        if (agree) {
            gui::license_accepted = true;
        } else {
            gui::Toggle();
        }
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

std::vector<std::string> stretchedWindows;
void gui::RenderMain() {   
    //SpeedhackAudio::update();

    if (isAnimating) {
        animateAlpha(anim_durr);
    }
    
    if (!gui::show) return;

    ImGuiIO &io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0, io.DisplaySize.y), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
    ImGui::Begin("BottomLeftText", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoFocusOnAppearing);
    ImGui::Text("%s v4.6 Geode | TobyAdd | Prevter | Qwix | https://discord.gg/ahYEz4MAwP | https://t.me/tobyadd_public", gui::onlyRE ? "Replay Engine" : "GDH");
    ImGui::End();

    if (!gui::license_accepted) {
        License();
        return;
    }

    if (gui::onlyRE) {
        //engine.renderFull();
        return;
    }

    for (auto& win : hacks::windows) {
        std::string windowName = win.name;
        if (std::find(stretchedWindows.begin(), stretchedWindows.end(), windowName) == stretchedWindows.end())
        {
            ImVec2 windowSize = ImVec2(win.w, win.h);
            ImVec2 windowPos = ImVec2(win.x, win.y);

            if (gui::needRescale) {
                windowSize = ImVec2(win.orig_w * gui::scale, win.orig_h * gui::scale);
                windowPos = ImVec2(win.orig_x * gui::scale, win.orig_y * gui::scale);
                geode::log::info("{} {} {} {}", windowSize.x, windowSize.y, windowPos.x, windowPos.y);
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

        if (windowName == "Framerate") {
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::DragFloat("##speedhack_value", &hacks::speed_value, 0.01f, 0, FLT_MAX, "Speed: %.2fx");

            ImGui::Checkbox("Speedhack Audio", &hacks::speedhack_audio, gui::scale);

        }
        else if (windowName == "Replay Engine") {
            //engine.render();
        }
        else if (windowName == "Labels") {
            ImGui::Checkbox("Custom Text", &labels::custom_text_enabled, gui::scale);

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::InputText("##Custom Text2", &labels::custom_text);
            
            ImGui::Checkbox("Time (24H)", &labels::time24_enabled, gui::scale);

            ImGui::SameLine();

            ImGui::Checkbox("Time (12H)", &labels::time12_enabled, gui::scale);            
            ImGui::Checkbox("Noclip Accuracy", &labels::noclip_accuracy_enabled, gui::scale);

            ImGui::Checkbox("CPS Counter", &labels::cps_counter_enabled, gui::scale);

            ImGui::Checkbox("Death Counter", &labels::death_enabled, gui::scale);
            
            const char *e[] = {"Top Left", "Bottom Left", "Top Right", "Bottom Right"};
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::Combo("##Labels Position", &labels::pos, e, 4, 4);
        }
        else if (windowName == "GDH Settings") {
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::InputTextWithHint("##Search", "Search:", &search_text);

            if (ImGui::GetIO().MouseWheel != 0 && ImGui::IsItemActive())
                ImGui::SetWindowFocus(NULL);

            
            const char* items[] = {"25%", "50%", "75%", "100%", "125%", "150%", "175%", "200%", "250%", "300%", "400%"};
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGui::Combo("##Menu scale", &gui::indexScale, items, IM_ARRAYSIZE(items))) {
                gui::scale = float(atof(items[gui::indexScale])) / 100.0f;    
                gui::needRescale = true;
                ImGuiCocos::get().reload();
            }
        }
        else {        
            for (auto& hck : win.hacks) {
                std::string search_name = hck.name;
                std::string search_item = search_text;
                
                std::transform(search_item.begin(), search_item.end(), search_item.begin(), ::tolower);
                std::transform(search_name.begin(), search_name.end(), search_name.begin(), ::tolower);

                bool founded = search_item.empty() ? true : (search_name.find(search_item) != std::string::npos);
                ImGui::PushStyleColor(ImGuiCol_Text, founded ? ImColor(255, 255, 255).Value : ImColor(64, 64, 64).Value);

                if (ImGui::Checkbox(hck.name.c_str(), &hck.enabled, gui::scale)) {
                    if (hck.name == "Unlock Items") { hacks::unlock_items = hck.enabled; }
                    else if (hck.name == "Ignore ESC") { hacks::ignore_esc = hck.enabled; }
                    else if (hck.name == "Startpos Switcher") { hacks::startpos_switcher = hck.enabled; }
                    else if (hck.name == "Reset Camera") { hacks::startpos_switcher_reset_camera = hck.enabled; }
                    else if (hck.name == "Use A/D") { hacks::use_a_s_d = hck.enabled; }
                    else if (hck.name == "Instant Complete") { hacks::instant_complate = hck.enabled; }
                    else if (hck.name == "RGB Icons") { hacks::rgb_icons = hck.enabled; }
                    else if (hck.name == "Hide Pause Menu") { 
                        auto pl = PlayLayer::get();
                        hacks::hide_menu = hck.enabled; 
                        if (pl && hooks::pauseLayer)
                            hooks::pauseLayer->setVisible(!hck.enabled);
                    }
                    else if (hck.name == "Auto Pickup Coins") { hacks::auto_pickup_coins = hck.enabled; }
                    else {
                        for (auto& opc : hck.opcodes) {
                            std::string bytesStr = hck.enabled ? opc.on : opc.off;
                            if (opc.address != 0) {
                                uintptr_t base = (uintptr_t)GetModuleHandleA(0);
                                if (!opc.module.empty())
                                {
                                    base = (uintptr_t)GetModuleHandleA(opc.module.c_str());
                                }
                                memory::WriteBytes(base + opc.address, bytesStr);
                            }
                        }
                    }
                }

                ImGui::PopStyleColor();

                if (ImGui::IsItemHovered() && !hck.desc.empty())
                    ImGui::SetTooltip(hck.desc.c_str());
                
                if (hck.name == "RGB Icons") {
                    ImGui::SameLine();
                    if (ImGui::ArrowButton("##rainbowSettings", ImGuiDir_Right)) {
                        ImGui::OpenPopup("Rainbow Icon Settings");
                    }

                    if (ImGui::BeginPopupModal("Rainbow Icon Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                        ImGui::DragFloat("##riconBrightness", &hacks::ricon_brightness, 0.01f, 0.0f, 1.0f, "Brightness: %.2f");
                        ImGui::DragFloat("##riconSaturation", &hacks::ricon_saturation, 0.01f, 0.0f, 1.0f, "Saturation: %.2f");

                        ImGui::DragFloat("##riconCoef", &hacks::ricon_coef, 0.01f, 0.0f, 10.0f, "Speed Coefficent: %.2f");
                        ImGui::DragFloat("##riconShift", &hacks::ricon_shift, 0.01f, 0.0f, 1.0f, "Secondary Color Shift: %.2f");

                        if (ImGui::Button("Close", {ImGui::GetContentRegionAvail().x, NULL})) {
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                }
            }
        }

        ImGui::PopFont();
        ImGui::End();
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
            updateCursorState();        
        }
    }
}

void gui::Unload() {
    stretchedWindows.clear();
}