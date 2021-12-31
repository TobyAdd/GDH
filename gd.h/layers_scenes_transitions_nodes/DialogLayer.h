#ifndef __DIALOGLAYER_H__
#define __DIALOGLAYER_H__

#include <gd.h>

namespace gd {
    class GDH_DLL DialogLayer : public cocos2d::CCLayerColor {
        public:
            char pad1[0x8];
            cocos2d::CCLayer *m_pLayer;
            cocos2d::CCLabelBMFont *m_pTitle;
            TextArea *m_pDialog;
            cocos2d::CCSprite *m_pIcon;
            cocos2d::CCArray *m_pDialogObjects;
            char PAD2[0X4];
            cocos2d::CCSprite *m_pChatBtn;
            bool m_bUnk;
            char m_bCanSkip;
            char PAD3[0x2];
            DWORD m_pUnk;
            char PAD4[0x8];
    };
}

#endif
