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

int r_hue = 0;

bool rainbow_accent = false;
float ra_brightness = 1.0f;
float ra_saturation = 1.0f;

bool rainbow_text = false;
float rt_brightness = 1.0f;
float rt_saturation = 1.0f;

int anim_durr = -1;
auto anim_starttime = std::chrono::steady_clock::now();
bool binding_mode = false;
std::string binding_now = "please dont create a hack with this name i will be very depressed";

bool gui::nc_acc = false;
bool gui::cps_counter = false;
bool gui::fps_label = false;
bool gui::message = false;
bool gui::noclip_deaths = false;

char gui::custom_message[256];

font_size current_font_size = font_size::small;

bool gui::confirm_exit = false;

bool gui::hacks_missing = false;

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

ImVec4 colorMultiply(float color[3], float multiplier) {
    return ImVec4(std::clamp(color[0] * multiplier, 0.0f, 1.0f),
                  std::clamp(color[1] * multiplier, 0.0f, 1.0f),
                  std::clamp(color[2] * multiplier, 0.0f, 1.0f), 1.0f);
}

void setStyle(ImGuiStyle &style, float accent[3], float text[3])
{
    /*
    float average = (accent[0] + accent[1] + accent[2]) / 3.0f;
    ImVec4 color = ImVec4(0.0f, 0.0f, 0.0f, accent[3]);;

    if (average < 0.5f) color = ImVec4(1.0f, 1.0f, 1.0f, accent[3]);
    */
    ImVec4 color = colorMultiply(text, 1.0f);

    style.Colors[ImGuiCol_Text] = color;
    style.Colors[ImGuiCol_TextSelectedBg] = colorMultiply(accent, 0.8f);

    style.Colors[ImGuiCol_CheckMark] = color;

    style.Colors[ImGuiCol_WindowBg] = colorMultiply(accent, 0.8f);

    style.Colors[ImGuiCol_FrameBg] = colorMultiply(accent, 0.6f);
    style.Colors[ImGuiCol_FrameBgHovered] = colorMultiply(accent, 0.4f);
    style.Colors[ImGuiCol_FrameBgActive] = colorMultiply(accent, 0.6f);

    style.Colors[ImGuiCol_TitleBg] = colorMultiply(accent, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = colorMultiply(accent, 1.0f);
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

    style.Colors[ImGuiCol_Border] = colorMultiply(accent, 0.6f);
}

float get_opacity() {
    auto now = std::chrono::steady_clock::now();

    long long diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - anim_starttime).count();
    if (gui::show)
        return (float)diff / (float)anim_durr;
    else
        return 1.0f - ((float)diff / (float)anim_durr);
}

bool opennedSP = false;
vector<string> replay_list;

void SelectReplay()
{
    auto itemRectMin = ImGui::GetItemRectMin();
    auto itemRectMax = ImGui::GetItemRectMax();
    auto itemRectSize = ImGui::GetItemRectSize();

    ImGui::SameLine();

    if (ImGui::ArrowButton("##comboopen", ImGuiDir_Down))
    {
        opennedSP = true;
        replay_list.clear();
        for (const auto &entry : filesystem::directory_iterator("GDH/macros"))
        {
            string replay = entry.path().filename().string();
            if (replay.substr(replay.size() - 4, replay.size()) == ".txt")
            {
                replay_list.push_back(entry.path().filename().string().erase(replay.size() - 4, replay.size()));
            }
        }
    }

    if (opennedSP)
    {
        ImGui::SetNextWindowPos(ImVec2(itemRectMin.x, itemRectMax.y + 4));
        ImGui::SetNextWindowSize(ImVec2(itemRectSize.x + ImGui::GetItemRectSize().x + 5, 150));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1);
        ImGui::Begin("##replaylist", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        if (!ImGui::IsWindowFocused()) {
            opennedSP = false;
        }
        for (int i = 0; i < (int)replay_list.size(); i++)
        {
            if (ImGui::MenuItem(replay_list[i].c_str()))
            {
                strcpy_s(engine.replay_name, replay_list[i].c_str());
                opennedSP = false;
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }
}

void MetaRender()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1);
    ImGui::Begin("Meta", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
    if (startposSwitcher::playLayer != nullptr)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 200).Value);
        
        if (current_font_size == font_size::small) {
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        }
        else if (current_font_size == font_size::normal) {
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
        }        
        else if (current_font_size == font_size::large) {
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
        }

        if (gui::nc_acc)
            ImGui::Text("%.2f%%", hooks::noclip_accuracy.getPercentage()); 

        if (gui::cps_counter)
            ImGui::Text("%i/%i", hooks::cps_counter.cps, hooks::cps_counter.highscore);     

        if (gui::fps_label)
            ImGui::Text("%.2f FPS", ImGui::GetIO().Framerate);

        if (gui::message)
            ImGui::Text("%s", gui::custom_message);

        if (gui::noclip_deaths)
            ImGui::Text("%d death%s", hooks::noclip_accuracy.deaths_full, hooks::noclip_accuracy.deaths_full == 1 ? "" : "s");

        ImGui::PopFont();
        ImGui::PopStyleColor();
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

