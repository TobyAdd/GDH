#include "hooks.hpp"
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GameStatsManager.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/CCTextInputNode.hpp>
#include <Geode/modify/LevelPage.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/SliderTouchLogic.hpp>
#include <Geode/modify/GJScaleControl.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/UILayer.hpp>
#include <Geode/modify/CCTransitionFade.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/LevelTools.hpp>
#include <Geode/modify/CCParticleSystemQuad.hpp>
#include <Geode/modify/OptionsLayer.hpp>
#include <Geode/modify/CameraTriggerGameObject.hpp>
#include <Geode/modify/ShaderLayer.hpp>
#include <Geode/modify/GameToolbox.hpp>
#include <Geode/modify/CCMotionStreak.hpp>
#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/SongSelectNode.hpp>
#include <Geode/modify/MoreSearchLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/AchievementNotifier.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/modify/CCDrawNode.hpp>
#include <Geode/modify/CCCircleWave.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/SecretLayer2.hpp>
#include "hacks.hpp"
#include "config.hpp"
#include "labels.hpp"
#include "replayEngine.hpp"
#include "recorder.hpp"
#include "gui.hpp"
#include "practiceFix.hpp"

std::vector<GameObject*> dualPortals, gamemodePortals, miniPortals, speedChanges, mirrorPortals;

std::vector<StartPosObject*> startPositions;
int selectedStartpos = -1;

std::deque<cocos2d::CCRect> playerTrail1, playerTrail2;

void hooksH::switchStartPos(int incBy, bool direction) {
    auto &config = Config::get();
    auto pl = PlayLayer::get();

    if (!pl || startPositions.empty()) return;

    selectedStartpos += incBy;

    if (selectedStartpos < -1)
        selectedStartpos = startPositions.size() - 1;

    if (selectedStartpos >= startPositions.size())
        selectedStartpos = -1;

    if (direction) {
        StartPosObject* obj = selectedStartpos == -1 ? nullptr : startPositions[selectedStartpos];
        
        pl->m_currentCheckpoint = nullptr;
        pl->setStartPosObject(obj);
        pl->resetLevel();

        if (config.get<bool>("startos_switcher::reset_camera", true))
            pl->resetCamera();

        pl->startMusic();

        return;
    }
}

void setupStartPos(StartPosObject* startPos) {
    LevelSettingsObject* startPosSettings = startPos->m_startSettings;
    LevelSettingsObject* levelSettings = PlayLayer::get()->m_levelSettings;

    startPosSettings->m_startDual = levelSettings->m_startDual;
    startPosSettings->m_startMode = levelSettings->m_startMode;
    startPosSettings->m_startMini = levelSettings->m_startMini;
    startPosSettings->m_startSpeed = levelSettings->m_startSpeed;

    auto getClosestObject = [](std::vector<GameObject*>& vec, StartPosObject* startPos) -> GameObject* {
        GameObject* closest = nullptr;

        std::sort(vec.begin(), vec.end(), [](GameObject* a, GameObject* b) {
            return a->getPositionX() < b->getPositionX();
        });

        for (auto obj : vec) {
            if (obj->getPositionX() - 10 > startPos->getPositionX())
                break;
            if (obj->getPositionX() - 10 < startPos->getPositionX())
                closest = obj;
        }

        return closest;
    };

    GameObject* obj = getClosestObject(dualPortals, startPos);
    if (obj) {
        startPosSettings->m_startDual = (obj->m_objectID == 286);
    }        

    obj = getClosestObject(gamemodePortals, startPos);
    if (obj) {
        switch (obj->m_objectID) {
            case 12:  startPosSettings->m_startMode = 0; break;
            case 13:  startPosSettings->m_startMode = 1; break;
            case 47:  startPosSettings->m_startMode = 2; break;
            case 111: startPosSettings->m_startMode = 3; break;
            case 660: startPosSettings->m_startMode = 4; break;
            case 745: startPosSettings->m_startMode = 5; break;
            case 1331: startPosSettings->m_startMode = 6; break;
            case 1933: startPosSettings->m_startMode = 7; break;
        }
    }

    obj = getClosestObject(miniPortals, startPos);
    if (obj) {
        startPosSettings->m_startMini = (obj->m_objectID == 101);
    }        

    obj = getClosestObject(speedChanges, startPos);
    if (obj) {
        switch (obj->m_objectID) {
            case 200:   startPosSettings->m_startSpeed = Speed::Slow; break;
            case 201:   startPosSettings->m_startSpeed = Speed::Normal; break;
            case 202:   startPosSettings->m_startSpeed = Speed::Fast; break;
            case 203:   startPosSettings->m_startSpeed = Speed::Faster; break;
            case 1334:  startPosSettings->m_startSpeed = Speed::Fastest; break;
        }
    }
}


float color_dt = 0.f;
float left_over = 0.f;

class $modify(MyCCScheduler, cocos2d::CCScheduler) {
    void update(float dt) {
        auto &config = Config::get();
        auto &engine = ReplayEngine::get();
        auto& recorder = Recorder::get();
        auto& recorderAudio = RecorderAudio::get();

        auto handleRestoreWinSize = [&]() {
            if (recorder.is_recording || recorder.needRevertOld) {
                if (recorder.needRevertOld) {
                    recorder.needRevertOld = false;
                }
                recorder.restoreWinSize();
            }
        };

        if (config.get<bool>("speedhack_enabled", false))
            dt *= config.get<float>("speedhack_value", 1.f);

        if (engine.engine_v2 && recorder.is_recording) {
            if (recorder.is_recording) recorder.applyWinSize();
            CCScheduler::update(1.f / static_cast<float>(recorder.fps));
            handleRestoreWinSize();
            return;
        }

        if (!config.get<bool>("tps_enabled", false))
            return CCScheduler::update(dt);

        float tps_value = config.get<float>("tps_value", 60.f);
        float new_dt = 1.f / tps_value;

        auto pl = PlayLayer::get();
        if (engine.frame_advance && pl && !pl->m_isPaused) {
            if (engine.next_frame) {
                engine.next_frame = false;
                if (recorder.is_recording) recorder.applyWinSize();
                CCScheduler::update(new_dt);
                handleRestoreWinSize();
            }

            return;
        }

        if (!config.get<bool>("tps::real_time", true)) {
            if (recorder.is_recording) recorder.applyWinSize();
            CCScheduler::update(new_dt);
            handleRestoreWinSize();
            return;
        }             

        unsigned times = static_cast<int>((dt + left_over) / new_dt);  
        if (dt == 0.f)
            return CCScheduler::update(new_dt);

        auto start = std::chrono::high_resolution_clock::now();
        using namespace std::literals;

        for (unsigned i = 0; i < times; ++i) {
            if (recorder.is_recording) recorder.applyWinSize();
            CCScheduler::update(new_dt);
            handleRestoreWinSize();

            if (std::chrono::high_resolution_clock::now() - start > 33.333ms) {         
                times = i + 1;
                break;
            }
        }

        left_over = std::max(0.f, left_over + dt - new_dt * times);
    }
};


class $modify(MyFMODAudioEngine, FMODAudioEngine) {
    void update(float delta) {
        auto &config = Config::get();

        FMODAudioEngine::update(delta);
        
        FMOD::ChannelGroup* group;
        float speed = config.get<bool>("speedhackAudio_enabled", false) ? config.get<float>("speedhack_value", 1.f) : 1.f;
        if (m_system->getMasterChannelGroup(&group) == FMOD_OK) {
            group->setPitch(speed);
        }
    }
};


class $modify(MyPlayLayer, PlayLayer) {
    struct Fields {
        cocos2d::CCLabelBMFont* labels_top_left;
        cocos2d::CCLabelBMFont* labels_top_right;
        cocos2d::CCLabelBMFont* labels_top;
        cocos2d::CCLabelBMFont* labels_bottom_left;
        cocos2d::CCLabelBMFont* labels_bottom_right;
        cocos2d::CCLabelBMFont* labels_bottom;
        
