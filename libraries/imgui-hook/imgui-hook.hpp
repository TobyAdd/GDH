#include <windows.h>
#include <gl/GL.h>
#include <functional>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_stdlib.h"

typedef BOOL(WINAPI* swapBuffersType)(HDC hdc);

namespace ImGuiHook {
    extern float scale;
    
    void Load(std::function<void(void*, void*, void**)> hookFunc);
    void Unload();


    void setRenderFunction(std::function<void()> func);
    void setUnloadFunction(std::function<void()> func);
    void setKeyPressHandler(std::function<void(int)> func);
}