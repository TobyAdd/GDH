#include "include.hpp"
#include "replayEngine.hpp"
#include "noclipAccuracy.hpp"
#include "cps_counter.hpp"

namespace hooks {
    extern bool musicUnlocker;
    extern int transitionSelect;
    extern bool g_enabledEKG;
    extern bool g_enabledAutoPickupCoins;
    extern bool g_enabledNoEffectCircle;
    extern unsigned frame;
    extern bool confirm_exit;
    extern bool frame_advance;

    extern bool noclip_accuracy_enabled;  
    extern NoclipAccuracy noclip_accuracy;

    extern CPSCounter cps_counter;

    inline auto base = reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr));

    inline bool(__thiscall* PlayLayer_init)(void*, void*, bool, bool);
    inline bool __fastcall PlayLayer_init_H(void* self, int edx, void* GJGameLevel, bool a3, bool a4);

    inline void(__thiscall *playLayer_update)(void *self, float deltatime);
    inline void __fastcall playLayer_update_H(void *self, int edx, float deltatime);

    inline void(__thiscall* PlayLayer_resetLevel)(void*);
    inline void __fastcall PlayLayer_resetLevel_H(void* self);

    inline void (__thiscall* playLayer_levelComplate)(int *self);
    void __fastcall playLayer_levelComplate_H(int *self);

    inline void(__thiscall* PlayLayer_destructor)(void*);
    inline void __fastcall PlayLayer_destructor_H(void* self);

    inline int (__thiscall* GJBaseGameLayer_HandleButton)(void*, int push, int player_button, BOOL is_player1);
    inline int __fastcall GJBaseGameLayer_HandleButton_H(void *self, uintptr_t, int push, int player_button, BOOL is_player1);

    inline void(__thiscall* togglePractice)(void* self, bool practice);
    void __fastcall togglePractice_H(void* self, int edx, bool practice);

    inline int (__thiscall* playLayer_exit)(void *self);
    int __fastcall playLayer_exit_H(void *self);

    inline void(__thiscall* playLayer_death)(void* self, void* player, void* obj);
    void __fastcall playLayer_death_H(void* self, int edx, void* player, void* obj);

    void init();
}
