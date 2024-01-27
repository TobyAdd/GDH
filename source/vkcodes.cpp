#include <map>
#include <algorithm>
#include <string>
#include <Windows.h>

std::map<uint32_t, std::string> key_names = {
    {0, "..."}, // unset key
    {VK_LBUTTON, "LMB"},
    {VK_RBUTTON, "RMB"},
    {VK_MBUTTON, "MMB"},
    {VK_XBUTTON1, "X1"},
    {VK_XBUTTON2, "X2"},
    {VK_BACK, "Backspace"},
    {VK_TAB, "Tab"},
    {VK_CLEAR, "Clear"},
    {VK_RETURN, "Enter"},
    {VK_SHIFT, "Shift"},
    {VK_CONTROL, "Ctrl"},
    {VK_MENU, "Alt"},
    {VK_PAUSE, "Pause"},
    {VK_CAPITAL, "Caps Lock"},
    {VK_ESCAPE, "Esc"},
    {VK_SPACE, "Space"},
    {VK_PRIOR, "Page Up"},
    {VK_NEXT, "Page Down"},
    {VK_END, "End"},
    {VK_HOME, "Home"},
    {VK_LEFT, "Left"},
    {VK_UP, "Up"},
    {VK_RIGHT, "Right"},
    {VK_DOWN, "Down"},
    {VK_SELECT, "Select"},
    {VK_PRINT, "Print"},
    {VK_EXECUTE, "Execute"},
    {VK_SNAPSHOT, "Print Screen"},
    {VK_INSERT, "Insert"},
    {VK_DELETE, "Delete"},
    {VK_HELP, "Help"},
    {VK_LWIN, "Left Windows"},
    {VK_RWIN, "Right Windows"},
    {VK_APPS, "Applications"},
    {VK_SLEEP, "Sleep"},
    {VK_NUMPAD0, "Numpad 0"},
    {VK_NUMPAD1, "Numpad 1"},
    {VK_NUMPAD2, "Numpad 2"},
    {VK_NUMPAD3, "Numpad 3"},
    {VK_NUMPAD4, "Numpad 4"},
    {VK_NUMPAD5, "Numpad 5"},
    {VK_NUMPAD6, "Numpad 6"},
    {VK_NUMPAD7, "Numpad 7"},
    {VK_NUMPAD8, "Numpad 8"},
    {VK_NUMPAD9, "Numpad 9"},
    {VK_MULTIPLY, "Numpad *"},
    {VK_ADD, "Numpad +"},
    {VK_SEPARATOR, "Numpad Separator"},
    {VK_SUBTRACT, "Numpad -"},
    {VK_DECIMAL, "Numpad ."},
    {VK_DIVIDE, "Numpad /"},
    {VK_F1, "F1"},
    {VK_F2, "F2"},
    {VK_F3, "F3"},
    {VK_F4, "F4"},
    {VK_F5, "F5"},
    {VK_F6, "F6"},
    {VK_F7, "F7"},
    {VK_F8, "F8"},
    {VK_F9, "F9"},
    {VK_F10, "F10"},
    {VK_F11, "F11"},
    {VK_F12, "F12"},
    {VK_F13, "F13"},
    {VK_F14, "F14"},
    {VK_F15, "F15"},
    {VK_F16, "F16"},
    {VK_F17, "F17"},
    {VK_F18, "F18"},
    {VK_F19, "F19"},
    {VK_F20, "F20"},
    {VK_F21, "F21"},
    {VK_F22, "F22"},
    {VK_F23, "F23"},
    {VK_F24, "F24"},
    {VK_NUMLOCK, "Num Lock"},
    {VK_SCROLL, "Scroll Lock"},
    {VK_LSHIFT, "Left Shift"},
    {VK_RSHIFT, "Right Shift"},
    {VK_LCONTROL, "Left Ctrl"},
    {VK_RCONTROL, "Right Ctrl"},
    {VK_LMENU, "Left Alt"},
    {VK_RMENU, "Right Alt"},
    {VK_BROWSER_BACK, "Browser Back"},
    {VK_BROWSER_FORWARD, "Browser Forward"},
    {VK_BROWSER_REFRESH, "Browser Refresh"},
    {VK_BROWSER_STOP, "Browser Stop"},
    {VK_BROWSER_SEARCH, "Browser Search"},
    {VK_BROWSER_FAVORITES, "Browser Favorites"},
    {VK_BROWSER_HOME, "Browser Home"},
    {VK_VOLUME_MUTE, "Volume Mute"},
    {VK_VOLUME_DOWN, "Volume Down"},
    {VK_VOLUME_UP, "Volume Up"},
    {VK_MEDIA_NEXT_TRACK, "Next Track"},
    {VK_MEDIA_PREV_TRACK, "Previous Track"},
    {VK_MEDIA_STOP, "Stop"},
    {VK_MEDIA_PLAY_PAUSE, "Play/Pause"},
    {VK_LAUNCH_MAIL, "Launch Mail"},
    {VK_LAUNCH_MEDIA_SELECT, "Select Media"},
    {VK_LAUNCH_APP1, "Launch App 1"},
    {VK_LAUNCH_APP2, "Launch App 2"},
    {VK_OEM_1, ";"},
    {VK_OEM_PLUS, "+"},
    {VK_OEM_COMMA, ","},
    {VK_OEM_MINUS, "-"},
    {VK_OEM_PERIOD, "."},
    {VK_OEM_2, "/"},
    {VK_OEM_3, "`"},
    {VK_OEM_4, "["},
    {VK_OEM_5, "\\"},
    {VK_OEM_6, "]"},
    {VK_OEM_7, "'"},
    {VK_OEM_8, "OEM 8"},
    {VK_OEM_102, "OEM 102"},
    {VK_PROCESSKEY, "IME Process"},
    {VK_PACKET, "Packet"},
    {VK_ATTN, "Attn"},
    {VK_CRSEL, "CrSel"},
    {VK_EXSEL, "ExSel"},
    {VK_EREOF, "Erase EOF"},
    {VK_PLAY, "Play"},
    {VK_ZOOM, "Zoom"},
    {VK_NONAME, "NoName"},
    {VK_PA1, "PA1"},
    {VK_OEM_CLEAR, "Clear"},
};

std::string key_name(uint32_t keycode)
{
    // find key in map
    for (auto& key : key_names)
    {
        if (key.first == keycode)
        {
            return key.second;
        }
    }

    // check if key is a letter
    if (keycode >= 0x41 && keycode <= 0x5A)
    {
        return std::string(1, (char)keycode);
    }

    // check if key is a number
    if (keycode >= 0x30 && keycode <= 0x39)
    {
        return std::string(1, (char)keycode);
    }

    return "Unknown";
}

uint32_t decode_key(std::string key)
{
    // find key in map
    for (auto& key_ : key_names)
    {
        // compare lowercase strings
        std::string key_lower = key;
        std::transform(key_lower.begin(), key_lower.end(), key_lower.begin(), ::tolower);
        std::string key_lower_ = key_.second;
        std::transform(key_lower_.begin(), key_lower_.end(), key_lower_.begin(), ::tolower);
        if (key_lower == key_lower_)
        {
            return key_.first;
        }
    }

    // check if key is a letter
    if (key.length() == 1)
    {
        return (uint32_t)key[0];
    }

    return 0;
}