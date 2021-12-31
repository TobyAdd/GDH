#ifndef __EFFECTGAMEOBJECT_H__
#define __EFFECTGAMEOBJECT_H__

#include <gd.h>

namespace gd {

    enum EasingType {
        kEasingTypeNone = 0,
        kEasingTypeEaseInOut = 1,
        kEasingTypeEaseIn = 2,
        kEasingTypeEaseOut = 3,
        kEasingTypeElasticInOut = 4,
        kEasingTypeElasticIn = 5,
        kEasingTypeElasticOut = 6,
        kEasingTypeBounceInOut = 7,
        kEasingTypeBounceIn = 8,
        kEasingTypeBounceOut = 9,
        kEasingTypeExponentialInOut = 10,
        kEasingTypeExponentialIn = 11,
        kEasingTypeExponentialOut = 12,
        kEasingTypeSineInOut = 13,
        kEasingTypeSineIn = 14,
        kEasingTypeSineOut = 15,
        kEasingTypeBackInOut = 16,
        kEasingTypeBackIn = 17,
        kEasingTypeBackOut = 18,
    };

    enum ComparisonType {
        kComparisonTypeEquals = 0,
        kComparisonTypeLarger = 1,
        kComparisonTypeSmaller = 2,
    };

    enum MoveTargetType {
        kMoveTargetTypeBoth = 0,
        kMoveTargetTypeXOnly = 1,
        kMoveTargetTypeYOnly = 2,
    };

    enum TouchToggleMode {
        kTouchToggleModeNormal = 0,
        kTouchToggleModeToggleOn = 1,
        kTouchToggleModeToggleOff = 2,
    };

    class GDH_DLL EffectGameObject : public GameObject {
        public:
            cocos2d::ccColor3B m_colColor; // 0x468
            float m_fDuration; // 0x46c - Also Move Time, Duration etc.
            float m_fOpacity; // 0x470
            int m_nTargetGroupID; // 0x0474
            int m_nCenterGroupID; // 0x0478
            float m_fShakeStrength; //0x047C
            float m_fShakeInterval; //0x0480
	        bool m_bTintGround;
            bool m_bPlayerColor1; //0x0485
            bool m_bPlayerColor2; //0x0486
            bool m_bBlending; //0x0487
	        cocos2d::CCPoint m_obMove; // multiplied by 3
            EasingType m_nEasingType; //0x0490
            float m_fEasingRate; //0x0494
            bool m_bLockToPlayerX; //0x0498
            bool m_bLockToPlayerY; //0x0499
            bool m_bUseTarget; //0x049A
            MoveTargetType m_nMoveTargetType; //0x049C
            int m_nRotateDegrees; //0x04A0
            int m_nTimes360; //0x04A4
            bool m_bLockObjectRotation; //0x04A8
	        cocos2d::CCPoint m_obFollowMod; // 0x04AC
	        bool UndocuementedLevelProperty74;  // 0x04B4
            float m_fFollowYSpeed; //0x04B8
            float m_fFollowYDelay; //0x04BC
            int m_nFollowYOffset; //0x04C0 - Multiplied by 3
            float m_fFollowYMaxSpeed; //0x04C4
            float m_fFadeInTime; //0x04C8
            float m_fHoldTime; //0x04CC
            float m_fFadeOutTime; //0x04D0
            bool m_bPulseHSVMode; //0x04D4
            PAD(3)
            bool m_bPulseGroupMode; //0x04D8
            PAD(3)
            cocos2d::ccHSVValue m_obHSVValue; // 0x04dc
            int m_nCopyColorID; //0x04EC
            bool m_bCopyOpacity; //0x04F0
            bool m_bPulseMainOnly; //0x04F1
            bool m_bPulseDetailOnly; //0x04F2
            bool m_bPulseExclusive; //0x04F3
            bool m_bActivateGroup; //0x04F4
            bool m_bTouchHoldMode; //0x04F5
            TouchToggleMode m_nTouchToggleMode; //0x04F8
            bool m_bTouchDualMode; //0x04FC
            int m_nAnimationID; //0x0500
            float m_fSpawnDelay; //0x0504
	        cocos2d::CCPoint m_obSpawnPosition;
            bool m_bMultiTrigger; //0x0510
            bool m_bEditorDisabled; //0x0511
            int m_nTargetCount; //0x0514
	        bool m_bSubtractCount;
            ComparisonType m_nComparisonType; //0x051C
            bool m_bMultiActivate; //0x0520
            bool m_bTriggerOnExit; //0x0521
            int m_nBlockBID; //0x0524
            bool m_bDynamicBlock; //0x0528
            int m_nTargetItemID; //0x052C
        	int m_ePickupMode;
            PAD(36)

            static EffectGameObject* create(const char* sprName) {
                return reinterpret_cast<EffectGameObject*(__fastcall*)(const char*)>(
                    base + 0x253c30
                )(sprName);
            }

            std::string getSaveString() {
                std::string ret;

                reinterpret_cast<void(__thiscall*)(EffectGameObject*, std::string*)>(
                    base + 0x257560
                )(
                    this, &ret
                );

                return ret;
            }

            void updateLabel() {
                auto label = reinterpret_cast<cocos2d::CCLabelBMFont*>(this->getChildByTag(999));

                if (label) {
                    switch (this->m_nObjectID) {
                        // instant count
                        case 0x713:
                            label->setString(
                                cocos2d::CCString::createWithFormat("%i", this->m_nTargetItemID)->getCString()
                            );
                            break;
                        
                        //   color,    pulse
                        case 899: case 1006: {
                            int target = this->m_nObjectID == 1006 ? m_nTargetGroupID : m_nTargetColorID;
                            if (target > 999) {
                                label->setString(GJSpecialColorSelect::textForColorIdx(target));
                            } else {
                                label->setString(
                                    cocos2d::CCString::createWithFormat("%i", target)->getCString()
                                );
                            }
                            label->limitLabelWidth(30.0f, 0.5f, 0.0f);
                            } break;
                        

                        default:
                            label->setString(
                                cocos2d::CCString::createWithFormat("%i", this->m_nTargetGroupID)->getCString()
                            );
                    }
                }
            }
    };
}

#endif
