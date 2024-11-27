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
#include <Geode/modify/LevelEditorLayer.hpp>
#include "hacks.hpp"
#include "config.hpp"

std::vector<StartPosObject*> startPositions;
int selectedStartpos = -1;

void switchStartPos(int incBy, bool direction = true) {
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

class $modify(cocos2d::CCScheduler) {
    void update(float dt) {
        auto &config = Config::get();

        if (config.get<bool>("speedhack_enabled", false))
            dt *= config.get<float>("speedhack_value", 1.f);
      
        return CCScheduler::update(dt);
    }
};

class $modify(PlayLayer) {
    struct Fields {
        GameObject* anticheat_obj = nullptr;
        std::vector<GameObject*> coinsObjects;
        cocos2d::CCMenu* startposSwitcherUI;
        
        ~Fields() {
            startPositions.clear();
            selectedStartpos = -1;
        }
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        auto& config = Config::get();
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        if (config.get<bool>("auto_practice_mode", false))
            togglePracticeMode(true);

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
                switchStartPos(-1);
                label->setCString(fmt::format("{}/{}", selectedStartpos+1, startPositions.size()).c_str());
            });
            left_arrowClick->setPosition(win_size.width/2 - 50, cocos2d::CCDirector::sharedDirector()->getScreenBottom() + left_arrowClick->getScaledContentHeight());
            left_arrowClick->setOpacity(100);
            left_arrowClick->setID("startposSwitcherLeftArrowClick"_spr);

            auto right_arrow = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");  
            right_arrow->setScale(0.5f);
            right_arrow->setFlipX(true);
            auto right_arrowClick = geode::cocos::CCMenuItemExt::createSpriteExtra(right_arrow, [this, label](CCMenuItemSpriteExtra* sender) {
                switchStartPos(1);
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

        return true;
    }

    void togglePracticeMode(bool practiceMode) {
        if (m_fields->startposSwitcherUI) {
            m_fields->startposSwitcherUI->setVisible(!practiceMode);
        }
        PlayLayer::togglePracticeMode(practiceMode);
    }

    void addObject(GameObject* obj) {
        auto& config = Config::get();
        
        if (config.get<bool>("no_glow", false)) {
            obj->m_hasNoGlow = true;
        }

        if (config.get<bool>("force_dont_enter", false)) {
            // geode::log::debug("force_dont_enter {}", obj->m_isDontEnter);
            obj->m_isDontEnter = true;
        }

        if (config.get<bool>("force_dont_fade", false)) {
            // geode::log::debug("force_dont_fade {}", obj->m_isDontFade);
            obj->m_isDontFade = true;
        }

        PlayLayer::addObject(obj);

        if (obj->m_objectID == 1329 || obj->m_objectID == 142) {
            m_fields->coinsObjects.push_back(obj);
        }
        else if (obj->m_objectID == 31) {
            startPositions.push_back(static_cast<StartPosObject *>(obj));
        }
    }
    
    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        auto& config = Config::get();

        bool testmode = m_isTestMode;

        if (!m_fields->anticheat_obj)
            m_fields->anticheat_obj = obj;
        
        if (obj == m_fields->anticheat_obj)
            PlayLayer::destroyPlayer(player, obj);

        if (!config.get<bool>("noclip", false)) {
            if (Config::get().get<bool>("safe_mode", false)) {
                m_isTestMode = true;
            }
            PlayLayer::destroyPlayer(player, obj);
            m_isTestMode = testmode;
        }

        if (config.get<bool>("respawn_time", false)) {
            if (auto* respawnSequence = this->getActionByTag(0x10)) {
                this->stopAction(respawnSequence);
                auto* newSequence = cocos2d::CCSequence::create(
                    cocos2d::CCDelayTime::create(config.get<float>("respawn_time_value", 1.f)),
                    cocos2d::CCCallFunc::create(this, callfunc_selector(PlayLayer::delayedResetLevel)),
                    nullptr
                );
                newSequence->setTag(0x10);
                this->runAction(newSequence);
            }
        }
    }

    
    void resetLevel() {
        auto& config = Config::get();

        PlayLayer::resetLevel();

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
    }

    void levelComplete() {
        auto& config = Config::get();
        bool testmode = m_isTestMode;

        if (config.get<bool>("safe_mode", false))
            m_isTestMode = true;

        PlayLayer::levelComplete();

        m_isTestMode = testmode;
    }

    void showNewBest(bool p0, int p1, int p2, bool p3, bool p4, bool p5) {
        if (Config::get().get<bool>("no_new_best_popup", false)) return;        
        PlayLayer::showNewBest(p0, p1, p2, p3, p4, p5);
    }

    void updateVisibility(float dt)  {   
        auto& config = Config::get();

        if (!config.get<bool>("pulse_size", false) && config.get<bool>("no_pulse", false))
            m_audioEffectsLayer->m_notAudioScale = 0.5;

        if (config.get<bool>("pulse_size", false)) {
            float value = config.get<float>("pulse_size_value", 0.5f);
            if (config.get<bool>("pulse_multiply", false))
                m_audioEffectsLayer->m_notAudioScale *= value;
            else
                m_audioEffectsLayer->m_notAudioScale = value;
        }

        PlayLayer::updateVisibility(dt);
    }
};

