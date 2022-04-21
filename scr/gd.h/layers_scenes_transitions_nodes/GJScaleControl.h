#ifndef __GJSCALECONTROL_H__
#define __GJSCALECONTROL_H__

#include <gd.h>

namespace gd {
    class Slider;
    class GJScaleControlDelegate;

    class GDH_DLL GJScaleControl : public cocos2d::CCLayer {
        public:
            gd::Slider* m_pSlider;
            unsigned int m_nTouchID;
            float m_fValue;
            PAD(0x4)
            cocos2d::CCLabelBMFont* m_pLabel;
            gd::GJScaleControlDelegate* m_pDelegate;

            void updateLabel(float value) {
                __asm movss xmm0, value

                reinterpret_cast<void(__fastcall*)(GJScaleControl*)>(
                    base + 0x94990
                )(this);
            }

            void loadValues(GameObject* obj, cocos2d::CCArray* objs) {
                reinterpret_cast<void(__thiscall*)(
                    GJScaleControl*, GameObject*, cocos2d::CCArray*
                )>(
                    base + 0x94590
                )(
                    this, obj, objs
                );
            }
    };

}

#endif
