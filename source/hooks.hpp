#include "include.hpp"
#include "replayEngine.hpp"

namespace hooks {
    extern bool musicUnlocker;
    extern int transitionSelect;
    extern unsigned frame;
    extern unsigned checkpoint_frame;
    extern void* playLayer;

    inline auto base = reinterpret_cast<uintptr_t>(GetModuleHandle(0));

    inline void(__thiscall *CCScheduler_update)(void *, float);
    void __fastcall CCScheduler_update_H(void *self, int, float dt);

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

    inline void *(__cdecl *CCTransitionFade_create)(float, void *);
    void *__cdecl CCTransitionFade_create_H(float duration, void *scene);

    inline void(__thiscall* dispatchKeyboardMSG)(void* self, int key, bool down);
    void __fastcall dispatchKeyboardMSG_H(void *self, uintptr_t *, int key, bool down);

    inline void(__thiscall* playLayer_loadFromCheckpoint)(void*, void* checkpoint);
	void __fastcall playLayer_loadFromCheckpoint_H(void* self, void*, void* checkpoint);

    inline void*(__thiscall* checkpointObjectInit)(void*);
	void* __fastcall checkpointObjectInit_H(void* self, void*);

    inline void(__thiscall* playLayer_resetLevelFromStart)(void*);
    void __fastcall playLayer_resetLevelFromStart_H(void* self, void*);

    inline const char*(__thiscall* splashString)();
    const char* __fastcall splashString_H();

    // inline int (__thiscall* playLayer_destroyPlayer)(void*, void* player, void* obj);
    // int __fastcall playLayer_destroyPlayer_H(void* self, void*, void* player, void* obj);

    void init();
}
