#include "hacks.hpp"
#include "hooks.hpp"
#include "config.hpp"
#include "gui.hpp"
#include "labels.hpp"
#include "recorder.hpp"
#include "replayEngine.hpp"
#include <imgui-cocos.hpp>
#include "popupSystem.hpp"
#include "utils.hpp"

void Hacks::Init() {
    m_windows = {
        {"Core", 10, 10, 200, 230, 
            {
                #ifdef GEODE_IS_WINDOWS
                {"Free Window Resize", "Allows free window resizing", "free_win_resize"}, // +
                #endif
                {"Noclip", "The player will be invincible to obstacles", "noclip"}, // +
                {"Unlock Items", "The following elements will be unlocked:\n- Icons + Colors\n- Practice Music Sync\n- Music Unlocker", "unlock_items"}, // +
                {"No Respawn Blink", "Upon respawning, the cube will not produce an unpleasant flicker", "no_respawn_blink"}, // +
                {"No Death Effect", "Upon death, the cube will not emit an exploding effect", "no_death_effect"}, // +
                {"No Transition Old", "Fast transition between scenes (legacy)", "no_transition"}, // +
                {"Safe Mode", "Disables progress on levels", "safe_mode"}
                #ifdef GEODE_IS_ANDROID
                , // +
                {"TPS", "Multiplies the number of ticks per second, used mainly for botting\n(not recommended for normal use as it ruins the game's performance)", "tps_enabled"}, // +
                {"Speedhack", "", "speedhack_enabled"}, // +
                {"Speedhack Audio", "", "speedhackAudio_enabled"}, // +
                {"Invisible UI Button", "Hides the button that opens the menu", "invisible_ui_button"}, // +
                #endif
            }
        },
        {"Bypass", 10, 250, 200, 250, 
            {
                {"Text Length", "Removes the text input limit", "text_lenght"}, // +
                {"Slider Limit", "Removes the limit on sliding beyond set limits", "slider_limit"}, // + 
                {"Character Filter", "Lets you input any character in all text inputs", "char_filter"},  // +   
                {"Checkpoint Limit", "Removes the limit that deletes previous checkpoints after the 50th checkpoint", "checkpoint_limit"}, // +   
                {"Treasure Room", "Unlocks the Treasure Room", "treasure_room"}, // +    
                {"Unlock Shops", "Unlocks all shops", "unlock_shops"}, // +  
                {"Unlock Vaults", "Unlocks all Vaults and doors", "unlock_vaults"}, // +    
                {"Main Levels", "Unlocks all main levels", "main_levels"} // +     
            }
        },
        {"Player", 220, 10, 220, 660, 
            {
                {"Accurate Percentage", "Shows decimals in level progress", "accurate_percentage", "0126"}, // +
                {"Auto Pickup Coins", "Collects all coins in the level", "auto_pickup_coins"}, // +
                {"Auto Practice Mode", "Auto-enables practice mode", "auto_practice_mode"}, // +
                {"Auto Song Download", "Automatic downloading of song when you enter an online level", "auto_song_download"},  // +
                #ifdef GEODE_IS_WINDOWS
                {"Auto Deafen", "Deafens user in Discord after a certain %", "auto_deafen"},  // +
                #endif
                {"Allow Low Volume", "Removes the limit on minimum volume percentage", "allow_low_volume"},  // +
                {"Coins In Practice", "The ability to collect coins in practice", "coins_in_practice"},  // +
                {"Confirm Exit", "Warning before level exit", "confim_exit", "0167"},  // +  
                {"Cheat Indicator", "Adds a cheat indicator on the Level Complete screen:\nthe first dot shows current hacks legitimacy, and the second indicates if illegitimate cheats were used during the attempt", "cheat_indicator"},
                {"Fast Chest Open", "Removes the delay for opening chests", "fast_chest_open"}, 
                {"Fast Complete", "Fast level completion without animations", "fast_complete"}, 
                // {"Random Seed", "Changes the seed game so that the random trigger is not triggered randomly", "random_seed"},
                {"Respawn Time", "Changes respawn time on death", "respawn_time"},  // +
                {"Ignore ESC", "Prevents exiting the level", "ignore_esc"},  // +
                {"Instant Complete", "Instant level completion", "instant_complete"},  // +
                {"Jump Hack", "Removes the barrier to jump gravity", "jump_hack"},  // +
                {"Layout Mode", "Removes all decoration and colour from levels", "layout_mode"},  // +
                {"Show Percentage", "Show percentages in level progress", "show_percentage", "0040"},  // +
                {"Smart Startpos", "Restores correct gameplay without startpos settings", "smart_startpos"},  // +
                {"Spam Bot", "AKA Auto-Clicker", "spambot_enabled"},  // +
                {"Startpos Switcher", "The ability to switch between starting positions using the keys that you setted in keybinds (Q/E by default)", "startpos_switcher"}, // +
                {"RGB Icons", "LGBT icons, yes :3", "rgb_icons"}, // +
                {"Solid Wave Trail", "Disables wave blending", "solid_wave_trail"}, // +
                {"Show Hitboxes", "Visualizes hitbox levels", "show_hitboxes"}, // +
                {"Show Total Attempts", "", "show_total_attempts"},  // +
                {"Straight Fly Bot", "", "straight_fly_bot"},  // +
                {"Stop triggers on death", "Stops move/rotation triggers on death so you can see what killed you", "stop_triggers_on_death"},   // +
                // {"All Modes Platformer", "Removes the limit on all modes in the platformer", "all_modes_platformer"},
                {"Force Platformer", "Enables platformer mode in all levels", "force_platformer"}, // +
                {"Hide Attempts", "Hides the attempt count in-game", "hide_attempts", "0135"}, // +
                {"Hide Pause Button", "Removes the pause button when the cursor is shown", "hide_pause_button"}, // +
                {"Hide Pause Menu", "Removes the pause menu", "hide_pause_menu"},  // +        
                {"Hide Playtest Text", "Hides text in the top left when using start positions or ignore damage", "hide_playtest_text", "0174"},  // +    
                {"No Camera Move", "Disables camera movement via trigger", "no_camera_move"},  // +
                {"No Camera Zoom", "Disables camera zooming via trigger", "no_camera_zoom"},  // +
                {"No \"Do Not\" Flip", "Accurate display of attempts with the \"Do not\" function enabled, without flips", "no_do_not_flip"}, // +
                {"No Effect Circles", "Removes effect circles from orb, portal and pad activations", "no_orb_ring"}, // +
                {"No Shaders", "Disabling shaders in levels", "no_shaders"},  // +
                {"No Particles", "Disables resuming the particle system", "no_particles"}, // +
                {"No Short Numbers", "All numbers are displayed in full\n(For example, \"1.5M\" becomes \"1500000\")", "no_short_numbers"}, // +
                {"No BG Flash", "Removes the unpleasant flicker when triggering the portal", "no_bg_flash"}, // +
                {"No Glow", "Disables glow on objects", "no_glow"}, // +
                {"No Mirror", "Disables level mirroring", "no_mirror_portal"}, // +
                {"No New Best Popup", "Disable the new best popup", "no_new_best_popup"}, // +
                {"No Portal Lighting", "Disables lightning when entering mini/large portal", "no_portal_lighting"}, // +
                {"No Pulse", "Disables pulsation of falls, orbs, etc", "no_pulse"}, // +
                {"Pause On Complete", "Lets you pause during the level complete animation", "pause_during_complete"}, // +
                {"Pitch Shifter", "Shifts the pitch of background music", "pitch_shifter"}, // +
                {"Practice Fix", "More accurate respawning in practice mode (used for botting)", "practice_fix"},
                {"Pulse Size", "Changes pulsation of falls, orbs, etc", "pulse_size"}, // +
                #ifdef GEODE_IS_ANDROID
                {"Uncomplete Level", "Resets level completion statistics (it's just a button, it doesn't depend on on/off switch)", "uncomplete_level"}, // +
                #endif
                {"No Robot Fire", "Hides robot boost fire", "no_robot_fire"}, // +
                {"No Spider Dash", "Disables spider dash trail when teleporting", "no_spider_dash"}, // +
                {"No Trail", "Removes the trail located near the player", "no_trail"},  // +
                {"Always Trail", "Displays the trail near the player at any location", "always_trail"},  // +
                {"Wave Trail Size", "Resizes the wave trail", "wave_trail_size"},   // +
                {"No Wave Trail Fade", "", "wave_trail_on_death"}   // +
            }
        },
        {"Creator", 450, 10, 220, 310, 
            {
                {"Copy Hack", "Copy any online level without a password", "copy_hack"},  // +
                {"Custom Object Bypass", "Removes the limit restricted to 1000 objects", "custom_object_bypass"}, // +
                {"Default Song Bypass", "Removes restrictions on secret official songs", "default_song_bypass"}, // +
                #if defined(GEODE_IS_WINDOWS) || defined(GEODE_IS_ANDROID64)
                {"Editor Extension", "Increases the editor length by a factor of 128", "editor_extension"}, // +
                #endif
                {"Hide UI", "Hides the editor Ul while building", "hide_ui"}, // +
                {"Verify Hack", "Publish a level without verification", "verify_hack"}, // +
                {"Smooth Editor Trail", "Makes the wave smoother in the editor", "smooth_editor_trail"}, // +
                {"Level Edit", "Edit any online level", "level_edit"}, // +
                {"No (C) Mark", "Removes copyright on copied levels", "no_c_mark"}, // +
                #ifdef GEODE_IS_WINDOWS //ui layer class is almost broken for mobile :(
                {"Zoom Bypass", "", "zoom_bypass"} // +
                #endif
            }
        },
        {"Framerate", 450, 330, 220, 130},
        {"GDH Settings", 450, 470, 220, 230},
        {"Replay Engine", 680, 10, 300, 200},
        {"Labels", 680, 220, 300, 320},
        {"Variables", 10, 510, 200, 160},
        {"Shortcuts", 990, 10, 200, 270}
    };

    auto &config = Config::get();

    #ifdef GEODE_IS_WINDOWS
    SetCustomWindowHandlerByConfig("auto_deafen", [this, &config]() {
        auto &gui = Gui::get();
        #ifdef GEODE_IS_WINDOWS
  
        gui.renderKeyButton("Letter Key: ", gui.m_autoDeafenKey, true);
        
        int start = config.get<int>("auto_deafen::start", 50);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::SliderInt("##DeafenStart", &start, 0, 100, "Deafen At: %d%%"))
            config.set<int>("auto_deafen::start", start);

        int end = config.get<int>("auto_deafen::end", 100);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::SliderInt("##DeafenEnd", &end, 0, 100, "Undeafen At: %d%%"))
            config.set<int>("auto_deafen::end", end);

        bool defaen_in_practice = config.get<bool>("auto_deafen::defaen_in_practice", false);
        if (ImGuiH::Checkbox("Deafen in Practice", &defaen_in_practice, gui.m_scale))
            config.set<bool>("auto_deafen::defaen_in_practice", defaen_in_practice);

        bool defaen_with_startpos = config.get<bool>("auto_deafen::defaen_with_startpos", false);
        if (ImGuiH::Checkbox("Deafen with StartPos", &defaen_with_startpos, gui.m_scale))
            config.set<bool>("auto_deafen::defaen_with_startpos", defaen_with_startpos);

        bool undeafen_on_pause = config.get<bool>("auto_deafen::undeafen_on_pause", true);
        if (ImGuiH::Checkbox("Undeafen on Pause", &undeafen_on_pause, gui.m_scale))
            config.set<bool>("auto_deafen::undeafen_on_pause", undeafen_on_pause);
    
        #endif
    });
    #endif

    #ifdef GEODE_IS_ANDROID
    SetCustomWindowHandlerByConfig("tps_enabled", [this, &config]() {
        bool tps_real_time = config.get<bool>("tps::real_time", true);
        float tps_value = config.get<float>("tps_value", 60.f);
        int resumeTimer = config.get<int>("resumeTimer_value", 2);

        auto popup = popupSystem::create();
        popup->AddText("TPS Value:");
        popup->AddFloatInput("TPS Value", tps_value, [this, &config](float value) 
        {
            if (value > 1.f)
                config.set<float>("tps_value", value);
        },
        35.f);

        popup->AddToggle("Real Time", tps_real_time, [this, &config](bool enabled) 
        {
            config.set<bool>("tps::real_time", enabled);
        });

        popup->AddText("Resumer Timer:");

        popup->AddIntInput("Resumer Timer", resumeTimer, [this, &config](int value) 
        {
            config.set<int>("resumeTimer_value", std::clamp(value, 2, INT_MAX));
        },
        30.f);

        popup->AddText("Higher resume timer means a smoother\nstart but a delay (adjust as needed)", 0.35f);

        popup->show();
    });

    SetCustomWindowHandlerByConfig("speedhack_enabled", [this, &config]() {
        float speedhack_value = config.get<float>("speedhack_value", 1.f);
        auto popup = popupSystem::create();
        popup->AddText("Speedhack Value:");
        popup->AddFloatInput("Speedhack Value", speedhack_value, [this, &config](float value) 
        {
            if (value > 0.005f)
                config.set<float>("speedhack_value", value);
        },
        35.f);
        popup->show();
    });
    #endif

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

    SetCustomWindowHandlerByConfig("noclip", [this, &config]() {
        bool noclip_p1 = config.get<bool>("noclip::p1", true);
        bool noclip_p2 = config.get<bool>("noclip::p2", true);
        bool tint_on_death = config.get<bool>("noclip::tint_on_death", false);
        int tint_opacity = config.get<int>("noclip::tint_opacity", 100);
        float tint_fade = config.get<float>("noclip::tint_fade", 0.35f);

        #ifdef GEODE_IS_WINDOWS 
        auto &gui = Gui::get();
        if (ImGuiH::Checkbox("Player 1", &noclip_p1, gui.m_scale))
            config.set<bool>("noclip::p1", noclip_p1);

        ImGui::SameLine();

        if (ImGuiH::Checkbox("Player 2", &noclip_p2, gui.m_scale))
            config.set<bool>("noclip::p2", noclip_p2);

        if (ImGuiH::Checkbox("Tint on Death", &tint_on_death, gui.m_scale))
            config.set<bool>("noclip::tint_on_death", tint_on_death);

        if (ImGui::DragInt("##noclip::tint_opacity", &tint_opacity, 1, 0, 255, "Tint Opacity: %i"))
            config.set<int>("noclip::tint_opacity", tint_opacity);

        if (ImGui::DragFloat("##noclip::tint_fade", &tint_fade, 0.01f, 0, 1.f, "Tint Fade: %.2f"))
            config.set<float>("noclip::tint_fade", tint_fade);
            

        #elif defined(GEODE_IS_ANDROID) 
        auto popup = popupSystem::create();
        popup->AddToggle("Player 1", noclip_p1, [this, &config](bool enabled) 
        {
            config.set<bool>("noclip::p1", enabled);
        });

        popup->AddToggle("Player 2", noclip_p2, [this, &config](bool enabled) 
        {
            config.set<bool>("noclip::p2", enabled);
        });

        popup->AddToggle("Tint on Death", tint_on_death, [this, &config](bool enabled) 
        {
            config.set<bool>("noclip::tint_on_death", enabled);
        });

        popup->AddText("Tint Opacity (0 - 255):");

        popup->AddIntInput("Tint Opacity", tint_opacity, [this, &config](int value) 
        {
            config.set("noclip::tint_opacity", std::clamp(value, 0, 255));
        }, 25.f);

        popup->AddText("Tint Fade (0.0 - 1.0):");

        popup->AddFloatInput("Tint Fade", tint_fade, [this, &config](float value) 
        {
            config.set("noclip::tint_fade", std::clamp(value, 0.f, 1.f));
        }, 25.f);

        popup->show();
        #endif
    });

    SetCustomWindowHandlerByConfig("unlock_items", [this, &config]() {
        bool all_icons = config.get<bool>("unlock_items::all_icons", true);
        bool practice_music_sync = config.get<bool>("unlock_items::practice_music_sync", true);
        bool music_unlocker = config.get<bool>("unlock_items::music_unlocker", true);

        #ifdef GEODE_IS_WINDOWS 
        auto &gui = Gui::get();
        if (ImGuiH::Checkbox("All Icons", &all_icons, gui.m_scale))
            config.set<bool>("unlock_items::all_icons", all_icons);

        if (ImGuiH::Checkbox("Practice Music Sync", &practice_music_sync, gui.m_scale))
            config.set<bool>("unlock_items::practice_music_sync", practice_music_sync);

        if (ImGuiH::Checkbox("Music Unlocker", &music_unlocker, gui.m_scale))
            config.set<bool>("unlock_items::music_unlocker", music_unlocker);

        #elif defined(GEODE_IS_ANDROID) 
        auto popup = popupSystem::create();
        popup->AddToggle("All Icons", all_icons, [this, &config](bool enabled) 
        {
            config.set<bool>("unlock_items::all_icons", enabled);
        });

        popup->AddToggle("Practice Music Sync", practice_music_sync, [this, &config](bool enabled) 
        {
            config.set<bool>("unlock_items::practice_music_sync", enabled);
        });

        popup->AddToggle("Music Unlocker", music_unlocker, [this, &config](bool enabled) 
        {
            config.set<bool>("unlock_items::music_unlocker", enabled);
        });
        popup->show();
        #endif
    });

    SetHandlerByConfig("pitch_shifter", [this, &config](bool enabled) {
        utilsH::setPitchShifter(enabled ? config.get<float>("pitch_shifter_value", 1.f) : 1.f);
    });

    SetHandlerByConfig("uncomplete_level", [this, &config](bool enabled) {
        config.set<bool>("uncomplete_level", false);
        utilsH::UncompleteLevel();
    });

    SetHandlerByConfig("rgb_icons", [this](bool enabled) {
        auto& colors = RGBIcons::get();
        if (enabled && colors.colors.empty()) 
            for (const auto& color : colors.rainbowColors)
                colors.colors.push_back(color);
    });


    #if defined(GEODE_IS_WINDOWS) || defined(GEODE_IS_ANDROID64)

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

    #endif
    

    SetHandlerByConfig("hide_pause_menu", [this](bool enabled) {
        auto pl = PlayLayer::get();
        if (pl && pl->m_isPaused && pauseLayer != nullptr)
            pauseLayer->setVisible(!enabled);
    });

    SetHandlerByConfig("hide_ui", [this](bool enabled) {
        auto ui = EditorUI::get();
        if (ui)
            ui->setVisible(!enabled);
    });

    SetHandlerByConfig("spambot_enabled", [this](bool enabled) {
        auto& spamBot = SpamBot::get();
        spamBot.reset_temp();
    });
    
    SetHandlerByConfig("straight_fly_bot", [this](bool enabled) {
        auto& straightFly = StraightFly::get();
        straightFly.start(GJBaseGameLayer::get());
    });

    SetHandlerByConfig("show_hitboxes", [this](bool enabled) {
        auto pl = PlayLayer::get();
        if (pl && !enabled && !(pl->m_isPracticeMode && GameManager::get()->getGameVariable("0166"))) {
            pl->m_debugDrawNode->setVisible(false);
        }
    });

    SetCustomWindowHandlerByConfig("pitch_shifter", [this, &config]() {
        float pitch_value = config.get<float>("pitch_shifter_value", 1.f);

        #ifdef GEODE_IS_WINDOWS 
        auto &gui = Gui::get();

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::DragFloat("##PitchValue", &pitch_value, 0.01f, 0.5f, 2.f, "Pitch: %.2f")) {
            config.set<float>("pitch_shifter_value", pitch_value);
            if (config.get<bool>("pitch_shifter", false))
                utilsH::setPitchShifter(pitch_value);
        }
            
        #elif defined(GEODE_IS_ANDROID) 
        auto popup = popupSystem::create();
        popup->AddText("Pitch (0.5 - 2):");
        popup->AddFloatInput("Pitch Value", pitch_value, [&](float value) 
        {
            float limited_value = std::clamp(value, 0.5f, 2.f);
            config.set<float>("pitch_shifter_value", limited_value);
            if (config.get<bool>("pitch_shifter", false))
                utilsH::setPitchShifter(limited_value);
        },
        35.f);
        popup->show();
        #endif
    });

    SetCustomWindowHandlerByConfig("straight_fly_bot", [this, &config]() {
        auto& straightFly = StraightFly::get();
        #ifdef GEODE_IS_WINDOWS 
        auto &gui = Gui::get();

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::DragFloat("##StraightFlyAcc", &straightFly.accuracy, 0.1f, 0.f, 100.f, "Y Accuracy: %.1f");
        ImGui::Text("Note: Straight Fly Bot works only on first player");
        #elif defined(GEODE_IS_ANDROID) 
        auto popup = popupSystem::create();
        popup->AddText("Y Accuracy:");
        popup->AddFloatInput("Y Accuracy", straightFly.accuracy, [this, &straightFly](float value) 
        {
            straightFly.accuracy = value;
        },
        35.f);

        popup->AddText("Note: Straight Fly Bot works only\non first player", 0.45f);
        popup->show();
        #endif
    });

    SetCustomWindowHandlerByConfig("spambot_enabled", [this, &config]() {
        int spamBotHoldLength = config.get<int>("hold_length", 5);
        int spamBotReleaseLength = config.get<int>("release_length", 5);
        bool spamBotPlayer1 = config.get<bool>("spambot_player1", true);
        bool spamBotPlayer2 = config.get<bool>("spambot_player2", false);

        #ifdef GEODE_IS_WINDOWS 
        auto &gui = Gui::get();

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 2);
        if (ImGui::DragInt("##spamhold", &spamBotHoldLength, 1, 1, INT_MAX, "Hold Length: %i"))
            config.set<int>("hold_length", spamBotHoldLength);

        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::DragInt("##spamrelease", &spamBotReleaseLength, 1, 1, INT_MAX, "Release Length: %i"))
            config.set<int>("release_length", spamBotReleaseLength);

        if (ImGuiH::Checkbox("Player 1", &spamBotPlayer1, gui.m_scale))
            config.set<bool>("spambot_player1", spamBotPlayer1);

        ImGui::SameLine();
        if (ImGuiH::Checkbox("Player 2", &spamBotPlayer2, gui.m_scale))
            config.set<bool>("spambot_player2", spamBotPlayer2);

        #elif defined(GEODE_IS_ANDROID) 
        auto popup = popupSystem::create();
        popup->AddText("Hold Length:");
        popup->AddIntInput("Hold Length", spamBotReleaseLength, [this, &config](int value) 
        {
            config.set<int>("hold_length", value);
        },
        30.f);

        popup->AddText("Release Length:");
        popup->AddIntInput("Release Length", spamBotHoldLength, [this, &config](int value) 
        {
            config.set<int>("release_length", value);
        },
        30.f);

        popup->AddToggle("Player 1", spamBotPlayer1, [this, &config](bool enabled) 
        {
            config.set<bool>("spambot_player1", enabled);
        });

        popup->AddToggle("Player 2", spamBotPlayer2, [this, &config](bool enabled) 
        {
            config.set<bool>("spambot_player2", enabled);
        });

        popup->show();
        #endif
    });

    SetCustomWindowHandlerByConfig("wave_trail_size", [this, &config]() {
        float value = config.get<float>("wave_trail_size_value", 1.f);
        #ifdef GEODE_IS_WINDOWS 

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::DragFloat("##waveTrailSize", &value, 0.01f, 0.0f, FLT_MAX, "Wave Trail Size: %.2f")) 
            config.set("wave_trail_size_value", value);
        #elif defined(GEODE_IS_ANDROID) 

        auto popup = popupSystem::create();
        popup->AddText("Wave Trail Size:");
        popup->AddFloatInput("Wave Trail Size", value, [this, &config](float value) 
        {
            config.set("wave_trail_size_value", value);
        },
        35.f);
        popup->show();
        #endif
    });

    SetCustomWindowHandlerByConfig("respawn_time", [this, &config]() {
        float value = config.get<float>("respawn_time_value", 1.f);

        #ifdef GEODE_IS_WINDOWS 
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::DragFloat("##respawn_time_value", &value, 0.01f, 0.0f, FLT_MAX, "Respawn Time: %.2f")) 
            config.set("respawn_time_value", value);
        #elif defined(GEODE_IS_ANDROID) 

        auto popup = popupSystem::create();
        popup->AddText("Respawn Time:");
        popup->AddFloatInput("Respawn Time", value, [this, &config](float value) 
        {
            config.set("respawn_time_value", value);
        },
        35.f);
        popup->show();
        #endif
    });

    SetCustomWindowHandlerByConfig("pulse_size", [this, &config]() {
        float value = config.get<float>("pulse_size_value", 0.5f);
        bool multiply = config.get<bool>("pulse_multiply", false);

        #ifdef GEODE_IS_WINDOWS 
        auto &gui = Gui::get();

        if (ImGuiH::Checkbox("Multiply pulsation", &multiply, gui.m_scale)) {
            config.set<bool>("pulse_multiply", multiply);
        }

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::DragFloat("##pulse_size_value", &value, 0.01f, 0.0f, FLT_MAX, "Pulse Size: %.2f")) 
            config.set("pulse_size_value", value);
        #elif defined(GEODE_IS_ANDROID) 

        auto popup = popupSystem::create();
        popup->AddToggle("Multiply pulsation", multiply, [this, &config](bool enabled) 
        {
            config.set<bool>("pulse_multiply", enabled);
        });

        popup->AddText("Pulse Size:");

        popup->AddFloatInput("Pulse Size", value, [this, &config](float value) 
        {
            config.set("pulse_size_value", value);
        },
        35.f);
        popup->show();
        #endif
    });

    SetCustomWindowHandlerByConfig("startpos_switcher", [this, &config]() {
        bool reset_camera = config.get<bool>("startos_switcher::reset_camera", true);
        bool sort_objects_by_x = config.get<bool>("startos_switcher::sort_objects_by_x", true);

        #ifdef GEODE_IS_WINDOWS 
        auto &gui = Gui::get();       

        if (ImGuiH::Checkbox("Reset Camera", &reset_camera, gui.m_scale))
            config.set<bool>("startos_switcher::reset_camera", reset_camera);

        if (ImGuiH::Checkbox("Sort objects by X", &sort_objects_by_x, gui.m_scale))
            config.set<bool>("startos_switcher::sort_objects_by_x", sort_objects_by_x);
        #elif defined(GEODE_IS_ANDROID) 

        auto popup = popupSystem::create();
        popup->AddToggle("Reset Camera", reset_camera, [this, &config](bool enabled) 
        {
            config.set<bool>("startos_switcher::reset_camera", enabled);
        });

        popup->AddToggle("Sort objects by X", sort_objects_by_x, [this, &config](bool enabled) 
        {
            config.set<bool>("startos_switcher::sort_objects_by_x", enabled);
        });
        popup->show();
        #endif
    });

    SetCustomWindowHandlerByConfig("show_hitboxes", [this, &config]() {
        bool draw_trail = config.get<bool>("show_hitboxes::draw_trail", false);
        bool show_hitboxes_on_death = config.get<bool>("show_hitboxes::on_death", false);

        int trail_length = config.get<int>("show_hitboxes::trail_length", 240);
        float size = config.get<float>("show_hitboxes::size", 0.25f);

        #ifdef GEODE_IS_WINDOWS 
        auto &gui = Gui::get();

        if (ImGuiH::Checkbox("Show Hitboxes on Death", &show_hitboxes_on_death, gui.m_scale))
            config.set<bool>("show_hitboxes::on_death", show_hitboxes_on_death);

        if (ImGuiH::Checkbox("Draw Trail", &draw_trail, gui.m_scale))
            config.set<bool>("show_hitboxes::draw_trail", draw_trail);

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::DragInt("##show_hitboxes::trail_length", &trail_length, 1, 0, INT_MAX, "Trail Length: %i"))
            config.set<int>("show_hitboxes::trail_length", trail_length);

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::DragFloat("##show_hitboxes::size", &size, 0.01f, 0.f, FLT_MAX, "Size: %0.2f"))
            config.set<float>("show_hitboxes::size", size);

        ImGui::Text("Tip: Enable hitboxes in the editor by checking the \"Show\nHitboxes\" option in the Editor Pause menu");
        #elif defined(GEODE_IS_ANDROID) 

        auto popup = popupSystem::create();

        popup->AddToggle("Show Hitboxes on Death", show_hitboxes_on_death, [this, &config](bool enabled) 
        {
            config.set<bool>("show_hitboxes::on_death", enabled);
        });

        popup->AddToggle("Draw Trail", draw_trail, [this, &config](bool enabled) 
        {
            config.set<bool>("show_hitboxes::draw_trail", enabled);
        }, 20.f);

        popup->AddText("Trail Length:", 0.35f, 20.f);

        popup->AddIntInput("Trail Length", trail_length, [this, &config](int value) 
        {
            config.set("show_hitboxes::trail_length", value);
        }, 25.f);

        popup->AddText("Hitbox Size:", 0.35f, 20.f);

        popup->AddFloatInput("Hitbox Size", size, [this, &config](float value) 
        {
            config.set("show_hitboxes::size", value);
        });
        popup->show();
        #endif
    });

    // SetCustomWindowHandlerByConfig("random_seed", [this, &config]() {
    //     auto &gui = Gui::get();

    //     int random_seed_value = config.get<int>("random_seed_value", 1337);
    //     ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    //     if (ImGui::DragInt("##random_seed_value", &random_seed_value, 1, 1, INT_MAX, "Seed: %i"))
    //         config.set<int>("random_seed_value", random_seed_value);
    // });

    SetCustomWindowHandlerByConfig("rgb_icons", [this, &config]() {
        auto &colors = RGBIcons::get();

        float size = config.get<float>("rgb_icons::speed", 0.20f);

        bool player_p1 = config.get<bool>("rgb_icons::player_p1", true);
        bool player_p2 = config.get<bool>("rgb_icons::player_p2", true);
        bool wave_trail_p1 = config.get<bool>("rgb_icons::wave_trail_p1", true);
        bool wave_trail_p2 = config.get<bool>("rgb_icons::wave_trail_p2", true);
        
        #ifdef GEODE_IS_WINDOWS 
        auto &gui = Gui::get();

        if (ImGuiH::Checkbox("Player 1", &player_p1, gui.m_scale))
            config.set<bool>("rgb_icons::player_p1", player_p1);

        ImGui::SameLine();

        if (ImGuiH::Checkbox("Player 2", &player_p2, gui.m_scale))
            config.set<bool>("rgb_icons::player_p2", player_p2);

        if (ImGuiH::Checkbox("Wave Trail P1", &wave_trail_p1, gui.m_scale))
            config.set<bool>("rgb_icons::wave_trail_p1", wave_trail_p1);

        ImGui::SameLine();

        if (ImGuiH::Checkbox("Wave Trail P2", &wave_trail_p2, gui.m_scale))
            config.set<bool>("rgb_icons::wave_trail_p2", wave_trail_p2);

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::DragFloat("##rgb_icons::speed", &size, 0.01f, 0.f, FLT_MAX, "Speed: %0.2f"))
            config.set<float>("rgb_icons::speed", size);

        ImGui::Separator();

        if (ImGuiH::Button("Add Color", {ImGui::GetContentRegionAvail().x / 3, NULL})) {
            colors.colors.push_back({0, 0, 0});
        }
        
        ImGui::SameLine();
        if (ImGuiH::Button("Rainbow Colors", {ImGui::GetContentRegionAvail().x / 2, NULL})) {
            for (const auto& color : colors.rainbowColors) {
                colors.colors.push_back(color);
            }
        }

        ImGui::SameLine();
        if (ImGuiH::Button("Clear", {ImGui::GetContentRegionAvail().x, NULL})) {
            colors.colors.clear();
        }

        if (colors.colors.empty())
            ImGui::TextDisabled("No colors is here");

        for (int i = 0; i < colors.colors.size(); ++i) {
            ImGui::PushID(i);

            ImGui::Selectable("  =", false, ImGuiSelectableFlags_DontClosePopups, {20.f * gui.m_scale, 20.f * gui.m_scale});
            ImGui::SameLine();

            if (ImGui::IsItemHovered() && ImGui::GetMouseClickedCount(ImGuiMouseButton_Left) == 2) {
                colors.colors.erase(colors.colors.begin() + i);
            }

            if (ImGui::BeginDragDropSource()) {
                ImGui::TextUnformatted(fmt::format("Color {}", i).c_str());
                ImGui::SetDragDropPayload("LBLMOVE", &i, sizeof(size_t));
                ImGui::EndDragDropSource();
            }

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("LBLMOVE")) {
                    if (payload->DataSize == sizeof(size_t)) {
                        size_t move_from = *(size_t*)payload->Data;
                        if (i != move_from) {
                            std::iter_swap(colors.colors.begin() + move_from, colors.colors.begin() + i);
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }

            float color[3] = { colors.colors[i].r / 255.0f, colors.colors[i].g / 255.0f, colors.colors[i].b / 255.0f };
            if (ImGui::ColorEdit3(fmt::format("Color {}", i).c_str(), color)) {
                colors.colors[i] = cocos2d::ccColor3B(
                    static_cast<GLubyte>(color[0] * 255),
                    static_cast<GLubyte>(color[1] * 255),
                    static_cast<GLubyte>(color[2] * 255)
                );
            }

            ImGui::PopID();
        }
        #elif defined(GEODE_IS_ANDROID) 
        auto popup = popupSystem::create();
        popup->AddToggle("Player 1", player_p1, [this, &config](bool enabled) 
        {
            config.set<bool>("rgb_icons::player_p1", enabled);
        });

        popup->AddToggle("Player 2", player_p2, [this, &config](bool enabled) 
        {
            config.set<bool>("rgb_icons::player_p2", enabled);
        });

        popup->AddToggle("Player 2", wave_trail_p1, [this, &config](bool enabled) 
        {
            config.set<bool>("rgb_icons::wave_trail_p1", enabled);
        });

        popup->AddToggle("Wave Trail P1", wave_trail_p1, [this, &config](bool enabled) 
        {
            config.set<bool>("rgb_icons::wave_trail_p1", enabled);
        });

        popup->AddToggle("Wave Trail P2", wave_trail_p2, [this, &config](bool enabled) 
        {
            config.set<bool>("rgb_icons::wave_trail_p2", enabled);
        }, 20.f);

        popup->AddText("Speed:", 0.30f, 20.f);

        popup->AddFloatInput("Speed", size, [this, &config](float value) 
        {
           config.set<float>("rgb_icons::speed", value);
        },
        35.f);
        popup->show();
        #endif
    });

    Recorder::get().folderShowcasesPath = Config::get().get<std::filesystem::path>("showcases_path", folderPath / "Showcases");
    ReplayEngine::get().engine_v2 = Config::get().get<bool>("engine::v2", false);

    Labels::get().load();
    RGBIcons::get().load();

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

