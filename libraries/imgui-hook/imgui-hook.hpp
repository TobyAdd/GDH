#include <windows.h>
#include <cocos2d.h>
#include <gl/GL.h>
#include <functional>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_internal.h"
#include "imgui_stdlib.h"

namespace ImGuiHook {
    extern float scale;
    extern bool needReload;

    void Load(const std::function<void(void*, void*, void**)>& hookFunc);
    void Unload(bool reloadFonts);

    void setRenderFunction(std::function<void()> func);
    void setUnloadFunction(std::function<void()> func);
    void setKeyPressHandler(std::function<void(int)> func);
}