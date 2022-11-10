#ifndef __LEVELBROWSERLAYER_H__
#define __LEVELBROWSERLAYER_H__

#include <gd.h>

namespace gd {
    class GJSearchObject;

    class LevelBrowserLayer : public cocos2d::CCLayer {
        public:
            static LevelBrowserLayer* create(gd::GJSearchObject* pSearch) {
                return reinterpret_cast<LevelBrowserLayer*(__fastcall*)(
                    gd::GJSearchObject*
                )>(
                    gd::base + 0x159fa0
                )(
                    pSearch
                );
            }
    };
}

#endif