void Hacks::saveKeybinds() {
    #ifdef GEODE_IS_WINDOWS
    auto& gui = Gui::get();
    nlohmann::json keybindJson;
    
    for (const auto& win : m_windows) {
        for (auto& hck : win.hacks) {
            if (hck.keybind != 0) {
                keybindJson[hck.name] = hck.keybind;
            }
        }
    }

    keybindJson["gui::toggle"] = gui.m_toggleKey;

    keybindJson["speedhackKey"] = gui.m_speedhackKey;
    keybindJson["tpsKey"] = gui.m_tpsKey;

    keybindJson["playbackKey"] = gui.m_playbackKey;
    keybindJson["saveMacroByCurrentNameKey"] = gui.m_saveMacroByCurrentNameKey;
    keybindJson["loadMacroByCurrentNameKey"] = gui.m_loadMacroByCurrentNameKey;
    keybindJson["frameAdvanceEnableKey"] = gui.m_frameAdvanceEnableKey;
    keybindJson["frameAdvanceDisableKey"] = gui.m_frameAdvanceDisableKey;

    keybindJson["optionsKey"] = gui.m_optionsKey;
    keybindJson["resetLevelKey"] = gui.m_resetLevelKey;
    keybindJson["practiceModeKey"] = gui.m_practiceModeKey;
    keybindJson["resetVolumeKey"] = gui.m_resetVolumeKey;
    keybindJson["uncompleteLevelKey"] = gui.m_uncompleteLevelKey;

    keybindJson["startposSwitcherLeftKey"] = gui.m_startposSwitcherLeftKey;
    keybindJson["startposSwitcherRightKey"] = gui.m_startposSwitcherRightKey;

    keybindJson["autoDeafenKey"] = gui.m_autoDeafenKey;

    if (!keybindJson.empty()) {
        std::ofstream file(folderPath / "keybinds.json");
        if (file.is_open()) {
            file << keybindJson.dump(4);
        }
    }
    #endif
}

