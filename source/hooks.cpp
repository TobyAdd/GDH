#include "hooks.hpp"
#include "memory.hpp"
#include "replayEngine.hpp"
#include <chrono>
#include <algorithm>
#include "gd.hpp"
#include "recorder.hpp"
#include "labels.hpp"

std::vector<gd::StartPosObject *> StartPosObjects;
std::vector<gd::GameObject *> gravityPortals, dualPortals, gamemodePortals, miniPortals, speedChanges, mirrorPortals, coinsObjects;

void SetupSmartStartPos(gd::PlayLayer *self)
{
    if (!hacks::smart_startpos)
        return;
    if (StartPosObjects.empty())
        return;
    for (gd::StartPosObject *startPos : StartPosObjects)
    {
        auto levelSettings = startPos->m_pLevelSettingsObject();

        for (auto &portal : gravityPortals)
        {
            if (portal->getPositionX() - 10 > startPos->getPositionX())
            {
                break;
            }
            if (portal->getPositionX() - 10 < startPos->getPositionX())
            {
                levelSettings->m_flipGravity() = portal->m_nObjectID() == 11;
            }
        }

        for (auto &portal : mirrorPortals)
        {
            if (portal->getPositionX() - 10 > startPos->getPositionX())
            {
                break;
            }
            if (portal->getPositionX() - 10 < startPos->getPositionX())
            {
                levelSettings->m_reverseGameplay() = portal->m_nObjectID() == 45;
            }
        }

        levelSettings->m_startDual() = self->m_pLevelSettingsObject()->m_startDual();
        for (auto &portal : dualPortals)
        {
            if (portal->getPositionX() - 10 > startPos->getPositionX())
            {
                break;
            }
            if (portal->getPositionX() - 10 < startPos->getPositionX())
            {
                levelSettings->m_startDual() = portal->m_nObjectID() == 286;
            }
        }

        levelSettings->m_startMode() = self->m_pLevelSettingsObject()->m_startMode();
        for (auto &portal : gamemodePortals)
        {
            if (portal->getPositionX() - 10 > startPos->getPositionX())
            {
                break;
            }
            if (portal->getPositionX() - 10 < startPos->getPositionX())
            {
                switch (portal->m_nObjectID())
                {
                case 12:
                    levelSettings->m_startMode() = 0;
                    break;
                case 13:
                    levelSettings->m_startMode() = 1;
                    break;
                case 47:
                    levelSettings->m_startMode() = 2;
                    break;
                case 111:
                    levelSettings->m_startMode() = 3;
                    break;
                case 660:
                    levelSettings->m_startMode() = 4;
                    break;
                case 745:
                    levelSettings->m_startMode() = 5;
                    break;
                case 1331:
                    levelSettings->m_startMode() = 6;
                    break;
                case 1933:
                    levelSettings->m_startMode() = 7;
                    break;
                }
            }
        }

        levelSettings->m_startMini() = self->m_pLevelSettingsObject()->m_startMini();
        for (auto &portal : miniPortals)
        {
            if (portal->getPositionX() - 10 > startPos->getPositionX())
            {
                break;
            }
            if (portal->getPositionX() - 10 < startPos->getPositionX())
            {
                levelSettings->m_startMini() = portal->m_nObjectID() == 101;
            }
        }

        levelSettings->m_startSpeed() = self->m_pLevelSettingsObject()->m_startSpeed();
        for (auto &change : speedChanges)
        {
            if (change->getPositionX() - 50 > startPos->getPositionX())
            {
                break;
            }
            if (change->getPositionX() - 50 < startPos->getPositionX())
            {
                switch (change->m_nObjectID())
                {
                case 200:
                    levelSettings->m_startSpeed() = 1;
                    break;
                case 201:
                    levelSettings->m_startSpeed() = 0;
                    break;
                case 202:
                    levelSettings->m_startSpeed() = 2;
                    break;
                case 203:
                    levelSettings->m_startSpeed() = 3;
                    break;
                case 1334:
                    levelSettings->m_startSpeed() = 4;
                    break;
                }
            }
        }
    }
}

bool next_frame = false;

