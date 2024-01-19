#include "gui.hpp"
#include "console.hpp"
#include "content.hpp"
#include "memory.hpp"
#include "hooks.hpp"
#include "replayEngine.hpp"
#include "data.hpp"

bool gui::show = false;
bool gui::inited = false;

bool gui::license_accepted = false;

std::vector<std::string> stretchedWindows;
void gui::RenderMain() {
    if (!show) return;

    if (!license_accepted) {
        if (!inited) {
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowSize({480, 450});
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            inited = true;
        }


        ImGui::Begin("License agreement", 0, ImGuiWindowFlags_NoCollapse);

        ImGui::BeginChild("##LicenseChild", {0, ImGui::GetContentRegionAvail().y - 30});
        ImGui::Text("%s", license.c_str());
        ImGui::EndChild();

        ImGui::Separator();

        static bool agree = false;                
        ImGui::Checkbox("I have read and agree to the terms of the license agreement", &agree);
        ImGui::SameLine();

        ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 90);

        if (ImGui::Button(agree ? "Agree" : "Disagree", {80, NULL})) {
            if (agree) {
                license_accepted = true;
                Content::content["license_accepted"] = true;
            } else {
                exit(-1);
            }
        }

        ImGui::End();
        return;
    }

    ImGuiIO &io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0, io.DisplaySize.y), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
    ImGui::Begin("BottomRightText", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoFocusOnAppearing);
    ImGui::Text("GDH is a free and open-source cheat for Geometry Dash. If you paid for this software then you should ask for a refund.");
    ImGui::Text("GDH - это бесплатный мод меню с открытым исходным кодом для Geometry Dash. Если вы заплатили за это программное обеспечение, вам следует попросить вернуть деньги.");
    ImGui::End();

    for (auto &item : Content::content["Hacks"].items()) {
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
                bool enabled = component["enabled"];
                if (ImGui::Checkbox(component["name"].get<std::string>().c_str(), &enabled))
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
            }
            else if (type == "separator") {
                ImGui::Separator();
            }            
            else if (type == "fps_multiplier") {
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 25);
                ImGui::DragFloat("##fps", &engine.fps, 1.00f, 1, 240.f, "%.2f FPS");

                ImGui::SameLine();

                ImGui::Checkbox("##fps_enabled", &engine.fps_enabled);
                ImGui::Checkbox("Real Time", &engine.real_time);
            }            
            else if (type == "speedhack") {
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::DragFloat("##speed", &engine.speed, 0.01f, 0, FLT_MAX, "%.2fx");
            }
            else if (type == "transitionCustomizerCBX")
            {
                const char *e[] = {"Fade", "CrossFade", "Fade Bottom Left", "Fade Down", "Fade Top Right", "Fade Up", "Flip Angular", "Flip X", "Flip Y", "Jump Zoom", "Move In Bottom", "Move In Left", "Move In Right", "Move In Top", "Page Turn", "Progress Horizontal", "Progress In Out", "Progress Out In", "Progress Radial CWW", "Progress Radial CW", "Progress Vertical", "Rotation Zoom", "Shrink Grow", "Slider In Bottom", "Slider In Left", "Slider In Right", "Slider In Top", "Split Columns", "Split Rows", "Titles", "Zoom Flip Angular", "Zoom Flip X", "Zoom Flip Y"};
                int index = component["index"];
                if (ImGui::Combo("Transition", &index, e, 33, 5)) {
                    component["index"] = index;
                    hooks::transitionSelect = index;
                }
            }
            else if (type == "replay_engine")
            {
                static std::string log = "Record/Replay or Save/Load Macros!";
                int mode = (int)engine.mode;

                if (ImGui::RadioButton("Disable", &mode, 0))
                    engine.mode = state::disable;
                ImGui::SameLine();

                if (ImGui::RadioButton("Record", &mode, 1))
                {
                    if (engine.fps_enabled) {
                        if (engine.mode != state::record)
                            engine.replay.clear();
                        engine.mode = state::record;
                    }
                    else {
                        log = "Enable the FPS multiplier before recording";
                        engine.mode = state::disable;
                    }
                }
                ImGui::SameLine();

                if (ImGui::RadioButton("Play", &mode, 2))
                    engine.mode = state::play;

                ImGui::Separator();

                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::InputText("##replay_name", engine.replay_name, IM_ARRAYSIZE(engine.replay_name));

                if (ImGui::Button("Save", {60, NULL}))
                {
                    log = engine.save(engine.replay_name);
                }

                ImGui::SameLine();

                if (ImGui::Button("Load", {60, NULL}))
                {
                    log = engine.load(engine.replay_name);
                }

                ImGui::SameLine();

                if (ImGui::Button("Clear Replay", {80, NULL}))
                {
                    log = "The replay has been cleared";
                    engine.replay.clear();
                }

                ImGui::Separator();

                ImGui::Text("Replay size: %i", engine.replay.size());
                ImGui::Text("Frame: %i", hooks::frame);

                ImGui::Separator();

                ImGui::Text(log.c_str());
            }
        }

        ImGui::End();
    }
    
}

void gui::Toggle() {
    gui::show = !gui::show;
}

void gui::Unload() {
    stretchedWindows.clear();
    gui::show = false;
}