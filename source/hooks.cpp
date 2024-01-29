#include "hooks.hpp"
#include "replayEngine.hpp"
#include "console.hpp"
#include "memory.hpp"
#include <chrono>

int hooks::transitionSelect = 0;
unsigned hooks::frame = 0;
bool hooks::rgb_icons = false;

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

bool __fastcall hooks::PlayLayer_init_H(gd::PlayLayer *self, int edx, void *GJGameLevel, bool a3, bool a4)
{
    objectsReferences::startPositions.clear();

    const auto res = PlayLayer_init(self, GJGameLevel, a3, a4);
    if (res)
    {        
        //idk
    }

    return res;
}

void __fastcall hooks::playLayer_update_H(gd::PlayLayer *self, int edx, float deltaTime)
{
    playLayer_update(self, deltaTime);

    if (rgb_icons) {
        float r, g, b;
        ImGui::ColorConvertHSVtoRGB((float)ImGui::GetTime() * 0.25f, 1, 1, r, g, b);
        cocos2d::ccColor3B color = {(GLubyte)(r * 255.0f), (GLubyte)(g * 255.0f), (GLubyte)(b * 255.0f)};

        self->m_pPlayer1()->setColor(color);
        self->m_pPlayer2()->setColor(color);
    
        self->m_pPlayer1()->m_pSecondarySprite()->setColor(color);
        self->m_pPlayer2()->m_pSecondarySprite()->setColor(color);

        reinterpret_cast<cocos2d::CCNodeRGBA*>(self->m_pPlayer1()->m_waveTrail())->setColor(color);
        reinterpret_cast<cocos2d::CCNodeRGBA*>(self->m_pPlayer2()->m_waveTrail())->setColor(color);
    }

    frame = engine.getFrame(self);

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

void __fastcall hooks::PlayLayer_resetLevel_H(gd::PlayLayer *self)
{
    PlayLayer_resetLevel(self);

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
    if (engine.mode == state::record)
        engine.mode = state::disable;
}

void __fastcall hooks::PlayLayer_destructor_H(void *self)
{
    PlayLayer_destructor(self);
    if (engine.mode == state::record)
        engine.mode = state::disable;
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

    void(__thiscall *GameObject_setVisible)(uint32_t *, bool);
    void __fastcall GameObject_setVisibleH(uint32_t *self, int, bool visible)
    {

        if (!enabled)
        {
            GameObject_setVisible(self, visible);
            return;
        }

        uint32_t objectID = self[0xE1];

        int objectType = from<int>(self, 0x31c);

        // std::stringstream stfdgh;
        // stfdgh << objectType << "\t" << objectID;
        // Console::WriteLn(stfdgh.str());

        if (objectType == 7 && objectID == 0)
        {
            GameObject_setVisible(self, visible);
            return;
        }

        if (objectType == 7 && objectID != 2 && objectID != 35 && objectID != 7 && objectID != 44 && objectID != 749 && objectID != 12 &&
            objectID != 38 && objectID != 47 && objectID != 111 && objectID != 8 &&
            objectID != 13 && objectID != 660 && objectID != 745 && objectID != 101 &&
            objectID != 99 && objectID != 1331)
        {
            GameObject_setVisible(self, false);
            return;
        }

        GameObject_setVisible(self, visible);
    }

    void(__thiscall *GameObject_setOpacity)(uint32_t *, int);
    void __fastcall GameObject_setOpacityH(uint32_t *self, int, int opp)
    {

        if (!enabled)
        {
            GameObject_setOpacity(self, opp);
            return;
        }

        uint32_t objectID = self[0xE1];
        int objectType = from<int>(self, 0x31c);

        if (objectType == 7 && objectID != 2 && objectID != 35 && objectID != 7 && objectID != 44 && objectID != 749 && objectID != 12 &&
            objectID != 38 && objectID != 47 && objectID != 111 && objectID != 8 &&
            objectID != 13 && objectID != 660 && objectID != 745 && objectID != 101 &&
            objectID != 99 && objectID != 1331)
        {
            GameObject_setOpacity(self, 0);
            return;
        }

        GameObject_setOpacity(self, opp);
    }

    void(__thiscall *EffectGameObject_triggerObject)(uint32_t *, void *, void *, void *);
    void __fastcall EffectGameObject_triggerObjectHook(uint32_t *self, int, void *dt, void *a, void *v)
    {

        if (!enabled)
        {
            EffectGameObject_triggerObject(self, dt, a, v);
            return;
        }

        std::stringstream suytfr;
        if (self)
        {
            auto typeTrigger = from<int>(self, 0x384);
            // suytfr << typeTrigger << "\t" << a << "\t" << v;
            // Console::WriteLn(suytfr.str());

            if (typeTrigger == 899 || typeTrigger == 1006 || typeTrigger == 1007 || typeTrigger == 2903)
            {
                return;
            }
        }

        EffectGameObject_triggerObject(self, dt, a, v);
    }

    void(__thiscall *GameObject_setObjectColor)(uint32_t *, char *);
    void __fastcall GameObject_setObjectColorH(uint32_t *self, int, char *visible)
    {
        if (!enabled)
        {
            GameObject_setObjectColor(self, visible);
            return;
        }
        // GameObject_setObjectColor(self, visible);
    }

    void(__thiscall *GameObject_setGlowColor)(uint32_t *, char *);
    void __fastcall GameObject_setGlowColorH(uint32_t *self, int, char *visible)
    {
        if (!enabled)
        {
            GameObject_setGlowColor(self, visible);
            return;
        }
        // GameObject_setGlowColor(self, visible);
    }

    void(__thiscall *GameObject_setChildColor)(uint32_t *, cocos2d::_ccColor3B *);
    void __fastcall GameObject_setChildColorH(uint32_t *self, int, cocos2d::_ccColor3B *visible)
    {
        if (!enabled)
        {
            GameObject_setChildColor(self, visible);
            return;
        }
        // GameObject_setChildColor(self, visible);

        auto typeTrigger = from<int>(self, 0x31c);
        if (typeTrigger == 47)
        {
            cocos2d::_ccColor3B color = {255, 255, 255};
            GameObject_setChildColor(self, &color);
        }

        // GameObject_setChildColor(self, visible);
    }

    void(__thiscall *AnimatedGameObject_updateChildSpriteColor)(uint32_t *, char *);
    void __fastcall AnimatedGameObject_updateChildSpriteColorH(uint32_t *self, int, char *visible)
    {
        if (!enabled)
        {
            AnimatedGameObject_updateChildSpriteColor(self, visible);
            return;
        }
        // AnimatedGameObject_updateChildSpriteColor(self, visible);
    }

    void(__thiscall *AnimatedGameObject_setObjectColor)(uint32_t *, cocos2d::_ccColor3B *);
    void __fastcall AnimatedGameObject_setObjectColorH(uint32_t *self, int, cocos2d::_ccColor3B *visible)
    {
        if (!enabled)
        {
            AnimatedGameObject_setObjectColor(self, visible);
            return;
        }
        cocos2d::_ccColor3B color = {0, 0, 0};
        AnimatedGameObject_setObjectColor(self, &color);
    }

    const char *getNameObjA(cocos2d::CCNode *obj)
    {
        return (typeid(*obj).name() + 6);
    }

    void(__thiscall *updateGround01Color)(cocos2d::CCNode *, cocos2d::_ccColor3B);
    void __fastcall updateGround01ColorH(cocos2d::CCNode *self, void *unk, cocos2d::_ccColor3B p1)
    {

        updateGround01Color(self, p1);
    }

    void(__thiscall *GJBaseGameLayer_updateLevelColors)(void *);
    void __fastcall GJBaseGameLayer_updateLevelColors_H(void *self, void *unk)
    {
        if (!enabled)
        {
            GJBaseGameLayer_updateLevelColors(self);
            return;
        }

        auto GJEffectManagerOBJ = from<void *>(self, 0x650);
        if (GJEffectManagerOBJ)
        {
            auto coloractions = from<cocos2d::CCDictionary *>(GJEffectManagerOBJ, 0x110);
            if (coloractions)
            {
                coloractions->removeAllObjects();
            }
        }

        cocos2d::_ccColor3B color1 = {40, 125, 255};
        cocos2d::_ccColor3B color2 = {0, 102, 255};
        cocos2d::_ccColor3B color3 = {255, 255, 255};

        // bg 40 125 255
        // g1 0 102 255
        // g2 0 102 255
        // line 255 255 255
        // mg 40 125 255
        // mg2 40 125 255

        from<cocos2d::CCSprite *>(self, 0x9c4)->setColor(color1); // bg
        auto ground1 = from<gd::GJGroundLayer *>(self, 0x9cc);
        auto ground2 = from<gd::GJGroundLayer *>(self, 0x9d0);
        auto mg = from<gd::GJMGLayer *>(self, 0x9d4);

        if (ground1)
        {
            // updateGround01Color(ground1, color1);
            ground1->updateGround01Color(color1);
            ground2->updateGround01Color(color1);

            if (from<bool>(ground1, 0x140))
            {
                ground1->updateGround02Color(color1);
                ground2->updateGround02Color(color1);
            }
            if (ground1->getChildByTag(2))
            {
                reinterpret_cast<cocos2d::CCSprite *>(ground1->getChildByTag(2))->setColor(color3);
            }
            if (ground2->getChildByTag(2))
            {
                reinterpret_cast<cocos2d::CCSprite *>(ground2->getChildByTag(2))->setColor(color3);
            }
        }

        if (mg)
        {
            mg->updateGroundColor(color1, true);
            mg->updateGroundOpacity(255, true);
        }
    }

    void(__thiscall *PlayLayer_loadDefaultColors)(gd::PlayLayer *);
    void __fastcall PlayLayer_loadDefaultColorsH(gd::PlayLayer *self)
    {

        if (from<cocos2d::CCArray *>(self, 0x884))
        {
            std::stringstream skjgfd;
            skjgfd << from<cocos2d::CCArray *>(self, 0x884)->count();
            Console::WriteLn(skjgfd.str());
        }

        if (!enabled)
        {
            PlayLayer_loadDefaultColors(self);
            return;
        }
    }

    void init()
    {
        HMODULE gd = GetModuleHandleA(0);

        MH_CreateHook((void *)((DWORD)gd + 0x13b890), GameObject_setVisibleH, (void **)&GameObject_setVisible);
        MH_CreateHook((void *)((DWORD)gd + 0x13b490), GameObject_setOpacityH, (void **)&GameObject_setOpacity);
        MH_CreateHook((void *)((DWORD)gd + 0x141300), GameObject_setObjectColorH, (void **)&GameObject_setObjectColor);
        MH_CreateHook((void *)((DWORD)gd + 0x1415f0), GameObject_setGlowColorH, (void **)&GameObject_setGlowColor);
        MH_CreateHook((void *)((DWORD)gd + 0x141630), GameObject_setChildColorH, (void **)&GameObject_setChildColor);
        MH_CreateHook((void *)((DWORD)gd + 0x39bc20), AnimatedGameObject_setObjectColorH, (void **)&AnimatedGameObject_setObjectColor);
        MH_CreateHook((void *)((DWORD)gd + 0x39b590), AnimatedGameObject_updateChildSpriteColorH, (void **)&AnimatedGameObject_updateChildSpriteColor);
        MH_CreateHook((void *)((DWORD)gd + 0x39ccf0), EffectGameObject_triggerObjectHook, (void **)&EffectGameObject_triggerObject);
        // MH_CreateHook((void*)((DWORD)gd + 0x2e4e80), PlayLayer_loadDefaultColorsH, (void**)&PlayLayer_loadDefaultColors);
        MH_CreateHook((void *)((DWORD)gd + 0x194490), GJBaseGameLayer_updateLevelColors_H, (void **)&GJBaseGameLayer_updateLevelColors);
        // MH_CreateHook((void*)((DWORD)gd + 0x1f7ab0), updateGround01ColorH, (void**)&updateGround01Color);
    }

    void set_enabled(bool enable)
    {
        HMODULE gd = GetModuleHandleA(0);

        enabled = enable;

        if (enable)
        {
            memory::WriteBytes((DWORD)gd + 0x13EFA1, "90 90 90 90 90 90");
            memory::WriteBytes((DWORD)gd + 0x2E4ED8, "90 90 EB 3A");
            memory::WriteBytes((DWORD)gd + 0x2E4F41, "90 90 EB 5C");
        }
        else
        {
            memory::WriteBytes((DWORD)gd + 0x13EFA1, "88 87 62 04 00 00");
            memory::WriteBytes((DWORD)gd + 0x2E4ED8, "85 C0 74 3A");
            memory::WriteBytes((DWORD)gd + 0x2E4F41, "85 C0 74 5C");
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
    MH_CreateHook((void *)((DWORD)gd + 0x1b69f0), GJBaseGameLayer_HandleButton_H, (void **)&GJBaseGameLayer_HandleButton);

    MH_CreateHook((void *)(gd::base + 0x2e19b0), PlayLayer_addObjectHook, (void **)&PlayLayer_addObject);
    MH_CreateHook(GetProcAddress((HMODULE)gd::cocos_base, "?dispatchKeyboardMSG@CCKeyboardDispatcher@cocos2d@@QAE_NW4enumKeyCodes@2@_N1@Z"), dispatchKeyboardMSGHook, (void **)&dispatchKeyboardMSG);

    MH_CreateHook(GetProcAddress(cocos, "?update@CCScheduler@cocos2d@@UAEXM@Z"), CCScheduler_update_H, (void **)&CCScheduler_update);
    MH_CreateHook(GetProcAddress(cocos, "?create@CCTransitionFade@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"), CCTransitionFade_createH, (void **)&CCTransitionFade_create);
    MH_CreateHook((void *)((DWORD)gd + 0x176570), GameStatsManager_isItemUnlockedH, (void **)&GameStatsManager_isItemUnlocked);
}