float left_over = 0.f;
void(__thiscall *CCScheduler_update)(void *, float);
void __fastcall CCScheduler_update_H(void *self, int, float dt)
{
    dt *= hacks::speed;
    
    float newdt = 1.f / hacks::tps; 

    if (engine.frame_advance) {
        if (next_frame) {
            next_frame = false;
            return CCScheduler_update(self, hacks::tps_enabled ? newdt : dt);
        }

        return;
    }

    if (!hacks::tps_enabled)    
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

void __fastcall hooks::PlayLayer_addObjectH(gd::PlayLayer *self, void *, gd::GameObject *obj)
{
    PlayLayer_addObject(self, obj);
    switch (obj->m_nObjectID())
    {
    case 9:
    case 10:
    case 11:
        gravityPortals.push_back(obj);
        SetupSmartStartPos(self);
        break;
    case 12:
    case 13:
    case 47:
    case 111:
    case 660:
    case 745:
    case 1331:
        gamemodePortals.push_back(obj);
        SetupSmartStartPos(self);
        break;
    case 31:
        StartPosObjects.push_back(static_cast<gd::StartPosObject *>(obj));
        SetupSmartStartPos(self);
        break;
    case 45:
        mirrorPortals.push_back(obj);
        SetupSmartStartPos(self);
        break;
    case 99:
    case 101:
        miniPortals.push_back(obj);
        SetupSmartStartPos(self);
        break;
    case 286:
    case 287:
        dualPortals.push_back(obj);
        SetupSmartStartPos(self);
        break;
    case 200:
    case 201:
    case 202:
    case 203:
    case 1334:
        speedChanges.push_back(obj);
        SetupSmartStartPos(self);
        break;
    case 1329:
    case 142:
        coinsObjects.push_back(obj);
    default:
        break;
    }
}

void replaceShit(std::string& str, const std::string& from, const std::string& to) {
    if (from.empty())
        return;
    
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        if (start_pos + from.length() == str.length() && str.find_first_not_of(' ', start_pos) == std::string::npos) {
            str.erase(start_pos);
        } else {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
    }
}

gd::GJGameLevel *m_level_;
bool __fastcall hooks::PlayLayer_initH(gd::PlayLayer *self, int edx, gd::GJGameLevel *m_level, bool a3, bool a4)
{
    auto ret = PlayLayer_init(self, m_level, a3, a4);
    if (ret)
    {
        if (hacks::discord_rpc)
            presence::UpdatePresence(Status::IN_LEVEL, m_level);
        
        std::string level = m_level->m_levelName();
        replaceShit(level, " ", "_");

        engine.replay_name = level;
        recorder.video_name = level + ".mp4";
        recorderAudio.audio_name = level + ".mp3";

        m_level_ = m_level;

        if (hacks::auto_practice_mode)
            reinterpret_cast<void(__thiscall *)(gd::PlayLayer *, bool practice)>(gd::base + 0x2ead30)(self, true);

        CCLabelBMFont* labels = CCLabelBMFont::create("", "bigFont.fnt");

		labels->setZOrder(999);
		labels->setTag(999999);
		labels->setScale(0.4f);
        labels->setOpacity(150);
        labels->setAnchorPoint({1, 0});
        self->addChild(labels);
    }
    return ret;
}

void __fastcall hooks::PlayLayer_update_H(gd::PlayLayer *self, int, float dt)
{
    if (recorder.is_recording) {
        recorder.handle_recording(self, dt);
    }

    if (recorderAudio.is_recording) {
        recorderAudio.handle_recording(self, dt);
    }

    PlayLayer_update(self, dt);

    noclip_accuracy.handle_update(self, dt);

    if (engine.mode == state::play)
    {
        engine.handle_playing(self);
        if (engine.continue_toggled && !engine.replay.empty() && engine.get_frame() >= engine.replay.back().frame)
        {
            engine.mode = record;
            engine.continue_toggled = false;
        }
    }
    else if (engine.mode == state::record) {
        engine.handle_recording(self, true);
        engine.handle_recording(self, false);
    }

    spamBot.handle_spambot(self);
    straightFly.handle_straightfly(self);

    if (hacks::hide_player_enabled) {
        if (hacks::hide_player_p1) {
            self->m_pPlayer1()->setVisible(false);
        }

        if (hacks::hide_player_p2) {
            self->m_pPlayer2()->setVisible(false);
        }
    }

    if (hacks::rgb_icons)
    {
        cocos2d::ccColor3B color1 = {(GLubyte)(hacks::iconcolor1[0] * 255.0f), (GLubyte)(hacks::iconcolor1[1] * 255.0f), (GLubyte)(hacks::iconcolor1[2] * 255.0f)};
        cocos2d::ccColor3B color2 = {(GLubyte)(hacks::iconcolor2[0] * 255.0f), (GLubyte)(hacks::iconcolor2[1] * 255.0f), (GLubyte)(hacks::iconcolor2[2] * 255.0f)};
        self->m_pPlayer1()->setColor(color1);
        self->m_pPlayer2()->setColor(color1);
        self->m_pPlayer1()->m_pSecondarySprite()->setColor(color2);
        self->m_pPlayer2()->m_pSecondarySprite()->setColor(color2);

        reinterpret_cast<cocos2d::CCNodeRGBA *>(self->m_pPlayer1()->m_waveTrail())->setColor(color1);
        reinterpret_cast<cocos2d::CCNodeRGBA *>(self->m_pPlayer2()->m_waveTrail())->setColor(color2);
    }

    CCLabelBMFont* labels = (CCLabelBMFont*)self->getChildByTag(999999);
    if (labels) {
		auto screenLeft = CCDirector::sharedDirector()->getScreenLeft();
        auto screenTop = CCDirector::sharedDirector()->getScreenTop();
        auto screenBottom = CCDirector::sharedDirector()->getScreenBottom();
        auto screenRight = CCDirector::sharedDirector()->getScreenRight();

        labels->setCString(labels::get_labels().c_str());
        auto size = labels->getScaledContentSize();

        if (labels::pos == 0) {
            labels->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);
            labels->setPosition({screenLeft + size.width + 5, screenTop - size.height - 3});
        }
        else if (labels::pos == 1) {
            labels->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);
            labels->setPosition({screenLeft + size.width + 5, screenBottom + 3});
        }
        else if (labels::pos == 2) {
            labels->setAlignment(CCTextAlignment::kCCTextAlignmentRight);
            labels->setPosition({screenRight - 5, screenTop - size.height - 3});
        }
        else if (labels::pos == 3) {
            labels->setAlignment(CCTextAlignment::kCCTextAlignmentRight);
            labels->setPosition({screenRight - 5, screenBottom + 3});
        }
		
    }
}

