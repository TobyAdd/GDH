#include "includes.hpp"

template <class R, class T>
R& from(T base, intptr_t offset) {
	return *reinterpret_cast<R*>(reinterpret_cast<uintptr_t>(base) + offset);
}

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

    int keybind = 0;
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
    static const auto folderPath = geode::Mod::get()->getSaveDir();
    static const auto folderMacroPath = folderPath / "Macros";
    extern std::filesystem::path folderShowcasesPath;
    static const auto fileDataPath = folderPath / "config.json";

    extern std::vector<window> windows;
    void init();

    extern bool nolcip_enabled;
    extern bool noclip_p1;
    extern bool noclip_p2;

    extern bool auto_song_download;

    extern bool auto_pickup_coins;
    extern bool unlock_items;
    extern bool ignore_esc;
    extern bool startpos_switcher;
    extern bool startpos_switcher_reset_camera;
    extern bool instant_complate;

    extern bool rgb_icons;
    extern float ricon_coef;
    extern float ricon_shift;
    extern float ricon_saturation;
    extern float ricon_brightness;
    extern float ricon_delta;

    extern bool tps_enabled;
    extern float tps_value;

    extern int speed_key;
    extern bool speed_enabled;
    extern float speed_value;
    extern bool speedhack_audio;

    extern bool hide_menu;

    extern bool respawn_time_enabled;
    extern float respawn_time_value;
    
    extern bool jump_hack;

    extern bool show_hitboxes;
    extern bool draw_trail;
    extern int trail_length;
    extern bool show_hitboxes_on_death;

    extern bool wave_trail;
    extern float wave_trail_size;

    extern bool tint_on_death;

    extern int playback_key;

    extern bool random_seed_enabled;
    extern int seed_value;

    extern int frame_advance_key;
    extern int frame_advance_disable_key;

    extern bool reset_maket;

    extern bool disable_render;

    extern bool no_death_effect;

    void update_framerate(float value);

    void save(const std::vector<window>& windows, const std::filesystem::path &filename);
    void load(const std::filesystem::path &filename, std::vector<window>& windows);

    std::string GetKeyName(int key);
}

namespace startpos_switcher {
    extern bool smart_startpos;
    extern int left_key;
    extern int right_key;
}