        std::vector<GameObject*> coinsObjects;
        cocos2d::CCMenu* startposSwitcherUI;

        cocos2d::CCSprite* tint_death_bg;

        std::vector<checkpoint_data> checkpoints_p1;
        std::vector<checkpoint_data> checkpoints_p2;
        
        ~Fields() {
            startPositions.clear();
            dualPortals.clear();
            gamemodePortals.clear();
            miniPortals.clear();
            speedChanges.clear();
            mirrorPortals.clear();

            selectedStartpos = -1;
            playerTrail1.clear();
            playerTrail2.clear();
            color_dt = 0.f;
        }
    };

    #ifdef GEODE_IS_ANDROID
    void createObjectsFromSetupFinished() {
	    PlayLayer::createObjectsFromSetupFinished();
        auto& gui = Gui::get();
        auto& config = Config::get();

        if (config.get<bool>("startpos_switcher", false) && !startPositions.empty()) {
            auto win_size = cocos2d::CCDirector::sharedDirector()->getWinSize();

            auto label = cocos2d::CCLabelBMFont::create(fmt::format("{}/{}", selectedStartpos+1, startPositions.size()).c_str(), "bigFont.fnt");
            label->setScale(0.5f);
            label->setPosition(win_size.width/2, 20.f);
            label->setOpacity(100);
            label->setID("startposSwitcherLabels"_spr);

            auto left_arrow = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
            left_arrow->setScale(0.5f);
            auto left_arrowClick = geode::cocos::CCMenuItemExt::createSpriteExtra(left_arrow, [this, label](CCMenuItemSpriteExtra* sender) {
                hooksH::switchStartPos(-1);
                label->setCString(fmt::format("{}/{}", selectedStartpos+1, startPositions.size()).c_str());
            });
            left_arrowClick->setPosition(win_size.width/2 - 50, cocos2d::CCDirector::sharedDirector()->getScreenBottom() + left_arrowClick->getScaledContentHeight());
            left_arrowClick->setOpacity(100);
            left_arrowClick->setID("startposSwitcherLeftArrowClick"_spr);

            auto right_arrow = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");  
            right_arrow->setScale(0.5f);
            right_arrow->setFlipX(true);
            auto right_arrowClick = geode::cocos::CCMenuItemExt::createSpriteExtra(right_arrow, [this, label](CCMenuItemSpriteExtra* sender) {
                hooksH::switchStartPos(1);
                label->setCString(fmt::format("{}/{}", selectedStartpos+1, startPositions.size()).c_str());
            });       
            right_arrowClick->setPosition(win_size.width/2 + 50, cocos2d::CCDirector::sharedDirector()->getScreenBottom() + right_arrowClick->getScaledContentHeight());
            right_arrowClick->setOpacity(100);
            right_arrowClick->setID("startpos_switcher_rightArrowClick"_spr);


            m_fields->startposSwitcherUI = cocos2d::CCMenu::create();
            m_fields->startposSwitcherUI->setID("startposSwitcherUI"_spr);
            m_fields->startposSwitcherUI->setPosition(0, 0);
            m_fields->startposSwitcherUI->setZOrder(999);

            m_fields->startposSwitcherUI->addChild(left_arrowClick);
            m_fields->startposSwitcherUI->addChild(right_arrowClick);
            m_fields->startposSwitcherUI->addChild(label);

            m_uiLayer->addChild(m_fields->startposSwitcherUI);
        }
    }
    #endif

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        auto& config = Config::get();
        auto& engine = ReplayEngine::get();
        auto& recorder = Recorder::get();
        auto& recorderAudio = RecorderAudio::get();

        auto wnd_size = cocos2d::CCDirector::sharedDirector()->getWinSize();
        m_fields->tint_death_bg = cocos2d::CCSprite::create("game_bg_13_001.png");
        auto sprSize = m_fields->tint_death_bg->getContentSize();
        m_fields->tint_death_bg->setPosition({wnd_size.width/2, wnd_size.height/2});            
        m_fields->tint_death_bg->setScaleX(wnd_size.width / sprSize.width * 2.f);
        m_fields->tint_death_bg->setScaleY(wnd_size.height / sprSize.height * 2.f);
        m_fields->tint_death_bg->setColor({255, 0, 0});
        m_fields->tint_death_bg->setOpacity(0);
        m_fields->tint_death_bg->setZOrder(999);
        addChild(m_fields->tint_death_bg);

        if (config.get<bool>("auto_practice_mode", false))
            togglePracticeMode(true);

        if (engine.mode == state::disable)
            engine.replay_name = fmt::format("{}", level->m_levelName);

        if (!recorder.is_recording)
            recorder.video_name = fmt::format("{}.mp4", level->m_levelName);

        if (!recorderAudio.is_recording)
            recorderAudio.audio_name = fmt::format("{}.wav", level->m_levelName);
        
        m_fields->labels_top_left     = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
        m_fields->labels_top_right    = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
        m_fields->labels_bottom_left  = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
        m_fields->labels_bottom_right = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
        m_fields->labels_top          = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
        m_fields->labels_bottom       = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
        m_fields->labels_top_left->    setID("labels_top_left"_spr);
        m_fields->labels_top_right->   setID("labels_top_right"_spr);
        m_fields->labels_bottom_left-> setID("labels_bottom_left"_spr);
        m_fields->labels_bottom_right->setID("labels_bottom_right"_spr);
        m_fields->labels_top->         setID("labels_top"_spr);
        m_fields->labels_bottom->      setID("labels_bottom"_spr);

        for (size_t i = 0; i < 6; i++) {
            cocos2d::CCLabelBMFont* label_object =
                i == 0 ? m_fields->labels_top_left :
                i == 1 ? m_fields->labels_top_right :
                i == 2 ? m_fields->labels_bottom_left :
                i == 3 ? m_fields->labels_bottom_right :
                i == 4 ? m_fields->labels_bottom :
                m_fields->labels_top;
            label_object->setZOrder(999);
            if (i == 0) label_object->setAnchorPoint({0, 1});
            if (i == 1) label_object->setAnchorPoint({1, 1});
            if (i == 2) label_object->setAnchorPoint({0, 0});
            if (i == 3) label_object->setAnchorPoint({1, 0});
            if (i == 4) label_object->setAnchorPoint({0.5f, 0});
            if (i == 5) label_object->setAnchorPoint({0.5f, 1});
        }
        
        addChild(m_fields->labels_top_left);
        addChild(m_fields->labels_top_right);
        addChild(m_fields->labels_bottom_left);
        addChild(m_fields->labels_bottom_right);
        addChild(m_fields->labels_bottom);
        addChild(m_fields->labels_top);

        Labels::get().attempts = 1;
        Labels::get().session_time = 0.f;

