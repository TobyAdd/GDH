#include "imgui-hook.hpp"

#include <utility>
#include "imgui-theme.hpp"
#include "font.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace ImGuiHook {
    bool inited = false;
    float scale = 1.0f;
    bool needReload = false;
    bool unloadFonts = false;

    std::function<void()> drawFunc = []() {};
    std::function<void()> unloadFunc = []() {};
    std::function<void(int)> keyPressHandler = [](int _) {};

    HWND window;

    void setup(cocos2d::CCEGLView *view) {
        if (inited)
            return;

        auto *glfwWindow = view->getWindow();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        window = WindowFromDC(*reinterpret_cast<HDC *>(reinterpret_cast<uintptr_t>(glfwWindow) + 0x244));
        ImGui_ImplWin32_Init(window);
        ImGui_ImplOpenGL3_Init();

        io.IniFilename = nullptr;
        ApplyColor();
        ApplyStyle();

        io.Fonts->Clear();
        io.Fonts->AddFontFromMemoryCompressedTTF(roboto_font_data, roboto_font_size, 18.f * ImGuiHook::scale, nullptr, io.Fonts->GetGlyphRangesCyrillic());
        io.Fonts->AddFontFromMemoryCompressedTTF(roboto_font_data, roboto_font_size, 32.f * ImGuiHook::scale, nullptr, io.Fonts->GetGlyphRangesCyrillic());
        io.Fonts->AddFontFromMemoryCompressedTTF(roboto_font_data, roboto_font_size, 20.f * ImGuiHook::scale, nullptr, io.Fonts->GetGlyphRangesCyrillic());
        io.Fonts->Build();
        ImGui_ImplOpenGL3_CreateFontsTexture();

        inited = true;
    }

    void handleEvents() {
        if (!inited)
            return;

        auto &io = ImGui::GetIO();
        bool blockInput = false;
        MSG msg;

        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);

            // Block input if ImGui wants to capture it
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
            } else if (msg.message == WM_KEYDOWN) {
                keyPressHandler(msg.wParam);
            }

            if (!blockInput)
                DispatchMessage(&msg);

            ImGui_ImplWin32_WndProcHandler(msg.hwnd, msg.message, msg.wParam, msg.lParam);
        }
    }

    void destroy() {
        if (!inited) return;

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        inited = false;
    }

    void draw(cocos2d::CCEGLView *view) {
        if (!inited) setup(view);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();

        ImGui::NewFrame();
        drawFunc();
        ImGui::EndFrame();

        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glFlush();

        if (needReload) {
            unloadFunc();
            destroy();
            needReload = false;
        }
    }

    void (__thiscall *swapBuffers)(cocos2d::CCEGLView *) = nullptr;
    void __fastcall swapBuffers_H(cocos2d::CCEGLView *self)
    {
        draw(self);
        swapBuffers(self);
    }

    void (__thiscall *toggleFullScreen)(cocos2d::CCEGLView *, bool, bool);
    void __fastcall toggleFullScreen_H(cocos2d::CCEGLView *self, void*, bool fullscreen, bool borderless)
    {
        destroy();
        toggleFullScreen(self, fullscreen, borderless);
    }

    void (__thiscall *pollEvents)(cocos2d::CCEGLView *) = nullptr;
    void __fastcall pollEvents_H(cocos2d::CCEGLView *self)
    {
        handleEvents();
        pollEvents(self);
    }

    void Load(const std::function<void(void*, void*, void**)>& hookFunc)
    {
        hookFunc(
                (void*)GetProcAddress(GetModuleHandleA("libcocos2d.dll"), "?swapBuffers@CCEGLView@cocos2d@@UAEXXZ"),
                (void*)swapBuffers_H, (void **)&swapBuffers
        );

        hookFunc(
                (void*)GetProcAddress(GetModuleHandleA("libcocos2d.dll"), "?pollEvents@CCEGLView@cocos2d@@QAEXXZ"),
                (void*)pollEvents_H, (void **)&pollEvents
        );

        hookFunc(
                (void*)GetProcAddress(GetModuleHandleA("libcocos2d.dll"), "?toggleFullScreen@CCEGLView@cocos2d@@QAEX_N0@Z"),
                (void*)toggleFullScreen_H, (void **)&toggleFullScreen
        );
    }

    void Unload(bool reloadFonts)
    {
        needReload = true;
        unloadFonts = reloadFonts;
    }

    void setRenderFunction(std::function<void()> func) { drawFunc = func; }
    void setUnloadFunction(std::function<void()> func) { unloadFunc = func; }
    void setKeyPressHandler(std::function<void(int)> func) { keyPressHandler = func; }
}