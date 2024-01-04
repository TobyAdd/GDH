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

bool isEmpty(char *str)
{
    return (str != NULL && str[0] == '\0');
}

bool gui::show = false;
bool gui::inited = false;
bool gui::recording = true;

int gui::currentkeycode = -1;
int oldkeycode;

int anim_durr = -1;
auto anim_starttime = std::chrono::steady_clock::now();

bool binding_mode = false;
std::string binding_now = "please dont create a hack with this name i will be very depressed";

std::vector<std::string> filtered, styles_filtered;

int replayEditor_index = 0, editorPlayer_index = 0;
int style_index = 0;

vector<std::string> styles;

static const char* replayEditor_selected = NULL;
char *replayEditor_items[] = { "Up", "Left", "Right" };

char replay_name[128];
char sequence_replay[128];
char style_name[128];

template<typename T>
T checkProperty(const json& j, const std::string& key, const T& defaultValue) {
	if (j.contains(key) && !j[key].is_null()) {
		return j[key].get<T>();
	}
	return defaultValue;
}

void gui::import_style(std::string style) {
    if (style.empty())
    {
        ImGui::InsertNotification({ ImGuiToastType_Error, 5000, "Style name is empty!" });
    }
    else
    {
        std::ifstream file("GDH/styles/" + style + ".json");

        if (file.is_open())
        {
            json content;
        
            try
            {
                file >> content;
            }
            catch (const std::exception& error)
            {
                ImGui::InsertNotification({ ImGuiToastType_Error, 5000, "Failed to parse JSON: \"%s\"", error.what() });
                file.close();
                return;
            }
            
            ImGuiStyle& style = ImGui::GetStyle();

            ImGui::GetIO().FontGlobalScale = content["GlobalScale"];
            style.FramePadding.x = content["FramePaddingX"];
            style.FramePadding.y = content["FramePaddingY"];
            style.CellPadding.x = content["CellPaddingX"];
            style.CellPadding.y = content["CellPaddingY"];
            style.ItemSpacing.x = content["ItemSpacingX"];
            style.ItemSpacing.y = content["ItemSpacingY"];
            style.ItemInnerSpacing.x = content["ItemInnerSpacingX"];
            style.ItemInnerSpacing.y = content["ItemInnerSpacingY"];
            style.TouchExtraPadding.x = content["TouchExtraPaddingX"];
            style.TouchExtraPadding.y = (float)content["TouchExtraPaddingY"];
            style.WindowPadding.x = (float)checkProperty(content, "WindowPaddingX", 8);
            style.WindowPadding.y = (float)checkProperty(content, "WindowPaddingY", 8);
            style.IndentSpacing = content["IndentSpacing"];
            style.ScrollbarSize = content["ScrollbarSize"];
            style.GrabMinSize = content["GrabMinSize"];
            style.WindowBorderSize = content["WindowBorderSize"];
            style.ChildBorderSize = content["ChildBorderSize"];
            style.PopupBorderSize = content["PopupBorderSize"];
            style.FrameBorderSize = content["FrameBorderSize"];
            style.TabBorderSize = content["TabBorderSize"];
            style.WindowRounding = content["WindowRounding"];
            style.ChildRounding = content["ChildRounding"];
            style.FrameRounding = content["FrameRounding"];
            style.PopupRounding = content["PopupRounding"];
            style.ScrollbarRounding = content["ScrollbarRounding"];
            style.GrabRounding = content["GrabRounding"];
            style.LogSliderDeadzone = content["LogSliderDeadzone"];
            style.TabRounding = content["TabRounding"];
            style.WindowTitleAlign.x = content["WindowTitleAlignX"];
            style.WindowTitleAlign.y = content["WindowTitleAlignY"];
            style.WindowMenuButtonPosition = content["WindowMenuButtonPosition"];
            style.ColorButtonPosition = content["ColorButtonPosition"];
            style.ButtonTextAlign.x = content["ButtonTextAlignX"];
            style.ButtonTextAlign.y = content["ButtonTextAlignY"];
            style.SelectableTextAlign.x = content["SelectableTextAlignX"];
            style.SelectableTextAlign.y = content["SelectableTextAlignY"];
            style.DisplaySafeAreaPadding.x = content["DisplaySafeAreaPaddingX"];
            style.DisplaySafeAreaPadding.y = content["DisplaySafeAreaPaddingY"];
            style.AntiAliasedLines = content["AntiAliasedLines"];
            style.AntiAliasedLinesUseTex = content["AntiAliasedLinesUseTex"];
            style.AntiAliasedFill = content["AntiAliasedFill"];
            style.CurveTessellationTol = content["CurveTessellationTol"];
            style.CircleTessellationMaxError = (float)content["CircleTessellationMaxError"];
            style.Alpha = (float)min(15, content["Alpha"]);
            style.DisabledAlpha = content["DisabledAlpha"];

            for (int i = 0; i < ImGuiCol_COUNT; i++)
            {
                std::string colorName = ImGui::GetStyleColorName(i);

                if (content.contains(colorName))
                {
                    auto colorArray = content[colorName];
                    ImVec4 color(colorArray[0], colorArray[1], colorArray[2], colorArray[3]);
                    style.Colors[i] = color;
                }
            }
        }
        else
        {
            ImGui::InsertNotification({ ImGuiToastType_Error, 5000, "Failed to read: \"%s\" (%s)", "GDH/styles/" + style + ".json", std::system_category().message(GetLastError()) });
            return;
        }
        
        StylesList();
        file.close();
    }
}

