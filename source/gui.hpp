#pragma once
#include "includes.hpp"

namespace gui {
    extern bool show;
    extern bool inited;
    extern bool needRescale;
    extern int indexScale;
    extern bool onlyRE;
    extern bool license_accepted;

    void RenderMain();
    void Toggle();
    void Unload();
}