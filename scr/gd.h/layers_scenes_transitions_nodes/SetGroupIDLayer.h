#ifndef __SETGROUPIDLAYER_H__
#define __SETGROUPIDLAYER_H__

#include <gd.h>

namespace gd {
    
    GDH_ADD(
	enum ZLayer {
		kZLayerB4 = -3,
		kZLayerB3 = -1,
		kZLayerB2 = 1,
		kZLayerB1 = 3,
        kZLayerDefault = 0,
		kZLayerT1 = 5,
		kZLayerT2 = 7,
		kZLayerT3 = 9,
	};
    );

    class GDH_DLL SetGroupIDLayer : public gd::FLAlertLayer, public gd::TextInputDelegate {
        public:
            gd::GameObject* m_pObj;         // 0x1d0
            cocos2d::CCArray* m_pObjs;      // 0x1d4
            cocos2d::CCArray* m_pArray0;    // 0x1d8
            cocos2d::CCArray* m_pArray1;    // 0x1dc
            cocos2d::CCLabelBMFont* m_pEditorLayerText;     // 0x1e0
            cocos2d::CCLabelBMFont* m_pEditorLayer2Text;    // 0x1e4
            cocos2d::CCLabelBMFont* m_pZOrderText;          // 0x1e8
            gd::CCTextInputNode* m_pGroupIDInput;           // 0x1ec
            int m_nGroupIDValue;        // 0x1f0
            int m_nEditorLayerValue;    // 0x1f4
            int m_nEditorLayer2Value;   // 0x1f8
            int m_nZOrderValue;         // 0x1fc
            ZLayer m_nZLayerValue;      // 0x200
            bool m_bUnk204;             // 0x204
            bool m_bUnk205;             // 0x205
            bool m_bHighDetail;         // 0x206
            bool m_bDontFade;           // 0x207
            bool m_bDontEnter;          // 0x208
            void* m_pSomePointerProlly; // 0x20c
            bool m_bGroupParent;        // 0x210
            bool m_bHasEditedGroups;    // 0x211
            int m_nUnknown;             // 0x214

            void updateEditorLayerID() {
                reinterpret_cast<void(__fastcall*)(SetGroupIDLayer*)>(
                    gd::base + 0x22e0b0
                )( this );
            }

            void updateEditorLayerID2() {
                reinterpret_cast<void(__fastcall*)(SetGroupIDLayer*)>(
                    gd::base + 0x22e110
                )( this );
            }

            void updateZOrder() {
                reinterpret_cast<void(__fastcall*)(SetGroupIDLayer*)>(
                    gd::base + 0x22e3d0
                )( this );
            }
            
            void updateGroupIDLabel() {
                reinterpret_cast<void(__fastcall*)(SetGroupIDLayer*)>(
                    gd::base + 0x22e450
                )( this );
            }
    };
}

#endif