void gui::export_style(std::string path) {
	ImGuiStyle& style = ImGui::GetStyle();
	json content;

	content["GlobalScale"] = ImGui::GetIO().FontGlobalScale;
	content["FramePaddingX"] = style.FramePadding.x;
	content["FramePaddingY"] = style.FramePadding.y;
	content["CellPaddingX"] = style.CellPadding.x;
	content["CellPaddingY"] = style.CellPadding.y;
	content["ItemSpacingX"] = style.ItemSpacing.x;
	content["ItemSpacingY"] = style.ItemSpacing.y;
	content["ItemInnerSpacingX"] = style.ItemInnerSpacing.x;
	content["ItemInnerSpacingY"] = style.ItemInnerSpacing.y;
	content["TouchExtraPaddingX"] = style.TouchExtraPadding.x;
	content["TouchExtraPaddingY"] = style.TouchExtraPadding.y;
	content["WindowPaddingX"] = style.WindowPadding.x;
	content["WindowPaddingY"] = style.WindowPadding.y;
	content["IndentSpacing"] = style.IndentSpacing;
	content["ScrollbarSize"] = style.ScrollbarSize;
	content["GrabMinSize"] = style.GrabMinSize;
	content["WindowBorderSize"] = style.WindowBorderSize;
	content["ChildBorderSize"] = style.ChildBorderSize;
	content["PopupBorderSize"] = style.PopupBorderSize;
	content["FrameBorderSize"] = style.FrameBorderSize;
	content["TabBorderSize"] = style.TabBorderSize;
	content["WindowRounding"] = style.WindowRounding;
	content["ChildRounding"] = style.ChildRounding;
	content["FrameRounding"] = style.FrameRounding;
	content["PopupRounding"] = style.PopupRounding;
	content["ScrollbarRounding"] = style.ScrollbarRounding;
	content["GrabRounding"] = style.GrabRounding;
	content["LogSliderDeadzone"] = style.LogSliderDeadzone;
	content["TabRounding"] = style.TabRounding;
	content["WindowTitleAlignX"] = style.WindowTitleAlign.x;
	content["WindowTitleAlignY"] = style.WindowTitleAlign.x;
	content["WindowMenuButtonPosition"] = style.WindowMenuButtonPosition;
	content["ColorButtonPosition"] = style.ColorButtonPosition;
	content["ButtonTextAlignX"] = style.ButtonTextAlign.x;
	content["ButtonTextAlignY"] = style.ButtonTextAlign.y;
	content["SelectableTextAlignX"] = style.SelectableTextAlign.x;
	content["SelectableTextAlignY"] = style.SelectableTextAlign.y;
	content["DisplaySafeAreaPaddingX"] = style.DisplaySafeAreaPadding.x;
	content["DisplaySafeAreaPaddingY"] = style.DisplaySafeAreaPadding.y;
	content["AntiAliasedLines"] = style.AntiAliasedLines;
	content["AntiAliasedLinesUseTex"] = style.AntiAliasedLinesUseTex;
	content["AntiAliasedFill"] = style.AntiAliasedFill;
	content["CurveTessellationTol"] = style.CurveTessellationTol;
	content["CircleTessellationMaxError"] = style.CircleTessellationMaxError;
	content["Alpha"] = min(15, style.Alpha);
	content["DisabledAlpha"] = style.DisabledAlpha;

	for (int i = 0; i < ImGuiCol_COUNT; i++)
	{
		ImVec4 color = ImGui::GetStyleColorVec4(i);
		std::string colorName = ImGui::GetStyleColorName(i);
		content[colorName] = { color.x, color.y, color.z, color.w };
	}

	std::ofstream file("GDH\\styles\\" + path + ".json");
	
    if (file.is_open())
	{
		file << content.dump(4);
        file.close();
	}

    StylesList();
}

