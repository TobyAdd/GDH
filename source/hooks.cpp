#include "hooks.hpp"
#include "replayEngine.hpp"
#include "console.hpp"
#include "memory.hpp"
#include <chrono>
#include <gd.h>
#include "gui.hpp"
#include "lables.hpp"

int hooks::transitionSelect = 0;
unsigned hooks::frame = 0;
bool isdead = false;
bool lastFrameDead = false;
float delta = 0;

namespace objectsReferences
{

    std::vector<uint32_t *> gamemodePortals;
    std::vector<uint32_t *> sizePortals;
    std::vector<uint32_t *> speedPortals;
    std::vector<uint32_t *> startPositions;
    std::vector<uint32_t *> coinsObject;

}

float left_over = 0.f;
void(__thiscall *CCScheduler_update)(void *, float);
void __fastcall CCScheduler_update_H(void *self, int, float dt)
{
    dt *= engine.speed;

    if (!engine.fps_enabled)
        return CCScheduler_update(self, dt);

    float newdt = 1.f / engine.fps;

    if (!engine.real_time)
        return CCScheduler_update(self, newdt);

    unsigned times = static_cast<int>((dt + left_over) / newdt);
    auto start = std::chrono::high_resolution_clock::now();

    for (unsigned i = 0; i < times; ++i)
    {
        CCScheduler_update(self, newdt);
        using namespace std::literals;
        if (std::chrono::high_resolution_clock::now() - start > 33.333ms)
        {
            times = i + 1;
            break;
        }
    }
    left_over += dt - newdt * times;
}

const ccColor3B tintColor = {255, 0, 0};
CCSprite* tintptr = nullptr;
bool __fastcall hooks::PlayLayer_init_H(CCLayer *self, int edx, void *GJGameLevel, bool a3, bool a4)
{
    objectsReferences::startPositions.clear();

    delta = 0;
    if (gui::best_run == true)
        Labels::SetupLabel(self, "bestrun", "Best Run: 0%");

    if (gui::message == true)
        Labels::SetupLabel(self, "message", gui::custom_message);

    if (gui::ndeaths == true)
        Labels::SetupLabel(self, "ndeaths", "Noclip Deaths: 0");
    
    Labels::setPositions();

    auto size = cocos2d::CCDirector::sharedDirector()->getWinSize();

    auto tint = CCSprite::createWithSpriteFrameName("block005b_05_001.png");
    tint->setPosition({size.width / 2, size.height / 2});
    tint->setScale(1000.0f);
    tint->setColor(tintColor);
    tint->setOpacity(0);
    tint->setZOrder(1000);

    tintptr = tint;

    self->addChild(tint);

    const auto res = PlayLayer_init(self, GJGameLevel, a3, a4);
    if (res)
    {
        // idk
    }

    return res;
}

void __fastcall hooks::playLayer_update_H(void *self, int edx, float dt)
{
    delta += dt;
    playLayer_update(self, dt);

    frame = engine.getFrame(self);

    if (isdead == true && lastFrameDead == false)
    {
        if (tintptr != nullptr && gui::tint == true)
            tintptr->setOpacity(50);
        lastFrameDead = true;
        Labels::updateNoclipDeaths();
    }

    if (isdead == false && lastFrameDead == true)
    {
        if (tintptr != nullptr && gui::tint == true)
            tintptr->setOpacity(0);
    }

    if (isdead == false)
    {
        lastFrameDead = false;
    }

    isdead = false;

    if (engine.mode == state::play)
    {
        while (engine.index < static_cast<int>(engine.replay.size()) &&
               engine.getFrame(self) >= engine.replay[engine.index].frame)
        {
            auto &replayData = engine.replay[engine.index];
            hooks::GJBaseGameLayer_HandleButton(self, replayData.hold, replayData.player_button, replayData.player);
            engine.index++;
        }
    }
}

void __fastcall hooks::PlayLayer_resetLevel_H(void *self)
{
    PlayLayer_resetLevel(self);
    Labels::updateNoclipDeaths(true);
    if (engine.mode == state::play)
    {
        engine.index = 0;
    }
    else if (engine.mode == state::record)
    {
        unsigned frame = engine.getFrame(self);

        auto check = [&](replay_data &action) -> bool
        {
            return action.frame >= frame;
        };

        engine.replay.erase(remove_if(engine.replay.begin(), engine.replay.end(), check), engine.replay.end());

        if (!engine.replay.empty() && engine.replay.back().hold)
        {
            engine.replay.push_back({frame, false, engine.replay.back().player_button, engine.replay.back().player});
        }
    }
}

