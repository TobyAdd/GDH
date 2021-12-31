#ifndef __LEVELSEARCHLAYER_H__
#define __LEVELSEARCHLAYER_H__

#include <gd.h>

namespace gd {
    class GDH_DLL LevelSearchLayer : public cocos2d::CCLayer {
        public:
            static LevelSearchLayer* create() {
                return reinterpret_cast<LevelSearchLayer*(__fastcall*)()>(
                    base + 0x17d9c0
                )();
            }
    };
}

#endif