void SeparatedText(const char* text)
{
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 textSize = ImGui::CalcTextSize(text);

    float xPosition = (windowSize.x - textSize.x) * 0.5f;

    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
    ImGui::SetCursorPosX(xPosition - 25); ImGui::Text(text);
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
}

std::string getFilenameFromFullPath(std::string str)
{
    int i; std::string file;

    for(i = str.length(); (str[i] != '\\') && (str[i] != '/') && i >= 0; --i);

    while (str[++i] != '\0')
        file += str[i];

    return file;
}

std::string eraseExtension(std::string str, const std::string tail)
{
    size_t offset = str.find_last_of(tail);

    if(offset != std::string::npos && offset == str.length()-1)
        str.erase(offset-tail.length()+1, tail.length());

    return str;
}

std::string getFileExt(const std::string& str) {
    size_t i = str.rfind('.', str.length());

    if (i != std::string::npos)
        return (str.substr(i+1, str.length() - i));

    return ("");
}

void gui::ReplaysList()
{
    filtered.clear();

    for (auto& folder : std::filesystem::directory_iterator(std::filesystem::current_path().string() + "\\GDH\\replays\\"))
    {
        if (getFileExt(folder.path().string()) == "txt")
            filtered.push_back(eraseExtension(getFilenameFromFullPath(folder.path().string()), ".txt"));
    }
}

void gui::StylesList()
{
    styles_filtered.clear();

    for (auto& folder : std::filesystem::directory_iterator(std::filesystem::current_path().string() + "\\GDH\\styles\\"))
    {
        if (getFileExt(folder.path().string()) == "json") {
            styles_filtered.push_back(eraseExtension(getFilenameFromFullPath(folder.path().string()), ".json"));
        }
    }
}

float get_opacity() {
    auto now = std::chrono::steady_clock::now();

    long long diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - anim_starttime).count();
    if (gui::show)
        return (float)diff / (float)anim_durr;
    else
        return 1.0f - ((float)diff / (float)anim_durr);
}

void gui::Init()
{
    ImGuiIO* io = &ImGui::GetIO();

    ImFontConfig font_cfg;
    font_cfg.FontDataOwnedByAtlas = false;

    // io->Fonts->AddFontFromMemoryTTF((void*)tahoma, sizeof(tahoma), 17.f, &font_cfg);

    ImGui::MergeIconsWithLatestFont(16.f, false);
}