        return true;
    }

    void togglePracticeMode(bool practiceMode) {
        if (m_fields->startposSwitcherUI) {
            m_fields->startposSwitcherUI->setVisible(!practiceMode);
        }
        PlayLayer::togglePracticeMode(practiceMode);
    }
        
    void postUpdate(float dt) {
        auto& config = Config::get();
        auto& labels = Labels::get();

        PlayLayer::postUpdate(dt);

        if (!(m_fields->labels_top_left &&
              m_fields->labels_top_right &&
              m_fields->labels_bottom_left &&
              m_fields->labels_bottom_right &&
              m_fields->labels_bottom &&
              m_fields->labels_top)) return;

            
        for (size_t i = 0; i < 6; i++) {
            cocos2d::CCLabelBMFont* label_object =
                i == 0 ? m_fields->labels_top_left :
                i == 1 ? m_fields->labels_top_right :
                i == 2 ? m_fields->labels_bottom_left :
                i == 3 ? m_fields->labels_bottom_right :
                i == 4 ? m_fields->labels_bottom :
                m_fields->labels_top;
            label_object->setScale(labels.size);
            label_object->setOpacity((int)(labels.opacity*255));
        }
        
        auto size = cocos2d::CCDirector::sharedDirector()->getWinSize();
        
        m_fields->labels_top_left->    setCString(labels.get_label_string(LabelCorner_TopLeft).c_str());
        m_fields->labels_top_right->   setCString(labels.get_label_string(LabelCorner_TopRight).c_str());
        m_fields->labels_bottom_left-> setCString(labels.get_label_string(LabelCorner_BottomLeft).c_str());
        m_fields->labels_bottom_right->setCString(labels.get_label_string(LabelCorner_BottomRight).c_str());
        m_fields->labels_bottom->      setCString(labels.get_label_string(LabelCorner_Bottom).c_str());
        m_fields->labels_top->         setCString(labels.get_label_string(LabelCorner_Top).c_str());
        
        m_fields->labels_top_left->    setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentLeft);
        m_fields->labels_top_left->    setPosition({labels.padding, size.height - labels.padding});
        
        m_fields->labels_bottom_left-> setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentLeft);
        m_fields->labels_bottom_left-> setPosition({labels.padding, labels.padding});
        
        m_fields->labels_top_right->   setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentRight);
        m_fields->labels_top_right->   setPosition({size.width - labels.padding, size.height - labels.padding});
        
        m_fields->labels_bottom_right->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentRight);
        m_fields->labels_bottom_right->setPosition({size.width - labels.padding, labels.padding});
        
        m_fields->labels_top->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentCenter);
        m_fields->labels_top->setPosition({size.width/2, size.height - labels.padding});
        
        m_fields->labels_bottom->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentCenter);
        m_fields->labels_bottom->setPosition({size.width/2, labels.padding});
    }

    void addObject(GameObject* obj) {
        auto& config = Config::get();
        
        if (config.get<bool>("no_glow", false)) {
            obj->m_hasNoGlow = true;
        }

        PlayLayer::addObject(obj);

        switch (obj->m_objectID) {
            case 1329:
            case 142:
                m_fields->coinsObjects.push_back(obj);
                break;

            case 31:
                startPositions.push_back(static_cast<StartPosObject*>(obj));
                break;

            case 12:
            case 13:
            case 47:
            case 111:
            case 660:
            case 745:
            case 1331:
            case 1933:
                gamemodePortals.push_back(static_cast<StartPosObject*>(obj));
                break;

            case 45:
            case 46:
                mirrorPortals.push_back(static_cast<StartPosObject*>(obj));
                break;

            case 99:
            case 101:
                miniPortals.push_back(static_cast<StartPosObject*>(obj));
                break;

            case 286:
            case 287:
                dualPortals.push_back(static_cast<StartPosObject*>(obj));
                break;

            case 200:
            case 201:
            case 202:
            case 203:
            case 1334:
                speedChanges.push_back(static_cast<StartPosObject*>(obj));
                break;

            default:
                break;
        }
    }
    
    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        auto& config = Config::get();

        bool testmode = m_isTestMode;
        
        if (m_anticheatSpike && obj == m_anticheatSpike)
            return PlayLayer::destroyPlayer(player, obj);

        bool shouldDestroy = config.get<bool>("noclip", false) ? 
            ((player == m_player1 && !config.get<bool>("noclip::p1", true)) || 
            (player == m_player2 && !config.get<bool>("noclip::p2", true))) 
            : true;

        if (shouldDestroy) {
            if (Config::get().get<bool>("safe_mode", false)) {
                m_isTestMode = true;
            }
            PlayLayer::destroyPlayer(player, obj);
            m_isTestMode = testmode;
        }

        if (config.get<bool>("noclip::tint_on_death", false)) {
            m_fields->tint_death_bg->stopAllActions();
            m_fields->tint_death_bg->setOpacity(config.get<int>("noclip::tint_opacity", 100));
            m_fields->tint_death_bg->runAction(cocos2d::CCFadeTo::create(0.35f, 0));
        }

        NoclipAccuracy::get().handle_death();

        if (config.get<bool>("respawn_time", false)) {
            if (auto* respawnSequence = getActionByTag(0x10)) {
                stopAction(respawnSequence);
                auto* newSequence = cocos2d::CCSequence::create(
                    cocos2d::CCDelayTime::create(config.get<float>("respawn_time_value", 1.f)),
                    cocos2d::CCCallFunc::create(this, callfunc_selector(PlayLayer::delayedResetLevel)),
                    nullptr
                );
                newSequence->setTag(0x10);
                runAction(newSequence);
            }
        }
    }
    
    void resetLevel() {
        auto& config = Config::get();
        auto& engine = ReplayEngine::get();
        auto& recorder = Recorder::get();
        auto& recorderAudio = RecorderAudio::get();

        // auto unk3188 = m_unk3188;
        // auto replayRandSeed = m_replayRandSeed;

        // if (config.get<bool>("random_seed", false)) {
        //     m_unk3188 = true;
        //     m_replayRandSeed = config.get<int>("random_seed_value", 1337);
        // }

        PlayLayer::resetLevel();

        // m_unk3188 = unk3188;
        // m_replayRandSeed = replayRandSeed;

        left_over = 0;

        if (config.get<bool>("practice_fix", false)) {
            if (m_isPracticeMode && !m_fields->checkpoints_p1.empty() && !m_fields->checkpoints_p2.empty()) {
                apply_checkpoint(m_player1, m_fields->checkpoints_p1.back());
                apply_checkpoint(m_player2, m_fields->checkpoints_p2.back());
            }
            else if (!m_isPracticeMode) {
                m_fields->checkpoints_p1.clear();
                m_fields->checkpoints_p2.clear();
            }
        }

        if (config.get<bool>("smart_startpos", false)) {
            for (StartPosObject* obj : startPositions)
                setupStartPos(obj);
        }

        SpamBot::get().reset_temp();
        engine.handle_reset();  

        if (!m_isPracticeMode && !startPositions.empty()) {
            recorder.delay = m_gameState.m_levelTime;
        }
        else if (startPositions.empty()) {
            recorder.delay = 0;
        }      

        if (config.get<bool>("no_do_not_flip", false) && m_attemptLabel)
            m_attemptLabel->setScaleY(1);

        if (config.get<bool>("show_total_attempts", false) && m_attemptLabel)
            m_attemptLabel->setCString(fmt::format("Attempt {}", static_cast<int>(m_level->m_attempts)).c_str());

        if (config.get<bool>("safe_mode", false))
            m_level->m_attempts = m_level->m_attempts - 1;

        if (config.get<bool>("auto_pickup_coins", false)) {
            for (auto* coin : m_fields->coinsObjects) {
                if (!coin) continue;
                destroyObject(coin);
                pickupItem(static_cast<EffectGameObject*>(coin));
            }
        }

        if (config.get<bool>("instant_complete", false)) {
            //bypass anticheat
            m_timePlayed = 10.0;
            m_bestAttemptTime = 10.0;

            PlayLayer::playEndAnimationToPos({0, 0});
        }

        playerTrail1.clear();
        playerTrail2.clear();

        CpsCounter::get().reset();
        NoclipAccuracy::get().handle_reset(this);
    }

    void levelComplete() {
        auto& config = Config::get();
        bool testmode = m_isTestMode;

        if (config.get<bool>("safe_mode", false))
            m_isTestMode = true;

        PlayLayer::levelComplete();

        m_isTestMode = testmode;
    }

    void playEndAnimationToPos(cocos2d::CCPoint pos) {
        auto& engine = ReplayEngine::get();
        PlayLayer::playEndAnimationToPos(pos);
        if (engine.mode == state::record)
            engine.mode = state::disable;
    }

    void playPlatformerEndAnimationToPos(cocos2d::CCPoint pos, bool idk) {
        auto& engine = ReplayEngine::get();
        PlayLayer::playPlatformerEndAnimationToPos(pos, idk);
        if (engine.mode == state::record)
            engine.mode = state::disable;
    }

    void showNewBest(bool p0, int p1, int p2, bool p3, bool p4, bool p5) {
        if (Config::get().get<bool>("no_new_best_popup", false)) return;        
        PlayLayer::showNewBest(p0, p1, p2, p3, p4, p5);
    }
    
    void updateAttempts() {
        PlayLayer::updateAttempts();
        Labels::get().attempts++;
    }

    void updateAttemptTime(float time) {
        PlayLayer::updateAttemptTime(time);
        float speed = Config::get().get<float>("speedhack_value", 1.f);

        if (speed != 0.f)
            Labels::get().session_time += time / speed;
    }

    void storeCheckpoint(CheckpointObject* obj) {
        auto& config = Config::get();

        if (!config.get("checkpoint_limit", false)) {
            PlayLayer::storeCheckpoint(obj);

            if (config.get<bool>("practice_fix", false)) {
                if (m_fields->checkpoints_p1.size() >= 50) 
                    m_fields->checkpoints_p1.erase(m_fields->checkpoints_p1.begin()); 

                m_fields->checkpoints_p1.push_back(create_checkpoint(m_player1));

                if (m_fields->checkpoints_p2.size() >= 50) 
                    m_fields->checkpoints_p2.erase(m_fields->checkpoints_p2.begin());
                
                m_fields->checkpoints_p2.push_back(create_checkpoint(m_player2));
            }
            return;
        }
        
        m_checkpointArray->addObject(obj);
        PlayLayer::addToSection(obj->m_physicalCheckpointObject);

        if (config.get<bool>("practice_fix", false)) {
            m_fields->checkpoints_p1.push_back(create_checkpoint(m_player1));
            m_fields->checkpoints_p2.push_back(create_checkpoint(m_player2));
        }
    }

    void removeCheckpoint(bool p0) {
        auto& config = Config::get();
        
        PlayLayer::removeCheckpoint(p0);
        if (config.get<bool>("practice_fix", false)) {
            if (!m_fields->checkpoints_p1.empty()) m_fields->checkpoints_p1.pop_back();
            if (!m_fields->checkpoints_p2.empty()) m_fields->checkpoints_p2.pop_back();
        }
    }

    void updateVisibility(float dt)  {   
        auto& config = Config::get();
        auto& recorderAudio = RecorderAudio::get();

        if (recorderAudio.is_recording || recorderAudio.enabled) {
            return;
        }
        

        if (!config.get<bool>("pulse_size", false) && config.get<bool>("no_pulse", false)) {
            m_audioEffectsLayer->m_notAudioScale = 0.5f;
            FMODAudioEngine::get()->m_pulse1 = 0.5f;
        }            

        if (config.get<bool>("pulse_size", false)) {
            float value = config.get<float>("pulse_size_value", 0.5f);
            if (config.get<bool>("pulse_multiply", false)) {
                m_audioEffectsLayer->m_notAudioScale *= value;
                FMODAudioEngine::get()->m_pulse1 *= value;
            }                
            else {
                m_audioEffectsLayer->m_notAudioScale = value;
                FMODAudioEngine::get()->m_pulse1 = value;
            }
        }

        PlayLayer::updateVisibility(dt);
    }
    
    void pauseGame(bool paused) {
        auto& config = Config::get();
        auto& recorderAudio = RecorderAudio::get();
        if (recorderAudio.enabled) {
            ImGuiH::Popup::get().add_popup("You can't pause because the audio is still recording");
            return;
        }

        bool levelEndAnimationStarted = m_levelEndAnimationStarted;
        if (config.get<bool>("pause_during_complete", false))
            m_levelEndAnimationStarted = false;

        PlayLayer::pauseGame(paused);

        m_levelEndAnimationStarted = levelEndAnimationStarted;
    }

    void startMusic() {
        auto& recorderAudio = RecorderAudio::get();
        PlayLayer::startMusic();
        if (recorderAudio.enabled && recorderAudio.showcase_mode) {
            if (recorderAudio.is_recording) {
                recorderAudio.stop();
            }
            recorderAudio.start();
        }
    }

    void updateProgressbar() {
        auto& config = Config::get();
        PlayLayer::updateProgressbar();

        if (config.get<bool>("show_hitboxes", false)) {
            if (!(m_isPracticeMode && GameManager::get()->getGameVariable("0166")))
                PlayLayer::updateDebugDraw();
            
            m_debugDrawNode->setVisible(true);
        }
    }
};

