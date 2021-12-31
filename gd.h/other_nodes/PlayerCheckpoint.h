#ifndef __PLAYERCHECKPOINT_H__
#define __PLAYERCHECKPOINT_H__

#include <gd.h>

namespace gd {
    class GDH_DLL PlayerCheckpoint : public cocos2d::CCNode {
    public:
        cocos2d::CCPoint m_position;
        float m_yAccel; // 0x0F4 
        bool m_isUpsideDown; // 0x0F8 
        bool m_isShip; // 0x0F9 
        bool m_isBall; // 0x0FA 
        bool m_isUFO; // 0x0FB 
        bool m_isWave; // 0x0FC 
        bool m_isRobot; // 0x0FD 
        bool m_isSpider; // 0x0FE 
        bool m_isOnGround; // 0x0FF 
        bool m_hasGhostTrail; // 0x100 this actually takes up 4 bytes for some reason, maybe rob has it as an int
        PAD(3);
        bool m_small; // 0x104 
        float m_speed; // 0x108 
        bool m_hidden; // 0x10C 
    };
}

#endif