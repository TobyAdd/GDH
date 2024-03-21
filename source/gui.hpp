#pragma once
#include "includes.hpp"
#include "hacks.hpp"

namespace gui {
    extern bool show;
    extern bool inited;

    extern bool license_accepted;    
    
    extern float scalePercentage;

    extern bool onlyRE;

    void RenderMain();
    void Toggle();
    void Unload();
}