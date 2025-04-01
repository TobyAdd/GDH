#include "utils.hpp"
#include "config.hpp"

bool utilsH::isNumeric(const std::string& str) {
    if (str.empty()) {
        return false;
    }

    bool hasDot = false;
    bool hasMinus = false;

    for (size_t i = 0; i < str.size(); ++i) {
        char ch = str[i];

        if (ch == '-') {
            if (hasMinus || i != 0) {
                return false;
            }
            hasMinus = true;
        } else if (ch == '.') {
            if (hasDot || i == 0 || i == str.size() - 1) {
                return false;
            }
            hasDot = true;
        } else if (!std::isdigit(static_cast<unsigned char>(ch))) {
            return false;
        }
    }

    return !(hasMinus && str.size() == 1) && !(hasDot && str.size() == 1);
}

bool utilsH::isLinuxWine() {
#ifdef GEODE_WINDOWS
    HMODULE ntdllMod = GetModuleHandleW(L"ntdll.dll");

    if (ntdllMod && GetProcAddress(ntdllMod, "wine_get_version")) {
        return true;
    }
#endif
    return false;
}

void utilsH::UncompleteLevel() {
    
    auto pl = PlayLayer::get();
    if (pl) {
        auto gsm = GameStatsManager::get();
        auto glm = GameLevelManager::get();
        auto level = pl->m_level;
        int level_id = level->m_levelID.value();

        if (level->m_normalPercent >= 100 && gsm->hasCompletedLevel(level)) {
            gsm->m_completedLevels->removeObjectForKey(fmt::format("c_{}", level_id));
            gsm->m_completedLevels->removeObjectForKey(fmt::format("star_{}", level_id)); 
            gsm->m_completedLevels->removeObjectForKey(fmt::format("gstar_{}", level_id));       
            gsm->m_completedLevels->removeObjectForKey(fmt::format("dstar_{}", level_id));     
            gsm->m_completedLevels->removeObjectForKey(fmt::format("gdemon_{}", level_id));   
            gsm->m_completedLevels->removeObjectForKey(fmt::format("demon_{}", level_id));    
            gsm->m_completedLevels->removeObjectForKey(fmt::format("ddemon_{}", level_id));

            // removing completed levels stat
            if (level_id >= 1 && level_id <= 22)
                gsm->setStat("4", gsm->getStat("4") - 1);
            else
                gsm->setStat("3", gsm->getStat("3") - 1);

            if (level->m_stars > 0) {
                // removing starts
                if (level->isPlatformer())
                    gsm->setStat("28", gsm->getStat("28") - level->m_stars);
                else
                    gsm->setStat("6", gsm->getStat("6") - level->m_stars);
            }

            // removing demons stat
            if (level->m_demon > 0) 
                gsm->setStat("5", gsm->getStat("5") - 1);
                
            // removing coins stat
            if (level_id >= 1 && level_id <= 22)
                gsm->setStat("8", gsm->getStat("8") - level->m_coinsVerified);
            else
                gsm->setStat("12", gsm->getStat("12") - level->m_coinsVerified);
                
        
            // to do: remove more stat
            // total jumps: 1
            // total attempts: 2
            // collected stars: 6 (done)
            // collected diamons 13
            // total orbs collected 22
            // completed levels 3 (done)
            // completed online levels 4 (done)
            // completed insane levels 42
            // completed demon levels 5 (done)
            // daily levels 15
            // collected secret coins 8 (done)
            // collected user coins 12 (done)
            // completed map packs 7
            // completed gauntlents 40
            // liked/disliked levels 10
            // rated levels 11
            // collected list rewards 41
            // players destroyed 9
        }  


        level->m_normalPercent = 0;
        level->m_practicePercent = 0;
        level->m_attempts = 0;
        level->m_jumps = 0;
        level->m_newNormalPercent2 = 0;
        level->m_clicks = 0;
        level->m_orbCompletion = 0;
        level->m_bestTime = 0;
        level->m_bestPoints = 0;
        level->m_isCompletionLegitimate = true;
        level->m_k111 = 0;

        for (auto i = 0; i < level->m_coins; i++) {
            auto key = level->getCoinKey(i + 1);
            gsm->m_verifiedUserCoins->removeObjectForKey(key);
        }
    }
    else {
        FLAlertLayer::create("Uncomplete Level", "Go to the level to uncomplete it", "OK");
    }
}

void utilsH::setPitchShifter(float pitch) {
    static FMOD::DSP* pitchShifter = nullptr;
    auto fmod_engine = FMODAudioEngine::get();
    FMOD::System* system = fmod_engine->m_system;

    if (pitch == 1.f) {
        if (pitchShifter) {
            fmod_engine->m_backgroundMusicChannel->removeDSP(pitchShifter);
            pitchShifter->release();
            pitchShifter = nullptr;
        }
        return;
    }

    if (!pitchShifter) {
        if (system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &pitchShifter) != FMOD_OK) {
            return;
        }
        fmod_engine->m_backgroundMusicChannel->addDSP(0, pitchShifter);
    }

    pitchShifter->setParameterFloat(FMOD_DSP_PITCHSHIFT_FFTSIZE, 2048); // Average balance of accuracy and delay (1024 by default)
    pitchShifter->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, pitch);
}