void Hacks::loadKeybinds() {
    #ifdef GEODE_IS_WINDOWS
    auto& gui = Gui::get();

    std::ifstream file(folderPath / "keybinds.json");
    if (!file.is_open()) {
        return;
    }

    std::string file_contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    nlohmann::json keybindJson = nlohmann::json::parse(file_contents, nullptr, false);
    if (keybindJson.is_discarded()) {
        return;
    }

    std::unordered_map<std::string, int> keybindMap;
    for (const auto& [name, key] : keybindJson.items()) {
        keybindMap[name] = key;
    }
    

    gui.m_toggleKey = keybindJson.value("gui::toggle", GLFW_KEY_TAB);

    gui.m_speedhackKey = keybindJson.value("speedhackKey", 0);
    gui.m_tpsKey = keybindJson.value("tpsKey", 0);

    gui.m_playbackKey = keybindJson.value("playbackKey", 0);
    gui.m_saveMacroByCurrentNameKey = keybindJson.value("saveMacroByCurrentNameKey", 0);
    gui.m_loadMacroByCurrentNameKey = keybindJson.value("loadMacroByCurrentNameKey", 0);
    gui.m_frameAdvanceEnableKey = keybindJson.value("frameAdvanceEnableKey", GLFW_KEY_C);
    gui.m_frameAdvanceDisableKey = keybindJson.value("frameAdvanceDisableKey", GLFW_KEY_V);

    gui.m_optionsKey = keybindJson.value("optionsKey", 0);
    gui.m_resetLevelKey = keybindJson.value("resetLevelKey", 0);
    gui.m_practiceModeKey = keybindJson.value("practiceModeKey", 0);
    gui.m_resetVolumeKey = keybindJson.value("resetVolumeKey", 0);
    gui.m_uncompleteLevelKey = keybindJson.value("uncompleteLevelKey", 0);

    gui.m_startposSwitcherLeftKey = keybindJson.value("startposSwitcherLeftKey", GLFW_KEY_Q);
    gui.m_startposSwitcherRightKey = keybindJson.value("startposSwitcherRightKey", GLFW_KEY_E);

    gui.m_autoDeafenKey = keybindJson.value("autoDeafenKey", 0);

    for (auto& win : m_windows) {
        for (auto& hck : win.hacks) {
            auto it = keybindMap.find(hck.name);
            if (it != keybindMap.end()) {
                hck.keybind = it->second;
            }
        }
    }
    #endif
}

