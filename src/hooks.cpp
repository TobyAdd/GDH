#include <Geode/modify/PlayLayer.hpp>
#include <Geode/Modify/GameStatsManager.hpp>
#include <Geode/Modify/PlayerObject.hpp>
#include <Geode/Modify/CCTextInputNode.hpp>
#include <Geode/Modify/LevelPage.hpp>
#include <Geode/Modify/GameManager.hpp>
#include <Geode/Modify/SliderTouchLogic.hpp>
#include <Geode/Modify/GJScaleControl.hpp>
#include <Geode/Modify/LevelInfoLayer.hpp>
#include <Geode/Modify/GJBaseGameLayer.hpp>
#include <Geode/Modify/UILayer.hpp>
#include <Geode/Modify/CCTransitionFade.hpp>
#include <Geode/Modify/PauseLayer.hpp>
#include <Geode/Modify/EditorUI.hpp>
#include <Geode/Modify/EditorPauseLayer.hpp>
#include <Geode/Modify/LevelTools.hpp>
#include <Geode/Modify/CCParticleSystemQuad.hpp>
#include <Geode/Modify/OptionsLayer.hpp>
#include <Geode/Modify/CameraTriggerGameObject.hpp>
#include <Geode/Modify/ShaderLayer.hpp>
#include <Geode/Modify/GameToolbox.hpp>
#include "hacks.hpp"
#include "config.hpp"

class $modify(PlayLayer) {
    struct Fields {
        GameObject* anticheat_obj = nullptr;
        std::vector<GameObject*> coinsObjects;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        auto& config = Config::get();
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        if (config.get<bool>("auto_practice_mode", false))
            togglePracticeMode(true);

        return true;
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
    }

    
    void resetLevel() {
        auto& config = Config::get();

        PlayLayer::resetLevel();

        if (config.get<bool>("no_do_not_flip", false) && m_attemptLabel) {
            m_attemptLabel->setScaleY(1);
        }

        if (config.get<bool>("safe_mode", false))
            m_level->m_attempts = m_level->m_attempts - 1;

        if (config.get<bool>("auto_pickup_coins", false)) {
            for (auto* coin : m_fields->coinsObjects) {
                if (!coin) continue;
                destroyObject(coin);
                pickupItem(static_cast<EffectGameObject*>(coin));
            }
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
};

class $modify(GJBaseGameLayer) {
    void update(float dt) {
        auto& config = Config::get();

        GJBaseGameLayer::update(dt);

        if (config.get<bool>("jump_hack", false))
            m_player1->m_isOnGround = true;
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
    virtual void update(float dt)
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

    void playSpiderDashEffect(cocos2d::CCPoint from, cocos2d::CCPoint to)
    {
        if (!Config::get().get<bool>("no_spider_dash", false))
            PlayerObject::playSpiderDashEffect(from, to);
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
            float value = m_sliderXY->getThumb()->getValue();
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