class $modify(GJBaseGameLayer) {
    static void onModify(auto& self) {
        (void) self.setHookPriority("GJBaseGameLayer::update", 0x99999);
    }

    void update(float dt) {
        auto& config = Config::get();

        if (config.get<bool>("stop_triggers_on_death", false) && m_player1->m_isDead || m_player2->m_isDead)
            return;

        if (config.get<bool>("jump_hack", false))
            m_player1->m_isOnGround = true;

        GJBaseGameLayer::update(dt);
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
};

class $modify(CameraTriggerGameObject) {
    void triggerObject(GJBaseGameLayer *p0, int p1, const gd::vector<int> *p2) {
        auto& config = Config::get();
        if (config.get<bool>("no_camera_move", false)) return;

        CameraTriggerGameObject::triggerObject(p0, p1, p2);
    }
};


class $modify(GameStatsManager) {
    bool isItemUnlocked(UnlockType p0, int p1) {
        if (GameStatsManager::isItemUnlocked(p0, p1))
            return true;

        auto& config = Config::get();
        return config.get<bool>("unlock_items", false);
    }
};

class $modify(PlayerObject) {
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
};

class $modify(CCTextInputNode){
    void updateLabel(gd::string p0)
    {
        if (Config::get().get<bool>("text_lenght", false))
            CCTextInputNode::setMaxLabelLength(9999);

        if (Config::get().get<bool>("char_filter", false))
            CCTextInputNode::setAllowedChars("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~ ");
        
        CCTextInputNode::updateLabel(p0);
    }
};

class $modify(LevelPage) {
    void onPlay(cocos2d::CCObject* sender) {
        auto coins = m_level->m_requiredCoins;

        if (Config::get().get<bool>("main_levels", false)) m_level->m_requiredCoins = 0;

        LevelPage::onPlay(sender);

        m_level->m_requiredCoins = coins;
    }
};

class $modify(GameManager) {
    bool getUGV(char const* p0) {
        if (GameManager::getUGV(p0))
            return true;

        // geode::log::debug("{}", p0);

        if (Config::get().get<bool>("treasure_room", false) && std::strcmp(p0, "5") == 0)
            return true;

        if (Config::get().get<bool>("unlock_shops", false) && std::strcmp(p0, "11") == 0 || std::strcmp(p0, "20") == 0 || std::strcmp(p0, "35") == 0 || std::strcmp(p0, "34") == 0)
            return true;

        return false;
    }

    // bool getGameVariable(const char *p0) {
    //     geode::log::debug("{}", p0);
    //     if (GameManager::getGameVariable(p0))
    //         return true;

