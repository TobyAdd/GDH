#ifndef __CHECKPOINTOBJECT_H__
#define __CHECKPOINTOBJECT_H__

#include <gd.h>
#include <string>

namespace gd {
    class GameObject;
    class PlayerCheckpoint;
    
    class GDH_DLL CheckpointObject : public cocos2d::CCNode {
    protected:
        CheckpointObject() {
            reinterpret_cast<void*(__thiscall*)(CheckpointObject*)>(base + 0x1fb0c0)(this);
        }
        ~CheckpointObject() {
            // we do some tomfoolery
            const auto deleteStr = [](std::string* str) {
                const auto capacity = reinterpret_cast<size_t*>(reinterpret_cast<uintptr_t>(str) + 20);
                const auto size = reinterpret_cast<size_t*>(reinterpret_cast<uintptr_t>(str) + 16);
                if (*capacity > 15) {
                    delete *reinterpret_cast<char**>(str);
                }
                *capacity = 15;
                *size = 0;
                *reinterpret_cast<char*>(str) = '\0';
            };
            deleteStr(&m_currentStateString);
            deleteStr(&m_objectsStateString);
        }
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