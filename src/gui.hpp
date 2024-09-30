#pragma once
#include "includes.hpp"

namespace gui {
    extern bool show;
    extern bool inited;
    extern bool needRescale;
    extern int indexScale;
    extern bool license_accepted;
    extern float scale;
    extern bool broken_save;
    extern bool russian_char_warning;

    extern bool change_keybind;
    extern int keybind_key;
    extern int menu_key;

    void toggleKeybinds(int key);

    void RenderMain();
    void Toggle();
    void Unload();
}

struct popup_message {
    std::string caption;
    double expiry_time;
};

namespace imgui_popup {
    const float popupDuration = 5.0f;
    extern bool enabled;

    extern std::vector<popup_message> messages;
    void add_popup(std::string caption);
    void render();
}