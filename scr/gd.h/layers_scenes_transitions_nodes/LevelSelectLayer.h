#ifndef __LEVELSELECTLAYER_H__
#define __LEVELSELECTLAYER_H__

#include <gd.h>

namespace gd {
    class GDH_DLL LevelSelectLayer : public cocos2d::CCLayer {
        public:
            PAD(16)
            BoomScrollLayer* m_pScrollLayer;    // 0x130

            static LevelSelectLayer* create(int lvl) {
                return reinterpret_cast<LevelSelectLayer*(__fastcall*)(int)>(
                    base + 0x185500
                )(lvl);
            }
    };
}

#endif
