#include "imgui/imgui.h"
#include "imgui-hook.hpp"
#include <iostream>
#include <sstream>

void ApplyColor() {
    auto* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImColor(27, 27, 29, 250);
    colors[ImGuiCol_Button] = ImColor(164, 201, 254);
    colors[ImGuiCol_ButtonHovered] = ImColor(147, 180, 228);
    colors[ImGuiCol_ButtonActive] = ImColor(131, 160, 203);
    colors[ImGuiCol_Border] = ImColor(64, 64, 64);
    colors[ImGuiCol_TitleBg] = ImColor(58,58,58);
    colors[ImGuiCol_TitleBgActive] = colors[ImGuiCol_TitleBg];
    colors[ImGuiCol_TitleBgCollapsed] = colors[ImGuiCol_TitleBg];
    colors[ImGuiCol_ScrollbarBg] = colors[ImGuiCol_WindowBg];
    colors[ImGuiCol_FrameBg] = ImColor(52,52,52);
    colors[ImGuiCol_FrameBgHovered] = ImColor(72,72,72);
    colors[ImGuiCol_FrameBgActive] = ImColor(90,90,90);
    colors[ImGuiCol_ResizeGrip] = ImColor(58,58,58);
    colors[ImGuiCol_ResizeGripHovered] = ImColor(77,77,77);
    colors[ImGuiCol_ResizeGripActive] = ImColor(94,94,94); //#343434
    colors[ImGuiCol_PopupBg] = colors[ImGuiCol_WindowBg];
    colors[ImGuiCol_Header] = ImColor(50,50,52);
    colors[ImGuiCol_HeaderHovered] = ImColor(70,70,72);
    colors[ImGuiCol_HeaderActive] = ImColor(88,88,90);
    colors[ImGuiCol_Tab] = ImColor(50,50,52);
    colors[ImGuiCol_TabHovered] = ImColor(70,70,72);
    colors[ImGuiCol_TabActive] = ImColor(88,88,90);

    //61,66,71
}

void ApplyStyle() {
    ImGuiStyle& style = ImGui::GetStyle();    
    style.WindowRounding = 12.0f * ImGuiHook::scale;
    style.ChildRounding = 6.0f * ImGuiHook::scale;
    style.FrameRounding = 20.0f * ImGuiHook::scale;
    style.PopupRounding = 12.f * ImGuiHook::scale;
    style.GrabRounding = 12.f * ImGuiHook::scale;
    style.TabRounding = 6.f * ImGuiHook::scale;
    style.ScrollbarSize = 14.f * ImGuiHook::scale;

    style.WindowPadding = ImVec2(10 * ImGuiHook::scale, 10 * ImGuiHook::scale);
    style.WindowBorderSize = 0;
    style.WindowTitleAlign = ImVec2(0.50, 0.50);
    style.WindowMenuButtonPosition = ImGuiDir_Right;
}