void gui::Render()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImGuiIO& io = ImGui::GetIO();

    if (!gui::inited) {
        ReplaysList();
        StylesList();

        gui::inited = true;
        style.Alpha = 0;
    }

    ImGui::GetStyle().Alpha = get_opacity();
    if (!gui::show && ImGui::GetStyle().Alpha <= 0.0f) {
        return;
    }

    static std::vector<std::string> stretchedWindows;

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
                            keybind = "Press key";

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
                else if (type == "button_url")
                {
                    std::string text = component["text"];
                    std::string url = component["url"];

                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
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
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    if (ImGui::DragFloat("##fps", &engine.fps, 1.00f, 1, 240.f, "FPS: %.2f"))
                    {
                        component["value"] = engine.fps;
                    }

                    ImGui::Checkbox("Enable Bypass##fps_enabled", &engine.fps_enabled); ImGui::SameLine();
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
                else if (type == "theme_editor")
                {
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    ImGui::InputText("##engine.style_name", style_name, IM_ARRAYSIZE(style_name), ImGuiInputTextFlags_EnterReturnsTrue);

                    if (ImGui::IsItemActivated())
                        ImGui::OpenPopup("##styles_filtered_popup");
                        
                    {
                        ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
                        ImGui::SetNextWindowSize({ ImGui::GetItemRectSize().x, 0 });
                        if (ImGui::BeginPopup("##styles_filtered_popup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ChildWindow))
                        {
                            for (size_t i = 0; i < styles_filtered.size(); i++)
                            {
                                if (strstr(styles_filtered[i].c_str(), style_name) == NULL)
                                    continue;
                                    
                                if (ImGui::Selectable(styles_filtered[i].c_str()))
                                {
                                    ImGui::ClearActiveID();
                                    strcpy_s(style_name, styles_filtered[i].c_str());
                                }
                            }

                            if (!ImGui::IsItemActive() && !ImGui::IsWindowFocused())
                                ImGui::CloseCurrentPopup();

                            ImGui::EndPopup();
                        }
                    }

                    if (ImGui::Button("Save", {75, NULL}))
                    {
                        export_style(style_name);
                    }

                    ImGui::SameLine();
                    
                    if (ImGui::Button("Load", {75, NULL}))
                    {
                        import_style(style_name);
                    }

                    ImGui::SameLine(); ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical | ImGuiSeparatorFlags_SpanAllColumns); ImGui::SameLine();

                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    ImGui::DragFloat("Scale", &io.FontGlobalScale, 0.01, 0.01, 3.0, "Size: %.1f");
                    
                    ImGui::Separator();

                    if (ImGui::BeginTabBar("##theme_editor_tabs", ImGuiTabBarFlags_None))
                    {
                        if (ImGui::BeginTabItem("Colors"))
                        {
                            static ImGuiTextFilter filter;

                            filter.Draw("Filter colors", ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("Filter colors").x - 10);

                            static ImGuiColorEditFlags alpha_flags = 0;

                            ImGui::BeginChild("##colors", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true, ImGuiWindowFlags_AlwaysAutoResize); ImGui::PushItemWidth(250);

                            for (int i = 0; i < ImGuiCol_COUNT; i++) {
                                const char* name = ImGui::GetStyleColorName(i);
                                if (!filter.PassFilter(name))
                                    continue;
                                    
                                ImGui::PushID(i);

                                if (ImGui::ColorButton(name, style.Colors[i])) {
                                    ImGui::OpenPopup("ColorPickerPopup");
                                }

                                if (ImGui::BeginPopup("ColorPickerPopup")) {
                                    ImGui::ColorPicker4(name, (float*)&style.Colors[i]);
                                    ImGui::EndPopup();
                                }


                                ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
                                ImGui::TextUnformatted(name);
                                ImGui::PopID();
                            }

                            ImGui::PopItemWidth();

                            ImGui::EndChild();
                            ImGui::EndTabItem();
                        }

                        if (ImGui::BeginTabItem("Sizes"))
                        {
                            ImGui::BeginChild("##sizes", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true, ImGuiWindowFlags_AlwaysAutoResize); ImGui::PushItemWidth(250);

                            SeparatedText("Main");
                            
                            ImGui::PushItemWidth(200);
                            ImGui::SliderFloat2("Window Padding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
                            ImGui::SliderFloat2("Frame Padding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
                            ImGui::SliderFloat2("Cell Padding", (float*)&style.CellPadding, 0.0f, 20.0f, "%.0f");
                            ImGui::SliderFloat2("Item Spacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
                            ImGui::SliderFloat2("Item-Inner Spacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
                            ImGui::SliderFloat2("Touch Extra Padding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
                            ImGui::SliderFloat("Indent Spacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
                            ImGui::SliderFloat("Scroll-bar Size", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
                            ImGui::SliderFloat("Grab Min Size", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");

                            SeparatedText("Borders");

                            ImGui::SliderFloat("Window Border Size", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
                            ImGui::SliderFloat("Child Border Size", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
                            ImGui::SliderFloat("Popup Border Size", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
                            ImGui::SliderFloat("Frame Border Size", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
                            ImGui::SliderFloat("Tab Border Size", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");

                            SeparatedText("Rounding");
                            
                            ImGui::SliderFloat("Window Rounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
                            ImGui::SliderFloat("Child Rounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
                            ImGui::SliderFloat("Frame Rounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
                            ImGui::SliderFloat("Popup Rounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
                            ImGui::SliderFloat("Scroll-bar Rounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
                            ImGui::SliderFloat("Grab Rounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
                            ImGui::SliderFloat("Log-Slider Deadzone", &style.LogSliderDeadzone, 0.0f, 12.0f, "%.0f");
                            ImGui::SliderFloat("Tab-Rounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");

                            SeparatedText("Alignment");

                            ImGui::DragFloat2("Window Title Align", (float*)&style.WindowTitleAlign, 0.01f, 0.0f, 1.0f, "%.2f");

                            int window_menu_button_position = style.WindowMenuButtonPosition + 1;
                            if (ImGui::Combo("Window Button Position", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
                                style.WindowMenuButtonPosition = window_menu_button_position - 1;

                            ImGui::Combo("Color Button Position", (int*)&style.ColorButtonPosition, "Left\0Right\0");
                            ImGui::DragFloat2("Button Text Align", (float*)&style.ButtonTextAlign, 0.01f, 0.0f, 1.0f, "%.2f");
                            ImGui::DragFloat2("Select Text Align", (float*)&style.SelectableTextAlign, 0.01f, 0.0f, 1.0f, "%.2f");

                            SeparatedText("Safe Area Padding");

                            ImGui::SliderFloat2("Safe Padding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");

                            ImGui::PopItemWidth();

                            ImGui::EndChild();
                            ImGui::EndTabItem();
                        }

                        ImGui::EndTabBar();
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
                    ImGui::InputText("##engine.replay_name", replay_name, IM_ARRAYSIZE(replay_name), ImGuiInputTextFlags_EnterReturnsTrue);

                    if (ImGui::IsItemActivated())
                        ImGui::OpenPopup("##filtered_popup");
                        
                    {
                        ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
                        ImGui::SetNextWindowSize({ ImGui::GetItemRectSize().x, 0 });
                        if (ImGui::BeginPopup("##filtered_popup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ChildWindow))
                        {
                            for (size_t index = 0; index < filtered.size(); index++)
                            {
                                if (strstr(filtered[index].c_str(), replay_name) == NULL)
                                    continue;

                                if (ImGui::Selectable(filtered[index].c_str()))
                                {
                                    ImGui::ClearActiveID();
                                    strcpy_s(replay_name, filtered[index].c_str());
                                }
                            }

                            if (!ImGui::IsItemActive() && !ImGui::IsWindowFocused())
                                ImGui::CloseCurrentPopup();

                            ImGui::EndPopup();
                        }
                    }

                    if (ImGui::Button("Save", {85, NULL}))
                    {
                        engine.save(replay_name);
                    }

                    ImGui::SameLine();
                    if (ImGui::Button("Load", {85, NULL}))
                    {
                        engine.load(replay_name);
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Clear Replay", {85, NULL}))
                    {
                        ImGui::InsertNotification({ ImGuiToastType_Warning, 5000, "Replay has been cleared" });
                        engine.replay.clear();
                    }

                    ImGui::Separator();

                    ImGui::Text("Replay size: %i", engine.replay.size());
                    ImGui::Text("Frame: %i", hooks::frame);
                    ImGui::Text("Checkpoint: %i", hooks::checkpoint_frame);
                }
                else if (type == "replay_engine.sequence")
                {
                    if (ImGui::Checkbox("Enable##sequence_enable", &engine.sequence_state)) 
                        engine.mode = engine.sequence_state ? state::play : state::disable;

                    ImGui::SameLine();

                    ImGui::Checkbox("Random", &engine.sequence_random);

                    ImGui::SameLine(); ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical | ImGuiSeparatorFlags_SpanAllColumns); ImGui::SameLine();

                    ImGui::Text("Sequence size: %i", engine.sequence.size());

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    ImGui::InputText("##engine.sequence_replay", sequence_replay, IM_ARRAYSIZE(sequence_replay), ImGuiInputTextFlags_EnterReturnsTrue);

                    if (ImGui::IsItemActivated())
                        ImGui::OpenPopup("##filtered_popup");
                        
                    {
                        ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
                        ImGui::SetNextWindowSize({ ImGui::GetItemRectSize().x, 0 });
                        if (ImGui::BeginPopup("##filtered_popup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ChildWindow))
                        {
                            for (size_t i = 0; i < filtered.size(); i++)
                            {
                                if (strstr(filtered[i].c_str(), sequence_replay) == NULL)
                                    continue;
                                    
                                if (ImGui::Selectable(filtered[i].c_str()))
                                {
                                    ImGui::ClearActiveID();
                                    strcpy_s(sequence_replay, filtered[i].c_str());
                                }
                            }

                            if (!ImGui::IsItemActive() && !ImGui::IsWindowFocused())
                                ImGui::CloseCurrentPopup();

                            ImGui::EndPopup();
                        }
                    }

                    if (ImGui::Button("Add", {85, NULL}))
                    {
                        if (isEmpty(sequence_replay))
                        {
                            ImGui::InsertNotification({ ImGuiToastType_Error, 5000, "Sequence replay name is empty!" });
                        }
                        else
                        {
                            if (std::filesystem::exists("GDH/replays/" + (string)sequence_replay + ".txt"))
                            {
                                engine.sequence.push_back(sequence_replay);
                            }
                        }
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Remove", {85, NULL}))
                    {
                        if (isEmpty(sequence_replay))
                        {
                            ImGui::InsertNotification({ ImGuiToastType_Error, 5000, "Please clear replay before loading another!" });
                        }
                        else
                        {
                            if (engine.sequence.size() > (size_t)engine.sequence_index)
                            {
                                engine.sequence.erase(engine.sequence.begin() + engine.sequence_index);
                                engine.sequence_first = true;
                            }
                        }
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Clear", {85, NULL}))
                    {
                        ImGui::InsertNotification({ ImGuiToastType_Warning, 5000, "Sequence list has been cleared" });
                        engine.sequence.clear();
                    }

                    ImGui::Separator();

                    ImGui::BeginChild("##sequence_window", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true, ImGuiWindowFlags_AlwaysAutoResize); ImGui::PushItemWidth(250);

                    for (size_t index = 0; index < engine.sequence.size(); index++)
                    {
                        bool is_selected = (engine.sequence_index == index);
                        string anticonflict = engine.sequence[index] + "##" + to_string(index);

                        if (ImGui::Selectable(anticonflict.c_str(), is_selected))
                            engine.sequence_index = index;
                    }

                    ImGui::EndChild();
                }
                else if (type == "replay_engine.macro_editor")
                {
                    auto size = ImGui::GetWindowSize();
                    auto pos = ImGui::GetWindowPos();
                    ImGui::SetNextWindowPos({size.x + pos.x + 10, pos.y});
                    ImGui::SetNextWindowSize({185, (float)(250)});
                    ImGui::Begin("Editor", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

                    if (!engine.replay.empty())
                    {
                        if (ImGui::Button("Add Action") && replayEditor_index >= 0 && replayEditor_index <= int(engine.replay.size()))
                        {
                            if (!engine.replay.empty() || replayEditor_index != engine.replay.size())
                            {
                                engine.replay.insert(engine.replay.begin() + replayEditor_index + 1, {(unsigned)replayEditor_index + 1, false, false});

                                replayEditor_index++;
                            }
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("Remove Action") && replayEditor_index >= 0 && replayEditor_index < int(engine.replay.size()))
                        {
                            engine.replay.erase(engine.replay.begin() + replayEditor_index);

                            if (replayEditor_index > 0)
                            {
                                replayEditor_index--;
                            }
                        }

                        if (ImGui::Button("Move Up", {75, NULL}) && replayEditor_index > 0)
                        {
                            swap(engine.replay[replayEditor_index], engine.replay[replayEditor_index - 1]);
                            replayEditor_index = (replayEditor_index - 1 >= 0) ? (replayEditor_index - 1) : 0;
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("Move Down", {90, NULL}) && replayEditor_index < int(engine.replay.size()) - 1)
                        {
                            swap(engine.replay[replayEditor_index], engine.replay[replayEditor_index + 1]);
                            replayEditor_index = (replayEditor_index + 1 < int(engine.replay.size())) ? (replayEditor_index + 1) : (int(engine.replay.size()) - 1);
                        }

                        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

                        int frame = engine.replay[replayEditor_index].frame;
                        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

                        if (ImGui::DragInt("##replay_engine.macro_editor.frame_editor", &frame, 1, 0, INT_MAX, "Frame: %i"))
                        {
                            engine.replay[replayEditor_index].frame = frame;
                        }

                        ImGui::RadioButton("Up", &engine.replay[replayEditor_index].player_button, 1);
                        ImGui::RadioButton("Left", &engine.replay[replayEditor_index].player_button, 2);
                        ImGui::RadioButton("Right", &engine.replay[replayEditor_index].player_button, 3);

                        ImGui::Spacing();

                        ImGui::Checkbox(engine.replay[replayEditor_index].player ? "First" : "Second", &engine.replay[replayEditor_index].player);
                    }
                    else
                    {
                        ImGui::Text("No Actions!");
                        if (ImGui::Button("Add action"))
                        {
                            engine.replay.push_back({0, false, true});
                        }
                    }

                    ImGui::End();

                    if (ImGui::BeginTable("##editor_table", 4, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders | 2))
                    {
                        ImGui::TableNextColumn(); ImGui::TableHeader("Frame");
                        ImGui::TableNextColumn(); ImGui::TableHeader("Player");
                        ImGui::TableNextColumn(); ImGui::TableHeader("Action");
                        ImGui::TableNextColumn(); ImGui::TableHeader("Button");

                        for (size_t index = 0; index < engine.replay.size(); index++)
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            
                            bool is_selected = (replayEditor_index == index);
                            string frame = to_string(engine.replay[index].frame) + "##" + to_string(index);

                            if (ImGui::Selectable(frame.c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns))
                            {
                                replayEditor_index = index;
                            }
                            
                            ImGui::TableNextColumn();

                            ImGui::Text(engine.replay[index].player ? "First" : "Second"); ImGui::TableNextColumn();
                            ImGui::Text(engine.replay[index].hold ? "Push" : "Release"); ImGui::TableNextColumn();
                            ImGui::Text(std::to_string(engine.replay[index].player_button).c_str());
                        }

                        ImGui::EndTable();
                    }
                }
            }
        }

        ImGui::End();
    }

    ImGui::End();

    // ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f); // Round borders
    // ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(43.f / 255.f, 43.f / 255.f, 43.f / 255.f, 100.f / 255.f)); // Background color
    ImGui::RenderNotifications(); // ImGui::PopStyleVar(1); // ImGui::PopStyleColor(1);
}

void gui::Toggle()
{
    gui::show = !gui::show;
    anim_starttime = std::chrono::steady_clock::now();
}