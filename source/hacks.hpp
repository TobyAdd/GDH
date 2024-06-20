#include <string>
#include <vector>
#include "memory.hpp"

struct opcode {    
    std::string pattern;
    std::string on;
    std::string module = "";

    std::string off;
    uintptr_t address;
};

struct hack {
    std::string name;
    std::string desc;
    std::vector<opcode> opcodes; 
    bool enabled = false;
};

struct window {
    std::string name;
    float x, y;
    float w, h;
    std::vector<hack> hacks;

    float orig_x, orig_y;
    float orig_w, orig_h;
};

namespace hacks {
    static const auto documentsPath = std::filesystem::path(getenv("USERPROFILE")) / "Documents";
    static const auto folderPath = documentsPath / "GDH";
    static const auto folderMacroPath = documentsPath / "GDH" / "Macros";
    static const auto fileDataPath = folderPath / "zxc.json";

    static const auto base = reinterpret_cast<uintptr_t>(GetModuleHandleA(0));
	static const auto cocos_base = reinterpret_cast<HMODULE>(GetModuleHandleA("libcocos2d.dll"));

    extern std::vector<window> windows;
    void init();

    extern bool unlock_items;
    extern bool ignore_esc;
    extern bool startpos_switcher;
    extern bool startpos_switcher_reset_camera;
    extern bool use_a_s_d;
    extern bool instant_complate;

    extern bool rgb_icons;
    extern float ricon_coef;
    extern float ricon_shift;
    extern float ricon_saturation;
    extern float ricon_brightness;
    extern float ricon_delta;

    extern bool fps_enabled;
    extern float fps_value;
    
    extern float speed_value;
    extern bool speedhack_audio;

    void update_framerate();

    void save(const std::vector<window>& windows, const std::filesystem::path &filename);
    void load(const std::filesystem::path &filename, std::vector<window>& windows);
}