cheat_state Hacks::cheatingCheck() {
    auto& config = Config::get();
    auto& engine = ReplayEngine::get();

    if (config.get<bool>("safe_mode", false)) 
        return cheat_state::safe_mode;

    if (config.get<bool>("noclip", false) ||
        config.get<bool>("auto_pickup_coins", false) ||
        config.get<bool>("instant_complete", false) ||
        config.get<bool>("jump_hack", false) ||
        config.get<bool>("layout_mode", false) ||
        config.get<bool>("spambot_enabled", false) ||
        (config.get<bool>("show_hitboxes", false) && !config.get<bool>("show_hitboxes::on_death", false)) ||
        config.get<bool>("straight_fly_bot", false) ||
        config.get<bool>("no_camera_move", false) ||
        config.get<bool>("no_camera_zoom", false) ||
        config.get<bool>("no_shaders", false) ||
        config.get<bool>("no_mirror_portal", false) ||
        config.get<bool>("tps_enabled", false) ||
        (config.get<float>("speedhack_value", 1.f) != 1.f) ||
        (engine.mode == state::record || engine.mode == state::play)) 
    {
        return cheat_state::cheating;
    }
    else if (config.get<bool>("no_particles", false) ||
            (config.get<bool>("wave_trail_size", false) && config.get<float>("wave_trail_size_value", 1.f) < 0.5f))   
    {
        return cheat_state::unwanted;
    }

    return cheat_state::legit;
}

