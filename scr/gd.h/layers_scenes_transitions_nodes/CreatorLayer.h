#ifndef __CREATORLAYER_H__
#define __CREATORLAYER_H__

#include <gd.h>

namespace gd {
    class GDH_DLL CreatorLayer : public cocos2d::CCLayer {
        public:
            static CreatorLayer* create() {
                return reinterpret_cast<CreatorLayer*(__fastcall*)()>(
                    base + 0x4dda0
                )();
            }
    };
}

#endif
