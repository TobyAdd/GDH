#pragma once
#ifndef GJGAMELEVEL_H
#define GJGAMELEVEL_H

#include <gd.h>

namespace gd {

    enum GJLevelType {
        kGJLevelTypeLocal = 1,
        kGJLevelTypeEditor = 2,
        kGJLevelTypeSaved = 3
    };

    class GJGameLevel : public cocos2d::CCNode {
    public:
        cocos2d::CCDictionary *lastBuildSave;

        int levelID_rand; // levelID + seed = levelID_rand
        int levelID_seed;
        int levelID;

        std::string levelName;
        std::string levelDesc;
        std::string levelString;
        std::string userName;
        std::string recordString;

        std::string uploadDate;
        std::string updateDate;

        int userID_rand;
        int userID_seed;
        int userID;

        int accountID_rand;
        int accountID_seed;
        int accountID;

        int difficulty;
        int audioTrack;
        int songID;

        int levelRev;

        bool unlisted;

        int objectCount_rand;
        int objectCount_seed;
        int objectCount;
        int averageDifficulty;
        int ratings;
        int ratingsSum;

        int downloads;

        bool isEditable;
        bool gauntletLevel;
        bool gauntletLevel2;

        int workingTime;
        int workingTime2;

        bool lowDetailMode;
        bool lowDetailModeToggled;

        int isVerified_rand;
        int isVerified_seed;
        bool isVerified;
        bool isUploaded;
        bool hasBeenModified;

        int levelVersion;
        int gameVersion;

        int attempts_rand;
        int attempts_seed;
        int attempts;

        int jumps_rand;
        int jumps_seed;
        int jumps;

        int clicks_rand;
        int clicks_seed;
        int clicks;

        int attemptTime_rand;
        int attemptTime_seed;
        int attemptTime;

        int chk;

        bool isChkValid;
        bool isCompletionLegitimate;

        int normalPercent; // yes, it is out of order
        int normalPercent_seed;
        int normalPercent_rand;

        int orbCompletion_rand;
        int orbCompletion_seed;
        int orbCompletion;

        int newNormalPercent2_rand;
        int newNormalPercent2_seed;
        int newNormalPercent2;

        int practicePercent;

        int likes;
        int dislikes;
        int levelLength;
        int featured;

        bool isEpic;
        bool levelFavorited;
        int levelFolder;

        int dailyID_rand;
        int dailyID_seed;
        int dailyID;

        int demon_rand;
        int demon_seed;
        int demon;
        int demonDifficulty;
        int stars_rand;
        int stars_seed;
        int stars;

        bool autoLevel;
        int coins;
        int coinsVerified_rand;
        int coinsVerified_seed;
        int coinsVerified;

        int password_rand;
        int password_seed;

        int originalLevel_rand;
        int originalLevel_seed;
        int originalLevel;

        bool twoPlayerMode;

        int failedPasswordAttempts;

        int firstCoinVerified_rand;
        int firstCoinVerified_seed;
        int firstCoinVerified;

        int secondCoinVerified_rand;
        int secondCoinVerified_seed;
        int secondCoinVerified;

        int thirdCoinVerified_rand;
        int thirdCoinVerified_seed;
        int thirdCoinVerified;

        int starsRequested;

        bool showedSongWarning;
        int starRatings;
        int starRatingsSum;
        int maxStarRatings;
        int minStarRatings;
        int demonVotes;
        int rateStars;
        int rateFeature;

        std::string rateUser;

        bool dontSave;
        bool levelNotDownloaded;

        int requiredCoins;
        bool isUnlocked;

        cocos2d::CCPoint lastCameraPos;

        float lastEditorZoom;
        int lastBuildTab;
        int lastBuildPage;
        int lastBuildGroupID;

        GJLevelType levelType;

        int M_ID;
        std::string tempName;
        std::string capacityString;

        bool highObjectsEnabled;
        std::string personalBests;

        // this function is inlined on pc builds
        static GJGameLevel *create() {
            auto pRet = new GJGameLevel();

            if (pRet && pRet->init()) {
                pRet->autorelease();
                return pRet;
            } else {
                delete pRet;
                pRet = nullptr;
                return nullptr;
            }
        }

        std::string getAudioFileName() {
            std::string ret;

            reinterpret_cast<void(__thiscall*)(
                GJGameLevel*, std::string*
            )>(
                base + 0xbdc70
            )(
                this, &ret
            );

            return ret;
        }

        void setLevelData(const char* data) {
            auto len = strlen(data);
            auto addedLvlStr = reinterpret_cast<uintptr_t>(this) + 0x12c;

            *reinterpret_cast<size_t*>(addedLvlStr + 16) = len;   // length
            *reinterpret_cast<size_t*>(addedLvlStr + 20) = max(len, 15); // capacity

            if (len <= 15)
                memcpy(reinterpret_cast<char*>(addedLvlStr), data, len + 1);
            else {
                void* newb = malloc(len + 1);
                memcpy(newb, data, len + 1);
                *reinterpret_cast<void**>(addedLvlStr) = newb;
            }
        }

        void setLevelData(std::string const& data) {
            this->setLevelData(data.c_str());
        }
};

}

#endif
