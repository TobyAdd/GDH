#ifndef __LEVELEDITORLAYER_H__
#define __LEVELEDITORLAYER_H__

#include <gd.h>

namespace gd {

    class GJBaseGameLayer;
    class EditorUI;
    class GameObject;
    class GJGroundLayer;
    class DrawGridLayer;

    GDH_ADD(
    enum PlaybackMode {
        kPlaybackModeNot        = 0,
        kPlaybackModePlaying    = 1,
        kPlaybackModePaused     = 2,
    };
    );

    class GDH_DLL LevelEditorLayer : public GJBaseGameLayer {
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
        PAD(0x14)
        DrawGridLayer* m_pDrawGridLayer;    // 0x3a4
        PAD(4)
        PlaybackMode m_ePlaybackMode;   // 0x3a8
        PAD(0x14)
        GJGroundLayer* m_pGroundLayer;  // 0x3c4

    public:
        static LevelEditorLayer* get() { return gd::GameManager::sharedState()->getEditorLayer(); }
        EditorUI* getEditorUI() { return this->m_pEditorUI; }

        static LevelEditorLayer* create(GJGameLevel* level) {
            return reinterpret_cast<LevelEditorLayer*(__fastcall*)(GJGameLevel*)>(
                base + 0x15ed60
            )(level);
        }

        static cocos2d::CCScene* scene(GJGameLevel* level) {
            auto scene = cocos2d::CCScene::create();

            scene->addChild(LevelEditorLayer::create(level));
            scene->setObjType(cocos2d::kCCObjectTypeLevelEditorLayer);

            cocos2d::CCDirector::sharedDirector()->replaceScene(
                cocos2d::CCTransitionFade::create(0.5f, scene)
            );
            return scene;
        }

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

        GameObject* objectAtPosition(cocos2d::CCPoint const& pos) {
            return reinterpret_cast<GameObject*(__thiscall*)(
                LevelEditorLayer*, cocos2d::CCPoint
            )>(base + 0x161300)(this, pos);
        }

        cocos2d::CCRect getObjectRect(GameObject* obj, bool unk) {
            cocos2d::CCRect res;
            reinterpret_cast<cocos2d::CCRect*(__thiscall*)(
                LevelEditorLayer*, cocos2d::CCRect*, GameObject*, bool
            )>(base + 0x1616b0)(this, &res, obj, unk);
            return res;
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

            float retVal;
            __asm { movss retVal, xmm0 }

            return retVal;
        }
    
        void updateOptions() {
            reinterpret_cast<void(__thiscall*)(LevelEditorLayer*)>(
                base + 0x15fcc0
            )(this);
        }

        void updateEditorMode() {
            reinterpret_cast<void(__thiscall*)(LevelEditorLayer*)>(
                base + 0x1652b0
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
