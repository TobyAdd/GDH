#include "hooks.hpp"
#include "hacks.hpp"
#include "recorder.hpp"
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/GameStatsManager.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/CCDrawNode.hpp>
#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/EffectGameObject.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include "speedhackAudio.hpp"
#include "labels.hpp"
#include "replayEngine.hpp"
#include <random>
#include "memory.hpp"
#include "gui.hpp"

PauseLayer* hooks::pauseLayer;

std::vector<StartPosObject*> startPositions;
std::vector<GameObject*> gravityPortals, dualPortals, gamemodePortals, miniPortals, speedChanges, mirrorPortals, coinsObjects;

float left_over = 0.f;

std::deque<cocos2d::CCRect> playerTrail1, playerTrail2;

void drawRect(cocos2d::CCDrawNode *node, const cocos2d::CCRect &rect, const cocos2d::_ccColor4F &color, float borderWidth, const cocos2d::_ccColor4F &borderColor) {
    std::vector<cocos2d::CCPoint> vertices = {
            cocos2d::CCPoint(rect.getMinX(), rect.getMinY()),
            cocos2d::CCPoint(rect.getMinX(), rect.getMaxY()),
            cocos2d::CCPoint(rect.getMaxX(), rect.getMaxY()),
            cocos2d::CCPoint(rect.getMaxX(), rect.getMinY())
    };
    node->drawPolygon(vertices.data(), vertices.size(), color, borderWidth, borderColor);
}

void drawTrail(cocos2d::CCDrawNode *node) {
    for (auto &hitbox: playerTrail1) {
        drawRect(node, hitbox, {0, 0, 0, 0}, 0.25f, {1.f, 1.f, 0.f, 1.0f});
    }
        
    for (auto &hitbox: playerTrail2) {
        drawRect(node, hitbox, {0, 0, 0, 0}, 0.25f, {1.f, 1.f, 0.f, 1.0f});
    }
}

int getRandomNumber(int min, int max)
{
    std::random_device rd;
    std::mt19937_64 generator(rd());

    std::uniform_int_distribution<int> distribution(min, max);

    int randomNum = distribution(generator);
    return randomNum;
}


void setupStartPos(StartPosObject* startPos) {
    LevelSettingsObject* startPosSettings = startPos->m_startSettings;
    LevelSettingsObject* levelSettings = PlayLayer::get()->m_levelSettings;

    auto getClosestObject = [](std::vector<GameObject*>& vec, StartPosObject* startPos) -> GameObject* {
        std::sort(vec.begin(), vec.end(), [] (GameObject* a, GameObject* b) {
            return a->getPositionX() < b->getPositionX();
        });

        for (auto obj : vec) {
            if (obj->getPositionX() - 10 > startPos->getPositionX())
                break;
            else if (obj->getPositionX() - 10 < startPos->getPositionX())
                return obj;
        }

        return nullptr;
    };

    startPosSettings->m_startDual = levelSettings->m_startDual;
    if (auto obj = getClosestObject(dualPortals, startPos))
        startPosSettings->m_startDual = obj->m_objectID == 286;

    startPosSettings->m_startMode = levelSettings->m_startMode;
    if (auto obj = getClosestObject(gamemodePortals, startPos)) {
        switch(obj->m_objectID) {
            case 12:   startPosSettings->m_startMode = 0; break;
            case 13:   startPosSettings->m_startMode = 1; break;
            case 47:   startPosSettings->m_startMode = 2; break;
            case 111:  startPosSettings->m_startMode = 3; break;
            case 660:  startPosSettings->m_startMode = 4; break;
            case 745:  startPosSettings->m_startMode = 5; break;
            case 1331: startPosSettings->m_startMode = 6; break;
            case 1933: startPosSettings->m_startMode = 7; break;
        }
    }

    startPosSettings->m_startMini = levelSettings->m_startMini;
    if (auto obj = getClosestObject(miniPortals, startPos))
        startPosSettings->m_startMini = obj->m_objectID == 101;
        
    startPosSettings->m_startSpeed = levelSettings->m_startSpeed;
    if (auto obj = getClosestObject(speedChanges, startPos)) {
        switch(obj->m_objectID) {
            case 200:  startPosSettings->m_startSpeed = Speed::Slow; break;
            case 201:  startPosSettings->m_startSpeed = Speed::Normal; break;
            case 202:  startPosSettings->m_startSpeed = Speed::Fast; break;
            case 203:  startPosSettings->m_startSpeed = Speed::Faster; break;
            case 1334: startPosSettings->m_startSpeed = Speed::Fastest; break;
        }
    }
}

