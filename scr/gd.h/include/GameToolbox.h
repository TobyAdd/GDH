#ifndef __GAMETOOLBOX_H__
#define __GAMETOOLBOX_H__

#include <gd.h>

namespace gd {
    namespace GameToolbox {
        static CCMenuItemToggler* createToggleButton(
            cocos2d::SEL_MenuHandler onToggle,
            bool bToggled,
            cocos2d::CCMenu* pToggleMenu,
            cocos2d::CCNode* pTarget,
            cocos2d::CCNode* pLabelParent,
            float fCheckboxScale,
            float fLabelSize,
            float fMaxWidth,
            const char* sIdk,
            bool bIdk,
            int nTag,
            cocos2d::CCArray* pToggleArray,
            std::string sText,
            cocos2d::CCPoint obPosition,
            cocos2d::CCPoint obLabelOffset
        ) {
            auto pRet = reinterpret_cast<CCMenuItemToggler*(__fastcall*)(
                cocos2d::SEL_MenuHandler, bool, cocos2d::CCMenu*, cocos2d::CCNode*, cocos2d::CCNode*,
                float, float, float, const char*, bool, int, cocos2d::CCArray*, std::string,
                cocos2d::CCPoint, cocos2d::CCPoint
            )>(
                base + 0x25fe0
            )(
                onToggle, bToggled, pToggleMenu, pTarget,
                pLabelParent, fCheckboxScale, fLabelSize, fMaxWidth,
                sIdk, bIdk, nTag, pToggleArray, sText, obPosition, obLabelOffset
            );

            __asm add esp, 0x50

            return pRet;
        }

        static void transformColor(cocos2d::ccColor3B const& src, cocos2d::ccColor3B & dest, cocos2d::ccHSVValue hsv) {
            reinterpret_cast<void(__thiscall*)(
                cocos2d::ccColor3B const*, cocos2d::ccColor3B*, cocos2d::ccHSVValue
            )>(
                base + 0x26a60
            )(
                &src, &dest, hsv
            );

            __asm add esp, 0x14
        }
    }
}

#endif
