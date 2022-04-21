#ifndef __MOREOPTIONSLAYER_H__
#define __MOREOPTIONSLAYER_H__

#include <gd.h>

namespace gd {
    class GDH_DLL MoreOptionsLayer : public FLAlertLayer {
        public:
            static MoreOptionsLayer* create() {
                return reinterpret_cast<MoreOptionsLayer*(__fastcall*)()>(
                    base + 0x1de850
                )();
            }

            void addToggle(const char* name, const char* key, const char* info) {
                reinterpret_cast<void*(__thiscall*)(MoreOptionsLayer*, const char*, const char*, const char*)>(base + 0x1DF6B0)(this, name, key, info);
            }
    };
}

#endif