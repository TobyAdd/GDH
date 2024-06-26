#include "hooks.hpp"
#include "hacks.hpp"
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/GameStatsManager.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "speedhackAudio.hpp"
#include "labels.hpp"
#include "replayEngine.hpp"

PauseLayer* hooks::pauseLayer;

std::vector<StartPosObject*> startPositions;
std::vector<GameObject *> coinsObjects;

float left_over = 0.f;
bool next_frame = false;

class $modify(cocos2d::CCScheduler) {
    void update(float dt) {
        dt *= hacks::speed_value;

        if (engine.mode == state::disable)    
            return CCScheduler::update(dt);
        
        float newdt = 1.f / hacks::tps_value; 

        if (engine.frame_advance) {
            if (next_frame) {
                next_frame = false;
                return CCScheduler::update(newdt);
            }

            return;
        }

        if (!engine.real_time) {
            return CCScheduler::update(newdt);
        }
    
        unsigned times = static_cast<int>((dt + left_over) / newdt);  
        auto start = std::chrono::high_resolution_clock::now();
        using namespace std::literals;

        for (unsigned i = 0; i < times; ++i) {
            CCScheduler::update(newdt);
            if (std::chrono::high_resolution_clock::now() - start > 33.333ms) {            
                times = i + 1;
                break;
            }
        }
        left_over += dt - newdt * times; 
    }
};

namespace startpos_switcher {
    int selectedStartpos = 0;

    void switchStartPos(int incBy, bool direction = true) {
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

            if (hacks::startpos_switcher_reset_camera)
                pl->resetCamera();

            pl->startMusic();

            return;
        }
    }

    void handleKeyPress(int keyCode) {
        auto pl = PlayLayer::get();
        if (!pl) return;

        if (!hacks::use_a_s_d && (keyCode == GLFW_KEY_LEFT || keyCode == GLFW_KEY_RIGHT)) {
            int increment = (keyCode == GLFW_KEY_LEFT) ? -1 : 1;
            startpos_switcher::switchStartPos(increment);
        } else if (hacks::use_a_s_d && (keyCode == GLFW_KEY_A || keyCode == GLFW_KEY_D)) {
            int increment = (keyCode == GLFW_KEY_A) ? -1 : 1;
            startpos_switcher::switchStartPos(increment);
        }
    }
}


class $modify(PlayLayer) {
    struct Fields {
        cocos2d::CCLabelBMFont* labels;
        ~Fields() {
            startPositions.clear();
            coinsObjects.clear();
            hacks::ricon_delta = 0;
        }
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        m_fields->labels = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");

        m_fields->labels->setID("labels"_spr);
		m_fields->labels->setZOrder(999);
		m_fields->labels->setScale(0.4f);
        m_fields->labels->setOpacity(150);
        m_fields->labels->setAnchorPoint({1, 0});
        addChild(m_fields->labels);

        return true;
    }

