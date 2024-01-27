#include "gui.hpp"
#include "console.hpp"
#include "content.hpp"
#include "memory.hpp"
#include "hooks.hpp"
#include <chrono>
#include "replayEngine.hpp"
#include "data.hpp"
#include "version.hpp"

bool gui::show = false;
bool gui::inited = false;

bool gui::message = false;
bool gui::best_run = false;
bool gui::ndeaths = false;
bool gui::tint = false;
bool gui::noclip = false;

char gui::custom_message[256];

bool gui::license_accepted = false;
bool license_show = false;

int style = 0;

int r_hue = 0;

bool rainbow_accent = false;
float ra_brightness = 1.0f;
float ra_saturation = 1.0f;

bool rainbow_text = false;
float rt_brightness = 1.0f;
float rt_saturation = 1.0f;

int anim_durr = -1;
auto anim_starttime = std::chrono::steady_clock::now();

ImVec4 colorMultiply(float color[3], float multiplier) {
    return ImVec4(std::clamp(color[0] * multiplier, 0.0f, 1.0f),
        std::clamp(color[1] * multiplier, 0.0f, 1.0f),
        std::clamp(color[2] * multiplier, 0.0f, 1.0f), 1.0f);
}

const char* styles[] = {"GDH Style", "Default Style" , "Default Style (overwrite colors)" };
int styles_length = 3;

void setStyle(ImGuiStyle& style, float accent[3], float text[3], int styleId)
{
    bool do_colors = false;

    switch (styleId) {
    case 0:
        style.WindowPadding = ImVec2(8.00f, 8.00f);
        style.FramePadding = ImVec2(4.00f, 4.00f);
        style.CellPadding = ImVec2(4.00f, 2.00f);
        style.ItemSpacing = ImVec2(4.00f, 4.00f);
        style.ItemInnerSpacing = ImVec2(4.00f, 4.00f);
        style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
        style.IndentSpacing = 21.00f;
        style.ScrollbarSize = 16.00f;
        style.GrabMinSize = 12.00f;
        style.WindowBorderSize = 1.00f;
        style.ChildBorderSize = 1.00f;
        style.PopupBorderSize = 1.00f;
        style.FrameBorderSize = 0.00f;
        style.TabBorderSize = 0.00f;
        style.WindowRounding = 6.00f;
        style.ChildRounding = 6.00f;
        style.FrameRounding = 6.00f;
        style.PopupRounding = 6.00f;
        style.ScrollbarRounding = 6.00f;
        style.GrabRounding = 6.00f;
        style.LogSliderDeadzone = 6.00f;
        style.TabRounding = 6.00f;
        style.WindowTitleAlign = ImVec2(0.50f, 0.50f);
        style.WindowMenuButtonPosition = 0;
        style.ColorButtonPosition = 1;
        style.ButtonTextAlign = ImVec2(0.50f, 0.50f);
        style.SelectableTextAlign = ImVec2(0.00f, 0.00f);
        style.DisplaySafeAreaPadding = ImVec2(3.00f, 3.00f);
        do_colors = true;
        break;
    case 1:
        ImGui::GetStyle().ImGuiStyle::ImGuiStyle();
        do_colors = true;
        break;
    case 2:
        ImGui::GetStyle().ImGuiStyle::ImGuiStyle();
        ImGui::StyleColorsDark();
        break;
    }

    if (do_colors) {
        ImVec4 color = colorMultiply(text, 1.0f);

        style.Colors[ImGuiCol_Text] = color;
        style.Colors[ImGuiCol_TextSelectedBg] = colorMultiply(accent, 0.8f);

        style.Colors[ImGuiCol_CheckMark] = color;

        style.Colors[ImGuiCol_WindowBg] = colorMultiply(accent, 0.8f);

        style.Colors[ImGuiCol_PopupBg] = colorMultiply(accent, 0.8f);

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
        style.Colors[ImGuiCol_ButtonHovered] = colorMultiply(accent, 0.4f);
        style.Colors[ImGuiCol_ButtonActive] = colorMultiply(accent, 0.6f);

        style.Colors[ImGuiCol_Separator] = colorMultiply(accent, 0.6f);

        style.Colors[ImGuiCol_ResizeGrip] = colorMultiply(accent, 1.0f);
        style.Colors[ImGuiCol_ResizeGripHovered] = colorMultiply(accent, 1.2f);
        style.Colors[ImGuiCol_ResizeGripActive] = colorMultiply(accent, 1.2f);

        style.Colors[ImGuiCol_Border] = colorMultiply(accent, 1.2f);

        style.Colors[ImGuiCol_Tab] = colorMultiply(accent, 0.8f);
        style.Colors[ImGuiCol_TabHovered] = colorMultiply(accent, 1.2f);
        style.Colors[ImGuiCol_TabActive] = colorMultiply(accent, 1.0f);
        style.Colors[ImGuiCol_TabUnfocused] = colorMultiply(accent, 0.6f);
        style.Colors[ImGuiCol_TabUnfocusedActive] = colorMultiply(accent, 0.6f);
    }
}

