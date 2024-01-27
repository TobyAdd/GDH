#pragma once
#include "includes.hpp"

namespace gui {
    extern bool show;
    extern bool inited;
    extern bool license_accepted;

    void RenderMain();
    void Toggle();
    void KeyPress(int key);
    void Unload();
}