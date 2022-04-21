#ifndef __GARAGEPAGE_H__
#define __GARAGEPAGE_H__

#include <gd.h>

namespace gd {
    class GJGarageLayer;

    class GDH_DLL GaragePage : public cocos2d::CCLayer, ListButtonBarDelegate {
        public:
            GJGarageLayer* m_pGarage;
            cocos2d::SEL_MenuHandler m_pHandler;
            cocos2d::CCSprite* m_pSelectSprite;
            CCMenuItemSpriteExtra* m_pUnkNode0x12c;
            IconType m_nType;
            int m_nUnknown;
            PAD(4)

            virtual void listButtonBarSwitchedPage(ListButtonBar* bar, int idk) override {
                reinterpret_cast<void(__thiscall*)(GaragePage*, ListButtonBar*, int)>(
                    base + 0x12bb40
                )(this, bar, idk);
            }

            GaragePage() {
                reinterpret_cast<void(__thiscall*)(GaragePage*)>(
                    base + 0x1252f0
                )(this);
            }

            bool init(IconType type, GJGarageLayer* pGarage, cocos2d::SEL_MenuHandler pSelectCallback) {
                return reinterpret_cast<bool(__thiscall*)(GaragePage*, IconType, GJGarageLayer*, cocos2d::SEL_MenuHandler)>(
                    base + 0x12af70
                )(this, type, pGarage, pSelectCallback);
            }

            static GaragePage* create(IconType type, GJGarageLayer* pGarage, cocos2d::SEL_MenuHandler pSelectCallback) {
                auto pRet = new GaragePage();

                if (pRet && pRet->init(type, pGarage, pSelectCallback)) {
                    pRet->autorelease();
                    return pRet;
                }

                CC_SAFE_DELETE(pRet);
                return nullptr;
            }
    };
}

#endif
