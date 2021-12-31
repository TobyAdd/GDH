#ifndef __SETUPPULSEPOPUP_H__
#define __SETUPPULSEPOPUP_H__

#include <gd.h>

namespace gd {
    class FLAlertLayer;
    class GDH_DLL SetupPulsePopup : public FLAlertLayer, ColorPickerDelegate, TextInputDelegate, GJSpecialColorSelectDelegate {
    public:
        cocos2d::extension::CCControlColourPicker* colorPicker;
        PAD(48);
        cocos2d::CCSprite* currentColorSpr;
        cocos2d::CCSprite* prevColorSpr;
        PAD(100);
        int pulseMode;

        virtual void colorValueChanged(cocos2d::ccColor3B color) {}
        virtual void idfk() {}
        virtual void idfk2() {}
    };
}

#endif