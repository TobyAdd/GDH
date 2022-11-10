#ifndef __GJSCALECONTROL_H__
#define __GJSCALECONTROL_H__

#include <gd.h>

namespace gd {
    class Slider;
    class GJScaleControlDelegate;

    class GJScaleControl : public cocos2d::CCLayer {
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
    };

}

#endif
