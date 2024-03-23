#pragma once

namespace hacks {
    extern float speed;

    extern bool tps_enabled;
    extern float tps;

    extern bool discord_rpc;
    
    extern bool auto_song_downloader;
    extern bool auto_ldm;
    extern bool auto_practice_mode;

    extern bool smart_startpos;
    extern bool rgb_icons;
    extern bool show_hitboxes;
    extern bool show_on_death;
    extern bool draw_trail;
    extern bool draw_only_trail;
    extern bool default_hitbox_size;

    extern int border_alpha;
    extern int fill_alpha;
    extern int circle_border_alpha;
    extern int circle_fill_alpha;

    extern float iconcolor1[];
    extern float iconcolor2[];
    extern float hitbox_size;
    extern float circle_hitbox_size;

    extern bool auto_pickup_coins;

    extern bool instant_complate;

    extern bool disable_render;

    extern bool transition_enabled;
    extern float transition_duration;
    extern int transition_index;

    extern bool hide_player_enabled;
    extern bool hide_player_p1;
    extern bool hide_player_p2;

    extern bool speedhack_audio;

    extern bool hide_menu;

    extern float respawnTime;

    void update_framerate();
    void init_custom_respawn_time();
}
