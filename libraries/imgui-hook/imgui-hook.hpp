#include <Windows.h>
#include <gl/GL.h>
#include <functional>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_impl_win32.h"

typedef BOOL(WINAPI* SwapBuffersType)(HDC hdc);

namespace ImGuiHook {
    void Load(std::function<void(void*, void*, void**)> hookFunc);
    void Unload();
    void setRenderFunction(std::function<void()> func);
    void setKeyPressHandler(std::function<void(int)> func);
}