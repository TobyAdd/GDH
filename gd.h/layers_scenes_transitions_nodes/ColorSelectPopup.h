#ifndef __COLORSELECTPOPUP_H__
#define __COLORSELECTPOPUP_H__

#include <gd.h>

namespace gd {
    class FLAlertLayer;
    class ColorPickerDelegate;
    class TextInputDelegate;
    class GJSpecialColorSelectDelegate;
    class Slider;
    class EffectGameObject;
    class GJColorSetupLayer;
    class ColorAction;
    class CCTextInputNode;
    class ConfigureHSVWidget;
    class CCMenuItemToggler;

    class GDH_DLL ColorSelectPopup : public FLAlertLayer, ColorPickerDelegate, TextInputDelegate, GJSpecialColorSelectDelegate {
    public:
        // TODO: actually figure out most of this stuff, this is a mess
        cocos2d::extension::CCControlColourPicker* m_colorPicker; // 0x01D8
        cocos2d::CCLabelBMFont* m_unk1DC;
        cocos2d::CCLabelBMFont* m_label; // 0x01E0
        Slider* m_unk1E4;
        Slider* m_slider; // 0x01E8
        EffectGameObject* m_effectGameObject; // 0x01EC
        cocos2d::CCArray* m_unk1F0;
        CCMenuItemToggler* m_toggler1; // 0x01F4
        CCMenuItemToggler* m_toggler2; // 0x01F8
        unsigned rgbLastColor; // 0x01FC
        cocos2d::CCSprite* m_unk200;
        cocos2d::CCSprite* m_unk204;
        unsigned bgrColor; // 0x0208
        GJColorSetupLayer* colorSetupLayer; // 0x020C
        float fadeTime; // 0x0210
        int playerColor; // 0x0214
        bool isBlending; // 0x0218
        PAD(3);
        float opacity; // 0x021C
        ColorAction* colorAction; // 0x0220
        CCTextInputNode* m_textInput1;
        bool m_unk228;
        bool touchTrigger; // 0x0229
        bool m_unk22A;
        bool isMultipleColorTrigger; // 0x022B
        bool m_unk22C;
        bool isColorTrigger; // 0x022D
        PAD(2);
        int colorID; // 0x0230
        bool m_unk234;
        PAD(3);
        int copyChannelID; // 0x0238
        bool copyOpacity; // 0x023C
        PAD(3);
        ConfigureHSVWidget* configurehsvwidget; // 0x0240
        PAD(16);
        cocos2d::CCArray* m_unk254;
        cocos2d::CCArray* m_unk258;
        CCTextInputNode* m_textInput2; // 0x025C
        PAD(4);
        CCMenuItemToggler* m_toggler3;
        CCMenuItemToggler* m_toggler4;
        PAD(8);
        cocos2d::CCArray* m_unk274;
        bool spawnTrigger; // 0x0278
        bool multiTrigger; // 0x0279
        bool copyColor; // 0x027A
        PAD(1);

        virtual void colorValueChanged(cocos2d::ccColor3B color) {
            reinterpret_cast<void(__thiscall*)(ColorSelectPopup*, cocos2d::ccColor3B)>(
                base + 0x46ee0
            )(this, color);
        }
        virtual void idfk() {}
        virtual void idfk2() {}

        auto getAlertLayer() { return m_pLayer; }
        auto getPickerColor() { return *reinterpret_cast<cocos2d::ccColor3B*>(reinterpret_cast<uintptr_t>(m_colorPicker) + 0x144); }
        void setPickerColor(cocos2d::ccColor3B color) {
            // fod spent like 5 hours trying to get this working on cocos-headers, so fuck it
            const static auto address = GetProcAddress(
                GetModuleHandleA("libExtensions.dll"),
                "?setColorValue@CCControlColourPicker@extension@cocos2d@@UAEXABU_ccColor3B@3@@Z"
            );
            reinterpret_cast<void (__thiscall*)(cocos2d::extension::CCControlColourPicker*, const cocos2d::ccColor3B&)>(address)(m_colorPicker, color);
        }
    };
}

#endif