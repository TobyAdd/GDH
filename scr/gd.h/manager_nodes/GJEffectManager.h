#ifndef __GJEFFECTMANAGER_H__
#define __GJEFFECTMANAGER_H__

#include <gd.h>

namespace gd {
    class ColorAction;
    class GDH_DLL GJEffectManager : public cocos2d::CCNode {
    public:
        PAD(4);
        cocos2d::CCDictionary* m_colorActionDict; // 0xF0

        ColorAction* getColorAction(int num) {
            return reinterpret_cast<ColorAction*(__thiscall*)(
                GJEffectManager*, int
            )>(
                base + 0x11cde0
            )(
                this, num
            );
        }
    };
}

#endif