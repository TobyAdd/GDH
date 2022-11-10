#ifndef __LEVELEDITORLAYER_H__
#define __LEVELEDITORLAYER_H__

#include <gd.h>

namespace gd {

    class GJBaseGameLayer;
    class EditorUI;
    class GameObject;
    class GJGroundLayer;

    class LevelEditorLayer : public GJBaseGameLayer {
    public:
        PAD(4)
        bool m_unk2d0;   // 0x2d0
        bool m_unk2d1;   // 0x2d1
        bool m_unk2d2;   // 0x2d2
        bool m_unk2d3;   // 0x2d3
        bool m_bGridEnabled;   // 0x2d4
        bool m_unk2d5;   // 0x2d5
        bool m_unk2d6;   // 0x2d6
        bool m_unk2d7;   // 0x2d7
        bool m_unk2d8;   // 0x2d8
        bool m_bMoreUndo;   // 0x2d9
        bool m_unk2da;   // 0x2da
        bool m_unk2db;   // 0x2db
        bool m_bHighDetail;   // 0x2dc
        PAD(0x33)
        GameObject* m_pCopyStateObject; // 0x310
        PAD(0x40)
        int m_nCurrentLayer;    // 0x354
        PAD(0x28)
        EditorUI* m_pEditorUI;  // 0x380
        PAD(4)
        cocos2d::CCArray* m_pUndoObjects;   // 0x388
        cocos2d::CCArray* m_pSomeArray; // 0x38c
        PAD(0x1c)
        bool m_bIsPlaybackMode;
        PAD(0x17)
        GJGroundLayer* m_pGroundLayer;  // 0x3c4

    public:
        static LevelEditorLayer* get() { return gd::GameManager::sharedState()->getEditorLayer(); }
        EditorUI* getEditorUI() { return this->m_pEditorUI; }

        GameObject* createObject(int id, cocos2d::CCPoint const& position, bool undo) {
            return reinterpret_cast<GameObject*(__thiscall*)(
                LevelEditorLayer*, int, cocos2d::CCPoint, bool
            )>(
                base + 0x160d70
            )(
                this, id, position, undo
            );
            
        }

        void removeObject(GameObject * obj, bool idk) {
            reinterpret_cast<void(__thiscall*)(
                LevelEditorLayer *,GameObject *,bool
            )>(
                base + 0x161cb0
            )(
                this, obj, idk
            );
        }

        int getNextFreeGroupID(cocos2d::CCArray* objs) {
            return reinterpret_cast<int(__thiscall*)(
                LevelEditorLayer*, cocos2d::CCArray*
            )>(
                base + 0x164ae0
            )(
                this, objs
            );
        }

        GameObject* addObjectFromString(std::string const& str) {
            return reinterpret_cast<GameObject*(__thiscall*)(LevelEditorLayer*, std::string)>(base + 0x160c80)(this, str);
        }

        // yes it's misspelled in GD aswell
        void pasteAtributeState(GameObject* obj, cocos2d::CCArray* objs) {
            reinterpret_cast<void(__thiscall*)(LevelEditorLayer*, GameObject*, cocos2d::CCArray*)>(
                base + 0x16b740
            )(this, obj, objs);
        }
        
        int getCurrentLayer() { return m_nCurrentLayer; }
        void setCurrentLayer(int n) { m_nCurrentLayer = n; }

        float getLastObjectX() {
            reinterpret_cast<void(__fastcall*)(LevelEditorLayer*)>(
                base + 0x167290
            )(this);

            float ret;
            __asm { movss ret, xmm0 }

            return ret;
        }
    
        void updateOptions() {
            reinterpret_cast<void(__thiscall*)(LevelEditorLayer*)>(
                base + 0x15fcc0
            )(this);
        }
    
        void addSpecial(GameObject* obj) {
            reinterpret_cast<void(__thiscall*)(LevelEditorLayer*, GameObject*)>(
                base + 0x162650
            )(this, obj);
        }
    };

}

#endif
