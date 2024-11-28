#include "hacks.hpp"
#include "config.hpp"
#include "gui.hpp"
#include <imgui-cocos.hpp>

void Hacks::Init() {
    m_windows = {
        {"Core", 10, 10, 200, 230, 
            {
                {"Free Window Resize", "Allows free window resizing", "free_win_resize"}, // +
                {"Noclip", "The player will be invincible to obstacles", "noclip"}, // +
                {"Unlock Items", "The following elements will be unlocked:\n- Icons + Colors\n- Practice Music Sync\n- Music Unlocker", "unlock_items"}, // +
                {"No Respawn Blink", "Upon respawning, the cube will not produce an unpleasant flicker", "no_respawn_blink"}, // +
                {"No Death Effect", "Upon death, the cube will not emit an exploding effect", "no_death_effect"}, // +
                {"No Transition Old", "Fast transition between scenes (legacy)", "no_transition"}, // +
                {"Safe Mode", "Disables progress on levels", "safe_mode"} // +
            }
        },
        {"Bypass", 10, 250, 200, 250, 
            {  
                {"Text Length", "Removes the text input limit", "text_lenght"}, // +
                {"Slider Limit", "Removes the limit on sliding beyond set limits", "slider_limit"},
                {"Character Filter", "Lets you input any character in all text inputs", "char_filter"},  // +     
                {"Checkpoint Limit", "Removes the limit that deletes previous checkpoints after the 50th checkpoint", "checkpoint_limit"},
                {"Treasure Room", "Unlocks the Treasure Room", "treasure_room"}, // +    
                {"Unlock Shops", "Unlocks all shops", "unlock_shops"}, // +  
                {"Unlock Vaults", "Unlocks all Vaults and doors", "unlock_vaults"},
                {"Main Levels", "Unlocks all main levels", "main_levels"} // +     
            }
        },
        {"Player", 220, 10, 220, 640, 
            {
                {"Accurate Percentage", "Shows decimals in level progress", "accurate_percentage", "0126"}, // +
                {"Auto Pickup Coins", "Collects all coins in the level", "auto_pickup_coins"}, // +
                {"Auto Practice Mode", "Auto-enables practice mode", "auto_practice_mode"}, // +
                {"Auto Song Download", "Automatic downloading of song when you enter an online level", "auto_song_download"},  // +
                {"Allow Low Volume", "Removes the limit on minimum volume percentage", "allow_low_volume"},  // +
                {"Coins In Practice", "The ability to collect coins in practice", "coins_in_practice"},
                {"Confirm Exit", "Warning before level exit", "confim_exit", "0167"},  // +  
                {"Fast Chest Open", "Removes the delay for opening chests", "fast_chest_open"},
                {"Force Dont Enter", "Disables effects when objects enter the viewable play area", "force_dont_enter"},
                {"Force Dont Fade", "Disables effects when objects leave the viewable play area", "force_dont_fade"},
                {"Random Seed", "Changes the seed game so that the random trigger is not triggered randomly", "random_seed"},
                {"Respawn Time", "Changes respawn time on death", "respawn_time"},  // +
                {"Restart Level", "Reload the level", "restart_level"},
                {"Practice Mode", "Enter practice mode", "practice_mode"},
                {"Ignore ESC", "Prevents exiting the level", "ignore_esc"},  // +
                {"Instant Complete", "Instant level completion", "instant_complete"},  // +
                {"Jump Hack", "Removes the barrier to jump gravity", "jump_hack"},  // +
                {"Show Percentage", "Show percentages in level progress", "show_percentage", "0040"},  // +
                {"Smart Startpos", "Restores correct gameplay without startpos settings", "smart_startpos"},
                {"Startpos Switcher", "The ability to switch between starting positions using the keys that you setted in keybinds", "startpos_switcher"}, // +
                {"RGB Icons", "LGBT icons, yes :3", "rgb_icons"},
                {"Solid Wave Trail", "Disables wave blending", "solid_wave_trail"},
                {"Show Triggers", "Displaying triggers on the PlayLayer", "show_triggers"},
                {"Show Hitboxes", "Visualizes hitbox levels", "show_hitboxes"},
                {"Show Total Attempts", "", "show_total_attempts"},   // +
                {"Stop triggers on death", "Stops move/rotation triggers on death so you can see what killed you", "stop_triggers_on_death"},   // +
                {"All Modes Platformer", "Removes the limit on all modes in the platformer", "all_modes_platformer"},
                {"Force Platformer", "Enables platformer mode in all levels", "force_platformer"},
                {"Hide Attempts", "Hides the attempt count in-game", "hide_attempts", "0135"}, // +
                {"Hide Pause Button", "Removes the pause button when the cursor is shown", "hide_pause_button"}, // +
                {"Hide Pause Menu", "Removes the pause menu", "hide_pause_menu"},  // +        
                {"Hide Playtest Text", "Hides text in the top left when using start positions or ignore damage", "hide_playtest_text", "0174"},  // +    
                {"No Camera Move", "Disables camera movement via trigger", "no_camera_move"},  // +
                {"No Camera Zoom", "Disables camera zooming via trigger", "no_camera_zoom"},  // +
                {"No \"Do Not\" Flip", "Accurate display of attempts with the \"Do not\" function enabled, without flips", "no_do_not_flip"}, // +
                {"No Shaders", "Disabling shaders in levels", "no_shaders"},  // +
                {"No Particles", "Disables resuming the particle system", "no_particles"}, // +
                {"No Short Numbers", "All numbers are displayed in full\n(For example, \"1.5M\" becomes \"1500000\")", "no_short_numbers"}, // +
                {"No BG Flash", "Removes the unpleasant flicker when triggering the portal", "no_bg_flash"}, // +
                {"No Glow", "Disables glow on objects", "no_glow"}, // +
                {"No Mirror", "Disables level mirroring", "no_mirror_portal"}, // +
                {"No New Best Popup", "Disable the new best popup", "no_new_best_popup"}, // +
                {"No Portal Lighting", "Disables lightning when entering mini/large portal", "no_portal_lighting"},
                {"No Pulse", "Disables pulsation of falls, orbs, etc", "no_pulse"}, // +
                {"Pulse Size", "Changes pulsation of falls, orbs, etc", "pulse_size"}, // +
                {"No Robot Fire", "Hides robot boost fire", "no_robot_fire"}, // +
                {"No Spider Dash", "Disables spider dash trail when teleporting", "no_spider_dash"}, // +
                {"No Trail", "Removes the trail located near the player", "no_trail"},  // +
                {"Always Trail", "Displays the trail near the player at any location", "always_trail"},  // +
                {"Wave Trail Size", "Resizes the wave trail", "wave_trail_size"},   // +
                {"Wave Trail On Death", "", "wave_trail_on_death"}   // +
            }
        },
        {"Creator", 450, 10, 220, 250, 
            {
                {"Copy Hack", "Copy any online level without a password", "copy_hack"},  // +
                {"Custom Object Bypass", "Removes the limit restricted to 1000 objects", "custom_object_bypass"},
                {"Default Song Bypass", "Removes restrictions on secret official songs", "default_song_bypass"}, // +
                {"Editor Extension", "", "editor_extension"},
                {"Verify Hack", "Publish a level without verification", "verify_hack"}, // +
                {"Smooth Editor Trail", "Makes the wave smoother in the editor", "smooth_editor_trail"}, // +
                {"Level Edit", "Edit any online level", "level_edit"}, // +
                {"No (C) Mark", "Removes copyright on copied levels", "no_c_mark"} // +
            }
        },
        {"Framerate", 450, 270, 220, 130},
        {"GDH Settings", 450, 410, 220, 130},
        {"Replay Engine", 680, 10, 300, 200},
        {"Labels", 680, 230, 300, 320},
        {"Variables", 10, 510, 200, 160}
    };

    auto &config = Config::get();

    #ifdef GEODE_IS_WINDOWS
    SetHandlerByConfig("free_win_resize", [this](bool enabled) {
        static auto result1 = geode::Mod::get()->patch((void*)(geode::base::getCocos() + 0xd6eca), {0x90, 0x90, 0x90, 0x90, 0x90});
        static auto patch1 = result1.isErr() ? nullptr : result1.unwrap();

        static auto result2 = geode::Mod::get()->patch((void*)(geode::base::getCocos() + 0xd5089), {0x90, 0x90, 0x90, 0x90, 0x90, 0x90});
        static auto patch2 = result1.isErr() ? nullptr : result2.unwrap();

        static auto result3 = geode::Mod::get()->patch((void*)(geode::base::getCocos() + 0xd6567), {0x48, 0xe9});
        static auto patch3 = result1.isErr() ? nullptr : result3.unwrap();

        if (enabled) {
            if (patch1) (void) patch1->enable();
            if (patch2) (void) patch1->enable();
            if (patch3) (void) patch1->enable();
        } else {
            if (patch1) (void) patch1->disable();
            if (patch2) (void) patch1->disable();
            if (patch3) (void) patch1->disable();
        }        
    });

    #endif

    SetHandlerByConfig("editor_extension", [this](bool enabled) {
        uintptr_t address1; //Pattern: 00 60 6A 48 (2x)
        uintptr_t address2; //Pattern: 80 67 6A 48 (1x)

        #ifdef GEODE_IS_WINDOWS 
        address1 = geode::base::get() + 0x607ca0;
        address2 = geode::base::get() + 0x607ca4;
        #elif defined(GEODE_IS_ANDROID64) 
        address1 = geode::base::get() + 0x65d740;
        address2 = geode::base::get() + 0x67790c;
        #endif


        static auto result1 = geode::Mod::get()->patch((void*)(address1), {0x00, 0x60, 0xEA, 0x4B});
        static auto patch1 = result1.isErr() ? nullptr : result1.unwrap();

        static auto result2 = geode::Mod::get()->patch((void*)(address2), {0x00, 0x60, 0xEA, 0x4B});
        static auto patch2 = result1.isErr() ? nullptr : result2.unwrap();

        if (enabled) {
            if (patch1) (void) patch1->enable();
            if (patch2) (void) patch2->enable();
        } else {
            if (patch1) (void) patch1->disable();
            if (patch2) (void) patch2->disable();
        }        
    });
    

    SetHandlerByConfig("hide_pause_menu", [this](bool enabled) {
        auto pl = PlayLayer::get();
        if (pl && pl->m_isPaused && pauseLayer != nullptr)
            pauseLayer->setVisible(!enabled);
    });

    SetCustomWindowHandlerByConfig("wave_trail_size", [this, &config]() {
        float value = config.get<float>("wave_trail_size_value", 1.f);

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::DragFloat("##waveTrailSize", &value, 0.01f, 0.0f, FLT_MAX, "Wave Trail Size: %.2f")) 
            config.set("wave_trail_size_value", value);
    });

    SetCustomWindowHandlerByConfig("respawn_time", [this, &config]() {
        float value = config.get<float>("respawn_time_value", 1.f);

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::DragFloat("##respawn_time_value", &value, 0.01f, 0.0f, FLT_MAX, "Respawn Time: %.2f")) 
            config.set("respawn_time_value", value);
    });

    SetCustomWindowHandlerByConfig("pulse_size", [this, &config]() {
        auto &gui = Gui::get();

        float value = config.get<float>("pulse_size_value", 0.5f);
        bool multiply = config.get<bool>("pulse_multiply", false);

        if (ImGuiH::Checkbox("Multiply pulsation", &multiply, gui.m_scale)) {
            config.set<bool>("pulse_multiply", multiply);
        }

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::DragFloat("##pulse_size_value", &value, 0.01f, 0.0f, FLT_MAX, "Pulse Size: %.2f")) 
            config.set("pulse_size_value", value);
    });

    SetCustomWindowHandlerByConfig("startpos_switcher", [this, &config]() {
        auto &gui = Gui::get();

        bool reset_camera = config.get<bool>("startos_switcher::reset_camera", true);

        if (ImGuiH::Checkbox("Reset Camera", &reset_camera, gui.m_scale)) {
            config.set<bool>("startos_switcher::reset_camera", reset_camera);
        }
    });

    for (auto& win : m_windows) {
        win.orig_x = win.x;
        win.orig_y = win.y;
        win.orig_w = win.w;
        win.orig_h = win.h;

        for (auto& hck : win.hacks) {
            if (!hck.game_var.empty()) config.set<bool>(hck.config, GameManager::get()->getGameVariable(hck.game_var.c_str()));
            if (config.get<bool>(hck.config, false) && hck.handlerFunc) hck.handlerFunc(true);
        }
    }
}