void __fastcall hooks::PlayLayer_resetLevel_H(gd::PlayLayer *self)
{
    PlayLayer_resetLevel(self);

    sequence.do_some_magic();
    noclip_accuracy.handle_reset(self);
    cps_counter.reset();

    engine.handle_reseting(self);
    if (engine.mode == continue_record)
    {
        engine.continue_toggled = true;
        engine.mode = play;
        if (!self->m_isPracticeMode())
            reinterpret_cast<void(__thiscall *)(gd::PlayLayer *, bool practice)>(gd::base + 0x2ead30)(self, true);
    }
    
    if (hacks::show_hitboxes && self->m_pHitboxNode() != nullptr)
    {
        self->m_pHitboxNode()->clear();
    }

    int coinCount = min(coinsObjects.size(), 3);

    if (hacks::auto_pickup_coins)
    {
        for (int i = 0; i < coinCount; i++)
        {
            if (coinsObjects[i])
            {
                // pickupCoin
                reinterpret_cast<void(__thiscall *)(void *, void *)>(gd::base + 0x19D100)(self, coinsObjects[i]);
                // desctoyObject
                reinterpret_cast<void(__thiscall *)(void *, void *)>(gd::base + 0x19CF50)(self, coinsObjects[i]);
            }
        }
    }

    if (hacks::instant_complate)
        reinterpret_cast<void(__thiscall *)(gd::PlayLayer *, cocos2d::CCPoint pos)>(gd::base + 0x2dfe00)(self, {2, 2});
}

void __fastcall hooks::PlayLayer_destructor_H(void *self)
{
    PlayLayer_destructor(self);

    if (engine.mode == record || engine.mode == continue_record)
    {
        engine.mode = state::disable;
        engine.continue_toggled = false;
    }

    if (recorderAudio.is_recording && recorderAudio.showcase_mode) {
        recorderAudio.stop();
    }

    sequence.first_sqp = true;

    presence::UpdatePresence(Status::IN_MENU);
    StartPosObjects.clear();
    gamemodePortals.clear();
    mirrorPortals.clear();
    miniPortals.clear();
    dualPortals.clear();
    speedChanges.clear();
    gravityPortals.clear();
    coinsObjects.clear();
}

