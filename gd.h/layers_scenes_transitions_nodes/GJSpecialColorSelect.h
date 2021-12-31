#ifndef __GJSPECIALCOLORSELECT_H__
#define __GJSPECIALCOLORSELECT_H__

#include <gd.h>

namespace gd {

    class GDH_DLL GJSpecialColorSelect : public gd::FLAlertLayer {
        // todo

        public:
            static const char* textForColorIdx(int id) {
                return reinterpret_cast<const char*(__fastcall*)(int)>(
                    base + 0x14e1d0
                )(id);
            }
    };

}

#endif
