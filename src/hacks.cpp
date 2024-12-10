#include "hacks.hpp"
#include "config.hpp"
#include "gui.hpp"
#include "labels.hpp"
#include "recorder.hpp"
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
                {"Slider Limit", "Removes the limit on sliding beyond set limits", "slider_limit"}, // + 
                {"Character Filter", "Lets you input any character in all text inputs", "char_filter"},  // +   
                {"Checkpoint Limit", "Removes the limit that deletes previous checkpoints after the 50th checkpoint", "checkpoint_limit"}, // +   
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
                // {"Force Dont Enter", "Disables effects when objects enter the viewable play area", "force_dont_enter"},
                // {"Force Dont Fade", "Disables effects when objects leave the viewable play area", "force_dont_fade"},
                {"Random Seed", "Changes the seed game so that the random trigger is not triggered randomly", "random_seed"},
                {"Respawn Time", "Changes respawn time on death", "respawn_time"},  // +
                {"Ignore ESC", "Prevents exiting the level", "ignore_esc"},  // +
                {"Instant Complete", "Instant level completion", "instant_complete"},  // +
                {"Jump Hack", "Removes the barrier to jump gravity", "jump_hack"},  // +
                {"Show Percentage", "Show percentages in level progress", "show_percentage", "0040"},  // +
                {"Smart Startpos", "Restores correct gameplay without startpos settings", "smart_startpos"},
                {"Startpos Switcher", "The ability to switch between starting positions using the keys that you setted in keybinds", "startpos_switcher"}, // +
                {"RGB Icons", "LGBT icons, yes :3", "rgb_icons"}, // +
                {"Solid Wave Trail", "Disables wave blending", "solid_wave_trail"}, // +
                {"Show Triggers", "Displaying triggers on the PlayLayer", "show_triggers"},
                {"Show Hitboxes", "Visualizes hitbox levels", "show_hitboxes"}, // +
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
                {"No Wave Trail Fade", "", "wave_trail_on_death"}   // +
            }
        },
        {"Creator", 450, 10, 220, 280, 
            {
                {"Copy Hack", "Copy any online level without a password", "copy_hack"},  // +
                {"Custom Object Bypass", "Removes the limit restricted to 1000 objects", "custom_object_bypass"}, // +
                {"Default Song Bypass", "Removes restrictions on secret official songs", "default_song_bypass"}, // +
                {"Editor Extension", "Increases the editor length by a factor of 128", "editor_extension"}, // +
                {"Verify Hack", "Publish a level without verification", "verify_hack"}, // +
                {"Smooth Editor Trail", "Makes the wave smoother in the editor", "smooth_editor_trail"}, // +
                {"Level Edit", "Edit any online level", "level_edit"}, // +
                {"No (C) Mark", "Removes copyright on copied levels", "no_c_mark"}, // +
                {"Zoom Bypass", "", "zoom_bypass"} // +
            }
        },
        {"Framerate", 450, 300, 220, 130},
        {"GDH Settings", 450, 440, 220, 150},
        {"Replay Engine", 680, 10, 300, 200},
        {"Labels", 680, 220, 300, 320},
        {"Variables", 10, 510, 200, 160},
        {"Shortcuts", 990, 10, 200, 240}
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

        if (ImGuiH::Checkbox("Reset Camera", &reset_camera, gui.m_scale))
            config.set<bool>("startos_switcher::reset_camera", reset_camera);
    });

    SetHandlerByConfig("show_hitboxes", [this](bool enabled) {
        auto pl = PlayLayer::get();
        if (pl && !enabled && !(pl->m_isPracticeMode && GameManager::get()->getGameVariable("0166"))) {
            pl->m_debugDrawNode->setVisible(false);
        }
    });

    SetCustomWindowHandlerByConfig("show_hitboxes", [this, &config]() {
        auto &gui = Gui::get();

        bool draw_trail = config.get<bool>("show_hitboxes::draw_trail", false);
        bool show_hitboxes_on_death = config.get<bool>("show_hitboxes::on_death", false);

        bool fill_color = config.get<bool>("show_hitboxes::fill_color", false);
        float fill_color_alpha = config.get<float>("show_hitboxes::fill_color_alpha", 0.2f);

        int trail_length = config.get<int>("show_hitboxes::trail_length", 240);
        float size = config.get<float>("show_hitboxes::size", 0.25f);

        if (ImGuiH::Checkbox("Fill Color", &fill_color, gui.m_scale))
            config.set<bool>("show_hitboxes::fill_color", fill_color);

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::DragFloat("##show_hitboxes::fill_color_alpha", &fill_color_alpha, 0.01f, 0.f, 1, "Fill Color Alpha: %0.2f"))
            config.set<float>("show_hitboxes::fill_color_alpha", fill_color_alpha);

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
    });

    SetCustomWindowHandlerByConfig("rgb_icons", [this, &config]() {
        auto &gui = Gui::get();
        auto &colors = RGBIcons::get();

        float size = config.get<float>("rgb_icons::speed", 0.25f);

        bool player_p1 = config.get<bool>("rgb_icons::player_p1", true);
        bool player_p2 = config.get<bool>("rgb_icons::player_p2", true);
        bool wave_trail_p1 = config.get<bool>("rgb_icons::wave_trail_p1", true);
        bool wave_trail_p2 = config.get<bool>("rgb_icons::wave_trail_p2", true);

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
            std::vector<cocos2d::ccColor3B> rainbowColors = {
                cocos2d::ccColor3B(255, 0, 0),
                cocos2d::ccColor3B(255, 127, 0),
                cocos2d::ccColor3B(255, 255, 0),
                cocos2d::ccColor3B(0, 255, 0),
                cocos2d::ccColor3B(0, 191, 255),
                cocos2d::ccColor3B(0, 0, 255),
                cocos2d::ccColor3B(139, 0, 255)
            };

            for (const auto& color : rainbowColors) {
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

            ImGui::Selectable("  =", false, ImGuiSelectableFlags_DontClosePopups, {20.f, 20.f});
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
    });

    Recorder::get().folderShowcasesPath = Config::get().get<std::filesystem::path>("showcases_path", folderPath / "Showcases");

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