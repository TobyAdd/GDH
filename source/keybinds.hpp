#include "include.hpp"

namespace keybinds
{
    extern json binds;
    void keypress(int keycode);
    void AddKeybind(string hackName, int keycode, json *jsonObj);
}
