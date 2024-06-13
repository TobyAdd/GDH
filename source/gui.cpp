#include "gui.hpp"
#include "console.hpp"
#include "memory.hpp"
#include "hacks.hpp"
#include "hooks.hpp"
#include "replayEngine.hpp"
#include "labels.hpp"

bool gui::show = false;
bool gui::inited = false;

std::chrono::steady_clock::time_point animationStartTime;
bool isAnimating = false;
bool isFadingIn = false;
int anim_durr = 100;

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
            //ShitoCursor();
        }

        return;
    }

    float t = elapsed / time;
    float alpha = isFadingIn ? 0.0f + t: 1.0f - t;
    style.Alpha = alpha;
}

std::vector<std::string> stretchedWindows;
void gui::RenderMain() {   
    if (isAnimating) {
        animateAlpha(anim_durr);
    }

    if (!gui::show) return;

    ImGuiIO &io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0, io.DisplaySize.y), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
    ImGui::Begin("BottomLeftText", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoFocusOnAppearing);
    ImGui::Text("GDH v4.2 Alpha | TobyAdd | Prevter | Qwix | https://discord.gg/ahYEz4MAwP | https://t.me/tobyadd_public");
    ImGui::End();

    for (auto& win : hacks::windows) {
        std::string windowName = win.name;
        if (std::find(stretchedWindows.begin(), stretchedWindows.end(), windowName) == stretchedWindows.end())
        {
            ImVec2 windowSize = ImVec2(win.w, win.h);
            ImVec2 windowPos = ImVec2(win.x, win.y);

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
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - (35 + 5) * ImGuiHook::scale);
            if (ImGui::DragFloat("##fps_value", &hacks::fps_value, 1, 1, FLT_MAX, "%0.f TPS")) {
                hacks::update_framerate();
            }

            ImGui::SameLine();
            if (ImGui::Checkbox("   ##TPS Enable", &hacks::fps_enabled, ImGuiHook::scale)) {
                if (!hacks::fps_enabled) {
                    float pre_fps = hacks::fps_value;
                    hacks::fps_value = 240;
                    hacks::update_framerate();
                    hacks::fps_value = pre_fps;
                }                
            }

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::DragFloat("##speedhack_value", &hacks::speed_value, 0.01f, 0, FLT_MAX, "Speed: %.2fx");

        }
        else if (windowName == "Replay Engine") {
            engine.render();
        }
        else if (windowName == "Labels") {
            ImGui::Checkbox("Custom Text", &labels::custom_text_enabled);

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::InputText("##Custom Text2", &labels::custom_text);
            
            ImGui::Checkbox("Time (24H)", &labels::time24_enabled);

            ImGui::SameLine();

            ImGui::Checkbox("Time (12H)", &labels::time12_enabled);            
            ImGui::Checkbox("Noclip Accuracy", &labels::noclip_accuracy_enabled);

            ImGui::Checkbox("CPS Counter", &labels::cps_counter_enabled);

            ImGui::Checkbox("Death Counter", &labels::death_enabled);
            
            const char *e[] = {"Top Left", "Bottom Left", "Top Right", "Bottom Right"};
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::Combo("##Labels Position", &labels::pos, e, 4, 4);
        }
        else {
            for (auto& hck : win.hacks) {
                if (ImGui::Checkbox(hck.name.c_str(), &hck.enabled)) {
                    if (hck.name == "Unlock Items") { hacks::unlock_items = hck.enabled; }
                    else if (hck.name == "Ignore ESC") { hacks::ignore_esc = hck.enabled; }
                    else if (hck.name == "Startpos Switcher") { hacks::startpos_switcher = hck.enabled; }
                    else if (hck.name == "Reset Camera") { hacks::startpos_switcher_reset_camera = hck.enabled; }
                    else if (hck.name == "Use A/D") { hacks::use_a_s_d = hck.enabled; }
                    else if (hck.name == "Instant Complete") { hacks::instant_complate = hck.enabled; }
                    else if (hck.name == "RGB Icons") { hacks::rgb_icons = hck.enabled; }
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

                if (ImGui::IsItemHovered() && !hck.desc.empty())
                    ImGui::SetTooltip(hck.desc.c_str());
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
            //ShitoCursor();         
        }
    }
}

void gui::Unload() {
    gui::show = false;
    stretchedWindows.clear();
}