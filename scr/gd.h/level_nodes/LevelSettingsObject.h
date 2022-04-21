#ifndef __LEVELSETTINGSOBJECT_H__
#define __LEVELSETTINGSOBJECT_H__

#include <gd.h>

namespace gd {
    class GJEffectManager;
    class GJGameLevel;

    enum Gamemode {
        kGamemodeCube = 0,
        kGamemodeShip = 1,
        kGamemodeBall = 2,
        kGamemodeUfo = 3,
        kGamemodeWave = 4,
        kGamemodeRobot = 5,
        kGamemodeSpider = 6,
    };

    enum Speed {
        kSpeedNormal = 0,
        kSpeedSlow = 1,
        kSpeedFast = 2,
        kSpeedFaster = 3,
        kSpeedFastest = 4,
    };

    class GDH_DLL LevelSettingsObject : public cocos2d::CCNode {
        public:
            GJEffectManager* m_pEffectManager;  // 0xec
            Gamemode m_gamemode; // 0xf0
            Speed m_speed; // 0xf4
            bool m_bMiniMode; // 0xf8
            bool m_bTwoPlayerMode;  // 0xf9
            PAD(23);
            bool m_bFlipped; // 0x111
            PAD(2);
            GJGameLevel* m_pLevel;  // 0x114
            std::string m_sUnknownStr1; // 0x118

            std::string getSaveString() {
                std::string res;

                reinterpret_cast<void(__thiscall*)(LevelSettingsObject*, std::string*)>(
                    base + 0x16ebf0
                )(this, &res);

                return res;
            }
    };
}

#endif
