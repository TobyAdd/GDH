#include "includes.hpp"
#include "gd.hpp"
#include "presence.hpp"

namespace hooks {
    inline bool(__thiscall* PlayLayer_init)(gd::PlayLayer*, gd::GJGameLevel*, bool, bool);
    bool __fastcall PlayLayer_initH(gd::PlayLayer *self, int edx, gd::GJGameLevel* m_level, bool a3, bool a4);

    inline void(__thiscall* PlayLayer_update)(gd::PlayLayer*, float);
    void __fastcall PlayLayer_update_H(gd::PlayLayer* self, int, float dt);

    inline void(__thiscall* PlayLayer_resetLevel)(gd::PlayLayer*);
    void __fastcall PlayLayer_resetLevel_H(gd::PlayLayer* self);

    inline void(__thiscall* PlayLayer_destructor)(void*);
    void __fastcall PlayLayer_destructor_H(void* self);

    inline void(__thiscall* PlayLayer_destroyPlayer)(gd::PlayLayer*, gd::PlayerObject*, gd::GameObject*);
    void __fastcall PlayLayer_destroyPlayer_H(gd::PlayLayer* self, int, gd::PlayerObject* player, gd::GameObject* obj);

    inline void(__thiscall *PlayLayer_levelComplete)(gd::PlayLayer*);
    void __fastcall PlayLayer_levelCompleteHook(gd::PlayLayer *self);

    inline void(__thiscall *PlayLayer_startMusic)(gd::PlayLayer *);
    void __fastcall PlayLayer_startMusicHook(gd::PlayLayer *self);

    inline void*(__stdcall* EndLevelLayer_create)();
    void* __fastcall EndLevelLayer_createHook();

    extern gd::PauseLayer* pause_menu;
    inline gd::PauseLayer*(__stdcall* PauseLayer_create)();
    gd::PauseLayer* __fastcall PauseLayer_createHook();

    inline void(__thiscall* PauseLayer_destructor)(gd::PauseLayer*, bool remove);
    void __fastcall PauseLayer_destructor_H(gd::PauseLayer* self, int, bool remove);
    
    inline int(__thiscall *PlayLayer_updateVisibility)(int, int a2);
    int __fastcall PlayLayer_updateVisibilityHook(int self, int, int a2);

    inline int (__thiscall* GJBaseGameLayer_HandleButton)(void*, int hold, int player_button, BOOL is_player1);
    int __fastcall GJBaseGameLayer_HandleButton_H(void *self, uintptr_t, int hold, int player_button, BOOL is_player1);

    inline void(__thiscall* PlayLayer_addObject)(gd::PlayLayer*, gd::GameObject*);
    void __fastcall PlayLayer_addObjectH(gd::PlayLayer* self, void*, gd::GameObject* obj);

    inline bool(__thiscall* CustomSongWidget_init)(gd::CustomSongWidget*, void*, void*, bool, bool, bool, bool, bool, bool);
    bool __fastcall CustomSongWidget_initH(gd::CustomSongWidget* self, int, void* obj, void* delegate, bool a4, bool a5, bool a6, bool a7, bool a8, bool a9);

    inline bool(__thiscall* LevelInfoLayer_init)(cocos2d::CCLayer*, gd::GJGameLevel*, bool);
    bool __fastcall LevelInfoLayer_initH(cocos2d::CCLayer* self, int, gd::GJGameLevel* level, bool challenge);

    inline bool(__thiscall* MenuLayer_init)(cocos2d::CCLayer*);
    bool __fastcall MenuLayer_initH(cocos2d::CCLayer* self, int);

    inline bool(__thiscall* LevelEditorLayer_init)(void*, gd::GJGameLevel*, bool);
    bool __fastcall LevelEditorLayer_initH(void *self, void*, gd::GJGameLevel* m_lvl, bool unk);

    inline void(__thiscall* EditorPauseLayer_onExitEditor)(cocos2d::CCObject*);
    void __fastcall EditorPauseLayer_onExitEditorH(cocos2d::CCObject* self, void* edx);

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

namespace hitboxes_mode
{
    void init();
    void enabled_hitbox(bool enabled);
    void enabled_draw_trail(bool enabled);
    inline void(__thiscall* resetLevel)(gd::PlayLayer*);
    void __fastcall resetLevel_H(gd::PlayLayer* self);

    inline bool(__thiscall* drawPolygon)(CCDrawNode*, CCPoint*, unsigned int, _ccColor4F const&, float, _ccColor4F const&);
    bool __fastcall drawPolygon_H(CCDrawNode* self, void*, cocos2d::CCPoint* position, unsigned int count, _ccColor4F const& fillColor, float borderWidth, _ccColor4F const& borderColor);

    inline bool(__thiscall* drawCircle)(CCDrawNode*, CCPoint*, float, _ccColor4F const&, float, _ccColor4F const&, unsigned int);
    bool __fastcall drawCircle_H(CCDrawNode* self, void*, cocos2d::CCPoint* position, float radius, _ccColor4F const& fillColor, float borderWidth, _ccColor4F const& borderColor, unsigned int segments);

    inline void(__thiscall* postUpdate)(gd::PlayLayer*, float);
    void __fastcall postUpdate_H(gd::PlayLayer* self, int edx, float dt);
}

extern float left_over;