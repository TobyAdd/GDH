#ifndef __CUSTOMIZEOBJECTLAYER_H__
#define __CUSTOMIZEOBJECTLAYER_H__

#include <gd.h>

namespace cocos2d {
    namespace extension {
        class CCScale9Sprite;
    }
}

namespace gd {
    class EffectGameObject;
    class ColorChannelSprite;
    
    class GDH_DLL CustomizeObjectLayer :
        public FLAlertLayer,
        public TextInputDelegate,
        public HSVWidgetPopupDelegate,
        public ColorSelectDelegate,
        public ColorSetupDelegate
    {
        public:
            EffectGameObject* m_pTargetObject;  // 0x1dc
            cocos2d::CCArray* m_pTargetObjects; // 0x1e0
            cocos2d::CCArray* m_pColorButtons; // 0x1e4
            cocos2d::CCArray* m_pColorNodes; // 0x1e8
            cocos2d::CCArray* m_pTextInputNodes; // 0x1ec
            PAD(4)
            cocos2d::CCArray* m_pDetailColorButtons; // 0x1f4
            int m_nSelectedMode;    // 0x1f8
            int m_nCustomColorChannel;  // 0x1fc
            bool m_bUnk0x200;   // 0x200
            bool m_bUnk0x201;   // 0x201
            bool m_bGlowDisabled; // 0x202
            PAD(1)
            CCMenuItemSpriteExtra* m_pBaseButton; // 0x204
            CCMenuItemSpriteExtra* m_pDetailButton; // 0x208
            CCMenuItemSpriteExtra* m_pTextButton; // 0x20c
            CCMenuItemSpriteExtra* m_pBaseColorHSV; // 0x210
            CCMenuItemSpriteExtra* m_pDetailColorHSV; // 0x214
            cocos2d::CCLabelBMFont* m_pTitleLabel;   // 0x218
            cocos2d::CCLabelBMFont* m_pSelectedColorLabel;   // 0x21c
            CCTextInputNode* m_pCustomColorInput;    // 0x220
            CCTextInputNode* m_pTextInput;    // 0x224
            ButtonSprite* m_pCustomColorButtonSprite; // 0x228
            CCMenuItemSpriteExtra* m_pCustomColorButton; // 0x22c
            CCMenuItemSpriteExtra* m_pArrowDown; // 0x230
            CCMenuItemSpriteExtra* m_pArrowUp; // 0x234
            cocos2d::extension::CCScale9Sprite* m_pCustomColorInputBG;
            ColorChannelSprite* m_pColorSprite; // 0x23c
            CCMenuItemSpriteExtra* m_pColorSpriteButton; // 0x240
            bool m_bShowTextInput;  // 0x244
            bool m_bCustomColorSelected;  // 0x245

            void onNextColorChannel(cocos2d::CCObject* pSender) {
                reinterpret_cast<void(__thiscall*)(CustomizeObjectLayer*, cocos2d::CCObject*)>(
                    base + 0x56c80
                )(this, pSender);
            }
            void onSelectColor(cocos2d::CCObject* pSender) {
                reinterpret_cast<void(__thiscall*)(CustomizeObjectLayer*, cocos2d::CCObject*)>(
                    base + 0x577b0
                )(this, pSender);
            }

            int getActiveMode(bool unknown) {
                return reinterpret_cast<int(__thiscall*)(CustomizeObjectLayer*, bool)>(
                    base + 0x57210
                )(this, unknown);
            }
    };
}

#endif
