#pragma once
#include "includes.hpp"

namespace gui {
    extern bool show;
    extern bool inited;
    extern bool needRescale;
    extern int indexScale;
    extern bool license_accepted;
    extern float scale;

    extern bool change_keybind;
    extern int keybind_key;
    extern int menu_key;

    void toggleKeybinds(int key);

    void RenderMain();
    void Toggle();
    void Unload();
}