#include <imgui.h>
#include <vector>

struct RGB {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

struct Theme {
    RGB color_bg;
    RGB color_circle;
    RGB color_hovered;
    RGB color_active;
};

const std::vector<Theme> themes = {
    {{165, 255, 190}, {0, 90, 5}, {105, 210, 160}, {85, 170, 130}},
    {{164, 201, 254}, {0, 50, 87}, {147, 180, 228}, {131, 160, 203}},
    {{255, 223, 186}, {80, 40, 0}, {255, 180, 140}, {255, 150, 120}},
    {{255, 182, 193}, {139, 0, 70}, {255, 140, 170}, {255, 110, 130}},
    {{255, 255, 153}, {64, 64, 0}, {255, 240, 100}, {255, 220, 80}},
    {{255, 255, 255}, {80, 80, 80}, {230, 230, 230}, {210, 210, 210}} 
};


static void ApplyGuiColors(bool invert) {
    auto* colors = ImGui::GetStyle().Colors;

    auto invertColor = [](const ImVec4& color) -> ImVec4 {
        return ImVec4(1.0f - color.x, 1.0f - color.y, 1.0f - color.z, color.w);
    };

    colors[ImGuiCol_Text] = invert ? invertColor(ImColor(225, 225, 225, 225)) : ImColor(225, 225, 225, 225).Value;
    colors[ImGuiCol_WindowBg] = invert ? invertColor(ImColor(27, 27, 29, 250)) : ImColor(27, 27, 29, 250).Value;
    colors[ImGuiCol_Border] = invert ? invertColor(ImColor(64, 64, 64)) : ImColor(64, 64, 64).Value;
    colors[ImGuiCol_TitleBg] = invert ? invertColor(ImColor(58, 58, 58)) : ImColor(58, 58, 58).Value;
    colors[ImGuiCol_TitleBgActive] = colors[ImGuiCol_TitleBg];
    colors[ImGuiCol_TitleBgCollapsed] = colors[ImGuiCol_TitleBg];
    colors[ImGuiCol_ScrollbarBg] = colors[ImGuiCol_WindowBg];
    colors[ImGuiCol_FrameBg] = invert ? invertColor(ImColor(52, 52, 52)) : ImColor(52, 52, 52).Value;
    colors[ImGuiCol_FrameBgHovered] = invert ? invertColor(ImColor(72, 72, 72)) : ImColor(72, 72, 72).Value;
    colors[ImGuiCol_FrameBgActive] = invert ? invertColor(ImColor(90, 90, 90)) : ImColor(90, 90, 90).Value;
    colors[ImGuiCol_ResizeGrip] = invert ? invertColor(ImColor(58, 58, 58)) : ImColor(58, 58, 58).Value;
    colors[ImGuiCol_ResizeGripHovered] = invert ? invertColor(ImColor(77, 77, 77)) : ImColor(77, 77, 77).Value;
    colors[ImGuiCol_ResizeGripActive] = invert ? invertColor(ImColor(94, 94, 94)) : ImColor(94, 94, 94).Value;
    colors[ImGuiCol_PopupBg] = colors[ImGuiCol_WindowBg];
    colors[ImGuiCol_Header] = invert ? invertColor(ImColor(50, 50, 52)) : ImColor(50, 50, 52).Value;
    colors[ImGuiCol_HeaderHovered] = invert ? invertColor(ImColor(70, 70, 72)) : ImColor(70, 70, 72).Value;
    colors[ImGuiCol_HeaderActive] = invert ? invertColor(ImColor(88, 88, 90)) : ImColor(88, 88, 90).Value;
    colors[ImGuiCol_Tab] = invert ? invertColor(ImColor(50, 50, 52)) : ImColor(50, 50, 52).Value;
    colors[ImGuiCol_TabHovered] = invert ? invertColor(ImColor(70, 70, 72)) : ImColor(70, 70, 72).Value;
    colors[ImGuiCol_TabActive] = invert ? invertColor(ImColor(88, 88, 90)) : ImColor(88, 88, 90).Value;
}

static void ApplyColor(const Theme& theme) {
    auto* colors = ImGui::GetStyle().Colors;
    
    colors[ImGuiCol_Button] = ImColor(theme.color_bg.r, theme.color_bg.g, theme.color_bg.b);
    colors[ImGuiCol_ButtonHovered] = ImColor(theme.color_hovered.r, theme.color_hovered.g, theme.color_hovered.b);
    colors[ImGuiCol_ButtonActive] = ImColor(theme.color_active.r, theme.color_active.g, theme.color_active.b);
}

static void ApplyStyle(float scale) {
    ImGuiStyle& style = ImGui::GetStyle();  

    style.WindowPadding = ImVec2(10.f * scale, 10.f * scale);
    style.FramePadding = ImVec2(4.f * scale, 3.f * scale);
    style.ItemSpacing = ImVec2(8.f * scale, 4.f * scale);
    style.ItemInnerSpacing = ImVec2(4.f * scale, 4.f * scale);
    style.IndentSpacing = 21.f * scale;
    style.ScrollbarSize = 14.f * scale;
    style.GrabMinSize = 12.f * scale;

    style.WindowRounding = 12.0f * scale;
    style.ChildRounding = 6.0f * scale;
    style.FrameRounding = 20.0f * scale;
    style.PopupRounding = 12.f * scale;
    style.ScrollbarRounding = 12.f * scale;
    style.GrabRounding = 12.f * scale;
    style.TabRounding = 6.f * scale;
    
    style.WindowBorderSize = 0;
    style.WindowTitleAlign = ImVec2(0.50, 0.50);
    style.WindowMenuButtonPosition = ImGuiDir_Right;
}