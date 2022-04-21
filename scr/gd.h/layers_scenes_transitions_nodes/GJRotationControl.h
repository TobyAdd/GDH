#ifndef __GJROTATIONCONTROL_H__
#define __GJROTATIONCONTROL_H__

#include <gd.h>

namespace gd {

    class GDH_DLL GJRotationControl : public cocos2d::CCLayer {
        public:
            float m_nUnknown0;
            float m_nUnknown1;
            cocos2d::CCPoint m_obSliderPosition;
            cocos2d::CCSprite* m_pSliderThumb;
            float m_fObjAngle;
            float m_fAngle;
            int m_nTouchID;

            void setAngle(float angle) {
                this->m_obSliderPosition = cocos2d::CCPoint { sinf(angle) * 60.0f, cosf(angle) * 60.0f };
                this->m_fAngle = angle;

                this->m_pSliderThumb->setPosition(this->m_obSliderPosition);
            }

            void updateSliderPosition(cocos2d::CCPoint const& pos) {
                reinterpret_cast<void(__thiscall*)(
                    GJRotationControl*, cocos2d::CCPoint
                )>(
                    base + 0x94020
                )(this, pos);
            }
    };
    
}

#endif
