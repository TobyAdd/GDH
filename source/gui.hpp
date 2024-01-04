#include "include.hpp"
#include <imgui_notify.h>

namespace gui
{
    extern bool show;
    extern bool inited;
    extern bool recording;
    extern int currentkeycode;

    extern void ReplaysList();
    extern void StylesList();

    void import_style(std::string path);
	void export_style(std::string path);
    
    void Init();
    void Render();
    void Toggle();
}