class $modify(MyEndLevelLayer, EndLevelLayer) {
    struct Fields {
        cocos2d::CCSprite* black_bg;
    };

    void pizdec(float) {
        auto& recorder = Recorder::get();
        auto& recorderAudio = RecorderAudio::get();
        if (m_fields->black_bg && recorder.is_recording && recorder.fade_out) {
            int opacity = (recorder.after_end_extra_time >= (recorder.after_end_duration - 0.1f)) ? 255 : 255 * recorder.after_end_extra_time / (recorder.after_end_duration - 0.1f);
            m_fields->black_bg->setOpacity(opacity);
        }
        else if (m_fields->black_bg && recorder.need_remove_black) {
            recorder.need_remove_black = false;
            m_fields->black_bg->setOpacity(0);
        }

        if (recorder.need_visible_lc) {
            setVisible(true);
            recorder.need_visible_lc = false;
        }
    }

    void showLayer(bool p0) {
        auto& config = Config::get();
        auto& recorder = Recorder::get();
        auto& recorderAudio = RecorderAudio::get();
        EndLevelLayer::showLayer(p0);

        auto pl = PlayLayer::get();
        if (recorder.is_recording && recorder.hide_level_complete) {
            setVisible(false);
        }        

        if (config.get<bool>("cheat_indicator", false)) {
            m_mainLayer->sortAllChildren();
            auto* sprite = m_mainLayer->getChildByType<cocos2d::CCSprite>(-1);
            sprite->setColor({255, 0, 0});
        }

        if (recorder.is_recording && recorder.fade_out) {
            auto wnd_size = cocos2d::CCDirector::sharedDirector()->getWinSize();

            m_fields->black_bg = cocos2d::CCSprite::create("game_bg_13_001.png");
            auto sprSize = m_fields->black_bg->getContentSize();
            m_fields->black_bg->setPosition({wnd_size.width/2, wnd_size.height/2});            
            m_fields->black_bg->setScaleX(wnd_size.width / sprSize.width * 2.f);
            m_fields->black_bg->setScaleY(wnd_size.height / sprSize.height * 2.f);
            m_fields->black_bg->setColor({0, 0, 0});
            m_fields->black_bg->setOpacity(0);
            m_fields->black_bg->setZOrder(999);

            if (recorder.hide_level_complete) {
                pl->addChild(m_fields->black_bg);;
            }
            else {
                addChild(m_fields->black_bg);
            }
        }

        schedule(schedule_selector(MyEndLevelLayer::pizdec), 0.0f);
    }

    //if ( GameManager::getGameVariable(v16, "0095") )

    void customSetup() {
        auto gm = GameManager::get();
        auto& config = Config::get();
        bool orig = gm->getGameVariable("0095");
        if (config.get("no_do_not_flip", false))
            gm->setGameVariable("0095", false);

        EndLevelLayer::customSetup();

        gm->setGameVariable("0095", orig);
    }
};

class $modify(MyGJBaseGameLayer, GJBaseGameLayer) {
    static void onModify(auto& self) {
        (void) self.setHookPriority("GJBaseGameLayer::update", 0x99999); 
    }

    void handleButton(bool down, int button, bool isPlayer1) {
        GJBaseGameLayer::handleButton(down, button, isPlayer1);
        ReplayEngine::get().handle_button(down, button, isPlayer1);
        if (down) CpsCounter::get().click();
    }

