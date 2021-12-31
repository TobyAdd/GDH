#ifndef __GJSPRITECOLOR_H__
#define __GJSPRITECOLOR_H__

#include <gd.h>

namespace gd {
    class GDH_DLL GJSpriteColor : public cocos2d::CCNode {
    public:
        int colorID; //0x00EC
        int defaultColorID; //0x00F0
        float unk_0F4;
        float hue; //0x00F8
        float saturation; //0x00FC
        float brightness; //0x0100
        bool saturationChecked; //0x0104
        bool brightnessChecked; //0x0105
        PAD(2);
        bool unk_108;
        PAD(3);
        float unk_10C;
        bool unk_110;
        PAD(3);
    };
}

#endif