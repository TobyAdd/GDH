#pragma once
#include "includes.hpp"
#include "gd.hpp"
#include "bindings/GJGameLevel.h"

namespace hooks {
    extern int transitionSelect;

    extern unsigned frame;

    inline bool(__thiscall* PlayLayer_init)(void*, void*, bool, bool);
    inline bool __fastcall PlayLayer_init_H(int* self, int edx, robtop::GJGameLevel* GJGameLevel, bool a3, bool a4);

    extern bool modify_icon_color;
    extern float iconcolor1[];
    extern float iconcolor2[];

    inline void(__thiscall *playLayer_update)(gd::PlayLayer *self, float deltatime);
    inline void __fastcall playLayer_update_H(gd::PlayLayer *self, int edx, float deltatime);

    inline void(__thiscall* PlayLayer_resetLevel)(gd::PlayLayer*);
    inline void __fastcall PlayLayer_resetLevel_H(gd::PlayLayer* self);

    inline void(__thiscall* PlayLayer_destructor)(void*);
    inline void __fastcall PlayLayer_destructor_H(void* self);

    inline void (__thiscall* playLayer_levelComplate)(int *self);
    void __fastcall playLayer_levelComplate_H(int *self);

    inline void(__thiscall* playLayer_death)(int self, void* player, void* obj);
    void __fastcall playLayer_death_H(int self, int edx, void* player, void* obj);

    inline int (__thiscall* GJBaseGameLayer_HandleButton)(void*, int push, int player_button, BOOL is_player1);
    inline int __fastcall GJBaseGameLayer_HandleButton_H(void *self, uintptr_t, int push, int player_button, BOOL is_player1);
    
    void init();
}

namespace startPosSwitcher
{
    extern bool enabled;
    extern int use_a_d;

    extern int startposIndex;

    void switchStartPos(gd::PlayLayer *pl, bool direction);
    void handleKeyPress(int keyCode);
}

namespace layout_mode
{
    void init();
    void set_enabled(bool enable);
}