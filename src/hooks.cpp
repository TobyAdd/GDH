#include "hooks.hpp"
#include "hacks.hpp"
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/GameStatsManager.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/CCDrawNode.hpp>
#include "speedhackAudio.hpp"
#include "labels.hpp"
#include "replayEngine.hpp"

PauseLayer* hooks::pauseLayer;

std::vector<StartPosObject*> startPositions;
std::vector<GameObject *> coinsObjects;

float left_over = 0.f;
bool next_frame = false;

std::deque<cocos2d::CCRect> playerTrail1, playerTrail2;

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

void drawRect(cocos2d::CCDrawNode *node, const cocos2d::CCRect &rect, const cocos2d::_ccColor4F &color, float borderWidth, const cocos2d::_ccColor4F &borderColor) {
    std::vector<cocos2d::CCPoint> vertices = {
            cocos2d::CCPoint(rect.getMinX(), rect.getMinY()),
            cocos2d::CCPoint(rect.getMinX(), rect.getMaxY()),
            cocos2d::CCPoint(rect.getMaxX(), rect.getMaxY()),
            cocos2d::CCPoint(rect.getMaxX(), rect.getMinY())
    };
    node->drawPolygon(vertices.data(), vertices.size(), color, borderWidth, borderColor);
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

        playerTrail1.clear();
        playerTrail2.clear();

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

    void updateProgressbar() {
        PlayLayer::updateProgressbar();

        if (hacks::show_hitboxes) {
            if (!(m_isPracticeMode && GameManager::get()->getGameVariable("0166")))
                PlayLayer::updateDebugDraw();
            
            m_debugDrawNode->setVisible(true);

            if (hacks::draw_trail) {
                for (auto &hitbox: playerTrail1) {
                    drawRect(m_debugDrawNode, hitbox, {0, 0, 0, 0}, 0.25f, {1.f, 1.f, 0.f, 1.0f});
                }
                    
                for (auto &hitbox: playerTrail2) {
                    drawRect(m_debugDrawNode, hitbox, {0, 0, 0, 0}, 0.25f, {1.f, 1.f, 0.f, 1.0f});
                }

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

    void processCommands(float dt) {
        GJBaseGameLayer::processCommands(dt);

        if (hacks::show_hitboxes && hacks::draw_trail) {
            if (!m_player1->m_isDead) {
                playerTrail1.push_back(m_player1->getObjectRect());
                playerTrail2.push_back(m_player2->getObjectRect());
            }

            auto maxLength = static_cast<size_t>(hacks::trail_length);
            while (playerTrail1.size() > maxLength)
                playerTrail1.pop_front();
            while (playerTrail2.size() > maxLength)
                playerTrail2.pop_front();
        }
    }
};

class $modify(GameManager) {
    bool isColorUnlocked(int key, UnlockType type) {
        if (GameManager::isColorUnlocked(key, type))
            return true;

        return hacks::unlock_items;
    }

    bool isIconUnlocked(int key, IconType type) {
        if (GameManager::isIconUnlocked(key, type))
            return true;

        return hacks::unlock_items;
    }
};

class $modify(GameStatsManager) {
    bool isItemUnlocked(UnlockType p0, int p1) {
        if (GameStatsManager::isItemUnlocked(p0, p1))
            return true;

        if (!hacks::unlock_items)
            return false;

        if (p0 == UnlockType::GJItem & p1 == 16)
            return true;

        if (p0 == UnlockType::GJItem & p1 == 17)
            return true;

        return false;
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


class $modify(cocos2d::CCDrawNode) {
    bool drawPolygon(cocos2d::CCPoint* vertex, unsigned int count, const cocos2d::ccColor4F& fillColor,
                     float borderWidth, const cocos2d::ccColor4F& borderColor) {

        if (hacks::show_hitboxes)
            borderWidth = abs(borderWidth);

        return cocos2d::CCDrawNode::drawPolygon(vertex, count, fillColor, borderWidth, borderColor);
    }

    bool drawCircle(const cocos2d::CCPoint& position, float radius, const cocos2d::ccColor4F& color,
                    float borderWidth, const cocos2d::ccColor4F& borderColor, unsigned int segments) {

        if (hacks::show_hitboxes)
            borderWidth = abs(borderWidth);            

        return cocos2d::CCDrawNode::drawCircle(position, radius, color, borderWidth, borderColor, segments);
    }
};