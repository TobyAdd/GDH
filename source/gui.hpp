#include "include.hpp"

enum font_size{small, normal, large};
extern font_size current_font_size;

namespace gui
{
    extern bool show;
    extern bool inited;
    extern bool recording;
    extern int currentkeycode;
    void BeginWindow(const char *title);
    void Render();
    void Toggle();
    void UnLoad();

    extern bool confirm_exit;

    extern bool nc_acc;
    extern bool cps_counter;
    extern bool fps_label;
    extern bool message;
    extern bool noclip_deaths;
    extern char custom_message[256];

    extern bool hacks_missing;
}