std::vector<std::string> stretchedWindows;

void gui::Render()
{
    MetaRender();
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1);

    if (gui::confirm_exit) {
        ImGui::OpenPopup("ConfirmExitPopup");
        gui::confirm_exit = false;
    }

    if (ImGui::BeginPopupModal("ConfirmExitPopup", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to exit the level?");
        
        ImGui::Separator();

        if (ImGui::Button("Yes")) {
            hooks::playLayer_exit(startposSwitcher::playLayer);
            hooks::frame_advance = false;
            ImGui::CloseCurrentPopup();
        }        
        
        ImGui::SameLine();
               
        if (ImGui::Button("No")) {
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }


    ImGui::PopStyleVar();

    if (!gui::inited) {
        gui::inited = true;
        ImGui::GetStyle().Alpha = 0;
    }

    ImGui::GetStyle().Alpha = get_opacity();
    if (!gui::show && ImGui::GetStyle().Alpha <= 0.0f) {
        return;
    }

    if (!hacks_missing) {
        ImGuiIO &io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(0, io.DisplaySize.y), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
        ImGui::Begin("BottomRightText", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::Text("hacks.json is missing. Please reinstall GDH as it is broken.");
        ImGui::End();
    }

    // IMGUI pinkish colors
    static float color[4] = { 0.337f, 0.176f, 0.464f };
    static float default_color[4] = { 0.337f, 0.176f, 0.464f };
    static float text_color[4] = { 1.0f, 1.0f, 1.0f };
    static float default_text_color[4] = { 1.0f, 1.0f, 1.0f };

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

        // if (!ImGui::IsWindowCollapsed()) {
        //     item.value()["windowSize"]["width"] = ImGui::GetWindowSize().x;
        //     item.value()["windowSize"]["height"] = ImGui::GetWindowSize().y;
        // }

        // item.value()["windowPosition"]["x"] = ImGui::GetWindowPos().x;
        // item.value()["windowPosition"]["y"] = ImGui::GetWindowPos().y;


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
                                            component.erase("bind");
                                        }
                                    }
                                }
                                else
                                    keybinds::AddKeybind(component["name"], currentkeycode, &component);
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

                    bool enabled = component["enabled"];
                    if (ImGui::Checkbox("##fps_enabled", &enabled)) {
                        engine.fps_enabled = enabled;
                        component["enabled"] = enabled;
                    }

                    bool real_time_enabled = component["real_time"];
                    if (ImGui::Checkbox("Real Time", &real_time_enabled)) {
                        engine.realtime = real_time_enabled;
                        component["real_time"] = real_time_enabled;
                    }
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
                    rainbow_accent = component["rainbow"]["enabled"][0];
                    rainbow_text = component["rainbow"]["enabled"][1];
                    ra_brightness = component["rainbow"]["accent"][0];
                    ra_saturation = component["rainbow"]["accent"][1];
                    rt_brightness = component["rainbow"]["text"][0];
                    rt_saturation = component["rainbow"]["text"][1];
                    color[0] = component["color"][0];
                    color[1] = component["color"][1];
                    color[2] = component["color"][2];
                    text_color[0] = component["text_color"][0];
                    text_color[1] = component["text_color"][1];
                    text_color[2] = component["text_color"][2];
                    setStyle(ImGui::GetStyle(), color, text_color);

                    ImGui::Checkbox("##rainbowAccent", &rainbow_accent);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Rainbow Color");

                    ImGui::SameLine();

                    r_hue = (r_hue + 1) % 360;
                    if (rainbow_accent) {
                        ImGui::SetNextItemWidth(95.0f);
                        ImGui::DragFloat("##raBrightness", &ra_brightness, 0.01f, 0.0f, 1.0f, "Brightness: %.2f");
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(95.0f);
                        ImGui::DragFloat("##raSaturation", &ra_saturation, 0.01f, 0.0f, 1.0f, "Saturation: %.2f");
                        ImGui::SameLine();
                        ImGui::Text("Accent");
                        float r, g, b;
                        ImGui::ColorConvertHSVtoRGB(r_hue / 360.0f, ra_saturation, ra_brightness, r, g, b);
                        color[0] = r;
                        color[1] = g;
                        color[2] = b;
                    }
                    else {
                        ImGui::ColorEdit3("Accent", (float*)&color);
                    }

                    ImGui::Checkbox("##rainbowText", &rainbow_text);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Rainbow Color");

                    ImGui::SameLine();

                    if (rainbow_text) {
                        ImGui::SetNextItemWidth(95.0f);
                        ImGui::DragFloat("##rtBrightness", &rt_brightness, 0.01f, 0.0f, 1.0f, "Brightness: %.2f");
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(95.0f);
                        ImGui::DragFloat("##rtSaturation", &rt_saturation, 0.01f, 0.0f, 1.0f, "Saturation: %.2f");
                        ImGui::SameLine();
                        ImGui::Text("Text");
                        float r, g, b;
                        ImGui::ColorConvertHSVtoRGB(r_hue / 360.0f, rt_saturation, rt_brightness, r, g, b);
                        text_color[0] = r;
                        text_color[1] = g;
                        text_color[2] = b;
                    }
                    else {
                        ImGui::ColorEdit3("Text", (float*)&text_color);
                    }

                    if (ImGui::Button("Reset Theme"))
                    {
                        rainbow_text = false;
                        rainbow_accent = false;
                        color[0] = default_color[0];
                        color[1] = default_color[1];
                        color[2] = default_color[2];
                        text_color[0] = default_text_color[0];
                        text_color[1] = default_text_color[1];
                        text_color[2] = default_text_color[2];
                    }

                    json color_json = { color[0], color[1], color[2] };
                    json text_color_json = { text_color[0], text_color[1], text_color[2] };
                    json rainbow_json = {ra_brightness, ra_saturation};
                    json text_rainbow_json = {rt_brightness, rt_saturation};
                    json rainbow_enabled = {rainbow_accent, rainbow_text};
                    component["color"] = color_json;
                    component["text_color"] = text_color_json;
                    component["rainbow"]["enabled"] = rainbow_enabled;
                    component["rainbow"]["accent"] = rainbow_json;
                    component["rainbow"]["text"] = text_rainbow_json;
                    setStyle(ImGui::GetStyle(), color, text_color);
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
                else if (type == "ekj_checkbox")
                {
                    bool value = component["value"];
                    if (ImGui::Checkbox("Every Key Jumps", &value))
                    {
                        component["value"] = value;
                        hooks::g_enabledEKG = value;
                    }
                }
                else if (type == "acoinpick_checkbox")
                {
                    bool value = component["value"];
                    if (ImGui::Checkbox("Auto Pickup Coins", &value))
                    {
                        component["value"] = value;
                        hooks::g_enabledAutoPickupCoins = value;
                    }
                }
                else if (type == "noeffectcirle_checkbox")
                {
                    bool value = component["value"];
                    if (ImGui::Checkbox("No Effect Circle", &value))
                    {
                        component["value"] = value;
                        hooks::g_enabledNoEffectCircle = value;
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

                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 25);
                    ImGui::InputText("##replay_name", engine.replay_name, IM_ARRAYSIZE(engine.replay_name));
                    SelectReplay();

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
                else if (type == "confirm_exit") {
                    bool value = component["value"];
                    if (ImGui::Checkbox("Confirm Exit", &value)) {
                        component["value"] = value;
                        hooks::confirm_exit = value;
                    }
                }      
                else if (type == "labels") {
                    const char *e[] = {"Small", "Normal", "Large"};
                    int size = component["size"];
                    if (ImGui::Combo("Font Size", &size, e, IM_ARRAYSIZE(e), 3)) {
                        component["size"] = size;
                        current_font_size = (font_size)size;
                    }
                    nc_acc = component["noclip_acc"];
                    cps_counter = component["cps"];
                    fps_label = component["fps"];
                    message = component["message"];
                    strcpy_s(custom_message, component["messageContent"].get<std::string>().c_str());
                    noclip_deaths = component["noclip_deaths"];
                    ImGui::Checkbox("Noclip Accuracy", &nc_acc);
                    ImGui::Checkbox("CPS Counter", &cps_counter);
                    ImGui::Checkbox("FPS Label", &fps_label);
                    ImGui::Checkbox("Message", &message);
                    if (message)
                        ImGui::InputText("##customMessage", (char*) &custom_message, 256);
                    ImGui::Checkbox("Noclip Deaths", &noclip_deaths);
                    component["noclip_acc"] = nc_acc;
                    component["cps"] = cps_counter;
                    component["fps"] = fps_label;
                    component["message"] = message;
                    component["messageContent"] = custom_message;
                    component["noclip_deaths"] = noclip_deaths;
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

void gui::UnLoad()
{
    stretchedWindows.clear();
    gui::show = false;
}
