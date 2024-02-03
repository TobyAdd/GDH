#pragma once
#include "SeedValue.h"

namespace robtop
{
    enum class GJDifficulty
    {
        kGJDifficultyAuto,
        kGJDifficultyEasy,
        kGJDifficultyNormal,
        kGJDifficultyHard,
        kGJDifficultyHarder,
        kGJDifficultyInsane,
        kGJDifficultyDemon,
        kGJDifficultyDemonEasy,
        kGJDifficultyDemonMedium,
        kGJDifficultyDemonInsane,
        kGJDifficultyDemonExtreme
    };

    enum class GJLevelType
    {
        kGJLevelTypeLocal = 1,
        kGJLevelTypeEditor,
        kGJLevelTypeSaved
    };

    // 2.200
    struct GJGameLevel_200
    {
        SeedValueRS m_isVerified;
        bool m_isVerifiedRaw;
        bool m_isUploaded;
        bool m_hasBeenModified;
        int m_levelVersion;
        int m_gameVersion;

        SeedValueRSV m_attempts;
        SeedValueRSV m_jumps;
        SeedValueRSV m_clicks;
        SeedValueRSV m_attemptTime;
        int m_chk;
        bool m_isChkValid;
        bool m_isCompletionLegitimate;
        SeedValueVSR m_normalPercent;
        SeedValueRSV m_orbCompletion;
        SeedValueRSV m_newNormalPercent2;
        int m_practicePercent;
        int m_likes;
        int m_dislikes;
        int m_levelLength;
        int m_featured;
        int m_isEpic;
        bool m_levelFavorited;
        int m_levelFolder;
        SeedValueRSV m_dailyID;
        SeedValueRSV m_demon;
        int m_demonDifficulty;
        SeedValueRSV m_stars;
        bool m_autoLevel;
        int m_coins;
        SeedValueRSV m_coinsVerified;
        SeedValueRS m_password;
        SeedValueRSV m_originalLevel;
        bool m_twoPlayerMode;
        int m_failedPasswordAttempts;
        SeedValueRSV m_firstCoinVerified;
        SeedValueRSV m_secondCoinVerified;
        SeedValueRSV m_thirdCoinVerified;
        int m_starsRequested;
        bool m_showedSongWarning;
        int m_starRatings;
        int m_starRatingsSum;
        int m_maxStarRatings;
        int m_minStarRatings;
        int m_demonVotes;
        int m_rateStars;
        int m_rateFeature;
        std::string m_rateUser;
        bool m_dontSave;
        bool m_levelNotDownloaded;
        int m_requiredCoins;
        bool m_isUnlocked;
        cocos2d::CCPoint m_lastCameraPos;
        float m_fastEditorZoom;
        int m_lastBuildTab;
        int m_lastBuildPage;
        int m_lastBuildGroupID;
        GJLevelType m_levelType;
        int m_M_ID;
        std::string m_tempName;
        std::string m_capacityString;
        bool m_highObjectsEnabled;
        bool m_unlimitedObjectsEnabled;
        std::string m_personalBests;
        int m_timestamp;
        int m_unkInt;
        std::string m_songIDs;
        std::string m_sfxIDs;
        int m_54;
        int m_bestTime;
        int m_bestPoints;
        int m_k111;
        std::string m_unkString3;
        std::string m_unkString4;
    };

    // 2.201+
    struct GJGameLevel_201
    {
        bool m_disableShake; // this was added in 2.201
        GJGameLevel_200 tail;
    };

    class GJGameLevel
    {
    public:
        inline bool is_auto_level()
        {
            return v201.tail.m_autoLevel;
        }
        inline bool is_demon()
        {
            return v201.tail.m_demon.value() > 0;
        }
        inline int get_demon_difficulty()
        {
            return v201.tail.m_demonDifficulty;
        }
        inline int get_length()
        {
            return v201.tail.m_levelLength;
        }
        inline int get_stars()
        {
            return v201.tail.m_stars.value();
        }
        inline int get_normal_percent()
        {
            return v201.tail.m_normalPercent.value();
        }

        ~GJGameLevel() {}

    private:
        char ccnode_pad[0x108];

    public:
        cocos2d::CCDictionary *m_lastBuildSave;
        SeedValueRSV m_levelID;
        std::string m_levelName;
        std::string m_levelDesc;
        std::string m_levelString;
        std::string m_creatorName;
        std::string m_recordString;
        std::string m_uploadDate;
        std::string m_updateDate;
        std::string m_unkString1;
        std::string m_unkString2;

        cocos2d::CCPoint m_unkPoint;
        SeedValueRSV m_userID;
        SeedValueRSV m_accountID;
        GJDifficulty m_difficulty;
        int m_audioTrack;
        int m_songID;
        int m_levelRev;
        bool m_unlisted;
        bool m_friendsOnly;
        SeedValueRSV m_objectCount;
        int m_levelIndex;
        int m_ratings;
        int m_ratingsSum;
        int m_downloads;
        bool m_isEditable;
        bool m_gauntletLevel;
        bool m_gauntletLevel2;
        int m_workingTime;
        int m_workingTime2;
        bool m_lowDetailMode;
        bool m_lowDetailModeToggled;
        bool m_selected;
        bool m_localOrSaved;

        union
        {
            GJGameLevel_200 v200;
            GJGameLevel_201 v201;
        };
    };
}