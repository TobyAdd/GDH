#define IMGUI_DEFINE_MATH_OPERATORS
#include "gui.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui-hook.hpp>
#include "hooks.hpp"
#include "hacks.hpp"
#include <shellapi.h>
#include <gl/GL.h>
#include "startposSwitcher.hpp"
#include "smartStartpos.hpp"

bool gui::show = false;
bool gui::inited = false;

bool secret = false;
bool startpos_switcher = false;
bool alternate_keys_for_startpos = false;
bool smart_startpos = false;

extern "C"
{
    __declspec(dllexport) int __stdcall GDH() {
        secret = true;
        return secret;
    };
}

void gui::Render()
{
    if (!gui::show)
        return;

    static std::vector<std::string> stretchedWindows;

    for (auto &item : hacks::content.items())
    {
        std::string windowName = item.key();

        if (std::find(stretchedWindows.begin(), stretchedWindows.end(), windowName) == stretchedWindows.end()) {
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
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    ImGui::DragFloat("##speed", &speed, 0.01f, 0, FLT_MAX, "Speed %.2f");
                }
                else if (type == "separator")
                {
                    ImGui::Separator();
                }                
                else if (type == "startpos_checkbox")
                {
                    if (ImGui::Checkbox("StartPos Switcher", &startpos_switcher)) {
                        startposSwitcher::setEnabled(startpos_switcher);
                    }
                    if (ImGui::Checkbox("Use A/D for StartPos Switcher ", &alternate_keys_for_startpos)) {
                        startposSwitcher::setAlternateKeys(alternate_keys_for_startpos);
                    }
                }
                else if (type == "smart_startpos_checkbox")
                {
                    if (ImGui::Checkbox("Smart Startpos", &smart_startpos)) {
                        smartStartpos::enabled = smart_startpos;
                    }
                }
            }
        }

        ImGui::End();
    }
}

void gui::Toggle()
{
    gui::show = !gui::show;
}