void __fastcall hooks::playLayer_levelComplate_H(int *self)
{
    playLayer_levelComplate(self);
    Labels::updateBestRun(100);
    if (engine.mode == state::record)
        engine.mode = state::disable;
}

void __fastcall hooks::PlayLayer_destructor_H(void *self)
{
    Labels::ClearLabels();
    Labels::updateBestRun(69, true);
    PlayLayer_destructor(self);
    if (engine.mode == state::record)
        engine.mode = state::disable;
}


void __fastcall hooks::playLayer_death_H(int self, int edx, void* player, void* obj)
{
    if (delta > 0.1f)
    {
        isdead = true;
    }
    Labels::updateBestRun(*((DWORD *)self + 0xBAD));
;
	playLayer_death(self, player, obj);
}


int __fastcall hooks::GJBaseGameLayer_HandleButton_H(void *self, uintptr_t, int push, int player_button, BOOL is_player1)
{
    if (engine.mode == state::play)
    {
        return 0;
    }

    auto ret = GJBaseGameLayer_HandleButton(self, push, player_button, is_player1);
    if (engine.mode == state::record)
    {
        engine.handle_action(self, push, player_button, is_player1);
    }
    return ret;
}

void *CreateTransition(float duration, void *scene, const char *functionName)
{
    auto cocos = GetModuleHandleA("libcocos2d.dll");
    return reinterpret_cast<void *(__cdecl *)(float, void *)>(
        GetProcAddress(cocos, functionName))(duration, scene);
}

