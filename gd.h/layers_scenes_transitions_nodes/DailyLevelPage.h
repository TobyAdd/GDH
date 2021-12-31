#ifndef __DAILYLEVELPAGE_H__
#define __DAILYLEVELPAGE_H__

#include <gd.h>

namespace gd {
    class GDH_DLL DailyLevelPage : public FLAlertLayer {
        public:
            static DailyLevelPage* create(bool weekly) {
                return reinterpret_cast<DailyLevelPage*(__fastcall*)(bool)>(
                    base + 0x6a860
                )(weekly);
            }
    };
}

#endif
