#include "includes.hpp"

namespace hooks {
    inline bool(__thiscall* PlayLayer_init)(gd::PlayLayer*, void*, bool, bool);
    bool __fastcall PlayLayer_initH(gd::PlayLayer* self, void* m_level, bool a3, bool a4);

    inline void(__thiscall* PlayLayer_addObject)(gd::PlayLayer*, gd::GameObject*);
    void __fastcall PlayLayer_addObjectH(gd::PlayLayer* self, gd::GameObject* obj);

    inline void(__thiscall* PlayLayer_destructor)(gd::PlayLayer*);
    void __fastcall PlayLayer_destructor_H(gd::PlayLayer* self);

    inline void(__thiscall *PlayLayer_levelComplete)(gd::PlayLayer*);
    void __fastcall PlayLayer_levelCompleteHook(gd::PlayLayer *self);

    inline int (__thiscall* GJBaseGameLayer_HandleButton)(gd::PlayLayer*, int hold, int player_button, bool is_player1);
    int __fastcall GJBaseGameLayer_HandleButton_H(gd::PlayLayer *self, int hold, int player_button, bool is_player1);

    inline void(__thiscall *PlayLayer_startMusic)(gd::PlayLayer *);
    void __fastcall PlayLayer_startMusicHook(gd::PlayLayer *self);

    inline void(__thiscall* PlayLayer_destroyPlayer)(gd::PlayLayer*, gd::PlayerObject*, gd::GameObject*);
    void __fastcall PlayLayer_destroyPlayer_H(gd::PlayLayer* self, gd::PlayerObject* player, gd::GameObject* obj);

    inline void(__thiscall* PauseLayer_customSetup)(gd::PauseLayer*);
    void __fastcall PauseLayer_customSetup_H(gd::PauseLayer* self);

    inline void(__thiscall* PauseLayer_destructor)(gd::PauseLayer*, bool remove);
    void __fastcall PauseLayer_destructor_H(gd::PauseLayer* self, bool remove);

    inline bool(__thiscall* ShaderLayer_init)(gd::ShaderLayer*);
    bool __fastcall ShaderLayer_initH(gd::ShaderLayer* self);

    inline void(__thiscall* ShaderLayer_destructor)(gd::ShaderLayer*, bool remove);
    void __fastcall ShaderLayer_destructor_H(gd::ShaderLayer* self, bool remove);

    extern gd::PlayLayer* pl;
    extern gd::PauseLayer* pause_menu;
    extern gd::ShaderLayer* shader_layer;

    void init();
}