#include "hooks.hpp"
#include "hacks.hpp"
#include "console.hpp"
#include "replayEngine.hpp"

std::string hooks::log;

std::vector<void *> startPositions;
void __fastcall hooks::PlayLayer_addObjectH(void *self, void *obj)
{
    PlayLayer_addObject(self, obj);
    switch (gd::gameobject::get_objid(obj)) {
    case 31:
        startPositions.push_back(obj);
        break;
    }
    
}

float left_over = 0.f;
void(__thiscall *CCScheduler_update)(void *, float);
void __fastcall CCScheduler_update_H(void *self, float dt)
{
    dt *= hacks::speed_value;

    float newdt = 1.f / hacks::fps_value; 

    // if (engine.frame_advance) {
    //     if (next_frame) {
    //         next_frame = false;
    //         return CCScheduler_update(self, hacks::tps_enabled ? newdt : dt);
    //     }

    //     return;
    // }

    if (!hacks::fps_enabled)    
        return CCScheduler_update(self, dt);

    if (!engine.real_time) {
        return CCScheduler_update(self, newdt);
    }
 
    unsigned times = static_cast<int>((dt + left_over) / newdt);  
    auto start = std::chrono::high_resolution_clock::now();
    using namespace std::literals;

    for (unsigned i = 0; i < times; ++i) {
        CCScheduler_update(self, newdt);
        if (std::chrono::high_resolution_clock::now() - start > 33.333ms) {            
            times = i + 1;
            break;
        }
    }
    left_over += dt - newdt * times; 
}

bool(__thiscall *GameStatsManager_isItemEnabled)(void *, int, int);
bool __fastcall GameStatsManager_isItemEnabledH(void *self, unsigned int a, unsigned int b)
{
    auto ret = GameStatsManager_isItemEnabled(self, a, b);

    if (hacks::unlock_items)
        return true;

    return ret;
}


void* pl = nullptr;
bool __fastcall hooks::PlayLayer_initH(void *self, void *m_level, bool a3, bool a4)
{
    auto ret = PlayLayer_init(self, m_level, a3, a4);
    if (ret)
    {
        pl = self;
    }
    return ret;
}

void(__thiscall* PlayLayer_update)(void*, float);
void __fastcall PlayLayer_update_H(void* self, float dt) {    
    PlayLayer_update(self, dt);

    if (engine.mode == state::play)
    {
        engine.handle_playing(self);
    }
    else if (engine.mode == state::record) {
        engine.handle_recording(self, true);
        engine.handle_recording(self, false);
    }


    // std::string eblyainti;
    // for (int i = 0; i < vector_array.size(); i++) {
    //     eblyainti += std::to_string(vector_array[i]) + " (" + std::to_string(from<bool>(self, vector_array[i])) + ")\n";
    // }
    // hooks::log = eblyainti;
}

void(__thiscall* PlayLayer_reset)(void*);
void __fastcall PlayLayer_reset_H(void* self) {    
    PlayLayer_reset(self);
    engine.handle_reseting(self);

    if (hacks::instant_complate) {
        float a = 0;
        reinterpret_cast<void(__thiscall *)(void* self, float *a2)>(hacks::base + 0x388570)(self, &a);
    }
        
}

void(__thiscall* PlayLayer_quit)(void*);
void __fastcall PlayLayer_quit_H(void* self) {    
    if (!hacks::ignore_esc)
        PlayLayer_quit(self);
}

void __fastcall hooks::PlayLayer_destructor_H(void *self)
{
    PlayLayer_destructor(self);

    if (engine.mode == record)
    {
        engine.mode = state::disable;
    }

    pl = nullptr;

    startPositions.clear();
}

int __fastcall hooks::GJBaseGameLayer_HandleButton_H(void *self, int hold, int player_button, bool is_player1)
{
    if (engine.mode == state::play && engine.ignore_inputs) {
        return 0;
    }

    auto ret = GJBaseGameLayer_HandleButton(self, hold, player_button, is_player1);
    if (engine.mode == state::record)
    {
        if (pl)
            engine.handle_recording2(hold, player_button, is_player1, self);
    }
    return ret;
}

// void *(__thiscall *dispatchKeyboardMSG)(void *self, int key, bool down, bool idk);
// void *__fastcall dispatchKeyboardMSGHook(void *self, int key, bool down, bool idk)
// {
//     return dispatchKeyboardMSG(self, key, down, idk);
// }

void hooks::init() {
    MH_CreateHook(GetProcAddress((HMODULE)hacks::cocos_base, "?update@CCScheduler@cocos2d@@UEAAXM@Z"), CCScheduler_update_H, (void **)&CCScheduler_update);
    MH_CreateHook((void *)(hacks::base + 0x1DABE0), GameStatsManager_isItemEnabledH, (void **)&GameStatsManager_isItemEnabled);
    MH_CreateHook((void *)(hacks::base + 0x382890), PlayLayer_initH, (void **)&PlayLayer_init);
    MH_CreateHook((void *)(hacks::base + 0x2277D0), PlayLayer_update_H, (void **)&PlayLayer_update);
    MH_CreateHook((void *)(hacks::base + 0x3958b0), PlayLayer_reset_H, (void **)&PlayLayer_reset);
    MH_CreateHook((void *)(hacks::base + 0x35cd70), PlayLayer_quit_H, (void **)&PlayLayer_quit);
    MH_CreateHook((void *)(hacks::base + 0x2238a0), GJBaseGameLayer_HandleButton_H, (void **)&GJBaseGameLayer_HandleButton);
    MH_CreateHook((void *)(hacks::base + 0x38a990), PlayLayer_addObjectH, (void **)&PlayLayer_addObject);//382540
    MH_CreateHook((void *)(hacks::base + 0x382540), PlayLayer_destructor_H, (void **)&PlayLayer_destructor);
    //MH_CreateHook(GetProcAddress(hacks::cocos_base, "?dispatchKeyboardMSG@CCKeyboardDispatcher@cocos2d@@QEAA_NW4enumKeyCodes@2@_N1@Z"), dispatchKeyboardMSGHook, (void **)&dispatchKeyboardMSG);
}