float get_opacity() {
    auto now = std::chrono::steady_clock::now();

    long long diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - anim_starttime).count();
    if (gui::show)
        return std::clamp((float)diff / (float)anim_durr, 0.0f, 1.0f);
    else
        return std::clamp(1.0f - ((float)diff / (float)anim_durr), 0.0f, 1.0f);
}

bool license_inited = false;
bool license_popup_inited = false;

std::vector<std::string> stretchedWindows;
void gui::RenderMain() {
    if (!inited) {
        if (!license_accepted) {
        }

        ImGui::GetStyle().Alpha = 0.0f;

        inited = true;
    }
    
    ImGui::GetStyle().Alpha = get_opacity();
    if (!show && ImGui::GetStyle().Alpha <= 0.0f) {
        return;
    }

    if (!license_accepted) {
        if (!license_inited) {
            ImGui::SetNextWindowSize({ 480, 450 });
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            license_inited = true;
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
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::Begin("BottomRightText", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoFocusOnAppearing);
    ImGui::Text("GDH is a free and open-source cheat for Geometry Dash. If you paid for this software then you should ask for a refund.");
    ImGui::Text("GDH - это бесплатное мод меню с открытым исходным кодом для Geometry Dash. Если вы заплатили за это программное обеспечение, вам следует попросить вернуть деньги.");
    ImGui::End();
    ImGui::PopStyleColor();

    if (license_show) {
        if (!license_popup_inited) {
            ImGui::SetNextWindowSize({ 480, 450 });
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            license_popup_inited = true;
        }

        ImGui::Begin("License", 0, ImGuiWindowFlags_NoCollapse);
        ImGui::BeginChild("##LicenseChild", { 0, ImGui::GetContentRegionAvail().y - 30 });
        ImGui::Text("%s", license.c_str());
        ImGui::EndChild();

        ImGui::Separator();
        if (ImGui::Button("Close")) {
            license_show = false;
            license_popup_inited = false;
        }

        ImGui::End();
    }

    static float color[3] = { 1.0f, 1.0f, 1.0f };
    static float text_color[3] = { 0.0f, 0.0f, 0.0f };

    static float default_color[3] = { 1.0f, 1.0f, 1.0f };
    static float default_text_color[3] = { 0.0f, 0.0f, 0.0f };

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
                    if (component["name"] == "NoClip" && enabled == true)
                    {
                        noclip = true;
                    } else if (component["name"] == "NoClip" && enabled == false)
                    {
                        noclip = false;
                    }
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
                if (component["name"] == "NoClip" && noclip == true)
                {
                    ImGui::Checkbox("Tint On Death", &tint);
                }
            }
            else if (type == "checkbox_startposSwitcher") {
                bool enabled = component["enabled"];
                bool use_a_d = component["use_a_d"];
                if (ImGui::Checkbox("Startpos switcher", &enabled)) {
                    component["enabled"] = enabled;
                    startPosSwitcher::enabled = enabled;
                }

                if (ImGui::Checkbox("Use A/D", &use_a_d)) {
                    component["use_a_d"] = use_a_d;
                    startPosSwitcher::use_a_d = use_a_d;
                }
            }            
            else if (type == "checkbox_layout_mode") {
                bool enabled = component["enabled"];
                if (ImGui::Checkbox("Layout Mode", &enabled)) {
                    component["enabled"] = enabled;
                    layout_mode::set_enabled(enabled);
                }
            }
            else if (type == "text") {
                ImGui::Text(component["text"].get<std::string>().c_str());
            }
            else if (type == "url") {
                if (ImGui::Button(component["text"].get<std::string>().c_str())) {
                    ShellExecuteA(0, "open", component["url"].get<std::string>().c_str(), 0, 0, 0);
                }

                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip(component["url"].get<std::string>().c_str());
            }
            else if (type == "debug_info") {
                bool debug;
                #ifndef DISABLE_CONSOLE
                    debug = true;
                #else
                    debug = false;
                #endif
                ImGui::Text("GDH %s; ImGui %s; DEBUG: %s", GDH_VERSION, IMGUI_VERSION, debug ? "ON" : "OFF");
            }
            else if (type == "separator") {
                ImGui::Separator();
            }
            else if (type == "separator_text") {
                ImGui::SeparatorText(component["text"].get<std::string>().c_str());
            }
            else if (type == "sameline") {
                ImGui::SameLine();
            }
            else if (type == "license") {
                if (ImGui::Button("License")) {
                    license_show = true;
                }
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
            else if (type == "color_picker") {
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
                style = component["style"];

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

                ImGui::Combo("Style", &style, styles, styles_length);

                json color_json = { color[0], color[1], color[2] };
                json text_color_json = { text_color[0], text_color[1], text_color[2] };
                json rainbow_json = { ra_brightness, ra_saturation };
                json text_rainbow_json = { rt_brightness, rt_saturation };
                json rainbow_enabled = { rainbow_accent, rainbow_text };
                component["style"] = style;
                component["color"] = color_json;
                component["text_color"] = text_color_json;
                component["rainbow"]["enabled"] = rainbow_enabled;
                component["rainbow"]["accent"] = rainbow_json;
                component["rainbow"]["text"] = text_rainbow_json;
                setStyle(ImGui::GetStyle(), color, text_color, style);
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
            else if (type == "labels") {
                // const char *e[] = {"Small", "Normal", "Large"};
                // int size = component["size"];
                // if (ImGui::Combo("Font Size", &size, e, IM_ARRAYSIZE(e), 3)) {
                //     component["size"] = size;
                    // current_font_size = (font_size)size;
                // }
                // const char *fontselecters[] = {"bigFont", "chatFont", "goldFont", "gjFont1", "gjFont2", "gjFont3", "gjFont4", "gjFont5", "gjFont6", "gjFont7", "gjFont8", "gjFont9", "gjFont10", "gjFont11", "gjFont12", "gjFont13", "gjFont14", "gjFont15", "gjFont16", "gjFont17", "gjFont18", "gjFont19", "gjFont20", "gjFont21", "gjFont22", "gjFont23", "gjFont24", "gjFont25", "gjFont26", "gjFont27", "gjFont28", "gjFont29", "gjFont30", "gjFont31", "gjFont32", "gjFont33", "gjFont34", "gjFont35", "gjFont36", "gjFont37", "gjFont38", "gjFont39", "gjFont40", "gjFont41", "gjFont42", "gjFont43", "gjFont44", "gjFont45", "gjFont46", "gjFont47", "gjFont48", "gjFont49", "gjFont50", "gjFont51", "gjFont52", "gjFont53", "gjFont54", "gjFont55", "gjFont56", "gjFont57", "gjFont58", "gjFont59"};
                // int font = component["font"];
                // if (ImGui::Combo("Font", &font, fontselecters, IM_ARRAYSIZE(fontselecters), 3)) {
                    // Console::WriteLine(to_string(font));
                    // component["font"] = font;
                    // string bim(fontselecters[font]);
                    // gui::fonttype = new char[((string) fontselecters[font] + ".fnt").length()];
                    // strcpy(gui::fonttype, ((string) fontselecters[font] + ".fnt").c_str());
                    // switch (font)
                    // {
                    // case 0:
                    //     gui::fonttype = "bigFont.fnt";
                    //     break;

                    // case 1:
                    //     gui::fonttype = "chatFont.fnt";
                    //     break;

                    // case 2:
                    //     gui::fonttype = "goldFont.fnt";
                    //     break;
                    
                    // default:
                    //     if (font > 2 && font < 62)
                    //     {
                    //         // strcpy_s(gui::fonttype, ("gjFont" + to_string(font - 2)).length() + 1, ("gjFont" + to_string(font - 2)).c_str());
                    //         gui::fonttype = new char[("gjFont" + to_string(font - 2) + ".fnt").length()];
                    //         strcpy(gui::fonttype, ("gjFont" + to_string(font - 2) + ".fnt")).c_str();
                    //         Console::WriteLine((string) (gui::fonttype));
                    //     }
                    //     else
                    //     {
                    //         Console::WriteLine("BRUH CODE ERROR");
                    //         gui::fonttype = "bigFont.fnt";
                    //     }
                    //     break;
                    // }
                    best_run = component["best_run"];
                    message = component["message"];
                    ndeaths = component["ndeaths"];
                    strcpy_s(custom_message, component["messageContent"].get<std::string>().c_str());;
                    ImGui::Checkbox("Best Run", &best_run);
                    ImGui::Checkbox("Message", &message);
                    if (message)
                        ImGui::InputText("##customMessage", (char*) &custom_message, 256);
                    ImGui::Checkbox("Noclip Deaths", &ndeaths);
                    component["message"] = message;
                    component["best_run"] = best_run;
                    component["ndeaths"] = ndeaths;
                    component["messageContent"] = custom_message;
                }
            }
        }
        

        ImGui::End();
    }
    


void gui::Toggle() {
    if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - anim_starttime).count() > anim_durr) {
        gui::show = !gui::show;
        anim_starttime = std::chrono::steady_clock::now();
    }
}

void gui::Unload() {
    stretchedWindows.clear();
    gui::show = false;
}