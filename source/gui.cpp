#define IMGUI_DEFINE_MATH_OPERATORS
#include "gui.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui-hook.hpp>
#include "hooks.hpp"
#include "hacks.hpp"
#include <shellapi.h>
#include "xorstr.hpp"
#include "startposSwitcher.hpp"
#include "smartStartpos.hpp"
#include "keybinds.hpp"

bool gui::show = false;
bool gui::inited = false;
bool gui::recording = true;
int gui::currentkeycode = -1;
int oldkeycode;
bool secret = false;

int anim_durr = -1;
auto anim_starttime = std::chrono::steady_clock::now();
bool binding_mode = false;
std::string binding_now = "please dont create a hack with this name i will be very depressed";

extern "C"
{
    __declspec(dllexport) int __stdcall GDH()
    {
        secret = true;
        return secret;
    };
}

bool isEmpty(char *str)
{
    return (str != NULL && str[0] == '\0');
}

ImVec4 colorMultiply(float color[4], float multiplier) {
    return ImVec4(std::clamp(color[0] * multiplier, 0.0f, 1.0f),
                  std::clamp(color[1] * multiplier, 0.0f, 1.0f),
                  std::clamp(color[2] * multiplier, 0.0f, 1.0f), color[3]);
}

void setStyle(ImGuiStyle &style, float accent[4])
{
    float average = (accent[0] + accent[1] + accent[2]) / 3.0f;
    ImVec4 color = ImVec4(0.0f, 0.0f, 0.0f, accent[3]);;

    if (average < 0.5f) color = ImVec4(1.0f, 1.0f, 1.0f, accent[3]);

    style.Colors[ImGuiCol_Text] = color;
    style.Colors[ImGuiCol_TextSelectedBg] = colorMultiply(accent, 0.8f);

    style.Colors[ImGuiCol_CheckMark] = color;

    style.Colors[ImGuiCol_WindowBg] = colorMultiply(accent, 0.8f);

    style.Colors[ImGuiCol_FrameBg] = colorMultiply(accent, 0.6f);
    style.Colors[ImGuiCol_FrameBgHovered] = colorMultiply(accent, 0.4f);
    style.Colors[ImGuiCol_FrameBgActive] = colorMultiply(accent, 0.6f);

    style.Colors[ImGuiCol_TitleBg] = colorMultiply(accent, 0.9f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = colorMultiply(accent, 0.8f);
    style.Colors[ImGuiCol_TitleBgActive] = colorMultiply(accent, 1.0f);

    style.Colors[ImGuiCol_ScrollbarBg] = colorMultiply(accent, 0.6f);
    style.Colors[ImGuiCol_ScrollbarGrab] = colorMultiply(accent, 0.8f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = colorMultiply(accent, 0.9f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = colorMultiply(accent, 1.0f);

    style.Colors[ImGuiCol_SliderGrab] = colorMultiply(accent, 0.8f);
    style.Colors[ImGuiCol_SliderGrabActive] = colorMultiply(accent, 1.0f);

    style.Colors[ImGuiCol_Button] = colorMultiply(accent, 0.6f);
    style.Colors[ImGuiCol_ButtonHovered] = colorMultiply(accent, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = colorMultiply(accent, 1.0f);

    style.Colors[ImGuiCol_Separator] = colorMultiply(accent, 0.6f);

    style.Colors[ImGuiCol_ResizeGrip] = colorMultiply(accent, 1.0f);
    style.Colors[ImGuiCol_ResizeGripHovered] = colorMultiply(accent, 1.2f);
    style.Colors[ImGuiCol_ResizeGripActive] = colorMultiply(accent, 1.2f);

    style.Colors[ImGuiCol_PopupBg] = colorMultiply(accent, 0.8f);
}

float get_opacity() {
    auto now = std::chrono::steady_clock::now();

    long long diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - anim_starttime).count();
    if (gui::show)
        return (float)diff / (float)anim_durr;
    else
        return 1.0f - ((float)diff / (float)anim_durr);
}

void gui::Render()
{
    if (!gui::inited) {
        gui::inited = true;
        ImGui::GetStyle().Alpha = 0;
    }

    ImGui::GetStyle().Alpha = get_opacity();
    if (!gui::show && ImGui::GetStyle().Alpha <= 0.0f) {
        return;
    }

    static std::vector<std::string> stretchedWindows;
    // IMGUI pinkish colors
    static float color[4] = { 0.337f, 0.176f, 0.464f, 1.000f };
    static float default_color[4] = { 0.337f, 0.176f, 0.464f, 1.000f };

    for (auto &item : hacks::content.items())
    {
        std::string windowName = item.key();

        if (std::find(stretchedWindows.begin(), stretchedWindows.end(), windowName) == stretchedWindows.end())
        {
            ImVec2 windowSize = ImVec2(float(item.value()["windowSize"]["width"]),
                                       float(item.value()["windowSize"]["height"]));
            ImVec2 windowPos = ImVec2(float(item.value()["windowPosition"]["x"]),
                                      float(item.value()["windowPosition"]["y"]));

            ImGui::SetNextWindowSize(windowSize);
            ImGui::SetNextWindowPos(windowPos);

            stretchedWindows.push_back(windowName);
        }

        ImGui::Begin(windowName.c_str());

        item.value()["windowSize"]["width"] = ImGui::GetWindowSize().x;
        item.value()["windowSize"]["height"] = ImGui::GetWindowSize().y;
        item.value()["windowPosition"]["x"] = ImGui::GetWindowPos().x;
        item.value()["windowPosition"]["y"] = ImGui::GetWindowPos().y;

        auto &components = item.value()["components"];
        if (!components.is_null())
        {
            for (auto &component : components)
            {
                std::string type = component["type"];
                if (type == "hack")
                {
                    if (binding_mode) {
                        std::string keybind = "";
                        for (auto &bind : keybinds::binds.items()) {
                            if (bind.value() == component["name"])
                                keybind.append(1, (char) std::stoi(bind.key()));
                        }
                        if (keybind == "") {
                            keybind = "None";
                        }

                        if (binding_now == component["name"].get<std::string>())
                            keybind = "???";

                        if (ImGui::Button((component["name"].get<std::string>() + (std::string) ": " + keybind).c_str())) {
                            binding_now = component["name"].get<std::string>();
                            recording = true;
                            oldkeycode = currentkeycode;
                        }

                        if (binding_now == component["name"].get<std::string>()) {
                            if (oldkeycode != currentkeycode) {
                                binding_now = "please dont create a hack with this name i will be very depressed";
                                recording = false;
                                if (currentkeycode == VK_ESCAPE) {
                                    for (auto& bind : keybinds::binds.items()) {
                                        if (bind.value() == component["name"]) {
                                            keybinds::binds.erase(bind.key());
                                        }
                                    }
                                }
                                else
                                    keybinds::AddKeybind(component["name"], currentkeycode);
                                currentkeycode = -1;
                            }
                        }
                    }
                    else {
                        bool enabled = component["enabled"];
                        if (ImGui::Checkbox(component["name"].get<std::string>().c_str(), &enabled))
                        {
                            component["enabled"] = enabled;
                            json opcodes = component["opcodes"];
                            for (auto& opcode : opcodes)
                            {
                                string addrStr = opcode["addr"];
                                string bytesStr = enabled ? opcode["on"] : opcode["off"];

                                uintptr_t address;
                                sscanf_s(addrStr.c_str(), "%x", &address);

                                DWORD base = (DWORD)GetModuleHandleA(0);
                                if (!opcode["lib"].is_null())
                                {
                                    base = (DWORD)GetModuleHandleA(string(opcode["lib"]).c_str());
                                }

                                hacks::writemem(base + address, bytesStr);
                            }

                            json references = component["references"];
                            for (auto& reference : references)
                            {
                                if (enabled && reference["on"].is_null())
                                {
                                    continue;
                                }

                                if (!enabled && reference["off"].is_null())
                                {
                                    continue;
                                }

                                string addrStr = reference["addr"];
                                string addRefStr = enabled ? reference["on"] : reference["off"];

                                uintptr_t address, refAdress;
                                sscanf_s(addrStr.c_str(), "%x", &address);
                                sscanf_s(addRefStr.c_str(), "%x", &refAdress);

                                DWORD baseAddr = (DWORD)GetModuleHandleA(0);
                                if (!reference["libAddr"].is_null() && !string(reference["libAddr"]).empty())
                                {
                                    baseAddr = (DWORD)GetModuleHandleA(string(reference["libAddr"]).c_str());
                                }

                                auto refSwitch = enabled ? reference["libON"] : reference["libOff"];

                                DWORD baseRef = (DWORD)GetModuleHandleA(0);
                                if (!refSwitch.is_null() && !string(refSwitch).empty())
                                {
                                    baseRef = (DWORD)GetModuleHandleA(string(refSwitch).c_str());
                                }

                                uintptr_t reference_address = baseAddr + address;
                                uintptr_t reference_value = baseRef + refAdress;

                                hacks::push_write(reference_address, reference_value);
                            }
                        }

                        if (ImGui::IsItemHovered() && component.contains("description"))
                            ImGui::SetTooltip(component["description"].get<std::string>().c_str());
                    }
                }
                else if (type == "text")
                {
                    std::string text = component["text"];
                    ImGui::Text(text.c_str());
                }
                else if (type == "text_url")
                {
                    std::string text = component["text"];
                    std::string url = component["url"];
                    if (ImGui::Button(text.c_str()))
                    {
                        ShellExecuteA(0, "open", url.c_str(), 0, 0, 0);
                    }

                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip(url.c_str());
                }
                else if (type == "speedhack")
                {
                    float value = component["value"];
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    if (ImGui::DragFloat("##speed", &value, 0.01f, 0, FLT_MAX, "Speed: %.2f"))
                    {
                        component["value"] = value;
                        engine.speed = value;
                    }
                }
                else if (type == "fps_bypass")
                {
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 25);
                    if (ImGui::DragFloat("##fps", &engine.fps, 1.00f, 1, 240.f, "FPS: %.2f"))
                    {
                        component["value"] = engine.fps;
                    }

                    ImGui::SameLine();
                    ImGui::Checkbox("##fps_enabled", &engine.fps_enabled);
                    ImGui::Checkbox("Real Time", &engine.realtime);
                }
                else if (type == "separator")
                {
                    ImGui::Separator();
                }
                else if (type == "startpos_checkbox")
                {
                    bool value = component["value"];
                    if (ImGui::Checkbox("StartPos Switcher", &value))
                    {
                        component["value"] = value;
                        startposSwitcher::setEnabled(value);
                    }
                }
                else if (type == "a_d_startpos") {
                    bool value = component["value"];
                    if (ImGui::Checkbox("Use A/D for StartPos Switcher ", &value))
                    {
                        component["value"] = value;
                        startposSwitcher::setAlternateKeys(value);
                    }
                }
                else if (type == "smart_startpos_checkbox")
                {
                    bool value = component["value"];
                    if (ImGui::Checkbox("Smart Startpos", &value))
                    {
                        component["value"] = value;
                        smartStartpos::enabled = value;
                    }
                }
                else if (type == "color_picker")
                {
                    color[0] = component["color"][0];
                    color[1] = component["color"][1];
                    color[2] = component["color"][2];
                    color[3] = component["color"][3];
                    setStyle(ImGui::GetStyle(), color);
                    if (ImGui::ColorEdit4("Menu Theme", (float*) &color))
                    {
                        json color_json = { color[0], color[1], color[2], color[3] };
                        component["color"] = color_json;
                        setStyle(ImGui::GetStyle(), color);
                    }
                    if (ImGui::Button("Reset Theme"))
                    {
                        color[0] = default_color[0];
                        color[1] = default_color[1];
                        color[2] = default_color[2];
                        color[3] = default_color[3];
                        json color_json = { color[0], color[1], color[2], color[3] };
                        component["color"] = color_json;
                        setStyle(ImGui::GetStyle(), color);
                    }
                }
                else if (type == "pmb_checkbox")
                {
                    bool value = component["value"];
                    if (ImGui::Checkbox(binding_mode ? "no shit" : "Practice Music Bypass", &value))
                    {
                        component["value"] = value;
                        hooks::musicUnlocker = value;
                    }
                }
                else if (type == "transitionCustomizerCBX")
                {
                    const char *e[] = {"Fade", "CrossFade", "Fade Bottom Left", "Fade Down", "Fade Top Right", "Fade Up", "Flip Angular", "Flip X", "Flip Y", "Jump Zoom", "Move In Bottom", "Move In Left", "Move In Right", "Move In Top", "Page Turn", "Progress Horizontal", "Progress In Out", "Progress Out In", "Progress Radial CWW", "Progress Radial CW", "Progress Vertical", "Rotation Zoom", "Shrink Grow", "Slider In Bottom", "Slider In Left", "Slider In Right", "Slider In Top", "Split Columns", "Split Rows", "Titles", "Zoom Flip Angular", "Zoom Flip X", "Zoom Flip Y"};
                    int value = component["value"];
                    if (ImGui::Combo("Transition", &value, e, 33, 5)) {
                        component["value"] = value;
                        hooks::transitionSelect = value;
                    }
                }
                else if (type == "keybinds_window")
                {
                    ImGui::Checkbox("Bind Mode", &binding_mode);
                    ImGui::SameLine();
                    if (ImGui::Button("Reset All Binds")) {
                        keybinds::binds = json();
                    }
                    
                }
                else if (type == "fade_speed") {
                    anim_durr = component["speed"];
                    if (ImGui::DragInt("Fade Speed", &anim_durr, 10.0f, 0, 10000, "%dms"))
                    {
                        component["speed"] = anim_durr;
                    }
                }
                else if (type == "replay_engine")
                {
                    static string log = "Record/Replay or Save/Load Macros!";
                    int mode = (int)engine.mode;

                    if (ImGui::RadioButton("Disable", &mode, 0))
                        engine.mode = state::disable;
                    ImGui::SameLine();

                    if (ImGui::RadioButton("Record", &mode, 1))
                    {
                        if (engine.mode != state::record)
                            engine.replay.clear();
                        engine.mode = state::record;
                    }
                    ImGui::SameLine();

                    if (ImGui::RadioButton("Play", &mode, 2))
                        engine.mode = state::play;

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    ImGui::InputText("##replay_name", engine.replay_name, IM_ARRAYSIZE(engine.replay_name));

                    if (ImGui::Button("Save", {60, NULL}))
                    {
                        if (engine.replay.empty())
                        {
                            log = "Replay doens't have actions";
                        }
                        else
                        {
                            if (isEmpty(engine.replay_name))
                            {
                                log = "Replay name is empty";
                            }
                            else
                            {
                                std::ofstream file("GDH/macros/" + (string)engine.replay_name + ".txt");

                                if (file.is_open())
                                {
                                    file << engine.fps << "\n";
                                    for (auto &action : engine.replay)
                                    {
                                        file << action.frame << " " << action.hold << " " << action.player_button << " " << action.player << "\n";
                                    }
                                    file.close();
                                    log = "Replay Saved";
                                }
                            }
                        }
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Load", {60, NULL}))
                    {
                        if (!engine.replay.empty())
                        {
                            log = "Please clear replay before loading another";
                        }
                        else
                        {
                            if (isEmpty(engine.replay_name))
                            {
                                log = "Replay name is empty";
                            }
                            else
                            {
                                std::ifstream file("GDH/macros/" + (string)engine.replay_name + ".txt");
                                std::string line;

                                if (file.is_open())
                                {
                                    std::getline(file, line);
                                    engine.fps = stof(line);
                                    while (std::getline(file, line))
                                    {
                                        std::istringstream iss(line);
                                        int value1, value2, value3, value4;

                                        if (iss >> value1 >> value2 >> value3 >> value4)
                                        {
                                            engine.replay.push_back({(unsigned)value1, (bool)value2, (int)value3, (bool)value4});
                                        }
                                    }
                                    file.close();
                                    log = "Replay Loaded";
                                }
                                else
                                {
                                    log = "Replay doens't exists";
                                }
                            }
                        }
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Clear Replay", {80, NULL}))
                    {
                        log = "Replay has been cleared";
                        engine.replay.clear();
                    }

                    ImGui::Separator();

                    ImGui::Text("Replay size: %i", engine.replay.size());
                    ImGui::Text("Frame: %i", hooks::frame);

                    ImGui::Separator();

                    ImGui::Text(log.c_str());
                }
            }
        }

        ImGui::End();
    }
    ImGui::End();
}

void gui::Toggle()
{
    if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - anim_starttime).count() > anim_durr) {
        gui::show = !gui::show;
        anim_starttime = std::chrono::steady_clock::now();
    }
}
