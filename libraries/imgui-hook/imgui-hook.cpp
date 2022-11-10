#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cocos2d.h>
#include "imgui-hook.hpp"
#include "../../src/data.h"

using namespace cocos2d;


void ApplyColor() {
    auto* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.15f, 0.15f, 0.22f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.15f, 0.15f, 0.22f, 1.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.31f, 0.29f, 0.39f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.36f, 0.35f, 0.47f, 0.54f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.45f, 0.43f, 0.56f, 0.54f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.22f, 0.21f, 0.30f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.22f, 0.21f, 0.30f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.22f, 0.21f, 0.30f, 1.00f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.22f, 0.21f, 0.30f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.20f, 0.20f, 0.27f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.29f, 0.39f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.39f, 0.37f, 0.49f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.46f, 0.44f, 0.58f, 0.54f);
    colors[ImGuiCol_CheckMark]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
    colors[ImGuiCol_Button]                 = ImVec4(0.31f, 0.29f, 0.39f, 0.54f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.36f, 0.35f, 0.47f, 0.54f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.45f, 0.43f, 0.56f, 0.54f);
    colors[ImGuiCol_Header]                 = ImVec4(0.31f, 0.29f, 0.39f, 0.54f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.36f, 0.35f, 0.47f, 0.54f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.45f, 0.43f, 0.56f, 0.54f);
    colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.31f, 0.29f, 0.39f, 0.54f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.36f, 0.35f, 0.47f, 0.54f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.31f, 0.29f, 0.39f, 0.54f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.36f, 0.35f, 0.47f, 0.54f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.22f, 0.21f, 0.30f, 1.00f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.17f, 0.16f, 0.23f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.22f, 0.21f, 0.30f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void ApplyStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(8.00f, 8.00f);
    style.FramePadding = ImVec2(4.00f, 4.00f);
    style.CellPadding = ImVec2(4.00f, 2.00f);
    style.ItemSpacing = ImVec2(4.00f, 4.00f);
    style.ItemInnerSpacing = ImVec2(4.00f, 4.00f);
    style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
    style.IndentSpacing = 21.00f;
    style.ScrollbarSize = 16.00f;
    style.GrabMinSize = 12.00f;
    style.WindowBorderSize = 0.00f;
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
}

void _stub() {}
std::function<void()> g_drawFunc = _stub;
std::function<void()> g_toggleCallback = _stub;
std::function<void()> g_initFunc = _stub;

void ImGuiHook::setRenderFunction(std::function<void()> func) {
    g_drawFunc = func;
}

void ImGuiHook::setToggleCallback(std::function<void()> func) {
    g_toggleCallback = func;
}

void ImGuiHook::setInitFunction(std::function<void()> func) {
    g_initFunc = func;
}

bool g_inited = false;

void (__thiscall* CCEGLView_swapBuffers)(CCEGLView*);
void __fastcall CCEGLView_swapBuffers_H(CCEGLView* self) {
    auto window = self->getWindow();

    if (!g_inited) {
        g_inited = true;
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromMemoryTTF(fontData, sizeof(fontData), 14.f);
        io.IniFilename = NULL;
        ApplyColor();
        ApplyStyle();

        auto hwnd = WindowFromDC(*reinterpret_cast<HDC*>(reinterpret_cast<uintptr_t>(window) + 0x244));
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplOpenGL3_Init();
        g_initFunc();
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    g_drawFunc();

    ImGui::EndFrame();
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    glFlush();

    CCEGLView_swapBuffers(self);
}

// why is this an extern
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void (__thiscall* CCEGLView_pollEvents)(CCEGLView*);
void __fastcall CCEGLView_pollEvents_H(CCEGLView* self) {
    auto& io = ImGui::GetIO();

    bool blockInput = false;
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);

        if (io.WantCaptureMouse) {
            switch (msg.message) {
                case WM_LBUTTONDBLCLK:
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                case WM_MBUTTONDBLCLK:
                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
                case WM_MOUSEACTIVATE:
                case WM_MOUSEHOVER:
                case WM_MOUSEHWHEEL:
                case WM_MOUSELEAVE:
                case WM_MOUSEMOVE:
                case WM_MOUSEWHEEL:
                case WM_NCLBUTTONDBLCLK:
                case WM_NCLBUTTONDOWN:
                case WM_NCLBUTTONUP:
                case WM_NCMBUTTONDBLCLK:
                case WM_NCMBUTTONDOWN:
                case WM_NCMBUTTONUP:
                case WM_NCMOUSEHOVER:
                case WM_NCMOUSELEAVE:
                case WM_NCMOUSEMOVE:
                case WM_NCRBUTTONDBLCLK:
                case WM_NCRBUTTONDOWN:
                case WM_NCRBUTTONUP:
                case WM_NCXBUTTONDBLCLK:
                case WM_NCXBUTTONDOWN:
                case WM_NCXBUTTONUP:
                case WM_RBUTTONDBLCLK:
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                case WM_XBUTTONDBLCLK:
                case WM_XBUTTONDOWN:
                case WM_XBUTTONUP:
                    blockInput = true;
            }
        }

        if (io.WantCaptureKeyboard) {
            switch (msg.message) {
                case WM_HOTKEY:
                case WM_KEYDOWN:
                case WM_KEYUP:
                case WM_KILLFOCUS:
                case WM_SETFOCUS:
                case WM_SYSKEYDOWN:
                case WM_SYSKEYUP:
                    blockInput = true;
            }
        } else if (msg.message == WM_KEYDOWN && msg.wParam == VK_TAB) {
            g_toggleCallback();
        }

        if (!blockInput)
            DispatchMessage(&msg);

        ImGui_ImplWin32_WndProcHandler(msg.hwnd, msg.message, msg.wParam, msg.lParam);
    }

    CCEGLView_pollEvents(self);
}

void (__thiscall* CCEGLView_toggleFullScreen)(cocos2d::CCEGLView*, bool);
void __fastcall CCEGLView_toggleFullScreen_H(cocos2d::CCEGLView* self, void*, bool toggle) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CCEGLView_toggleFullScreen(self, toggle);

    g_inited = false;
}

void (__thiscall* AppDelegate_applicationWillEnterForeground)(void*);
void __fastcall AppDelegate_applicationWillEnterForeground_H(void* self) {
    AppDelegate_applicationWillEnterForeground(self);
    ImGui::GetIO().ClearInputKeys();
}

void ImGuiHook::setupHooks(std::function<void(void*, void*, void**)> hookFunc) {
    auto cocosBase = GetModuleHandleA("libcocos2d.dll");
    hookFunc(
        GetProcAddress(cocosBase, "?swapBuffers@CCEGLView@cocos2d@@UAEXXZ"),
        CCEGLView_swapBuffers_H,
        reinterpret_cast<void**>(&CCEGLView_swapBuffers)
    );
    hookFunc(
        GetProcAddress(cocosBase, "?pollEvents@CCEGLView@cocos2d@@QAEXXZ"),
        CCEGLView_pollEvents_H,
        reinterpret_cast<void**>(&CCEGLView_pollEvents)
    );
    hookFunc(
        GetProcAddress(cocosBase, "?toggleFullScreen@CCEGLView@cocos2d@@QAEX_N@Z"),
        CCEGLView_toggleFullScreen_H,
        reinterpret_cast<void**>(&CCEGLView_toggleFullScreen)
    );
    hookFunc(
        reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(GetModuleHandleA(0)) + 0x3d130),
        reinterpret_cast<void*>(&AppDelegate_applicationWillEnterForeground_H),
        reinterpret_cast<void**>(&AppDelegate_applicationWillEnterForeground)
    );
}