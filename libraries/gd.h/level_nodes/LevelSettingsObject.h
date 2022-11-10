#ifndef __LEVELSETTINGSOBJECT_H__
#define __LEVELSETTINGSOBJECT_H__

#include <gd.h>

namespace gd {
    class GJEffectManager;
    class GJGameLevel;

    class LevelSettingsObject : public cocos2d::CCNode {
        public:
            GJEffectManager* m_pEffectManager;  // 0xec
            PAD(9);
            bool m_bTwoPlayerMode;  // 0xf9
            PAD(26);
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