void __fastcall hooks::PlayLayer_destroyPlayer_H(gd::PlayLayer* self, int, gd::PlayerObject* player, gd::GameObject* obj) {
    PlayLayer_destroyPlayer(self, player, obj);
    noclip_accuracy.handle_death();
}

void __fastcall hooks::PlayLayer_levelCompleteHook(gd::PlayLayer *self)
{
    PlayLayer_levelComplete(self);
    if (engine.mode == state::record)
        engine.mode = state::disable;
}

void __fastcall hooks::PlayLayer_startMusicHook(gd::PlayLayer *self) {
    PlayLayer_startMusic(self);
    if (recorderAudio.enabled && recorderAudio.showcase_mode) {
        if (recorderAudio.is_recording) {
            recorderAudio.stop();
        }
        recorderAudio.start();
    }
}

void* __fastcall hooks::EndLevelLayer_createHook() {
    auto ret = EndLevelLayer_create();
    if (ret) {
        if (recorder.is_recording) {
            recorder.has_level_complete_menu = true;
        }

        if (recorderAudio.is_recording) {
            recorderAudio.has_level_complete_menu = true;
        }
    }
    return ret;
}

gd::PauseLayer* hooks::pause_menu;
gd::PauseLayer* __fastcall hooks::PauseLayer_createHook() {
    auto ret = PauseLayer_create();
    if (ret) {
        pause_menu = ret;
        if (hacks::hide_menu)
            ret->setVisible(false);
    }
    return ret;
}

void __fastcall hooks::PauseLayer_destructor_H(gd::PauseLayer* self, int, bool remove) {
    PauseLayer_destructor(self, remove);
    pause_menu = nullptr;
}

int __fastcall hooks::PlayLayer_updateVisibilityHook(int self, int, int a2) {
    if (hacks::disable_render) {
        return 0;
    }
    return PlayLayer_updateVisibility(self, a2);
}

