#ifndef __OPTIONSLAYER_H__
#define __OPTIONSLAYER_H__

#include <gd.h>

namespace gd {
    class GDH_DLL OptionsLayer : public GJDropDownLayer {
        public:
            static OptionsLayer* create() {
                return reinterpret_cast<OptionsLayer*(__fastcall*)()>(
                    base + 0x1dd310
                )();
            }

            static OptionsLayer* addToCurrentScene(bool noTransition = false) {
                auto layer = OptionsLayer::create();
                cocos2d::CCDirector::sharedDirector()
                    ->getRunningScene()
                    ->addChild(layer);
                layer->showLayer(noTransition);
                return layer;
            }
    };
}

#endif
