#ifndef __HARDSTREAK_H__
#define __HARDSTREAK_H__

#include <gd.h>

namespace gd {
    class GDH_DLL HardStreak : public cocos2d::CCDrawNode {
        public:
            cocos2d::CCArray* m_pointsArr; // 0x011C
            cocos2d::CCPoint m_currentPoint; // 0x0120 where the wave is at rn
            float m_waveSize; // 0x0128 bad name but its 1.0 for regular and 0.6 mini
            float m_pulseSize; // 0x012C
            bool m_isSolid; // 0x0130
    }; // size = 0x134
}

#endif