    //     return false;
    // }
};

class $modify(SliderTouchLogic) {
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

class $modify(GJScaleControl) {
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

class $modify(LevelInfoLayer) {
    static LevelInfoLayer* create(GJGameLevel* level, bool challenge) {
        auto password = level->m_password;
        if (Config::get().get("copy_hack", false)) {
            level->m_password = 1;
        }

        return LevelInfoLayer::create(level, challenge);
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

class $modify(UILayer) {
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

class $modify(cocos2d::CCTransitionFade) {
    static cocos2d::CCTransitionFade* create(float t, cocos2d::CCScene *scene) {
        auto& config = Config::get();
        if (config.get<bool>("no_transition", false)) t = 0;

        return cocos2d::CCTransitionFade::create(t, scene);
    }
};

class $modify(PauseLayer) {
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
        auto& config = Config::get();
        auto levelType = PlayLayer::get()->m_level->m_levelType;

        if (config.get<bool>("level_edit", false))
            PlayLayer::get()->m_level->m_levelType = GJLevelType::Editor;

        PauseLayer::customSetup();

        PlayLayer::get()->m_level->m_levelType = levelType;

        hacks.pauseLayer = this;
        if (config.get<bool>("hide_pause_menu", false)) {
            this->setVisible(false);
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

class $modify(OptionsLayer) {
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
};

class $modify(EditorUI) {
    void onSettings(cocos2d::CCObject* sender) {
        auto& config = Config::get();
        auto levelType = LevelEditorLayer::get()->m_level->m_levelType;

        if (config.get<bool>("level_edit", false))
            LevelEditorLayer::get()->m_level->m_levelType = GJLevelType::Editor;
        EditorUI::onSettings(sender);
        LevelEditorLayer::get()->m_level->m_levelType = levelType;
    }
};

class $modify(EditorPauseLayer) {
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

class $modify(LevelTools) {
    static bool verifyLevelIntegrity(gd::string p0, int p1) {
        auto& config = Config::get();
        if (LevelTools::verifyLevelIntegrity(p0, p1))
            return true;
            
        return config.get<bool>("level_edit", false);
    }
};

class $modify(cocos2d::CCParticleSystemQuad) {
    void draw() {
        auto& config = Config::get();
        if (config.get<bool>("no_particles", false)) return;

        cocos2d::CCParticleSystemQuad::draw();
    }
};

class $modify(ShaderLayer) {
    void performCalculations() {
        auto& config = Config::get();
        if (config.get<bool>("no_shaders", false)) {
            m_state.m_usesShaders = false;
            return;
        }

        ShaderLayer::performCalculations();
    }
};

class $modify(GameToolbox) {
    static gd::string intToShortString(int value) {
        auto& config = Config::get();
        if (!config.get<bool>("no_short_numbers", false))
            return GameToolbox::intToShortString(value);

        gd::string str = fmt::format("{}", value);
        return str;
    }
};

class $modify(HardStreak) {
    void updateStroke(float p0) {
        auto& config = Config::get();

        if (config.get<bool>("wave_trail_size", false)) 
            m_pulseSize = config.get<float>("wave_trail_size_value", 1.f);

        HardStreak::updateStroke(p0);
    }
};

class $modify(cocos2d::CCMotionStreak) {
    void update(float delta) {
        auto& config = Config::get();
        if (config.get<bool>("always_trail", false))
            m_bStroke = true;

        if (config.get<bool>("no_trail", false))
            m_bStroke = false;

        CCMotionStreak::update(delta);
    }
};


class $modify(SongSelectNode) {
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

class $modify(MoreSearchLayer) {
    void audioPrevious(cocos2d::CCObject* sender) {
        if (!Config::get().get<bool>("default_song_bypass", false))
            return audioPrevious(sender);

        int song = std::max(1, GameLevelManager::get()->getIntForKey("song_filter"));
        MoreSearchLayer::selectSong(--song);
    }

    void audioNext(cocos2d::CCObject* sender) {
        if (!Config::get().get<bool>("default_song_bypass", false))
            return audioNext(sender);

        int song = std::max(1, GameLevelManager::get()->getIntForKey("song_filter"));
        MoreSearchLayer::selectSong(++song);
    }

    void selectSong(int songID) {
        if (!Config::get().get<bool>("default_song_bypass", false))
            return selectSong(songID);

        GameLevelManager::get()->setIntForKey(songID, "song_filter");
        updateAudioLabel();
    }
};

class $modify(EditLevelLayer) {
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

class $modify(LevelEditorLayer) {
    void postUpdate(float dt) {
        if (Config::get().get<bool>("smooth_editor_trail", false))
            m_trailTimer = 0.1f;
        LevelEditorLayer::postUpdate(dt);
    }
};