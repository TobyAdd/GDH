#include <imgui.h>

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

static void ApplyColorV4() {
    auto* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Button] = ImColor(165, 255, 190);
    colors[ImGuiCol_ButtonHovered] = ImColor(105, 210, 160);
    colors[ImGuiCol_ButtonActive] = ImColor(85, 170, 130);
}

static void ApplyColorV3() {
    auto* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Button] = ImColor(164, 201, 254);
    colors[ImGuiCol_ButtonHovered] = ImColor(147, 180, 228);
    colors[ImGuiCol_ButtonActive] = ImColor(131, 160, 203);
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