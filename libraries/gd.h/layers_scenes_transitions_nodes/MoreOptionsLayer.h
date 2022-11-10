#ifndef __MOREOPTIONSLAYER_H__
#define __MOREOPTIONSLAYER_H__

#include <gd.h>

namespace gd {
    class MoreOptionsLayer : public cocos2d::CCLayer {
    public:
        void addToggle(const char* name, const char* key, const char* info) {
            reinterpret_cast<void*(__thiscall*)(MoreOptionsLayer*, const char*, const char*, const char*)>(base + 0x1DF6B0)(this, name, key, info);
        }
    };
}

#endif