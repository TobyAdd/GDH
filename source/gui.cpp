#define IMGUI_DEFINE_MATH_OPERATORS
#include "gui.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui-hook.hpp>
#include "hooks.h"
#include "hacks.h"
#include <shellapi.h>
#include "speedhackAudio.hpp"
#include "startposSwitcher.hpp"

bool gui::show = false;
bool gui::inited = false;

bool secret = false;

bool speedhack_audio = true;
bool startpos_switcher = false;

extern "C"
{
    __declspec(dllexport) int __stdcall GDH();
}

int __stdcall GDH()
{
    secret = true;
    return secret;
}

void gui::Render()
{
    if (!gui::show) return;

    if (!gui::inited) {
        gui::inited = true;
        ImGui::SetNextWindowSize({426, 240});
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));        
    }
    
    ImGui::Begin("GDH");
    if (ImGui::BeginTabBar("Tabs", ImGuiTabBarFlags_::ImGuiTabBarFlags_Reorderable))
	{        
        for (auto &item : hacks::content.items())
        {
            if (ImGui::BeginTabItem(item.key().c_str()))
            {
                ImGui::BeginChild("HacksChild");

                json &tabContent = item.value();

                for (size_t i = 0; i < tabContent.size(); i++)
                {
                    json &itemHack = tabContent.at(i);
                    bool enabled = itemHack["enabled"];

                    if (ImGui::Checkbox(itemHack["name"].get<string>().c_str(), &enabled))
                    {
                        itemHack["enabled"] = enabled;

                        json opcodes = itemHack["opcodes"];
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

                        ofstream outputFile("hacks.json");
                        outputFile << hacks::content.dump(4);
                        outputFile.close();
                    }
                }

                if (item.key() == "Player") {
                    if (ImGui::Checkbox("StartPos Switcher", &startpos_switcher)) {
                        startposSwitcher::setEnabled(startpos_switcher);
                    }

                    if (ImGui::DragFloat("##Speed", &speed, 0.01f, 0, FLT_MAX, "Speed: %.2f")) {
                        speedhackAudio::set(speed);
                    }

                    ImGui::SameLine();
                    if (ImGui::Checkbox("Audio", &speedhack_audio)) {
                        if (speedhack_audio) {
                            speedhackAudio::set(speed);
                        }
                        else {
                            speedhackAudio::set(1);
                        }
                    }
                }

                ImGui::EndChild();
                ImGui::EndTabItem();
            }
        }

        if (hacks::content.empty()) {
            if (ImGui::BeginTabItem("No hacks")){ 
                ImGui::Text("Download hacks.json and copy it to GD folder");
                if (ImGui::Button("Update")) {
                    hacks::load();
                }
                ImGui::EndTabItem(); 
            }
        }

        if (ImGui::BeginTabItem("About")){ 
            ImGui::Text("GDH v2.2 Beta for Geometry Dash 2.2");
            ImGui::Text("Created by TobyAdd");
            if (ImGui::MenuItem("Discord Server"))
                ShellExecuteA(0, "open", "https://discord.gg/ahYEz4MAwP", 0, 0, 0);

            if (secret) {
                ImGui::Text("You found a secret!");
            }

            ImGui::EndTabItem(); 
        }

		ImGui::EndTabBar();
	}
	ImGui::End();
}

void gui::Toggle()
{
    gui::show = !gui::show;
}
