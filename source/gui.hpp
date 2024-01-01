#include "include.hpp"

namespace gui
{
    extern bool show;
    extern bool inited;
    extern bool recording;
    extern int currentkeycode;
    void BeginWindow(const char *title);
    void Render();
    void Toggle();
}