void Hacks::toggleKeybinds(int key) {
    #ifdef GEODE_IS_WINDOWS
    auto& config = Config::get();
    auto& gui = Gui::get();

    if (gui.m_speedhackKey == key)
        config.set<bool>("speedhack_enabled", !config.get<bool>("speedhack_enabled", false));

    if (gui.m_tpsKey == key)
        config.set<bool>("tps_enabled", !config.get<bool>("tps_enabled", false));

    
    if (gui.m_playbackKey == key) {
        auto& engine = ReplayEngine::get();
        engine.mode = (engine.mode == state::play) ? state::disable : state::play;
    }
    
    
    if (gui.m_saveMacroByCurrentNameKey == key) {
        auto pl = PlayLayer::get();
        auto& engine = ReplayEngine::get();
        if (!pl) {
            ImGuiH::Popup::get().add_popup("Can't get a level name to save a macro");
        }
        else {
            std::string level_name = pl->m_level->m_levelName;
            if (engine.engine_v2)
                ImGuiH::Popup::get().add_popup(engine.save2(level_name));
            else 
                ImGuiH::Popup::get().add_popup(engine.save(level_name));
        }
    }

    
    if (gui.m_loadMacroByCurrentNameKey == key) {
        auto pl = PlayLayer::get();
        auto& engine = ReplayEngine::get();
        if (!pl) {
            ImGuiH::Popup::get().add_popup("Can't get a level name to load a macro");
        }
        else {
            std::string level_name = pl->m_level->m_levelName;
            if (engine.engine_v2)
                ImGuiH::Popup::get().add_popup(engine.load2(level_name));
            else 
                ImGuiH::Popup::get().add_popup(engine.load(level_name));
        }
    }
    
    if (gui.m_frameAdvanceEnableKey == key) {
        auto& engine = ReplayEngine::get();
        if (engine.mode == state::record || engine.mode == state::play) {
            if (!engine.frame_advance)
                ImGuiH::Popup::get().add_popup("Frame Advance enabled");

            engine.frame_advance = true;
            engine.next_frame = true;
        }
    }

    if (gui.m_frameAdvanceDisableKey == key) {
        auto& engine = ReplayEngine::get();
        if (engine.frame_advance)
            ImGuiH::Popup::get().add_popup("Frame Advance disabled");

        engine.frame_advance = false;
    }

    if (gui.m_optionsKey == key) {
        auto options_layer = OptionsLayer::create();
        auto scene = cocos2d::CCScene::get();

        if (options_layer && scene) {
            auto zOrder = scene->getHighestChildZ();
            scene->addChild(options_layer, zOrder + 1);
            options_layer->showLayer(false);
        }
    }

    if (gui.m_resetLevelKey == key) {
        auto pl = PlayLayer::get();
        if (pl) pl->resetLevel();
    }

    if (gui.m_practiceModeKey == key) {
        auto pl = PlayLayer::get();
        if (pl) pl->togglePracticeMode(!pl->m_isPracticeMode);
    }

    if (gui.m_resetVolumeKey == key) {
        auto fmod_engine = FMODAudioEngine::sharedEngine();
        fmod_engine->setBackgroundMusicVolume(0.5f);
        fmod_engine->setEffectsVolume(0.5f);
    }

    if (gui.m_uncompleteLevelKey == key) {
        utilsH::UncompleteLevel();
    }

    if (gui.m_startposSwitcherLeftKey == key && config.get<bool>("startpos_switcher", false)) {
        hooksH::switchStartPos(-1);
    }

    if (gui.m_startposSwitcherRightKey == key && config.get<bool>("startpos_switcher", false)) {
        hooksH::switchStartPos(1);
    }

    if (config.get<bool>("use_keybinds_only_in_game", true) && !PlayLayer::get())
        return;

    for (auto& win : m_windows) {
        for (auto& hck : win.hacks) {
            if (hck.keybind == key && key != 0) {   
                bool value = !config.get(hck.config, false);
                config.set(hck.config, value);
                if (!hck.game_var.empty())
                    GameManager::get()->setGameVariable(hck.game_var.c_str(), value);
                if (hck.handlerFunc) hck.handlerFunc(value);
            }
        }
    }
    #endif
}
