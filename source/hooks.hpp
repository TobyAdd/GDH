#pragma once
#include "includes.hpp"

namespace hooks {
    extern int transitionSelect;
    
    extern unsigned frame;

    inline bool(__thiscall* PlayLayer_init)(void*, void*, bool, bool);
    inline bool __fastcall PlayLayer_init_H(void* self, int edx, void* GJGameLevel, bool a3, bool a4);

    inline void(__thiscall *playLayer_update)(void *self, float deltatime);
    inline void __fastcall playLayer_update_H(void *self, int edx, float deltatime);

    inline void(__thiscall* PlayLayer_resetLevel)(void*);
    inline void __fastcall PlayLayer_resetLevel_H(void* self);

    inline void(__thiscall* PlayLayer_destructor)(void*);
    inline void __fastcall PlayLayer_destructor_H(void* self);

    inline void (__thiscall* playLayer_levelComplate)(int *self);
    void __fastcall playLayer_levelComplate_H(int *self);

    inline int (__thiscall* GJBaseGameLayer_HandleButton)(void*, int push, int player_button, BOOL is_player1);
    inline int __fastcall GJBaseGameLayer_HandleButton_H(void *self, uintptr_t, int push, int player_button, BOOL is_player1);

    void init();
}