    float getCustomDelta(float dt, float tps, bool applyExtraDelta = true) {
        if (applyExtraDelta && m_resumeTimer > 0)
        {
            --m_resumeTimer;
            dt = 0.0;
        }
        
        float fixed_dt = 1.f / tps;

        auto timestep = std::min(m_gameState.m_timeWarp, 1.f) * fixed_dt;
        auto total_dt = dt + m_extraDelta;
        auto steps = std::round(total_dt / timestep);
        auto new_dt = steps * timestep;
        if (applyExtraDelta) m_extraDelta = total_dt - new_dt;

        return static_cast<float>(new_dt);
    }

    float getModifiedDelta(float dt) {
        auto& config = Config::get();
        if (!config.get<bool>("tps_enabled", false))
            return GJBaseGameLayer::getModifiedDelta(dt);

        return getCustomDelta(dt, config.get<float>("tps_value", 60.f));
    }

    void update(float dt) {
        auto& engine = ReplayEngine::get();
        auto& recorder = Recorder::get();
        auto& recorderAudio = RecorderAudio::get();
        auto& config = Config::get();

        if (config.get<bool>("stop_triggers_on_death", false) && PlayLayer::get() && (m_player1->m_isDead || m_player2->m_isDead))
            return;

        if (config.get<bool>("jump_hack", false))
            m_player1->m_isOnGround = true;

        if (recorder.is_recording) recorder.handle_recording(dt);        
        if (recorderAudio.is_recording) recorderAudio.handle_recording(dt);
    
        GJBaseGameLayer::update(dt);

        SpamBot::get().handle_spambot(this);
        StraightFly::get().handle_straightfly(this);

        if (!engine.engine_v2)
            engine.handle_update(this);

        NoclipAccuracy::get().handle_update(this, dt);

        if (config.get<bool>("rgb_icons", false)) {
            color_dt += dt * config.get<float>("rgb_icons::speed", 0.20f);

            auto& rgb_colors = RGBIcons::get();
            auto color1 = rgb_colors.interpolateColor(color_dt);
            auto color2 = rgb_colors.interpolateColor(color_dt, true);

            bool player_p1 = config.get<bool>("rgb_icons::player_p1", true);
            bool player_p2 = config.get<bool>("rgb_icons::player_p2", true);
            bool wave_trail_p1 = config.get<bool>("rgb_icons::wave_trail_p1", true);
            bool wave_trail_p2 = config.get<bool>("rgb_icons::wave_trail_p2", true);

            if (player_p1 && m_player1) {
                m_player1->setColor(color1);
                m_player1->setSecondColor(color2);
            }
            if (wave_trail_p1 && m_player1->m_waveTrail) m_player1->m_waveTrail->setColor(color1);

            if (player_p2 && m_player2) {
                m_player2->setColor(color2);
                m_player2->setSecondColor(color1);
            }

            if (wave_trail_p2 && m_player2->m_waveTrail) m_player2->m_waveTrail->setColor(color2);
        }
    }

    void updateDebugDraw() {
        auto& config = Config::get();

        GJBaseGameLayer::updateDebugDraw();

        if (!config.get<bool>("show_hitboxes", false))
            return;

        auto& node = m_debugDrawNode;

        auto pl = GameManager::sharedState()->getPlayLayer();
        if (config.get<bool>("show_hitboxes::on_death", false) && pl && !pl->m_player1->m_isDead) {
            node->clear();
            return;
        }            
        
        auto generateVertices = [](const cocos2d::CCRect& rect) {
            return std::vector<cocos2d::CCPoint>{
                cocos2d::CCPoint(rect.getMinX(), rect.getMinY()),
                cocos2d::CCPoint(rect.getMinX(), rect.getMaxY()),
                cocos2d::CCPoint(rect.getMaxX(), rect.getMaxY()),
                cocos2d::CCPoint(rect.getMaxX(), rect.getMinY())
            };
        };

        std::vector<cocos2d::CCPoint> player1 = generateVertices(m_player1->getObjectRect());
        node->drawPolygon(player1.data(), player1.size(), {0, 0, 0, 0}, 0.25f, {1.f, 1.f, 0.f, 0.75f});

        if (m_gameState.m_isDualMode) {
            std::vector<cocos2d::CCPoint> player2 = generateVertices(m_player2->getObjectRect());
            node->drawPolygon(player2.data(), player2.size(), {0, 0, 0, 0}, 0.25f, {1.f, 1.f, 0.f, 0.75f});
        }

        if (m_anticheatSpike) {
            std::vector<cocos2d::CCPoint> player2 = generateVertices(m_anticheatSpike->getObjectRect());
            node->drawPolygon(player2.data(), player2.size(), {0, 0, 0, 0}, 0.25f, {1.f, 0.f, 0.f, 1.0f});
        }

        for (auto &hitbox: playerTrail1) {
            std::vector<cocos2d::CCPoint> trail_p1 = generateVertices(hitbox);
            node->drawPolygon(trail_p1.data(), trail_p1.size(), {0, 0, 0, 0}, 0.25f, {1.f, 1.f, 0.f, 1.0f});
        }
            
        for (auto &hitbox: playerTrail2) {
            std::vector<cocos2d::CCPoint> trail_p2 = generateVertices(hitbox);
            node->drawPolygon(trail_p2.data(), trail_p2.size(), {0, 0, 0, 0}, 0.25f, {1.f, 1.f, 0.f, 1.0f});
        }
    }

    bool canBeActivatedByPlayer(PlayerObject *p0, EffectGameObject *p1) {
        auto& config = Config::get();
        if (config.get<bool>("no_mirror_portal", false) && p1->m_objectType == GameObjectType::InverseMirrorPortal) return false;

        return GJBaseGameLayer::canBeActivatedByPlayer(p0, p1);
    }

    void updateZoom(float p0, float p1, int p2, float p3, int p4, int p5) {
        auto& config = Config::get();
        if (config.get<bool>("no_camera_zoom", false)) return;

        GJBaseGameLayer::updateZoom(p0, p1, p2, p3, p4, p5);
    }

    
    void teleportPlayer(TeleportPortalObject *p0, PlayerObject *p1) {
        auto& config = Config::get();
        bool hasNoEffects = p0->m_hasNoEffects;

        if (config.get<bool>("no_bg_flash", false))
            p0->m_hasNoEffects = true;

        GJBaseGameLayer::teleportPlayer(p0, p1);
        p0->m_hasNoEffects = hasNoEffects;
    }

    void lightningFlash(cocos2d::CCPoint from, cocos2d::CCPoint to, cocos2d::ccColor3B color, float lineWidth, float duration, int displacement, bool flash, float opacity) {
        auto& config = Config::get();
        auto gm = GameManager::get();

        auto performanceMode = gm->m_performanceMode;

        if (config.get<bool>("no_portal_lighting", false))
            gm->m_performanceMode = true;

        if (config.get<bool>("no_bg_flash", false))
            flash = false;

        GJBaseGameLayer::lightningFlash(from, to, color, lineWidth, duration, displacement, flash, opacity);
        gm->m_performanceMode = performanceMode;
    }

    void processCommands(float dt) {
        auto& config = Config::get();
        auto& engine = ReplayEngine::get();
        
        GJBaseGameLayer::processCommands(dt);
        
        if (engine.engine_v2)
            engine.handle_update(this);

        if (config.get<bool>("show_hitboxes", false) && config.get<bool>("show_hitboxes::draw_trail", false)) {
            if (!m_player1->m_isDead) {
                playerTrail1.push_back(m_player1->getObjectRect());
                playerTrail2.push_back(m_player2->getObjectRect());
            }

            auto pl = GameManager::sharedState()->getPlayLayer();
            if (!pl) return;

            auto maxLength = static_cast<size_t>(config.get<int>("show_hitboxes::trail_length", 240));
            while (playerTrail1.size() > maxLength)
                playerTrail1.pop_front();
            while (playerTrail2.size() > maxLength)
                playerTrail2.pop_front();
        }
    }