void *(__cdecl *CCTransitionFade_create)(float, void *);
void *__cdecl CCTransitionFade_createH(float duration, void *scene)
{
    const char *transitionFunctions[] = {
        "?create@CCTransitionCrossFade@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionFadeBL@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionFadeDown@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionFadeTR@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionFadeUp@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionFlipAngular@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionFlipX@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionFlipY@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionJumpZoom@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionMoveInB@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionMoveInL@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionMoveInR@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionMoveInT@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionPageTurn@cocos2d@@SAPAV12@MPAVCCScene@2@_N@Z",
        "?create@CCTransitionProgressHorizontal@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionProgressInOut@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionProgressOutIn@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionProgressRadialCCW@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionProgressRadialCW@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionProgressVertical@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionRotoZoom@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionShrinkGrow@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionSlideInB@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionSlideInL@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionSlideInR@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionSlideInT@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionSplitCols@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionSplitRows@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionTurnOffTiles@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionZoomFlipAngular@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionZoomFlipX@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
        "?create@CCTransitionZoomFlipY@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    };

    if (hooks::transitionSelect >= 1 && hooks::transitionSelect <= sizeof(transitionFunctions) / sizeof(transitionFunctions[0]))
    {
        return CreateTransition(duration, scene, transitionFunctions[hooks::transitionSelect - 1]);
    }
    else
    {
        return CCTransitionFade_create(duration, scene);
    }
}

bool(__thiscall *GameStatsManager_isItemUnlocked)(void *, int, int);
bool __fastcall GameStatsManager_isItemUnlockedH(void *self, int, int a, int b)
{
    auto ret = GameStatsManager_isItemUnlocked(self, a, b);

    if (b == 17 && a == 12)
    {
        return true;
    }

    return ret;
}



namespace startPosSwitcher
{        
    bool enabled = false;
    int use_a_d = false;
    
    int startposIndex = -1;

    void switchStartPos(gd::PlayLayer *pl, bool direction)
    {

        // from<bool>(pl, 0x2a7c) practice
        // from<bool>(pl, 0x2d91) testmode

        if (pl == nullptr || from<bool>(pl, 0x2c28) || from<bool>(pl, 0x2eb1) || objectsReferences::startPositions.empty())
        {
            return;
        }

        if (direction)
        {
            startposIndex = startposIndex - 1;
            if (startposIndex < -1)
            {
                startposIndex = objectsReferences::startPositions.size() - 1;
            }
        }
        else
        {
            startposIndex = startposIndex + 1;
            if (static_cast<unsigned int>(startposIndex) >= objectsReferences::startPositions.size())
            {
                startposIndex = -1;
            }
        }

        from<int>(pl, 0x2e14) = 0;

        if (startposIndex >= 0)
        {
            pl->setStartPosObject(objectsReferences::startPositions[startposIndex]);
        }
        else
        {
            pl->setStartPosObject(nullptr);
        }

        Console::WriteF("Startpos Switcher (%i/%i)\n", startposIndex + 1, objectsReferences::startPositions.size());

        if (from<bool>(pl, 0x2a7c))
        { // practice variable

            auto checkpoints = from<cocos2d::CCArray *>(pl, 0x2e18);

            while (checkpoints->count() != 0)
            {
                pl->removeCheckpoint(false);
            }

            if (gd::GameStatsManager::sharedState()->isItemEnabled(0xc, 0x11))
            {
                pl->resetLevelFromStart();
                pl->startMusic();
            }
            else
            {
                pl->resetLevel();
            }
        }
        else
        {
            pl->resetLevelFromStart();
            pl->startMusic();
        }
    }

    void handleKeyPress(int keyCode)
    {
        auto pl = gd::GameManager::sharedState()->getPlayLayer();

        if (!pl)
        {
            return;
        }

        if (!use_a_d && (keyCode == VK_LEFT || keyCode == VK_RIGHT))
        {
            startPosSwitcher::switchStartPos(pl, keyCode == VK_LEFT);
        }
        else if (use_a_d && (keyCode == 'A' || keyCode == 'D'))
        {
            startPosSwitcher::switchStartPos(pl, keyCode == 'A');
        }
    }

}

void(__thiscall *PlayLayer_addObject)(gd::PlayLayer *, void *);
void __fastcall PlayLayer_addObjectHook(gd::PlayLayer *self, void *, uint32_t *object)
{
    uint32_t objectID = object[0xE1];
    auto obje = reinterpret_cast<cocos2d::CCNode *>(object);
    obje->retain();

    switch (objectID)
    {
    case 12:
    case 13:
    case 47:
    case 111:
    case 660:
    case 745:
    case 1331:
    case 1933:
        objectsReferences::gamemodePortals.push_back(object);
        // updateStartPos(self);
        break;

        // case 45:
        // case 46:
        //     mirrorPortals.push_back(object);
        //     updateStartPos();
        //     break;

    case 99:
    case 101:
        objectsReferences::sizePortals.push_back(object);
        // updateStartPos(self);
        break;
    case 200:
    case 201:
    case 202:
    case 203:
    case 1334:
        objectsReferences::speedPortals.push_back(object);
        // updateStartPos(self);
        break;

    case 0x1f:
        objectsReferences::startPositions.push_back(object);
        // updateStartPos(self);
        break;
    case 142:
    case 1329:
        objectsReferences::coinsObject.push_back(object);
    }

    obje->release();

    PlayLayer_addObject(self, object);
}

void *(__thiscall *dispatchKeyboardMSG)(void *self, int key, bool down, bool idk);
void *__fastcall dispatchKeyboardMSGHook(void *self, void *, int key, bool down, bool idk)
{
    if (!idk && down && startPosSwitcher::enabled)
    {
        startPosSwitcher::handleKeyPress(key);
    }

    return dispatchKeyboardMSG(self, key, down, idk);
}

namespace layout_mode
{
    bool enabled = false;

    void(__thiscall* GameObject_setVisible)(void*, bool);
    void __fastcall GameObject_setVisible_H(void *self, int, bool visible) {
        if (enabled) {
            int m_objectType = *(int*)((size_t)self + 0x31c);

            if (m_objectType == 7) {
                GameObject_setVisible(self, false);
                return;
            }
            if (m_objectType == 0) {
                return;
            }
        }
        GameObject_setVisible(self, visible);
    }

    void(__thiscall* GameObject_setOpacity)(void*, unsigned char);
    void __fastcall GameObject_setOpacity_H(void *self, int, unsigned char opacity) {
        if (enabled) {
            int m_objectType = *(int*)((size_t)self + 0x31c);

            if (m_objectType == 7) {
                GameObject_setOpacity(self, 0);
                return;
            }
            if (m_objectType == 0) {
                return;
            }
        }

        GameObject_setOpacity(self, opacity);
    }

    void(__thiscall* GameObject_setGlowColor)(void*, cocos2d::_ccColor3B const&);
    void __fastcall GameObject_setGlowColor_H(void *self, int, cocos2d::_ccColor3B const& color) {
        if (enabled) {
            GameObject_setGlowColor(self, cocos2d::ccc3(255, 255, 255));
            return;
        }

        GameObject_setGlowColor(self, color);
    }

    void(__thiscall* PlayLayer_updateVisibility)(void*, void*, void*, void*);
    void __fastcall PlayLayer_updateVisibility_H(void *self, int, void *dt, void *unk, void *unk_2) {
        if (enabled) {
            int typeTrigger = *(int*)((size_t)self + 0x384);

            if (typeTrigger == 899 || typeTrigger == 1006 || typeTrigger == 1007 || typeTrigger == 2903) {
                return;
            }
        }
        PlayLayer_updateVisibility(self, dt, unk, unk_2);
    }

    void(__thiscall* GameObject_setObjectColor)(void*, cocos2d::_ccColor3B const&);
    void __fastcall GameObject_setObjectColor_H(void *self, int, cocos2d::_ccColor3B const& color) {
        if (enabled) {
            int m_objectType = *(int*)((size_t)self + 0x31c);

            if (m_objectType != 7) {
                GameObject_setObjectColor(self, cocos2d::ccc3(255, 255, 255));
                return;
            }
        }
        GameObject_setObjectColor(self, color);
    }

    void(__thiscall* GJBaseGameLayer_updateLevelColors)(void*);
    void __fastcall GJBaseGameLayer_updateLevelColors_H(void* self, void* unk) {
        if (!enabled) {
            GJBaseGameLayer_updateLevelColors(self);
        }        
    }

    void init() {
        HMODULE gd = GetModuleHandleA(0);
        
        MH_CreateHook((void*)((DWORD)gd + 0x13B890), GameObject_setVisible_H, (void**)&GameObject_setVisible);
        MH_CreateHook((void*)((DWORD)gd + 0x13B490), GameObject_setOpacity_H, (void**)&GameObject_setOpacity);
        MH_CreateHook((void*)((DWORD)gd + 0x2E2BF0), PlayLayer_updateVisibility_H, (void**)&PlayLayer_updateVisibility);
        MH_CreateHook((void*)((DWORD)gd  + 0x1415F0), GameObject_setGlowColor_H, (void**)&GameObject_setGlowColor);
        MH_CreateHook((void*)((DWORD)gd + 0x141300), GameObject_setObjectColor_H, (void**)&GameObject_setObjectColor);
        MH_CreateHook((void*)((DWORD)gd + 0x194490), GJBaseGameLayer_updateLevelColors_H, (void**)&GJBaseGameLayer_updateLevelColors);
    }

    void set_enabled(bool enable) {
        HMODULE gd = GetModuleHandleA(0);

        enabled = enable;
        
        if (enable) {
            memory::WriteBytes((DWORD)gd + 0x13EFA1, "90 90 90 90 90 90");
        }
        else {
            memory::WriteBytes((DWORD)gd + 0x13EFA1, "88 87 62 04 00 00");
        }
    }
}


void hooks::init()
{
    HMODULE gd = GetModuleHandleA(0);
    HMODULE cocos = GetModuleHandleA("libcocos2d.dll");

    layout_mode::init();

    MH_CreateHook((void *)((DWORD)gd + 0x2dc4a0), PlayLayer_init_H, (void **)&PlayLayer_init);
    MH_CreateHook((void *)((DWORD)gd + 0x1bb780), playLayer_update_H, (void **)&playLayer_update);
    MH_CreateHook((void *)((DWORD)gd + 0x2ea130), PlayLayer_resetLevel_H, (void **)&PlayLayer_resetLevel);
    MH_CreateHook((void *)((DWORD)gd + 0x2dc080), PlayLayer_destructor_H, (void **)&PlayLayer_destructor);
    MH_CreateHook((void *)((DWORD)gd + 0x2ddb60), playLayer_levelComplate_H, (void **)&playLayer_levelComplate);
    MH_CreateHook((void *)((DWORD)gd + 0x2E6730), playLayer_death_H, (void **)(&playLayer_death));
    MH_CreateHook((void *)((DWORD)gd + 0x1b69f0), GJBaseGameLayer_HandleButton_H, (void **)&GJBaseGameLayer_HandleButton);
    
    MH_CreateHook((void *)(gd::base + 0x2e19b0), PlayLayer_addObjectHook, (void **)&PlayLayer_addObject);
    MH_CreateHook(GetProcAddress((HMODULE)gd::cocos_base, "?dispatchKeyboardMSG@CCKeyboardDispatcher@cocos2d@@QAE_NW4enumKeyCodes@2@_N1@Z"), dispatchKeyboardMSGHook, (void **)&dispatchKeyboardMSG);

    MH_CreateHook(GetProcAddress(cocos, "?update@CCScheduler@cocos2d@@UAEXM@Z"), CCScheduler_update_H, (void **)&CCScheduler_update);
    MH_CreateHook(GetProcAddress(cocos, "?create@CCTransitionFade@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"), CCTransitionFade_createH, (void **)&CCTransitionFade_create);
    MH_CreateHook((void *)((DWORD)gd + 0x176570), GameStatsManager_isItemUnlockedH, (void **)&GameStatsManager_isItemUnlocked);
}