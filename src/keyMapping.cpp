#ifdef GEODE_IS_WINDOWS
#include "keyMapping.hpp"
#include <algorithm>

namespace KeyMappingUtils {

static const std::vector<KeyMapping> keyMappings = {
    {0, 0, "None"},
    {GLFW_KEY_ESCAPE, VK_ESCAPE, "Esc"},
    {GLFW_KEY_ENTER, VK_RETURN, "Enter"},
    {GLFW_KEY_TAB, VK_TAB, "Tab"},
    {GLFW_KEY_BACKSPACE, VK_BACK, "Backspace"},
    {GLFW_KEY_INSERT, VK_INSERT, "Insert"},
    {GLFW_KEY_DELETE, VK_DELETE, "Delete"},
    
    {GLFW_KEY_RIGHT, VK_RIGHT, "Right"},
    {GLFW_KEY_LEFT, VK_LEFT, "Left"},
    {GLFW_KEY_DOWN, VK_DOWN, "Down"},
    {GLFW_KEY_UP, VK_UP, "Up"},
    
    {GLFW_KEY_PAGE_UP, VK_PRIOR, "Page Up"},
    {GLFW_KEY_PAGE_DOWN, VK_NEXT, "Page Down"},
    {GLFW_KEY_HOME, VK_HOME, "Home"},
    {GLFW_KEY_END, VK_END, "End"},
    
    {GLFW_KEY_CAPS_LOCK, VK_CAPITAL, "Caps Lock"},
    {GLFW_KEY_SCROLL_LOCK, VK_SCROLL, "Scroll Lock"},
    {GLFW_KEY_NUM_LOCK, VK_NUMLOCK, "Num Lock"},
    {GLFW_KEY_PRINT_SCREEN, VK_SNAPSHOT, "Print Screen"},
    {GLFW_KEY_PAUSE, VK_PAUSE, "Pause"},
    
    {GLFW_KEY_F1, VK_F1, "F1"},
    {GLFW_KEY_F2, VK_F2, "F2"},
    {GLFW_KEY_F3, VK_F3, "F3"},
    {GLFW_KEY_F4, VK_F4, "F4"},
    {GLFW_KEY_F5, VK_F5, "F5"},
    {GLFW_KEY_F6, VK_F6, "F6"},
    {GLFW_KEY_F7, VK_F7, "F7"},
    {GLFW_KEY_F8, VK_F8, "F8"},
    {GLFW_KEY_F9, VK_F9, "F9"},
    {GLFW_KEY_F10, VK_F10, "F10"},
    {GLFW_KEY_F11, VK_F11, "F11"},
    {GLFW_KEY_F12, VK_F12, "F12"},
    {GLFW_KEY_F13, VK_F13, "F13"},
    {GLFW_KEY_F14, VK_F14, "F14"},
    {GLFW_KEY_F15, VK_F15, "F15"},
    {GLFW_KEY_F16, VK_F16, "F16"},
    {GLFW_KEY_F17, VK_F17, "F17"},
    {GLFW_KEY_F18, VK_F18, "F18"},
    {GLFW_KEY_F19, VK_F19, "F19"},
    {GLFW_KEY_F20, VK_F20, "F20"},
    {GLFW_KEY_F21, VK_F21, "F21"},
    {GLFW_KEY_F22, VK_F22, "F22"},
    {GLFW_KEY_F23, VK_F23, "F23"},
    {GLFW_KEY_F24, VK_F24, "F24"},
    {GLFW_KEY_F25, 0, "F25"},
    
    {GLFW_KEY_0, '0', "0"},
    {GLFW_KEY_1, '1', "1"},
    {GLFW_KEY_2, '2', "2"},
    {GLFW_KEY_3, '3', "3"},
    {GLFW_KEY_4, '4', "4"},
    {GLFW_KEY_5, '5', "5"},
    {GLFW_KEY_6, '6', "6"},
    {GLFW_KEY_7, '7', "7"},
    {GLFW_KEY_8, '8', "8"},
    {GLFW_KEY_9, '9', "9"},
    
    {GLFW_KEY_A, 'A', "A"},
    {GLFW_KEY_B, 'B', "B"},
    {GLFW_KEY_C, 'C', "C"},
    {GLFW_KEY_D, 'D', "D"},
    {GLFW_KEY_E, 'E', "E"},
    {GLFW_KEY_F, 'F', "F"},
    {GLFW_KEY_G, 'G', "G"},
    {GLFW_KEY_H, 'H', "H"},
    {GLFW_KEY_I, 'I', "I"},
    {GLFW_KEY_J, 'J', "J"},
    {GLFW_KEY_K, 'K', "K"},
    {GLFW_KEY_L, 'L', "L"},
    {GLFW_KEY_M, 'M', "M"},
    {GLFW_KEY_N, 'N', "N"},
    {GLFW_KEY_O, 'O', "O"},
    {GLFW_KEY_P, 'P', "P"},
    {GLFW_KEY_Q, 'Q', "Q"},
    {GLFW_KEY_R, 'R', "R"},
    {GLFW_KEY_S, 'S', "S"},
    {GLFW_KEY_T, 'T', "T"},
    {GLFW_KEY_U, 'U', "U"},
    {GLFW_KEY_V, 'V', "V"},
    {GLFW_KEY_W, 'W', "W"},
    {GLFW_KEY_X, 'X', "X"},
    {GLFW_KEY_Y, 'Y', "Y"},
    {GLFW_KEY_Z, 'Z', "Z"},
    
    {GLFW_KEY_SPACE, VK_SPACE, "Space"},
    {GLFW_KEY_APOSTROPHE, VK_OEM_7, "'"},
    {GLFW_KEY_COMMA, VK_OEM_COMMA, ","},
    {GLFW_KEY_MINUS, VK_OEM_MINUS, "-"},
    {GLFW_KEY_PERIOD, VK_OEM_PERIOD, "."},
    {GLFW_KEY_SLASH, VK_OEM_2, "/"},
    {GLFW_KEY_SEMICOLON, VK_OEM_1, ";"},
    {GLFW_KEY_EQUAL, VK_OEM_PLUS, "="},
    {GLFW_KEY_LEFT_BRACKET, VK_OEM_4, "["},
    {GLFW_KEY_BACKSLASH, VK_OEM_5, "\\"},
    {GLFW_KEY_RIGHT_BRACKET, VK_OEM_6, "]"},
    {GLFW_KEY_GRAVE_ACCENT, VK_OEM_3, "`"},
    
    {GLFW_KEY_KP_0, VK_NUMPAD0, "Num 0"},
    {GLFW_KEY_KP_1, VK_NUMPAD1, "Num 1"},
    {GLFW_KEY_KP_2, VK_NUMPAD2, "Num 2"},
    {GLFW_KEY_KP_3, VK_NUMPAD3, "Num 3"},
    {GLFW_KEY_KP_4, VK_NUMPAD4, "Num 4"},
    {GLFW_KEY_KP_5, VK_NUMPAD5, "Num 5"},
    {GLFW_KEY_KP_6, VK_NUMPAD6, "Num 6"},
    {GLFW_KEY_KP_7, VK_NUMPAD7, "Num 7"},
    {GLFW_KEY_KP_8, VK_NUMPAD8, "Num 8"},
    {GLFW_KEY_KP_9, VK_NUMPAD9, "Num 9"},
    {GLFW_KEY_KP_DECIMAL, VK_DECIMAL, "Num ."},
    {GLFW_KEY_KP_DIVIDE, VK_DIVIDE, "Num /"},
    {GLFW_KEY_KP_MULTIPLY, VK_MULTIPLY, "Num *"},
    {GLFW_KEY_KP_SUBTRACT, VK_SUBTRACT, "Num -"},
    {GLFW_KEY_KP_ADD, VK_ADD, "Num +"},
    {GLFW_KEY_KP_ENTER, VK_RETURN, "Num Enter"},
    {GLFW_KEY_KP_EQUAL, VK_OEM_PLUS, "Num ="},
    
    {GLFW_KEY_LEFT_SHIFT, VK_LSHIFT, "Left Shift"},
    {GLFW_KEY_LEFT_CONTROL, VK_LCONTROL, "Left Ctrl"},
    {GLFW_KEY_LEFT_ALT, VK_LMENU, "Left Alt"},
    {GLFW_KEY_LEFT_SUPER, VK_LWIN, "Left Super"},
    {GLFW_KEY_RIGHT_SHIFT, VK_RSHIFT, "Right Shift"},
    {GLFW_KEY_RIGHT_CONTROL, VK_RCONTROL, "Right Ctrl"},
    {GLFW_KEY_RIGHT_ALT, VK_RMENU, "Right Alt"},
    {GLFW_KEY_RIGHT_SUPER, VK_RWIN, "Right Super"},
    {GLFW_KEY_MENU, VK_APPS, "Menu"},
    
    {GLFW_KEY_WORLD_1, 0, "World 1"},
    {GLFW_KEY_WORLD_2, 0, "World 2"}
};

static const std::vector<KeyMapping> xcbKeyMappings = {
    {0, 0, "None"},
    {GLFW_KEY_ESCAPE, 9, "Esc"},
    {GLFW_KEY_ENTER, 36, "Enter"},
    {GLFW_KEY_TAB, 23, "Tab"},
    {GLFW_KEY_BACKSPACE, 22, "Backspace"},
    {GLFW_KEY_INSERT, 110, "Insert"},
    {GLFW_KEY_DELETE, 119, "Delete"},
    
    {GLFW_KEY_RIGHT, 114, "Right"},
    {GLFW_KEY_LEFT, 113, "Left"},
    {GLFW_KEY_DOWN, 116, "Down"},
    {GLFW_KEY_UP, 111, "Up"},
    
    {GLFW_KEY_PAGE_UP, 112, "Page Up"},
    {GLFW_KEY_PAGE_DOWN, 117, "Page Down"},
    {GLFW_KEY_HOME, 110, "Home"},
    {GLFW_KEY_END, 115, "End"},
    
    {GLFW_KEY_CAPS_LOCK, 66, "Caps Lock"},
    {GLFW_KEY_SCROLL_LOCK, 78, "Scroll Lock"},
    {GLFW_KEY_NUM_LOCK, 77, "Num Lock"},
    {GLFW_KEY_PRINT_SCREEN, 107, "Print Screen"},
    {GLFW_KEY_PAUSE, 127, "Pause"},
    
    {GLFW_KEY_F1, 67, "F1"},
    {GLFW_KEY_F2, 68, "F2"},
    {GLFW_KEY_F3, 69, "F3"},
    {GLFW_KEY_F4, 70, "F4"},
    {GLFW_KEY_F5, 71, "F5"},
    {GLFW_KEY_F6, 72, "F6"},
    {GLFW_KEY_F7, 73, "F7"},
    {GLFW_KEY_F8, 74, "F8"},
    {GLFW_KEY_F9, 75, "F9"},
    {GLFW_KEY_F10, 76, "F10"},
    {GLFW_KEY_F11, 95, "F11"},
    {GLFW_KEY_F12, 96, "F12"},
    {GLFW_KEY_F13, 183, "F13"},
    {GLFW_KEY_F14, 184, "F14"},
    {GLFW_KEY_F15, 185, "F15"},
    {GLFW_KEY_F16, 186, "F16"},
    {GLFW_KEY_F17, 187, "F17"},
    {GLFW_KEY_F18, 188, "F18"},
    {GLFW_KEY_F19, 189, "F19"},
    {GLFW_KEY_F20, 190, "F20"},
    {GLFW_KEY_F21, 191, "F21"},
    {GLFW_KEY_F22, 192, "F22"},
    {GLFW_KEY_F23, 193, "F23"},
    {GLFW_KEY_F24, 194, "F24"},
    {GLFW_KEY_F25, 0, "F25"},
    
    {GLFW_KEY_0, 19, "0"},
    {GLFW_KEY_1, 10, "1"},
    {GLFW_KEY_2, 11, "2"},
    {GLFW_KEY_3, 12, "3"},
    {GLFW_KEY_4, 13, "4"},
    {GLFW_KEY_5, 14, "5"},
    {GLFW_KEY_6, 15, "6"},
    {GLFW_KEY_7, 16, "7"},
    {GLFW_KEY_8, 17, "8"},
    {GLFW_KEY_9, 18, "9"},
    
    {GLFW_KEY_A, 38, "A"},
    {GLFW_KEY_B, 56, "B"},
    {GLFW_KEY_C, 54, "C"},
    {GLFW_KEY_D, 40, "D"},
    {GLFW_KEY_E, 26, "E"},
    {GLFW_KEY_F, 41, "F"},
    {GLFW_KEY_G, 42, "G"},
    {GLFW_KEY_H, 43, "H"},
    {GLFW_KEY_I, 31, "I"},
    {GLFW_KEY_J, 44, "J"},
    {GLFW_KEY_K, 45, "K"},
    {GLFW_KEY_L, 46, "L"},
    {GLFW_KEY_M, 58, "M"},
    {GLFW_KEY_N, 57, "N"},
    {GLFW_KEY_O, 32, "O"},
    {GLFW_KEY_P, 33, "P"},
    {GLFW_KEY_Q, 24, "Q"},
    {GLFW_KEY_R, 27, "R"},
    {GLFW_KEY_S, 39, "S"},
    {GLFW_KEY_T, 28, "T"},
    {GLFW_KEY_U, 30, "U"},
    {GLFW_KEY_V, 55, "V"},
    {GLFW_KEY_W, 25, "W"},
    {GLFW_KEY_X, 53, "X"},
    {GLFW_KEY_Y, 29, "Y"},
    {GLFW_KEY_Z, 52, "Z"},
    
    {GLFW_KEY_SPACE, 65, "Space"},
    {GLFW_KEY_APOSTROPHE, 48, "'"},
    {GLFW_KEY_COMMA, 59, ","},
    {GLFW_KEY_MINUS, 20, "-"},
    {GLFW_KEY_PERIOD, 60, "."},
    {GLFW_KEY_SLASH, 61, "/"},
    {GLFW_KEY_SEMICOLON, 47, ";"},
    {GLFW_KEY_EQUAL, 21, "="},
    {GLFW_KEY_LEFT_BRACKET, 34, "["},
    {GLFW_KEY_BACKSLASH, 51, "\\"},
    {GLFW_KEY_RIGHT_BRACKET, 35, "]"},
    {GLFW_KEY_GRAVE_ACCENT, 49, "`"},
    
    {GLFW_KEY_KP_0, 90, "Num 0"},
    {GLFW_KEY_KP_1, 87, "Num 1"},
    {GLFW_KEY_KP_2, 88, "Num 2"},
    {GLFW_KEY_KP_3, 89, "Num 3"},
    {GLFW_KEY_KP_4, 83, "Num 4"},
    {GLFW_KEY_KP_5, 84, "Num 5"},
    {GLFW_KEY_KP_6, 85, "Num 6"},
    {GLFW_KEY_KP_7, 79, "Num 7"},
    {GLFW_KEY_KP_8, 80, "Num 8"},
    {GLFW_KEY_KP_9, 81, "Num 9"},
    {GLFW_KEY_KP_DECIMAL, 134, "Num ."},
    {GLFW_KEY_KP_DIVIDE, 91, "Num /"},
    {GLFW_KEY_KP_MULTIPLY, 63, "Num *"},
    {GLFW_KEY_KP_SUBTRACT, 82, "Num -"},
    {GLFW_KEY_KP_ADD, 86, "Num +"},
    {GLFW_KEY_KP_ENTER, 108, "Num Enter"},
    {GLFW_KEY_KP_EQUAL, 21, "Num ="},
    
    {GLFW_KEY_LEFT_SHIFT, 50, "Left Shift"},
    {GLFW_KEY_LEFT_CONTROL, 37, "Left Ctrl"},
    {GLFW_KEY_LEFT_ALT, 64, "Left Alt"},
    {GLFW_KEY_LEFT_SUPER, 133, "Left Super"},
    {GLFW_KEY_RIGHT_SHIFT, 62, "Right Shift"},
    {GLFW_KEY_RIGHT_CONTROL, 105, "Right Ctrl"},
    {GLFW_KEY_RIGHT_ALT, 108, "Right Alt"},
    {GLFW_KEY_RIGHT_SUPER, 134, "Right Super"},
    {GLFW_KEY_MENU, 135, "Menu"},
    
    {GLFW_KEY_WORLD_1, 0, "World 1"},
    {GLFW_KEY_WORLD_2, 0, "World 2"}
};

const std::vector<KeyMapping>& GetKeyMappings() {
    return keyMappings;
}

std::string GetNameFromGLFW(int glfwKey) {
    auto it = std::find_if(keyMappings.begin(), keyMappings.end(), 
        [glfwKey](const KeyMapping& mapping) { return mapping.glfwKey == glfwKey; });
    
    return (it != keyMappings.end()) ? it->name : "Unknown";
}

std::string GetNameFromWinAPI(int winApiKey) {
    auto it = std::find_if(keyMappings.begin(), keyMappings.end(), 
        [winApiKey](const KeyMapping& mapping) { return mapping.winApiKey == winApiKey; });
    
    return (it != keyMappings.end()) ? it->name : "Unknown";
}

int GetGLFWFromWinAPI(int winApiKey) {
    auto it = std::find_if(keyMappings.begin(), keyMappings.end(), 
        [winApiKey](const KeyMapping& mapping) { return mapping.winApiKey == winApiKey; });
    
    return (it != keyMappings.end()) ? it->glfwKey : 0;
}

int GetWinAPIFromGLFW(int glfwKey) {
    auto it = std::find_if(keyMappings.begin(), keyMappings.end(), 
        [glfwKey](const KeyMapping& mapping) { return mapping.glfwKey == glfwKey; });
    
    return (it != keyMappings.end()) ? it->winApiKey : 0;
}

int GetXcbFromGLFW(int glfwKey) {
    auto it = std::find_if(xcbKeyMappings.begin(), xcbKeyMappings.end(), 
        [glfwKey](const KeyMapping& mapping) { return mapping.glfwKey == glfwKey; });
    
    return (it != xcbKeyMappings.end()) ? it->winApiKey : 0;
}

const KeyMapping* GetMappingFromGLFW(int glfwKey) {
    auto it = std::find_if(keyMappings.begin(), keyMappings.end(), 
        [glfwKey](const KeyMapping& mapping) { return mapping.glfwKey == glfwKey; });
    
    return (it != keyMappings.end()) ? &(*it) : nullptr;
}

const KeyMapping* GetMappingFromWinAPI(int winApiKey) {
    auto it = std::find_if(keyMappings.begin(), keyMappings.end(), 
        [winApiKey](const KeyMapping& mapping) { return mapping.winApiKey == winApiKey; });
    
    return (it != keyMappings.end()) ? &(*it) : nullptr;
}

}
#endif