    void collisionCheckObjects(PlayerObject* player, gd::vector<GameObject*>* objects, int objectCount, float dt) {
        auto& config = Config::get();
        if (config.get<bool>("coins_in_practice", true) && m_isPracticeMode) {
            for (int i = 0; i < objectCount; i++) {
                auto* obj = objects->at(i);
                if (obj && geode::prelude::typeinfo_cast<EffectGameObject*>(obj)) {                
                    if (obj->m_objectType != GameObjectType::SecretCoin && obj->m_objectType != GameObjectType::UserCoin)
                        continue;

                    if (obj->getOpacity() == 0)
                        continue;

                    if (!player->getObjectRect().intersectsRect(obj->getObjectRect()))
                        continue;

                    GJBaseGameLayer::destroyObject(obj);
                    GJBaseGameLayer::gameEventTriggered(GJGameEvent::UserCoin, 0, 0);
                }
            }
        }
        GJBaseGameLayer::collisionCheckObjects(player, objects, objectCount, dt);
    }
};

class $modify(MyCameraTriggerGameObject, CameraTriggerGameObject) {
    void triggerObject(GJBaseGameLayer *p0, int p1, const gd::vector<int> *p2) {
        auto& config = Config::get();
        if (config.get<bool>("no_camera_move", false)) return;

        CameraTriggerGameObject::triggerObject(p0, p1, p2);
    }
};

bool bypassStat = false;
int statValue = 0;

class $modify(MyGameStatsManager, GameStatsManager) {
    bool isItemUnlocked(UnlockType p0, int p1) {
        if (GameStatsManager::isItemUnlocked(p0, p1))
            return true;

        auto& config = Config::get();

        if (config.get<bool>("unlock_items", false) && config.get<bool>("unlock_items::practice_music_sync", true) && p0 == UnlockType::GJItem & p1 == 17)
            return true;

        if (config.get<bool>("unlock_items", false) && config.get<bool>("unlock_items::music_unlocker", true) && p0 == UnlockType::GJItem & p1 == 16)
            return true;

        if (config.get<bool>("unlock_vaults", false) && p0 == UnlockType::GJItem && p1 >= 1 && p1 <= 4) {
            return true;
        }

        return false;
    }

    int getStat(const char* key) {
        if (bypassStat) {
            bypassStat = false;
            return statValue;
        }

        return GameStatsManager::getStat(key);
    }
};

class $modify(MyPlayerObject, PlayerObject) {
    void update(float dt)
    {
        PlayerObject::update(dt);

        if (Config::get().get<bool>("no_robot_fire", false)) {
            m_robotFire->setVisible(false);
            m_robotBurstParticles->setVisible(false);
        }
    }

    void playSpawnEffect() {
        if (Config::get().get<bool>("no_respawn_blink", false)) return;        
        PlayerObject::playSpawnEffect();
    }

    void playDeathEffect() {
        if (Config::get().get<bool>("no_death_effect", false)) return;            
        PlayerObject::playDeathEffect();
    }

    void incrementJumps() {
        if (Config::get().get<bool>("safe_mode", false)) return;

        PlayerObject::incrementJumps();
    }

    void playSpiderDashEffect(cocos2d::CCPoint from, cocos2d::CCPoint to) {
        if (!Config::get().get<bool>("no_spider_dash", false))
            PlayerObject::playSpiderDashEffect(from, to);
    }

    void fadeOutStreak2(float p0) {
        if (!Config::get().get<bool>("wave_trail_on_death", false))
            PlayerObject::fadeOutStreak2(p0);
    }

    void setupStreak() {
        auto& config = Config::get();
        auto gm = GameManager::get();

        bool gv0096 = m_gv0096;
        auto playerColor = gm->m_playerColor;

        if (config.get<bool>("solid_wave_trail", false)) {
            gv0096 = false;
            gm->m_playerColor = 15;
        }

        PlayerObject::setupStreak();

        m_gv0096 = gv0096;
        gm->m_playerColor = playerColor;
    }
};

class $modify(MyCCTextInputNode, CCTextInputNode){
    void updateLabel(gd::string p0)
    {
        if (Config::get().get<bool>("text_lenght", false))
            CCTextInputNode::setMaxLabelLength(9999);

        if (Config::get().get<bool>("char_filter", false))
            CCTextInputNode::setAllowedChars("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~ ");
        
        CCTextInputNode::updateLabel(p0);
    }
};

class $modify(MyLevelPage, LevelPage) {
    void onPlay(cocos2d::CCObject* sender) {
        auto coins = m_level->m_requiredCoins;

        if (Config::get().get<bool>("main_levels", false)) m_level->m_requiredCoins = 0;

        LevelPage::onPlay(sender);

        m_level->m_requiredCoins = coins;
    }
};

class $modify(MyGameManager, GameManager) {
    bool getUGV(char const* p0) {
        if (GameManager::getUGV(p0))
            return true;

        // geode::log::debug("{}", p0);

        if (Config::get().get<bool>("treasure_room", false) && std::strcmp(p0, "5") == 0)
            return true;

        if (Config::get().get<bool>("unlock_shops", false) && std::strcmp(p0, "11") == 0 || std::strcmp(p0, "20") == 0 || std::strcmp(p0, "35") == 0 || std::strcmp(p0, "34") == 0)
            return true;

        if (Config::get().get<bool>("unlock_vaults", false) && std::strcmp(p0, "4") == 0)
            return true;

        return false;
    }

    bool isColorUnlocked(int key, UnlockType type) {
        if (GameManager::isColorUnlocked(key, type))
            return true;

        auto& config = Config::get();
        return (config.get<bool>("unlock_items", false) && config.get<bool>("unlock_items::all_icons", true));
    }

    bool isIconUnlocked(int key, IconType type) {
        if (GameManager::isIconUnlocked(key, type))
            return true;

        auto& config = Config::get();
        return (config.get<bool>("unlock_items", false) && config.get<bool>("unlock_items::all_icons", true));
    }

    // bool getGameVariable(const char *p0) {
    //     geode::log::debug("{}", p0);
    //     if (GameManager::getGameVariable(p0))
    //         return true;

    //     return false;
    // }
};

class $modify(MySliderTouchLogic, SliderTouchLogic) {
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
        if (!Config::get().get<bool>("slider_limit", false)) 
            return ccTouchMoved(touch, event);

        if (m_thumb->m_vertical)
            m_thumb->setPositionY(convertToNodeSpace(touch->getLocation()).y);
        else
            m_thumb->setPositionX(convertToNodeSpace(touch->getLocation()).x);

        m_slider->updateBar();
        m_thumb->activate();
    }
};

class $modify(MyGJScaleControl, GJScaleControl) {
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
        if (!Config::get().get<bool>("slider_limit", false)) 
            return ccTouchMoved(touch, event);

        if (m_sliderXY && m_sliderXY->m_touchLogic->m_activateThumb) {
            m_sliderXY->getThumb()->setPositionX(convertToNodeSpace(touch->getLocation()).x);
            m_sliderXY->updateBar();
            float value = scaleFromValue(m_sliderXY->getThumb()->getValue());
            updateLabelXY(value);
            sliderChanged(m_sliderXY->getThumb());
            if (EditorUI::get()) 
                EditorUI::get()->scaleXYChanged(value, value, m_scaleLocked);
        }
    }
};

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
    static LevelInfoLayer* create(GJGameLevel* level, bool challenge) {
        auto password = level->m_password;
        if (Config::get().get("copy_hack", false)) {
            level->m_password = 1;
        }

        return LevelInfoLayer::create(level, challenge);
    }

    void tryCloneLevel(cocos2d::CCObject* sender) {
        if (Config::get().get("copy_hack", false)) {
            m_level->m_password = 1;
        }
        LevelInfoLayer::tryCloneLevel(sender);
    }

    bool init(GJGameLevel* p0, bool p1) {
        if (!LevelInfoLayer::init(p0, p1))
            return false;

        if (Config::get().get("auto_song_download", false) && m_songWidget->m_downloadBtn->isVisible()) {
            GameManager::sharedState()->setGameVariable("0016", true); // Bypass TOS
            m_songWidget->m_downloadBtn->activate();
        }            

        return true;
    }
};

