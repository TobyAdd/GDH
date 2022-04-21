#ifndef __VIDEOOPTIONSLAYER_H__
#define __VIDEOOPTIONSLAYER_H__

#include <gd.h>

namespace gd {
    class GDH_DLL VideoOptionsLayer : public FLAlertLayer {
        public:
            PAD(24)
            cocos2d::CCArray* m_pResolutions;   // 0x1e4
            PAD(4)
            int m_nCurrentResolution;   // 0x1ec
    };
}

#endif
