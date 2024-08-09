#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/GameStatsManager.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/GJGameLevel.hpp>
#include "hacks.hpp"

class $modify(PlayLayer) {
    struct Fields {
        GameObject* anticheat_obj = nullptr;
    };

    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        auto& hacks = Hacks::get();

        if (!m_fields->anticheat_obj)
            m_fields->anticheat_obj = obj;
        
        if (obj == m_fields->anticheat_obj)
            PlayLayer::destroyPlayer(player, obj);

        if (!hacks.noclip) {
            PlayLayer::destroyPlayer(player, obj);
        }
    }

    void showNewBest(bool p0, int p1, int p2, bool p3, bool p4, bool p5) {
        if (Hacks::get().safe_mode) return;        
        PlayLayer::showNewBest(p0, p1, p2, p3, p4, p5);
    }

    void resetLevel() {
        if (Hacks::get().safe_mode)
            m_level->m_attempts = m_level->m_attempts - 1;

        PlayLayer::resetLevel();
    }
};

class $modify(GameManager) {
    bool isColorUnlocked(int key, UnlockType type) {
        if (GameManager::isColorUnlocked(key, type))
            return true;

        auto& hacks = Hacks::get();
        return hacks.unlock_items;
    }

    bool isIconUnlocked(int key, IconType type) {
        if (GameManager::isIconUnlocked(key, type))
            return true;

        auto& hacks = Hacks::get();
        return hacks.unlock_items;
    }
};

class $modify(GameStatsManager) {
    bool isItemUnlocked(UnlockType p0, int p1) {
        if (GameStatsManager::isItemUnlocked(p0, p1))
            return true;
        
        auto& hacks = Hacks::get();
        if (!hacks.unlock_items)
            return false;

        if (p0 == UnlockType::GJItem & p1 == 16)
            return true;

        if (p0 == UnlockType::GJItem & p1 == 17)
            return true;

        return false;
    }
};

class $modify(PlayerObject) {
    void playSpawnEffect() {
        if (Hacks::get().no_respawn_blink) return;        
        PlayerObject::playSpawnEffect();
    }

    void playDeathEffect() {
        if (Hacks::get().no_death_effect) return;            
        PlayerObject::playDeathEffect();
    }

    void incrementJumps() {
        if (Hacks::get().safe_mode) return;

        PlayerObject::incrementJumps();
    }
};

class $modify(GJGameLevel) {
    void savePercentage(int percent, bool isPracticeMode, int clicks, int attempts, bool isChkValid) {
        if (Hacks::get().safe_mode) return;
        GJGameLevel::savePercentage(percent, isPracticeMode, clicks, attempts, isChkValid);
    }
};