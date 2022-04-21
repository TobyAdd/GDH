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

    enum GJDifficulty {
        kGJDifficultyAuto = 0,
        kGJDifficultyEasy = 1,
        kGJDifficultyNormal = 2,
        kGJDifficultyHard = 3,
        kGJDifficultyHarder = 4,
        kGJDifficultyInsane = 5,
        kGJDifficultyDemon = 6,
        kGJDifficultyDemonEasy = 7,
        kGJDifficultyDemonMedium = 8,
        kGJDifficultyDemonInsane = 9,
        kGJDifficultyDemonExtreme = 10
    };
    class GDH_DLL GJGameLevel /* 0x3bc */ : public cocos2d::CCNode {
    public:
        cocos2d::CCDictionary* m_pLastBuildSave;   // 0xec
        int m_nLevelID_rand; // 0xf0
        int m_nLevelID_seed; // 0xf4 
        int m_nLevelID;      // 0xf8
        std::string m_sLevelName;       // 0xfc
        std::string m_sLevelDesc;       // 0x114
        std::string m_sLevelString;     // 0x12c
        std::string m_sCreatorName;     // 0x144
        std::string m_sRecordString;    // 0x15c
        std::string m_sUploadDate; // 0x174
        std::string m_sUpdateDate; // 0x18c
        int m_nUserID_rand;     // 0x1a4
        int m_nUserID_seed;     // 0x1a8
        int m_nUserID;          // 0x1ac
        int m_nAccountID_rand;  // 0x1b0
        int m_nAccountID_seed;  // 0x1b4
        int m_nAccountID;       // 0x1b8
        GJDifficulty m_eDifficulty;      // 0x1bc
        int m_nAudioTrack;      // 0x1c0
        int m_nSongID;          // 0x1c4
        int m_nLevelRev;        // 0x1c8
        bool m_bUnlisted;       // 0x1cc
        int m_nObjectCount_rand;// 0x1d0
        int m_nObjectCount_seed;// 0x1d4
        int m_nObjectCount;     // 0x1d8
        int m_nLevelIndex;      // 0x1dc
        int m_nRatings;         // 0x1e0
        int m_nRatingsSum;      // 0x1e4
        int m_nDownloads;       // 0x1e8
        bool m_bIsEditable;     // 0x1ec
        bool m_bGauntletLevel;  // 0x1ed
        bool m_bGauntletLevel2; // 0x1ee
        int m_nWorkingTime;     // 0x1f0
        int m_nWorkingTime2;    // 0x1f4
        bool m_bLowDetailMode;  // 0x1f8
        bool m_bLowDetailModeToggled;   // 0x1f9
        int m_nIsVerified_rand; // 0x1fc
        int m_nIsVerified_seed; // 0x200
        bool m_bIsVerified;     // 0x204
        bool m_bIsUploaded;     // 0x205
        bool m_bHasBeenModified;// 0x206
        int m_nLevelVersion;    // 0x208
        int m_nGameVersion;     // 0x20c
        int m_nAttempts_rand;   // 0x210
        int m_nAttempts_seed;   // 0x214
        int m_nAttempts;        // 0x218
        int m_nJumps_rand;      // 0x21c
        int m_nJumps_seed;      // 0x220
        int m_nJumps;           // 0x224
        int m_nClicks_rand;     // 0x228
        int m_nClicks_seed;     // 0x22c
        int m_nClicks;          // 0x230
        int m_nAttemptTime_rand;// 0x234
        int m_nAttemptTime_seed;// 0x238
        int m_nAttemptTime;     // 0x23c
        int m_nChk;             // 0x240
        bool m_bIsChkValid;     // 0x244
        bool m_bIsCompletionLegitimate; // 0x245
        int m_nNormalPercent;   // 0x248, yes, it is out of order
        int m_nNormalPercent_seed;  // 0x24c
        int m_nNormalPercent_rand;  // 0x250
        int m_nOrbCompletion_rand;  // 0x254
        int m_nOrbCompletion_seed;  // 0x258
        int m_nOrbCompletion;       // 0x25c
        int m_nNewNormalPercent2_rand;  // 0x260
        int m_nNewNormalPercent2_seed;  // 0x264
        int m_nNewNormalPercent2;       // 0x268
        int m_nPracticePercent; // 0x26c
        int m_nLikes;           // 0x270
        int m_nDislikes;        // 0x274
        int m_nLevelLength;     // 0x278
        int m_nFeatured;        // 0x27c
        bool m_bIsEpic;         // 0x280
        bool m_bLevelFavorited; // 0x281
        int m_nLevelFolder;     // 0x284
        int m_nDailyID_rand;    // 0x288
        int m_nDailyID_seed;    // 0x28c
        int m_nDailyID;         // 0x290
        int m_nDemon_rand;      // 0x294
        int m_nDemon_seed;      // 0x298
        int m_nDemon;           // 0x29c
        int m_nDemonDifficulty; // 0x2a0
        int m_nStars_rand;      // 0x2a4
        int m_nStars_seed;      // 0x2a8
        int m_nStars;           // 0x2ac
        bool m_bAutoLevel;      // 0x2b0
        int m_nCoins;           // 0x2b4
        int m_nCoinsVerified_rand;  // 0x2b8
        int m_nCoinsVerified_seed;  // 0x2bc
        int m_nCoinsVerified;       // 0x2c0
        int m_nPassword_rand;       // 0x2c4
        int m_nPassword_seed;       // 0x2c8
        int m_nOriginalLevel_rand;  // 0x2cc
        int m_nOriginalLevel_seed;  // 0x2d0
        int m_nOriginalLevel;       // 0x2d4
        bool m_bTwoPlayerMode;      // 0x2d8
        int m_nFailedPasswordAttempts;  // 0x2dc
        int m_nFirstCoinVerified_rand;  // 0x2e0
        int m_nFirstCoinVerified_seed;  // 0x2e4
        int m_nFirstCoinVerified;       // 0x2e8
        int m_nSecondCoinVerified_rand; // 0x2ec
        int m_nSecondCoinVerified_seed; // 0x2f0
        int m_nSecondCoinVerified;      // 0x2f4
        int m_nThirdCoinVerified_rand;  // 0x2f8
        int m_nThirdCoinVerified_seed;  // 0x2fc
        int m_nThirdCoinVerified;   // 0x300
        int m_nStarsRequested;      // 0x304
        bool m_bShowedSongWarning;  // 0x308
        int m_nStarRatings;         // 0x30c
        int m_nStarRatingsSum;      // 0x310
        int m_nMaxStarRatings;      // 0x314
        int m_nMinStarRatings;      // 0x318
        int m_nDemonVotes;          // 0x31c
        int m_nRateStars;           // 0x320
        int m_nRateFeature;         // 0x324
        std::string m_sRateUser;    // 0x328
        bool m_bDontSave;           // 0x340
        bool m_bLevelNotDownloaded; // 0x341
        int m_nRequiredCoins;       // 0x344
        bool m_bIsUnlocked;         // 0x348
        cocos2d::CCPoint m_obLastCameraPos; // 0x34c
        float m_fLastEditorZoom;    // 0x354
        int m_nLastBuildTab;        // 0x358
        int m_nLastBuildPage;       // 0x35c
        int m_nLastBuildGroupID;    // 0x360
        GJLevelType m_eLevelType;   // 0x364
        int m_nM_ID;                // 0x368
        std::string m_sTempName;    // 0x36c
        std::string m_sCapacityString;  // 0x384
        bool m_bHighObjectsEnabled;     // 0x39c
        std::string m_sPersonalBests;   // 0x3a0

        void dataLoaded(DS_Dictionary* dict) {
            return reinterpret_cast<void(__thiscall*)(
                GJGameLevel*, DS_Dictionary*
            )>( base + 0xbded0 )(this, dict);
        }

        // this function is inlined on pc builds
        static GJGameLevel* create() {
            return reinterpret_cast<GJGameLevel*(__stdcall*)()>(
                base + 0xbd2b0
            )();
        }

        static GJGameLevel* createWithCoder(DS_Dictionary* dict) {
            auto ret = GJGameLevel::create();
            ret->dataLoaded(dict);
            return ret;
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

        GJDifficulty getAverageDifficulty() {
            return reinterpret_cast<GJDifficulty(__thiscall*)(
                GJGameLevel*
            )>( base + 0xbd9b0 )(this);
        }
};

}

#endif
