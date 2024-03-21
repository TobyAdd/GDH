#include "hacks.hpp"
#include "memory.hpp"
#include "gd.hpp"

bool hacks::discord_rpc = false;

bool hacks::auto_song_downloader = false;
bool hacks::auto_ldm = false;
bool hacks::auto_practice_mode = false;

bool hacks::smart_startpos = false;
bool hacks::rgb_icons = false;
bool hacks::show_hitboxes = false;
bool hacks::show_on_death = false;
bool hacks::draw_trail = false;
bool hacks::draw_only_trail = false;
bool hacks::default_hitbox_size = false;

int hacks::border_alpha = 0;
int hacks::fill_alpha = 0;
int hacks::circle_border_alpha = 0;
int hacks::circle_fill_alpha = 0;

float hacks::iconcolor1[] = {1.0f, 1.0f, 1.0f};
float hacks::iconcolor2[] = {1.0f, 1.0f, 1.0f};
float hacks::hitbox_size = 1.0f;
float hacks::circle_hitbox_size = 1.0f;

float hacks::speed = 1.0f;

bool hacks::tps_enabled = false;
float hacks::tps = 240.0f;

bool hacks::auto_pickup_coins = false;
bool hacks::instant_complate = false;

bool hacks::disable_render = false;

bool hacks::transition_enabled = false;
float hacks::transition_duration = 1.f;
int hacks::transition_index = 0;

bool hacks::hide_player_enabled = false;
bool hacks::hide_player_p1 = false;
bool hacks::hide_player_p2 = false;

bool hacks::hide_menu = false;

uintptr_t framerate_address = 0;
void hacks::update_framerate() {
    if (framerate_address == 0) {
        MODULEINFO moduleInfo;
        if (GetModuleInformation(GetCurrentProcess(), (HMODULE)gd::base, &moduleInfo, sizeof(MODULEINFO))) {
            framerate_address = memory::PatternScan(reinterpret_cast<uintptr_t>(moduleInfo.lpBaseOfDll), moduleInfo.SizeOfImage, "89 88 ? ? ? ? a3");
        }  
    }
    if (framerate_address != 0) {
        memory::WriteFloat(framerate_address, 1.f / hacks::tps);
    }        
}

float hacks::respawnTime = 1.0f;
void hacks::init_custom_respawn_time() {
    uintptr_t respawntime_address;
    MODULEINFO moduleInfo;
    if (GetModuleInformation(GetCurrentProcess(), (HMODULE)gd::base, &moduleInfo, sizeof(MODULEINFO))) {
        respawntime_address = memory::PatternScan(reinterpret_cast<uintptr_t>(moduleInfo.lpBaseOfDll), moduleInfo.SizeOfImage, "F3 0F 10 05 ? ? ? ? 68 ? ? ? ? C6");
    }  
    respawntime_address += 4;

    auto respawnTimeAddr = (uintptr_t)&respawnTime;

    DWORD oldProtect;
    VirtualProtectEx(GetCurrentProcess(), LPVOID(respawntime_address), sizeof(respawnTimeAddr), PAGE_EXECUTE_READWRITE, &oldProtect);
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<LPVOID>(respawntime_address), &respawnTimeAddr, sizeof(respawnTimeAddr), NULL);
    VirtualProtectEx(GetCurrentProcess(), LPVOID(respawntime_address), sizeof(respawnTimeAddr), oldProtect, &oldProtect);
}