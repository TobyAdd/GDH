#include "imgui-hook.hpp"
#include "imgui-theme.hpp"
#include "font.hpp"

bool inited = false;
float ImGuiHook::scale = 1.0f;

std::function<void()> drawFunc = []() {};
std::function<void()> unloadFunc = []() {};
std::function<void(int)> keyPressHandler = [](int _) {};

HGLRC newContext = 0;
HWND window;

WNDPROC wndProc = nullptr;
LRESULT CALLBACK wndProc_H(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

swapBuffersType swapBuffers = nullptr;
BOOL WINAPI swapBuffers_H(HDC hdc)
{
    if (!inited)
    {
        inited = true;

        newContext = wglCreateContext(hdc);
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();      
        io.IniFilename = nullptr;
        ApplyColor();
        ApplyStyle();
        io.Fonts->Clear();
        io.Fonts->AddFontFromMemoryCompressedTTF(roboto_font_data, roboto_font_size, 18.f * ImGuiHook::scale, nullptr, io.Fonts->GetGlyphRangesCyrillic());
        io.Fonts->AddFontFromMemoryCompressedTTF(roboto_font_data, roboto_font_size, 32.f * ImGuiHook::scale, nullptr, io.Fonts->GetGlyphRangesCyrillic());
        io.Fonts->AddFontFromMemoryCompressedTTF(roboto_font_data, roboto_font_size, 20.f * ImGuiHook::scale, nullptr, io.Fonts->GetGlyphRangesCyrillic());
        io.Fonts->Build();
        window = WindowFromDC(hdc);
        wndProc = (WNDPROC)GetWindowLongPtr(window, GWLP_WNDPROC);
        SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)wndProc_H);
        ImGui_ImplWin32_Init(window);
        ImGui_ImplOpenGL2_Init();
    }

    HGLRC originalContext = wglGetCurrentContext();

    wglMakeCurrent(hdc, newContext);

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    drawFunc();

    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    glFlush();

    wglMakeCurrent(hdc, originalContext);

    return swapBuffers(hdc);
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK wndProc_H(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);

    if (msg == WM_KEYDOWN && !ImGui::GetIO().WantCaptureKeyboard)
    {
        keyPressHandler(wParam);
    }

    bool blockInput = false;
    if (ImGui::GetIO().WantCaptureMouse)
    {
        switch (msg)
        {
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

    if (ImGui::GetIO().WantCaptureKeyboard)
    {
        switch (msg)
        {
            case WM_HOTKEY:
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_KILLFOCUS:
            case WM_SETFOCUS:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
                blockInput = true;
        }
    }

    if (blockInput)
        return true;

    return CallWindowProc(wndProc, hwnd, msg, wParam, lParam);
}

void (__thiscall* cocos2d_CCEGLView_toggleFullScreen)(void*, bool, bool);
void __fastcall cocos2d_CCEGLView_toggleFullScreenH(void *self, char a2, char a3)
{
    ImGuiHook::Unload();
    cocos2d_CCEGLView_toggleFullScreen(self, a2, a3);
}

void ImGuiHook::Load(std::function<void(void*, void*, void**)> hookFunc)
{
    hookFunc(
        GetProcAddress(GetModuleHandleA("opengl32.dll"), "wglSwapBuffers"),
        swapBuffers_H,
        (void **)&swapBuffers
    );

    hookFunc(
        GetProcAddress(GetModuleHandleA("libcocos2d.dll"), "?toggleFullScreen@CCEGLView@cocos2d@@QEAAX_N0@Z"),
        cocos2d_CCEGLView_toggleFullScreenH,
        reinterpret_cast<void**>(&cocos2d_CCEGLView_toggleFullScreen)
    );
}

void ImGuiHook::Unload()
{
    unloadFunc();
    SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)wndProc);
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();    
    inited = false;
}

void ImGuiHook::setRenderFunction(std::function<void()> func)
{
    drawFunc = func;
}

void ImGuiHook::setUnloadFunction(std::function<void()> func)
{
    unloadFunc = func;
}

void ImGuiHook::setKeyPressHandler(std::function<void(int)> func)
{
    keyPressHandler = func;
}