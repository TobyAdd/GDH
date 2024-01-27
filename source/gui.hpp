#pragma once
#include "includes.hpp"

namespace gui {
    extern bool show;
    extern bool inited;
    extern bool license_accepted;

    extern bool message;
    extern bool best_run;
    extern bool ndeaths;
    extern bool naccuracy;
    extern bool tint;
    extern bool noclip;
    extern char custom_message[256];

    void RenderMain();
    void Toggle();
    void Unload();
}