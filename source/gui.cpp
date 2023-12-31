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

bool gui::show = false;
bool gui::inited = false;

bool secret = false;

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

void gui::Render()
{
    if (!gui::show)
        return;

    static std::vector<std::string> stretchedWindows;
    // IMGUI pinkish colors
    static float color[4] = {0.2f, 0.15f, 0.25f, 1.0f};
    static float default_color[4] = {0.2f, 0.15f, 0.25f, 1.0f};

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
                    bool enabled = component["enabled"];
                    if (ImGui::Checkbox(component["name"].get<std::string>().c_str(), &enabled))
                    {
                        component["enabled"] = enabled;
                        json opcodes = component["opcodes"];
                        for (auto &opcode : opcodes)
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
                        for (auto &reference : references)
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
                    if (ImGui::MenuItem(text.c_str()))
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
                    if (ImGui::DragFloat("##speed", &value, 0.01f, 0, FLT_MAX, "Speed %.2f"))
                    {
                        component["value"] = value;
                        engine.speed = value;
                    }
                }
                else if (type == "fps_bypass")
                {
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 25);
                    if (ImGui::DragFloat("##fps", &engine.fps, 1.00f, 1, 240.f, "FPS %.2f"))
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
                if (type == "transparency_slider")
                {
                    color[3] = component["value"];
                    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(color[0], color[1], color[2], color[3]);
                    if (ImGui::SliderFloat("Transparency", &color[3], 0.0f, 1.0f))
                    {
                        ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(color[0], color[1], color[2], color[3]);
                        component["value"] = color[3];
                    }
                }
                if (type == "red_slider")
                {
                    color[0] = component["value"];
                    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(color[0], color[1], color[2], color[3]);
                    if (ImGui::SliderFloat("Red", &color[0], 0.0f, 1.0f))
                    {
                        ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(color[0], color[1], color[2], color[3]);
                        component["value"] = color[0];
                    }
                }
                else if (type == "green_slider")
                {
                    color[1] = component["value"];
                    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(color[0], color[1], color[2], color[3]);
                    if (ImGui::SliderFloat("Green", &color[1], 0.0f, 1.0f))
                    {
                        ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(color[0], color[1], color[2], color[3]);
                        component["value"] = color[1];
                    }
                }
                else if (type == "blue_slider")
                {
                    color[2] = component["value"];
                    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(color[0], color[1], color[2], color[3]);
                    if (ImGui::SliderFloat("Blue", &color[2], 0.0f, 1.0f))
                    {
                        ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(color[0], color[1], color[2], color[3]);
                        component["value"] = color[2];
                    }
                }
                if (type == "reset_button")
                {
                    if (ImGui::Button("Reset Theme"))
                    {
                        memcpy(color, default_color, sizeof(color));
                        ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(color[0], color[1], color[2], color[3]);
                        for (auto &component : components)
                        {
                            std::string type = component["type"];
                            if (type == "transparency_slider")
                            {
                                component["value"] = default_color[3];
                            }
                            else if (type == "red_slider")
                            {
                                component["value"] = default_color[0];
                            }
                            else if (type == "green_slider")
                            {
                                component["value"] = default_color[1];
                            }
                            else if (type == "blue_slider")
                            {
                                component["value"] = default_color[2];
                            }
                        }
                    }
                }
                else if (type == "pmb_checkbox")
                {
                    bool value = component["value"];
                    if (ImGui::Checkbox("Practice Music Bypass", &value))
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
    gui::show = !gui::show;
}
