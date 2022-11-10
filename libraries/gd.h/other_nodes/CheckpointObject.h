#ifndef __CHECKPOINTOBJECT_H__
#define __CHECKPOINTOBJECT_H__

#include <gd.h>

namespace gd {
    class GameObject;
    class PlayerCheckpoint;
    
    class CheckpointObject : public cocos2d::CCNode {
    public:
        GameObject* m_gameObject; // 0x0EC 
        PlayerCheckpoint* m_player1; // 0x0F0 
        PlayerCheckpoint* m_player2; // 0x0F4 
        bool m_isDual; // 0x0F8 
        bool m_isFlipped; // 0x0F9 
        cocos2d::CCPoint m_cameraPos; // 0x0FC unsure
        int unk104; // comes from playlayer + 2ac
        GameObject* m_lastPortal; // 0x108 
        PAD(4);
        double unk110;
        std::string m_currentStateString;
        std::string m_objectsStateString;
    };
}

#endif