class $modify(cocos2d::CCScheduler) {
    void update(float dt) {
        if (hacks::speed_enabled)
            dt *= hacks::speed_value;

        if (!hacks::tps_enabled)    
            return CCScheduler::update(dt);
        
        float newdt = 1.f / hacks::tps_value; 

        if (recorder.is_recording) {
            if (recorder.frame_has_data) {
                return;
            }
            else {
                return CCScheduler::update(newdt);
            }
        }

        auto pl = GameManager::sharedState()->getPlayLayer();
        if (engine.frame_advance && pl && !pl->m_isPaused) {
            if (engine.next_frame) {
                engine.next_frame = false;
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
    bool smart_startpos = false;

    int selectedStartpos = 0;
    int left_key = GLFW_KEY_Q;
    int right_key = GLFW_KEY_E;

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

        if (keyCode == left_key || keyCode == right_key) {
            int increment = (keyCode == left_key) ? -1 : 1;
            startpos_switcher::switchStartPos(increment);
        }
    }
}

class $modify(PlayLayer) {
    struct Fields {
        cocos2d::CCLabelBMFont* labels;
        cocos2d::CCSprite* tint_death_bg;
        GameObject* anticheat_obj = nullptr;

        ~Fields() {
            if (recorderAudio.is_recording && recorderAudio.showcase_mode) {
                recorderAudio.stop();
            }

            startPositions.clear();
            gamemodePortals.clear();
            mirrorPortals.clear();
            miniPortals.clear();
            dualPortals.clear();
            speedChanges.clear();
            gravityPortals.clear();
            coinsObjects.clear();
            hacks::ricon_delta = 0;
            playerTrail1.clear();
            playerTrail2.clear();

            if (engine.frame_advance) {
                imgui_popup::add_popup("Frame Advance disabled");
                engine.frame_advance = false;
            }
        }
    };

    void startMusic() {
        PlayLayer::startMusic();
        if (recorderAudio.enabled && recorderAudio.showcase_mode) {
            if (recorderAudio.is_recording) {
                recorderAudio.stop();
            }
            recorderAudio.start();
        }
    }

    void updateVisibility(float dt) {
        if (hacks::disable_render)
            return;
            
        PlayLayer::updateVisibility(dt);
    }

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        auto wnd_size = cocos2d::CCDirector::sharedDirector()->getWinSize();

        m_fields->labels = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");

        m_fields->labels->setID("labels"_spr);
		m_fields->labels->setZOrder(999);
		m_fields->labels->setScale(0.4f);
        m_fields->labels->setOpacity(150);
        m_fields->labels->setAnchorPoint({1, 0});
        addChild(m_fields->labels);

        m_fields->tint_death_bg = cocos2d::CCSprite::create("game_bg_13_001.png");
        m_fields->tint_death_bg->setPosition({wnd_size.width/2, wnd_size.height/2});
        m_fields->tint_death_bg->setScaleX(wnd_size.width);
        m_fields->tint_death_bg->setScaleY(wnd_size.height);
        m_fields->tint_death_bg->setColor({255, 0, 0});
        m_fields->tint_death_bg->setOpacity(0);
        addChild(m_fields->tint_death_bg);

        return true;
    }

    void addObject(GameObject* obj) {
        PlayLayer::addObject(obj);
        switch (obj->m_objectID)
        {
        case 9:
        case 10:
        case 11:
            gravityPortals.push_back(obj);
            break;
        case 12:
        case 13:
        case 47:
        case 111:
        case 660:
        case 745:
        case 1331:
        case 1933:
            gamemodePortals.push_back(obj);
            break;
        case 31:
            startPositions.push_back(static_cast<StartPosObject *>(obj));
            break;
        case 45:
        case 46:
            mirrorPortals.push_back(obj);
            break;
        case 99:
        case 101:
            miniPortals.push_back(obj);
            break;
        case 286:
        case 287:
            dualPortals.push_back(obj);
            break;
        case 200:
        case 201:
        case 202:
        case 203:
        case 1334:
            speedChanges.push_back(obj);
            break;        
        //auto coins shit yea
        case 1329:
        case 142:
            coinsObjects.push_back(obj);
        default:
            break;
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

        if (m_fields->tint_death_bg) {
            if (hacks::tint_on_death && noclip_accuracy.prevDied && noclip_accuracy.deaths != 0) {
                m_fields->tint_death_bg->setOpacity(getRandomNumber(70, 100)); // my worst solution, sorry :(
            }
            else {
                m_fields->tint_death_bg->setOpacity(0);
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

        if (startpos_switcher::smart_startpos) {
            for (StartPosObject* obj : startPositions)
                setupStartPos(obj);
        }


        if (hacks::random_seed_enabled) {
            memory::WriteInt(geode::base::get() + 0x687DD0, hacks::seed_value);
        }
    
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
        if (!m_fields->anticheat_obj)
            m_fields->anticheat_obj = obj;

        
        if (obj == m_fields->anticheat_obj)
            PlayLayer::destroyPlayer(player, obj);

        bool shouldDestroy = hacks::nolcip_enabled ? 
            ((player == m_player1 && !hacks::noclip_p1) || 
            (player == m_player2 && !hacks::noclip_p2)) 
            : true;

        if (shouldDestroy) {
            PlayLayer::destroyPlayer(player, obj);
        }

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

            if (hacks::draw_trail)
                drawTrail(m_debugDrawNode);
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
        if (recorder.is_recording) {
            recorder.handle_recording(dt);
        }
        
        if (recorderAudio.is_recording) {
            recorderAudio.handle_recording(dt);
        }

        GJBaseGameLayer::update(dt);

        if (hacks::jump_hack)
            m_player1->m_isOnGround = true;

        if (engine.mode == state::record) {
            engine.handle_recording(this, true);
            engine.handle_recording(this, false);
        }
        else if (engine.mode == state::play) {
            engine.handle_playing(this);
        }

        spamBot.handle_spambot(this);
        straightFly.handle_straightfly(this);
    }

    void processCommands(float dt) {
        GJBaseGameLayer::processCommands(dt);

        if (hacks::show_hitboxes && hacks::draw_trail) {
            if (!m_player1->m_isDead) {
                playerTrail1.push_back(m_player1->getObjectRect());
                playerTrail2.push_back(m_player2->getObjectRect());
            }

            auto pl = GameManager::sharedState()->getPlayLayer();
            if (!pl) return;

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

        auto pl = GameManager::sharedState()->getPlayLayer();
        //0x3242 - m_isDead
        if (hacks::show_hitboxes_on_death && pl && !pl->m_player1->m_isDead) {
            borderWidth = 0;
        }

        return cocos2d::CCDrawNode::drawPolygon(vertex, count, fillColor, borderWidth, borderColor);
    }

    bool drawCircle(const cocos2d::CCPoint& position, float radius, const cocos2d::ccColor4F& color,
                    float borderWidth, const cocos2d::ccColor4F& borderColor, unsigned int segments) {

        if (hacks::show_hitboxes)
            borderWidth = abs(borderWidth);    

        auto pl = GameManager::sharedState()->getPlayLayer();
        if (hacks::show_hitboxes_on_death && pl && !pl->m_player1->m_isDead) {
            borderWidth = 0;
        }        

        return cocos2d::CCDrawNode::drawCircle(position, radius, color, borderWidth, borderColor, segments);
    }
};

class $modify(HardStreak)
{
    void updateStroke(float p0)
    {
        if (hacks::wave_trail)
            m_pulseSize = hacks::wave_trail_size;

        HardStreak::updateStroke(p0);
    }
};

class $modify(LevelEditorLayer) {
    bool init(GJGameLevel *p0, bool p1) {
        bool ret = LevelEditorLayer::init(p0, p1);

        playerTrail1.clear();
        playerTrail2.clear();

        return ret;
    }

    void updateEditor(float dt) {
        LevelEditorLayer::updateEditor(dt);

        if (hacks::show_hitboxes && hacks::draw_trail) {            
            drawTrail(m_debugDrawNode);
        }
    }

    void onPlaytest() {
        LevelEditorLayer::onPlaytest();
        playerTrail1.clear();
        playerTrail2.clear();
    }
};

class $modify(LevelInfoLayer) {
    bool init(GJGameLevel* p0, bool p1) {
        if (!LevelInfoLayer::init(p0, p1))
            return false;

        if (hacks::auto_song_download && m_songWidget->m_downloadBtn->isVisible())
            m_songWidget->m_downloadBtn->activate();

        return true;
    }
};