    void addObject(GameObject* obj) {
        PlayLayer::addObject(obj);
        if (obj->m_objectID == 31) {
            startPositions.push_back(static_cast<StartPosObject *>(obj));
        }

        if (obj->m_objectID == 1329 || obj->m_objectID == 142) {
            coinsObjects.push_back(obj);
        }
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        noclip_accuracy.handle_update(this, dt);

        if (m_fields->labels) {
            auto screenLeft = cocos2d::CCDirector::sharedDirector()->getScreenLeft();
            auto screenTop = cocos2d::CCDirector::sharedDirector()->getScreenTop();
            auto screenBottom = cocos2d::CCDirector::sharedDirector()->getScreenBottom();
            auto screenRight = cocos2d::CCDirector::sharedDirector()->getScreenRight();

            m_fields->labels->setCString(labels::get_labels().c_str());
            auto size = m_fields->labels->getScaledContentSize();

            if (labels::pos == 0) {
                m_fields->labels->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentLeft);
                m_fields->labels->setPosition({screenLeft + size.width + 5, screenTop - size.height - 3});
            }
            else if (labels::pos == 1) {
                m_fields->labels->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentLeft);
                m_fields->labels->setPosition({screenLeft + size.width + 5, screenBottom + 3});
            }
            else if (labels::pos == 2) {
                m_fields->labels->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentRight);
                m_fields->labels->setPosition({screenRight - 5, screenTop - size.height - 3});
            }
            else if (labels::pos == 3) {
                m_fields->labels->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentRight);
                m_fields->labels->setPosition({screenRight - 5, screenBottom + 3});
            }
        }

        if (hacks::rgb_icons) {
            hacks::ricon_delta += dt;
            float r1, g1, b1, r2, g2, b2;

            ImGui::ColorConvertHSVtoRGB((float) hacks::ricon_delta * hacks::ricon_coef, hacks::ricon_saturation, hacks::ricon_brightness, r1, g1, b1);
            ImGui::ColorConvertHSVtoRGB((float) hacks::ricon_delta * hacks::ricon_coef + hacks::ricon_shift, hacks::ricon_saturation, hacks::ricon_brightness, r2, g2, b2);

            cocos2d::_ccColor3B color1 = {(GLubyte)(r1 * 255.0f), (GLubyte)(g1 * 255.0f), (GLubyte)(b1 * 255.0f)};
            cocos2d::_ccColor3B color2 = {(GLubyte)(r2 * 255.0f), (GLubyte)(g2 * 255.0f), (GLubyte)(b2 * 255.0f)};


            m_player1->setColor(color1);
            m_player2->setColor(color2);

            m_player1->setSecondColor(color2);
            m_player2->setSecondColor(color1);

            m_player1->m_waveTrail->setColor(color1);
            m_player2->m_waveTrail->setColor(color2);
        }
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        left_over = 0;

        engine.handle_reseting(this);
        
        noclip_accuracy.handle_reset(this);
        cps_counter.reset();

        if (hacks::auto_pickup_coins) {
            for (size_t i = 0; i < coinsObjects.size(); i++)
            {
                if (coinsObjects[i])
                {
                    PlayLayer::pickupItem(static_cast<EffectGameObject *>(coinsObjects[i]));
                    PlayLayer::destroyObject(coinsObjects[i]);
                }
            }
        }

        if (hacks::instant_complate) {
            PlayLayer::playEndAnimationToPos({0, 0});
        }
    }

    void playEndAnimationToPos(cocos2d::CCPoint pos) {
        PlayLayer::playEndAnimationToPos(pos);
        if (engine.mode == state::record)
            engine.mode = state::disable;
    }

    void playPlatformerEndAnimationToPos(cocos2d::CCPoint pos, bool idk) {
        PlayLayer::playPlatformerEndAnimationToPos(pos, idk);
        if (engine.mode == state::record)
            engine.mode = state::disable;
    }

    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        PlayLayer::destroyPlayer(player, obj);
        noclip_accuracy.handle_death();

        if (hacks::respawn_time_enabled) {
            if (auto* respawnSequence = this->getActionByTag(0x10)) {
                this->stopAction(respawnSequence);
                auto* newSequence = cocos2d::CCSequence::create(
                    cocos2d::CCDelayTime::create(hacks::respawn_time_value),
                    cocos2d::CCCallFunc::create(this, callfunc_selector(PlayLayer::delayedResetLevel)),
                    nullptr
                );
                newSequence->setTag(0x10);
                this->runAction(newSequence);
            }
        }
    }
};

class $modify(GJBaseGameLayer) {
    void handleButton(bool down, int button, bool isPlayer1) {
        GJBaseGameLayer::handleButton(down, button, isPlayer1);   
        if (engine.mode == state::record) {
            engine.handle_recording2(down, button, isPlayer1);
        } 
        if (down) cps_counter.recordClick();
    }

    void update(float dt) {
        GJBaseGameLayer::update(dt);

        if (engine.mode == state::record) {
            engine.handle_recording(this, true);
            engine.handle_recording(this, false);
        }
        else if (engine.mode == state::play) {
            engine.handle_playing(this);
        }
    }
};

class $modify(GameStatsManager) {
    bool isItemUnlocked(UnlockType p0, int p1) {
        auto ret = GameStatsManager::isItemUnlocked(p0, p1);

        if (hacks::unlock_items)
            return true;

        return ret;
    }
};

class $modify(PauseLayer) {
    static void onModify(auto& self) {
        (void) self.setHookPriority("PauseLayer::onQuit", 0x99999);
    }

    void onQuit(cocos2d::CCObject* sender) {
        if (!hacks::ignore_esc)
            PauseLayer::onQuit(sender);
    }

    void customSetup() {
        PauseLayer::customSetup();
        hooks::pauseLayer = this;
        if (hacks::hide_menu) {
            this->setVisible(false);
        }
    }

    void onResume(cocos2d::CCObject* sender) {
        PauseLayer::onResume(sender);
        hooks::pauseLayer = nullptr;
    }
};