class $modify(MyUILayer, UILayer) {
    bool init(GJBaseGameLayer* p0) {
        auto& config = Config::get();

        if (!UILayer::init(p0)) return false;
        if (!config.get<bool>("hide_pause_button", false)) return true;
        
        
        auto menu = getChildByType<cocos2d::CCMenu>(0);
        auto btn = menu->getChildByType<CCMenuItemSpriteExtra>(0);

        if (menu && btn)
            btn->getNormalImage()->setVisible(false);

        return true;
    }
};

class $modify(MyCCTransitionFade, cocos2d::CCTransitionFade) {
    static cocos2d::CCTransitionFade* create(float t, cocos2d::CCScene *scene) {
        auto& config = Config::get();
        if (config.get<bool>("no_transition", false)) t = 0;

        return cocos2d::CCTransitionFade::create(t, scene);
    }
};

class $modify(MyPauseLayer, PauseLayer) {
    static void onModify(auto& self) {
        (void) self.setHookPriority("PauseLayer::onQuit", 0x99999);
    }

    void onQuit(cocos2d::CCObject* sender) {
        auto& config = Config::get();
        if (!config.get<bool>("ignore_esc", false))
            PauseLayer::onQuit(sender);
    }

    void customSetup() {
        auto& hacks = Hacks::get();
        auto& engine = ReplayEngine::get();
        auto& config = Config::get();
        auto levelType = PlayLayer::get()->m_level->m_levelType;

        if (config.get<bool>("level_edit", false))
            PlayLayer::get()->m_level->m_levelType = GJLevelType::Editor;

        PauseLayer::customSetup();
        engine.auto_button_release();

        PlayLayer::get()->m_level->m_levelType = levelType;

        hacks.pauseLayer = this;
        if (config.get<bool>("hide_pause_menu", false)) {
            setVisible(false);
        }
    }

    void onResume(cocos2d::CCObject* sender) {
        auto& hacks = Hacks::get();

        PauseLayer::onResume(sender);
        hacks.pauseLayer = nullptr;
    }

    void onEdit(cocos2d::CCObject* sender) {
        auto& config = Config::get();
        auto levelType = PlayLayer::get()->m_level->m_levelType;

        if (config.get<bool>("level_edit", false))
            PlayLayer::get()->m_level->m_levelType = GJLevelType::Editor;

        PauseLayer::onEdit(sender);

        PlayLayer::get()->m_level->m_levelType = levelType;
    }

    void musicSliderChanged(cocos2d::CCObject* sender) {
        auto& config = Config::get();

        PauseLayer::musicSliderChanged(sender);
        
        if (!Config::get().get<bool>("allow_low_volume", false))
            return;
        
        auto value = geode::cast::typeinfo_cast<SliderThumb*>(sender)->getValue();

        auto* audio_engine = FMODAudioEngine::sharedEngine();
        if (value < 0.03f)
            audio_engine->setBackgroundMusicVolume(value);
    }

    // #ifdef GEODE_IS_ANDROID
    
    void sfxSliderChanged(cocos2d::CCObject* sender) {
        auto& config = Config::get();

        PauseLayer::sfxSliderChanged(sender);
        
        if (!Config::get().get<bool>("allow_low_volume", false))
            return;

        auto value = geode::cast::typeinfo_cast<SliderThumb*>(sender)->getValue();

        auto* audio_engine = FMODAudioEngine::sharedEngine();
        if (value < 0.03f)
            audio_engine->setEffectsVolume(value);
    }

    // #endif
};

class $modify(MyOptionsLayer, OptionsLayer) {
    void musicSliderChanged(cocos2d::CCObject* sender) {
        auto& config = Config::get();

        OptionsLayer::musicSliderChanged(sender);
        
        if (!config.get<bool>("allow_low_volume", false))
            return;
        
        auto value = geode::cast::typeinfo_cast<SliderThumb*>(sender)->getValue();

        auto* audio_engine = FMODAudioEngine::sharedEngine();
        if (value < 0.03f)
            audio_engine->setBackgroundMusicVolume(value);
    }

    void sfxSliderChanged(cocos2d::CCObject* sender) {
        auto& config = Config::get();

        OptionsLayer::sfxSliderChanged(sender);
        
        if (!config.get<bool>("allow_low_volume", false))
            return;

        auto value = geode::cast::typeinfo_cast<SliderThumb*>(sender)->getValue();

        auto* audio_engine = FMODAudioEngine::sharedEngine();
        if (value < 0.03f)
            audio_engine->setEffectsVolume(value);
    }
    
    void customSetup() {
        if (Config::get().get<bool>("unlock_vaults", false)) {
            bypassStat = true;
            statValue = 10;
        }
        OptionsLayer::customSetup();
    }

    void onSecretVault(cocos2d::CCObject* sender) {
        if (Config::get().get<bool>("unlock_vaults", false)) {
            bypassStat = true;
            statValue = 10;
        }
        OptionsLayer::onSecretVault(sender);
    }
};

class $modify(MySecretLayer2, SecretLayer2) {
    bool init() {
        if (Config::get().get<bool>("unlock_vaults", false)) {
            bypassStat = true;
            statValue = 200;
        }
        return SecretLayer2::init();
    }

    void onSecretLevel(cocos2d::CCObject* sender) {
        if (Config::get().get<bool>("unlock_vaults", false)) {
            bypassStat = true;
            statValue = 200;
        }
        SecretLayer2::onSecretLevel(sender);
    }
};

class $modify(MyCreatorLayer, CreatorLayer) {
    bool init() {
        if (Config::get().get<bool>("unlock_vaults", false)) {
            bypassStat = true;
            statValue = 50;
        }

        return CreatorLayer::init();
    }

    void onSecretVault(cocos2d::CCObject* sender) {
        if (Config::get().get<bool>("unlock_vaults", false)) {
            bypassStat = true;
            statValue = 51;
        }
        CreatorLayer::onSecretVault(sender);
    }
};

class $modify(MyEditorUI, EditorUI) {
    void onSettings(cocos2d::CCObject* sender) {
        auto& config = Config::get();
        auto levelType = LevelEditorLayer::get()->m_level->m_levelType;

        if (config.get<bool>("level_edit", false))
            LevelEditorLayer::get()->m_level->m_levelType = GJLevelType::Editor;
        EditorUI::onSettings(sender);
        LevelEditorLayer::get()->m_level->m_levelType = levelType;
    }

#ifdef GEODE_IS_WINDOWS

void zoomIn(cocos2d::CCObject* sender) {
    if (!Config::get().get("zoom_bypass", false)) {
        return EditorUI::zoomIn(sender);
    }

    float scale = m_editorLayer->m_objectLayer->getScale();
    float increment = (scale < 0.09f) ? 0.01f : 0.1f;
    EditorUI::updateZoom(scale + increment);
}

void zoomOut(cocos2d::CCObject* sender) {
    if (!Config::get().get("zoom_bypass", false)) {
        return EditorUI::zoomOut(sender);
    }

    float scale = m_editorLayer->m_objectLayer->getScale();
    float decrement = (scale > 0.1f) ? 0.1f : 0.01f;
    EditorUI::updateZoom(std::max(scale - decrement, 0.01f));
}
#endif

void onNewCustomItem(CCObject* sender) {
        if (!Config::get().get("custom_object_bypass", false))
            return EditorUI::onNewCustomItem(sender);
        
        auto gm = GameManager::get();
        if (gm) {
            cocos2d::CCArray* objects;
            if (m_selectedObjects->count()) {
                objects = m_selectedObjects;
            }
            else {
                objects = cocos2d::CCArray::create();
                objects->addObject(m_selectedObject);
            }
            gm->addNewCustomObject(copyObjects(objects, false, false));
            m_selectedObjectIndex = 0;
            reloadCustomItems();
        }
    }
};