int __fastcall hooks::GJBaseGameLayer_HandleButton_H(void *self, uintptr_t, int hold, int player_button, BOOL is_player1)
{
    if (hold) {
        cps_counter.recordClick();
    }

    if (engine.ignore_inputs) {
        return 0;
    }

    auto ret = GJBaseGameLayer_HandleButton(self, hold, player_button, is_player1);
    if (engine.mode == state::record)
    {
        auto pl = gd::GameManager::sharedState()->getPlayLayer();
        if (pl)
            engine.handle_recording2(hold, player_button, is_player1);
    }
    return ret;
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

bool __fastcall hooks::CustomSongWidget_initH(gd::CustomSongWidget *self, int, void *obj, void *delegate, bool a4, bool a5, bool a6, bool a7, bool a8, bool a9)
{
    auto ret = CustomSongWidget_init(self, obj, delegate, a4, a5, a6, a7, a8, a9);
    if (ret && hacks::auto_song_downloader && self->m_downloadBtn()->isVisible())
        self->onDownload();
    return ret;
}

bool __fastcall hooks::LevelInfoLayer_initH(cocos2d::CCLayer *self, int, gd::GJGameLevel *level, bool challenge)
{
    auto ret = LevelInfoLayer_init(self, level, challenge);
    if (ret && hacks::auto_ldm)
        level->m_lowDetail() = true;
    return ret;
}

bool __fastcall hooks::MenuLayer_initH(cocos2d::CCLayer *self, int)
{
    auto ret = MenuLayer_init(self);
    if (ret && hacks::discord_rpc)
    {
        presence::InitPresence();
        presence::UpdatePresence(Status::IN_MENU);
    }
    return ret;
}

bool __fastcall hooks::LevelEditorLayer_initH(void *self, void *, gd::GJGameLevel *m_lvl, bool unk)
{
    auto ret = LevelEditorLayer_init(self, m_lvl, unk);
    if (ret && hacks::discord_rpc)
        presence::UpdatePresence(Status::IN_EDITOR, m_lvl);
    return ret;
}

void __fastcall hooks::EditorPauseLayer_onExitEditorH(cocos2d::CCObject *self, void *edx)
{
    if (hacks::discord_rpc)
        presence::UpdatePresence(Status::IN_MENU);
    EditorPauseLayer_onExitEditor(self);
}

void* CreateTransition(float duration, void* scene, const char* functionName)
{
    return reinterpret_cast<void*(__cdecl*)(float, void*)>(
        GetProcAddress(gd::cocos_base, functionName))(duration, scene);
}

void *(__cdecl *CCTransitionFade_create)(float, void *);
void *__cdecl CCTransitionFade_createH(float duration, void *scene)
{
    const char* transitionFunctions[] = {
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

    if (hacks::transition_enabled && hacks::transition_index >= 1 && hacks::transition_index <= sizeof(transitionFunctions) / sizeof(transitionFunctions[0]))
    {
        return CreateTransition(hacks::transition_duration, scene, transitionFunctions[hacks::transition_index - 1]);
    }
    else
    {
        return CCTransitionFade_create(hacks::transition_enabled ? hacks::transition_duration : duration, scene);
    }
}

void *(__thiscall *dispatchKeyboardMSG)(void *self, int key, bool down, bool idk);
void *__fastcall dispatchKeyboardMSGHook(void *self, void *, int key, bool down, bool idk)
{
    if (!idk && down && startPosSwitcher::enabled)
    {
        startPosSwitcher::handleKeyPress(key);
    }

    auto pl = gd::GameManager::sharedState()->getPlayLayer();

    if (pl && down && key == 'C') {
        engine.frame_advance = true;
        next_frame = true;
    }        
    else if (pl && down && key == 'V') {
        engine.frame_advance = false;
        next_frame = false;
    }
    else if (down && key == 'P')
    {
        engine.mode = (engine.mode == state::play) ? state::disable : state::play;
    }

    return dispatchKeyboardMSG(self, key, down, idk);
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

        if (pl == nullptr || from<bool>(pl, 0x2c28) || from<bool>(pl, 0x2eb1) || StartPosObjects.empty())
        {
            return;
        }

        if (direction)
        {
            startposIndex = startposIndex - 1;
            if (startposIndex < -1)
            {
                startposIndex = StartPosObjects.size() - 1;
            }
        }
        else
        {
            startposIndex = startposIndex + 1;
            if (static_cast<unsigned int>(startposIndex) >= StartPosObjects.size())
            {
                startposIndex = -1;
            }
        }

        from<int>(pl, 0x2e14) = 0;

        if (startposIndex >= 0)
        {
            pl->setStartPosObject((uint32_t *)StartPosObjects[startposIndex]);
        }
        else
        {
            pl->setStartPosObject(nullptr);
        }

        Console::WriteF("Startpos Switcher (%i/%i)\n", startposIndex + 1, StartPosObjects.size());

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
        MH_CreateHook((void *)(gd::base + 0x13b890), GameObject_setVisibleH, (void **)&GameObject_setVisible);
        MH_CreateHook((void *)(gd::base + 0x13b490), GameObject_setOpacityH, (void **)&GameObject_setOpacity);
        MH_CreateHook((void *)(gd::base + 0x141300), GameObject_setObjectColorH, (void **)&GameObject_setObjectColor);
        MH_CreateHook((void *)(gd::base + 0x1415f0), GameObject_setGlowColorH, (void **)&GameObject_setGlowColor);
        MH_CreateHook((void *)(gd::base + 0x141630), GameObject_setChildColorH, (void **)&GameObject_setChildColor);
        MH_CreateHook((void *)(gd::base + 0x39bc20), AnimatedGameObject_setObjectColorH, (void **)&AnimatedGameObject_setObjectColor);
        MH_CreateHook((void *)(gd::base + 0x39b590), AnimatedGameObject_updateChildSpriteColorH, (void **)&AnimatedGameObject_updateChildSpriteColor);
        MH_CreateHook((void *)(gd::base + 0x39ccf0), EffectGameObject_triggerObjectHook, (void **)&EffectGameObject_triggerObject);
        // MH_CreateHook((void*)(gd::base + 0x2e4e80), PlayLayer_loadDefaultColorsH, (void**)&PlayLayer_loadDefaultColors);
        MH_CreateHook((void *)(gd::base + 0x194490), GJBaseGameLayer_updateLevelColors_H, (void **)&GJBaseGameLayer_updateLevelColors);
        // MH_CreateHook((void*)(gd::base + 0x1f7ab0), updateGround01ColorH, (void**)&updateGround01Color);
    }

    void set_enabled(bool enable)
    {
        enabled = enable;
        if (enable)
        {
            memory::WriteBytes(gd::base + 0x13EFA1, "90 90 90 90 90 90");
            memory::WriteBytes(gd::base + 0x2E4ED8, "90 90 EB 3A");
            memory::WriteBytes(gd::base + 0x2E4F41, "90 90 EB 5C");
        }
        else
        {
            memory::WriteBytes(gd::base + 0x13EFA1, "88 87 62 04 00 00");
            memory::WriteBytes(gd::base + 0x2E4ED8, "85 C0 74 3A");
            memory::WriteBytes(gd::base + 0x2E4F41, "85 C0 74 5C");
        }
    }
}

namespace hitboxes_mode
{
    void init()
    {
        MH_CreateHook((void *)(gd::base + 0x2E7220), postUpdate_H, (void **)&postUpdate);
        MH_CreateHook(GetProcAddress(gd::cocos_base, "?drawPolygon@CCDrawNode@cocos2d@@QAE_NPAVCCPoint@2@IABU_ccColor4F@2@M1@Z"), drawPolygon_H, reinterpret_cast<void **>(&drawPolygon));
        MH_CreateHook(GetProcAddress(gd::cocos_base, "?drawCircle@CCDrawNode@cocos2d@@QAE_NABVCCPoint@2@MABU_ccColor4F@2@M1I@Z"), drawCircle_H, reinterpret_cast<void **>(&drawCircle));
    }

    void enabled_hitbox(bool enabled)
    {
        if (enabled)
        {
            memory::WriteBytes(gd::base + 0x2DCBD4, "90 90 90 90 90 90 90");
            memory::WriteBytes(gd::base + 0x2E38F1, "90 90");
            memory::WriteBytes(gd::base + 0x2E38FA, "90 90");
        }
        else
        {
            memory::WriteBytes(gd::base + 0x2DCBD4, "80 BB 7C 2A 00 00 00");
            memory::WriteBytes(gd::base + 0x2E38F1, "74 13");
            memory::WriteBytes(gd::base + 0x2E38FA, "74 0A");
        }
    }

    void enabled_draw_trail(bool enabled)
    {
        if (enabled)
        {
            memory::WriteBytes(gd::base + 0x19873C, "90 90 90 90 90 90");
        }
        else
        {
            // memory::WriteBytes(gd::base + 0x19873C, "FF 15 C8 4A DC 00");
        }
    }

    bool __fastcall drawPolygon_H(CCDrawNode *self, void *, cocos2d::CCPoint *position, unsigned int count, _ccColor4F const &fillColor, float borderWidth, _ccColor4F const &borderColor)
    {
        ccColor4F colBorder = borderColor;
        ccColor4F colFill = fillColor;

        auto pl = gd::GameManager::sharedState()->getPlayLayer();
        if (pl && hacks::show_hitboxes)
        {
            colFill.a = hacks::fill_alpha / 255.f;
            colBorder.a = hacks::border_alpha / 255.f;
            borderWidth = hacks::hitbox_size;

            if (hacks::show_on_death && !pl->m_isDead())
            {
                borderWidth = 0;
            }
        }

        bool draw = drawPolygon(self, position, count, colFill, borderWidth, colBorder);
        return draw;
    }

    bool __fastcall drawCircle_H(CCDrawNode *self, void *, cocos2d::CCPoint *position, float radius, _ccColor4F const &fillColor, float borderWidth, _ccColor4F const &borderColor, unsigned int segments)
    {
        ccColor4F colBorder = borderColor;
        ccColor4F colFill = fillColor;

        auto pl = gd::GameManager::sharedState()->getPlayLayer();
        if (pl && hacks::show_hitboxes)
        {
            colFill.a = hacks::circle_fill_alpha / 255.f;
            colBorder.a = hacks::circle_border_alpha / 255.f;
            radius = (hacks::default_hitbox_size) ? radius : hacks::circle_hitbox_size;
        }

        bool draw = drawCircle(self, position, radius, colFill, borderWidth, colBorder, segments);
        return draw;
    }

    void __fastcall postUpdate_H(gd::PlayLayer *self, int edx, float dt)
    {
        postUpdate(self, dt);
        if (hacks::show_hitboxes && self->m_pHitboxNode() != nullptr)
            self->m_pHitboxNode()->setVisible(true);
        else
            self->m_pHitboxNode()->setVisible(false);
        if (self->m_flipped())
            self->updateDebugDraw();
    }
}

void hooks::init()
{
    MH_CreateHook((void *)(gd::base + 0x2dc4a0), PlayLayer_initH, (void **)&PlayLayer_init);
    MH_CreateHook((void *)(gd::base + 0x1bb780), PlayLayer_update_H, (void **)&PlayLayer_update);
    MH_CreateHook((void *)(gd::base + 0x2ea130), PlayLayer_resetLevel_H, (void **)&PlayLayer_resetLevel);
    MH_CreateHook((void *)(gd::base + 0x2dc080), PlayLayer_destructor_H, (void **)&PlayLayer_destructor);
    MH_CreateHook((void *)(gd::base + 0x2ddb60), PlayLayer_levelCompleteHook, (void **)&PlayLayer_levelComplete);
    MH_CreateHook((void *)(gd::base + 0x2e6730), PlayLayer_destroyPlayer_H, (void **)&PlayLayer_destroyPlayer);
    MH_CreateHook((void *)(gd::base + 0x2eb3a0), PlayLayer_startMusicHook, (void **)&PlayLayer_startMusic);
    MH_CreateHook((void *)(gd::base + 0xe7380), EndLevelLayer_createHook, (void **)&EndLevelLayer_create);
    MH_CreateHook((void *)(gd::base + 0x2b8940), PauseLayer_createHook, (void **)&PauseLayer_create);
    MH_CreateHook((void *)(gd::base + 0x2B88B0), PauseLayer_destructor_H, (void **)&PauseLayer_destructor);
    MH_CreateHook((void *)(gd::base + 0x2e2bf0), PlayLayer_updateVisibilityHook, (void **)&PlayLayer_updateVisibility);
    MH_CreateHook((void *)(gd::base + 0x1b69f0), GJBaseGameLayer_HandleButton_H, (void **)&GJBaseGameLayer_HandleButton);
    MH_CreateHook((void *)(gd::base + 0x176570), GameStatsManager_isItemUnlockedH, (void **)&GameStatsManager_isItemUnlocked);
    MH_CreateHook((void *)(gd::base + 0x2e19b0), PlayLayer_addObjectH, (void **)&PlayLayer_addObject);
    MH_CreateHook((void *)(gd::base + 0x92c20), CustomSongWidget_initH, (void **)&CustomSongWidget_init);
    MH_CreateHook((void *)(gd::base + 0x2516a0), LevelInfoLayer_initH, (void **)&LevelInfoLayer_init);
    MH_CreateHook((void *)(gd::base + 0x27b450), MenuLayer_initH, (void **)&MenuLayer_init);
    MH_CreateHook((void *)(gd::base + 0x239a70), LevelEditorLayer_initH, (void **)&LevelEditorLayer_init);
    MH_CreateHook((void *)(gd::base + 0xa2ef0), EditorPauseLayer_onExitEditorH, (void **)&EditorPauseLayer_onExitEditor);
    MH_CreateHook(GetProcAddress(gd::cocos_base, "?dispatchKeyboardMSG@CCKeyboardDispatcher@cocos2d@@QAE_NW4enumKeyCodes@2@_N1@Z"), dispatchKeyboardMSGHook, (void **)&dispatchKeyboardMSG);
    MH_CreateHook(GetProcAddress(gd::cocos_base, "?update@CCScheduler@cocos2d@@UAEXM@Z"), CCScheduler_update_H, (void **)&CCScheduler_update);
    MH_CreateHook(GetProcAddress(gd::cocos_base, "?create@CCTransitionFade@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"), CCTransitionFade_createH, (void **)&CCTransitionFade_create);
    layout_mode::init();
    hitboxes_mode::init();
}
