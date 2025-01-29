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

#ifdef GEODE_IS_WINDOWS
std::string utilsH::GetKeyName(int key) {
    switch (key) {
        case 0: return "None";
        case GLFW_KEY_SPACE: return "Space";
        case GLFW_KEY_APOSTROPHE: return "'";
        case GLFW_KEY_COMMA: return ",";
        case GLFW_KEY_MINUS: return "-";
        case GLFW_KEY_PERIOD: return ".";
        case GLFW_KEY_SLASH: return "/";
        case GLFW_KEY_0: return "0";
        case GLFW_KEY_1: return "1";
        case GLFW_KEY_2: return "2";
        case GLFW_KEY_3: return "3";
        case GLFW_KEY_4: return "4";
        case GLFW_KEY_5: return "5";
        case GLFW_KEY_6: return "6";
        case GLFW_KEY_7: return "7";
        case GLFW_KEY_8: return "8";
        case GLFW_KEY_9: return "9";
        case GLFW_KEY_SEMICOLON: return ";";
        case GLFW_KEY_EQUAL: return "=";
        case GLFW_KEY_A: return "A";
        case GLFW_KEY_B: return "B";
        case GLFW_KEY_C: return "C";
        case GLFW_KEY_D: return "D";
        case GLFW_KEY_E: return "E";
        case GLFW_KEY_F: return "F";
        case GLFW_KEY_G: return "G";
        case GLFW_KEY_H: return "H";
        case GLFW_KEY_I: return "I";
        case GLFW_KEY_J: return "J";
        case GLFW_KEY_K: return "K";
        case GLFW_KEY_L: return "L";
        case GLFW_KEY_M: return "M";
        case GLFW_KEY_N: return "N";
        case GLFW_KEY_O: return "O";
        case GLFW_KEY_P: return "P";
        case GLFW_KEY_Q: return "Q";
        case GLFW_KEY_R: return "R";
        case GLFW_KEY_S: return "S";
        case GLFW_KEY_T: return "T";
        case GLFW_KEY_U: return "U";
        case GLFW_KEY_V: return "V";
        case GLFW_KEY_W: return "W";
        case GLFW_KEY_X: return "X";
        case GLFW_KEY_Y: return "Y";
        case GLFW_KEY_Z: return "Z";
        case GLFW_KEY_LEFT_BRACKET: return "[";
        case GLFW_KEY_BACKSLASH: return "\\";
        case GLFW_KEY_RIGHT_BRACKET: return "]";
        case GLFW_KEY_GRAVE_ACCENT: return "`";
        case GLFW_KEY_WORLD_1: return "World 1";
        case GLFW_KEY_WORLD_2: return "World 2";
        case GLFW_KEY_ESCAPE: return "Esc";
        case GLFW_KEY_ENTER: return "Enter";
        case GLFW_KEY_TAB: return "Tab";
        case GLFW_KEY_BACKSPACE: return "Backspace";
        case GLFW_KEY_INSERT: return "Insert";
        case GLFW_KEY_DELETE: return "Delete";
        case GLFW_KEY_RIGHT: return "Right";
        case GLFW_KEY_LEFT: return "Left";
        case GLFW_KEY_DOWN: return "Down";
        case GLFW_KEY_UP: return "Up";
        case GLFW_KEY_PAGE_UP: return "Page Up";
        case GLFW_KEY_PAGE_DOWN: return "Page Down";
        case GLFW_KEY_HOME: return "Home";
        case GLFW_KEY_END: return "End";
        case GLFW_KEY_CAPS_LOCK: return "Caps Lock";
        case GLFW_KEY_SCROLL_LOCK: return "Scroll Lock";
        case GLFW_KEY_NUM_LOCK: return "Num Lock";
        case GLFW_KEY_PRINT_SCREEN: return "Print Screen";
        case GLFW_KEY_PAUSE: return "Pause";
        case GLFW_KEY_F1: return "F1";
        case GLFW_KEY_F2: return "F2";
        case GLFW_KEY_F3: return "F3";
        case GLFW_KEY_F4: return "F4";
        case GLFW_KEY_F5: return "F5";
        case GLFW_KEY_F6: return "F6";
        case GLFW_KEY_F7: return "F7";
        case GLFW_KEY_F8: return "F8";
        case GLFW_KEY_F9: return "F9";
        case GLFW_KEY_F10: return "F10";
        case GLFW_KEY_F11: return "F11";
        case GLFW_KEY_F12: return "F12";
        case GLFW_KEY_F13: return "F13";
        case GLFW_KEY_F14: return "F14";
        case GLFW_KEY_F15: return "F15";
        case GLFW_KEY_F16: return "F16";
        case GLFW_KEY_F17: return "F17";
        case GLFW_KEY_F18: return "F18";
        case GLFW_KEY_F19: return "F19";
        case GLFW_KEY_F20: return "F20";
        case GLFW_KEY_F21: return "F21";
        case GLFW_KEY_F22: return "F22";
        case GLFW_KEY_F23: return "F23";
        case GLFW_KEY_F24: return "F24";
        case GLFW_KEY_F25: return "F25";
        case GLFW_KEY_KP_0: return "0";
        case GLFW_KEY_KP_1: return "Num 1";
        case GLFW_KEY_KP_2: return "Num 2";
        case GLFW_KEY_KP_3: return "Num 3";
        case GLFW_KEY_KP_4: return "Num 4";
        case GLFW_KEY_KP_5: return "Num 5";
        case GLFW_KEY_KP_6: return "Num 6";
        case GLFW_KEY_KP_7: return "Num 7";
        case GLFW_KEY_KP_8: return "Num 8";
        case GLFW_KEY_KP_9: return "Num 9";
        case GLFW_KEY_KP_DECIMAL: return ".";
        case GLFW_KEY_KP_DIVIDE: return "/";
        case GLFW_KEY_KP_MULTIPLY: return "*";
        case GLFW_KEY_KP_SUBTRACT: return "-";
        case GLFW_KEY_KP_ADD: return "+";
        case GLFW_KEY_KP_ENTER: return "Enter";
        case GLFW_KEY_KP_EQUAL: return "=";
        case GLFW_KEY_LEFT_SHIFT: return "Left Shift";
        case GLFW_KEY_LEFT_CONTROL: return "Left Ctrl";
        case GLFW_KEY_LEFT_ALT: return "Left Alt";
        case GLFW_KEY_LEFT_SUPER: return "Left Super";
        case GLFW_KEY_RIGHT_SHIFT: return "Right Shift";
        case GLFW_KEY_RIGHT_CONTROL: return "Right Ctrl";
        case GLFW_KEY_RIGHT_ALT: return "Right Alt";
        case GLFW_KEY_RIGHT_SUPER: return "Right Super";
        case GLFW_KEY_MENU: return "Menu";
        default: return "Unknown";
    }
}
#endif