#ifndef __GJITEMICON_H__
#define __GJITEMICON_H__

#include <gd.h>

namespace gd {
    class GJItemIcon : public cocos2d::CCSprite {
        public:
            static GJItemIcon* create(
                UnlockType _type,
                int _id,
                cocos2d::_ccColor3B _col1,
                cocos2d::_ccColor3B _col2,
                bool _un0,
                bool _un1,
                bool _un2,
                cocos2d::_ccColor3B _col3
            ) {
                auto ret = reinterpret_cast<GJItemIcon*(__fastcall*)(
                    UnlockType, int,
                    cocos2d::_ccColor3B,
                    cocos2d::_ccColor3B,
                    bool, bool, bool,
                    cocos2d::_ccColor3B
                )>(
                    base + 0x12cbf0
                )(
                    _type, _id, _col1, _col2, _un0, _un1, _un2, _col3
                );

                __asm add esp, 0x18

                return ret;
            }

            static GJItemIcon* createBrowserIcon(UnlockType _type, int _id) {
                return GJItemIcon::create(_type, _id,
                    { 0xaf, 0xaf, 0xaf }, { 0xff, 0xff, 0xff },
                    false, true, true,
                    { 0xff, 0xff, 0xff }
                );
            }
    };
}

#endif
