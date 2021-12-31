#ifndef __LEVELBROWSERLAYER_H__
#define __LEVELBROWSERLAYER_H__

#include <gd.h>

namespace gd {
    class GJSearchObject;

    class GDH_DLL LevelBrowserLayer : public cocos2d::CCLayer {
        public:
            PAD(24)
            GJListLayer* m_pList;   // 0x134

            static LevelBrowserLayer* create(gd::GJSearchObject* pSearch) {
                return reinterpret_cast<LevelBrowserLayer*(__fastcall*)(
                    gd::GJSearchObject*
                )>(
                    gd::base + 0x159fa0
                )(
                    pSearch
                );
            }

            static void scene(gd::GJSearchObject* pSearch) {
                auto pScene = cocos2d::CCScene::create();

                pScene->addChild(gd::LevelBrowserLayer::create(pSearch));

                cocos2d::CCDirector::sharedDirector()->replaceScene(
                    cocos2d::CCTransitionFade::create(.5f, pScene)
                );
            }
    };
}

#endif
