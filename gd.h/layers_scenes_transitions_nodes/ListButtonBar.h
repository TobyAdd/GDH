#ifndef __LISTBUTTONBAR_H__
#define __LISTBUTTONBAR_H__

#include <gd.h>

namespace gd {
    class BoomScrollLayer;

    class GDH_DLL ListButtonPage : public cocos2d::CCLayer {
        // todo
    };

    class GDH_DLL ListButtonBar : public cocos2d::CCNode {
        public:
            BoomScrollLayer* m_pScrollLayer;    // 0xec
    };
}

#endif
