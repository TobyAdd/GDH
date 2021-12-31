#ifndef __UNDOOBJECT_H__
#define __UNDOOBJECT_H__

#include <gd.h>

namespace gd {
    enum UndoCommand {
        kUndoCommandDelete = 1,
        kUndoCommandNew = 2,
        kUndoCommandPaste = 3,
        kUndoCommandDeleteMulti = 4,
        kUndoCommandTransform = 5,
        kUndoCommandSelect = 6,
    };

    class GDH_DLL UndoObject /* 0x30 */ : public cocos2d::CCObject /* 0x20 */ {
        public:
            GameObject* m_pGameObject;      // 0x20
            UndoCommand m_eCommand;         // 0x24
            cocos2d::CCArray* m_pObjects;   // 0x28
            bool m_bRedo;                   // 0x2c

            static UndoObject* create(GameObject* pObject, UndoCommand nCommand) {
                return reinterpret_cast<UndoObject*(__fastcall*)(GameObject*, UndoCommand)>(
                    base + 0x16bc20
                )(pObject, nCommand);
            }
            static UndoObject* createWithArray(cocos2d::CCArray* pObjects, UndoCommand nCommand) {
                return reinterpret_cast<UndoObject*(__fastcall*)(cocos2d::CCArray*, UndoCommand)>(
                    base + 0x16bee0
                )(pObjects, nCommand);
            }
            static UndoObject* createWithTransformObjects(cocos2d::CCArray* pObjects, UndoCommand nCommand) {
                return reinterpret_cast<UndoObject*(__fastcall*)(cocos2d::CCArray*, UndoCommand)>(
                    base + 0x16bcd0
                )(pObjects, nCommand);
            }
    };
}

#endif
