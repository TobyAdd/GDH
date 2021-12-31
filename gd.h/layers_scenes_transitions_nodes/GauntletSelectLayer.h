#ifndef __GAUNTLETSELECTLAYER_H__
#define __GAUNTLETSELECTLAYER_H__

#include <gd.h>

namespace gd {
    class GauntletSelectLayer : public cocos2d::CCLayer {
        public:
            static GauntletSelectLayer* create() {
                return reinterpret_cast<GauntletSelectLayer*(__fastcall*)()>(
                    base + 0x105120
                )();
            }
    };
}

#endif
