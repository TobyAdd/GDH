#ifndef __COLORCHANNELSPRITE_H__
#define __COLORCHANNELSPRITE_H__

#include <gd.h>

namespace gd {
    class ColorAction;

    class GDH_DLL ColorChannelSprite : public cocos2d::CCSprite {
        public:
            cocos2d::CCLabelBMFont* m_pCopyLabel;    // 0x1e4
            cocos2d::CCLabelBMFont* m_pOpacityLabel; // 0x1e8
            cocos2d::CCLabelBMFont* m_pBlendingDot;  // 0x1ec
        
            static ColorChannelSprite* create() {
                return reinterpret_cast<ColorChannelSprite*(__stdcall*)()>(
                    base + 0x114340
                )();
            }

            void updateValues(ColorAction* action) {
                reinterpret_cast<void(__thiscall*)(ColorChannelSprite*, ColorAction*)>(
                    base + 0x114800
                )(this, action);
            }
            void updateOpacity(float opacity) {
                __asm movss xmm1, opacity

                reinterpret_cast<void(__thiscall*)(ColorChannelSprite*)>(
                    base + 0x1145c0
                )(this);
            }
            void updateCopyLabel(int channel, bool unk) {
                reinterpret_cast<void(__thiscall*)(ColorChannelSprite*, int, bool)>(
                    base + 0x114440
                )(this, channel, unk);
            }
            void updateBlending(bool enabled) {
                reinterpret_cast<void(__thiscall*)(ColorChannelSprite*, bool)>(
                    base + 0x114710
                )(this, enabled);
            }
    };
}

#endif