class $modify(MyEditorPauseLayer, EditorPauseLayer) {
    bool init(LevelEditorLayer* p0) {
        auto& config = Config::get();
        auto levelType = LevelEditorLayer::get()->m_level->m_levelType;

        if (config.get<bool>("level_edit", false))
            LevelEditorLayer::get()->m_level->m_levelType = GJLevelType::Editor;
        auto ret = EditorPauseLayer::init(p0);
        LevelEditorLayer::get()->m_level->m_levelType = levelType;

        return ret;
    }
};

class $modify(MyLevelTools, LevelTools) {
    static bool verifyLevelIntegrity(gd::string p0, int p1) {
        auto& config = Config::get();
        if (LevelTools::verifyLevelIntegrity(p0, p1))
            return true;
            
        return config.get<bool>("level_edit", false);
    }
};

class $modify(MyCCParticleSystemQuad, cocos2d::CCParticleSystemQuad) {
    void draw() {
        auto& config = Config::get();
        if (config.get<bool>("no_particles", false)) return;

        cocos2d::CCParticleSystemQuad::draw();
    }
};

class $modify(MyShaderLayer, ShaderLayer) {
    void performCalculations() {
        auto& config = Config::get();
        if (config.get<bool>("no_shaders", false)) {
            m_state.m_usesShaders = false;
            return;
        }

        ShaderLayer::performCalculations();
    }
};

class $modify(MyGameToolbox, GameToolbox) {
    static gd::string intToShortString(int value) {
        auto& config = Config::get();
        if (!config.get<bool>("no_short_numbers", false))
            return GameToolbox::intToShortString(value);

        gd::string str = fmt::format("{}", value);
        return str;
    }
};

class $modify(MyHardStreak, HardStreak) {
    void updateStroke(float p0) {
        auto& config = Config::get();

        if (config.get<bool>("wave_trail_size", false)) 
            m_pulseSize = config.get<float>("wave_trail_size_value", 1.f);

        HardStreak::updateStroke(p0);
    }
};

class $modify(MyCCMotionStreak, cocos2d::CCMotionStreak) {
    void update(float delta) {
        auto& config = Config::get();
        if (config.get<bool>("always_trail", false))
            m_bStroke = true;

        if (config.get<bool>("no_trail", false))
            m_bStroke = false;

        CCMotionStreak::update(delta);
    }
};


class $modify(MySongSelectNode, SongSelectNode) {
    void audioNext(cocos2d::CCObject* p0) {
        if (Config::get().get<bool>("default_song_bypass", false)) {
            m_selectedSongID++;
            getLevelSettings()->m_level->m_audioTrack = m_selectedSongID;

            return SongSelectNode::updateAudioLabel();
        }

        SongSelectNode::audioNext(p0);
    }

    void audioPrevious(cocos2d::CCObject* p0) {
        if (Config::get().get<bool>("default_song_bypass", false)) {
            m_selectedSongID--;
            getLevelSettings()->m_level->m_audioTrack = m_selectedSongID;
            
            return SongSelectNode::updateAudioLabel();
        }

        SongSelectNode::audioPrevious(p0);
    }
};

class $modify(MyMoreSearchLayer, MoreSearchLayer) {
    void audioPrevious(cocos2d::CCObject* sender) {
        if (!Config::get().get<bool>("default_song_bypass", false))
            return MoreSearchLayer::audioPrevious(sender);

        int song = std::max(1, GameLevelManager::get()->getIntForKey("song_filter"));
        MoreSearchLayer::selectSong(--song);
    }

    void audioNext(cocos2d::CCObject* sender) {
        if (!Config::get().get<bool>("default_song_bypass", false))
            return MoreSearchLayer::audioNext(sender);

        int song = std::max(1, GameLevelManager::get()->getIntForKey("song_filter"));
        MoreSearchLayer::selectSong(++song);
    }

    void selectSong(int songID) {
        if (!Config::get().get<bool>("default_song_bypass", false))
            return MoreSearchLayer::selectSong(songID);

        GameLevelManager::get()->setIntForKey(songID, "song_filter");
        updateAudioLabel();
    }
};

class $modify(MyEditLevelLayer, EditLevelLayer) {
    bool init(GJGameLevel *gjgl) {
        if (Config::get().get<bool>("verify_hack", false))
            gjgl->m_isVerified = true;
        return EditLevelLayer::init(gjgl);
    }

    void onShare(CCObject* sender) {
        if (Config::get().get<bool>("no_c_mark", false)) m_level->m_originalLevel = 0;

        EditLevelLayer::onShare(sender);
    }
};

class $modify(MyLevelEditorLayer, LevelEditorLayer) {
    void postUpdate(float dt) {
        if (Config::get().get<bool>("smooth_editor_trail", false))
            m_trailTimer = 0.1f;
        LevelEditorLayer::postUpdate(dt);
    }

    void onPlaytest() {
        auto& engine = ReplayEngine::get();
        LevelEditorLayer::onPlaytest();

        playerTrail1.clear();
        playerTrail2.clear();

        if (engine.mode == state::play) {
            engine.handle_reset();
        }
    }
};

class $modify(MyCCDrawNode, cocos2d::CCDrawNode) {
    bool drawPolygon(cocos2d::CCPoint* vertex, unsigned int count, const cocos2d::ccColor4F& fillColor,
                     float borderWidth, const cocos2d::ccColor4F& borderColor) {

        auto& config = Config::get(); 

        cocos2d::ccColor4F fillColor2;
        fillColor2 = {borderColor.r, borderColor.g, borderColor.b, static_cast<GLfloat>(config.get<float>("show_hitboxes::fill_color_alpha", 0.2f))};

        if (config.get<bool>("show_hitboxes", false)) {
            borderWidth = abs(borderWidth);
            borderWidth = config.get<float>("show_hitboxes::size", 0.25f);
        }

        return cocos2d::CCDrawNode::drawPolygon(vertex, count, config.get<bool>("show_hitboxes::fill_color", false) ? fillColor2 : fillColor, borderWidth, borderColor);
    }

    bool drawCircle(const cocos2d::CCPoint& position, float radius, const cocos2d::ccColor4F& color,
                    float borderWidth, const cocos2d::ccColor4F& borderColor, unsigned int segments) {
        
        auto& config = Config::get(); 

        cocos2d::ccColor4F color2;
        color2 = {borderColor.r, borderColor.g, borderColor.b, static_cast<GLfloat>(config.get<float>("show_hitboxes::fill_color_alpha", 0.2f))};

        if (config.get<bool>("show_hitboxes", false)) {
            borderWidth = abs(borderWidth);
            borderWidth = config.get<float>("show_hitboxes::size", 0.25f);
        } 

        return cocos2d::CCDrawNode::drawCircle(position, radius, config.get<bool>("show_hitboxes::fill_color", false) ? color2 : color, borderWidth, borderColor, segments);
    }
};

class $modify(MyAchievementNotifier, AchievementNotifier) {
    void notifyAchievement(char const* title, char const* desc, char const* icon, bool quest) {
        if (Recorder::get().is_recording || RecorderAudio::get().is_recording) return;
        
        AchievementNotifier::notifyAchievement(title, desc, icon, quest);
    }
};

class $modify(MyCCCircleWave, CCCircleWave) {
    void draw()
    {
        auto& config = Config::get(); 
        if (!config.get<bool>("no_orb_ring", false))
            CCCircleWave::draw();
    }
};
