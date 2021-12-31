#ifndef __KEYBINDINGSLAYER_H__
#define __KEYBINDINGSLAYER_H__

#include <gd.h>

namespace gd {

    class GDH_DLL KeybindingsLayer : public FLAlertLayer {
        public:
            int m_nCurrentPage;
            int m_nItemCount;
            int m_nPageCount;
            cocos2d::CCDictionary* m_pPages;
            cocos2d::CCDictionary* m_pUnused;
            cocos2d::CCNode* m_pLeftArrow;
            cocos2d::CCNode* m_pRightArrow;

            void onClose(cocos2d::CCObject* pSender) {
                reinterpret_cast<void(__thiscall*)(KeybindingsLayer*, cocos2d::CCObject*)>(
                    base + 0x49c60
                )(this, pSender);
            }

            void onPrevPage(cocos2d::CCObject* pSender) {
                reinterpret_cast<void(__thiscall*)(KeybindingsLayer*, cocos2d::CCObject*)>(
                    base + 0x153cd0
                )(this, pSender);
            }

            void onNextPage(cocos2d::CCObject* pSender) {
                reinterpret_cast<void(__thiscall*)(KeybindingsLayer*, cocos2d::CCObject*)>(
                    base + 0x153cc0
                )(this, pSender);
            }

            void goToPage(int page) {
                reinterpret_cast<void(__thiscall*)(KeybindingsLayer*, int)>(
                    base + 0x153ce0
                )(this, page);
            }
    };

}

#endif
