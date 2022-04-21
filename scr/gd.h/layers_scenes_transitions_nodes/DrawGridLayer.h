#ifndef __DRAWGRIDLAYER_H__
#define __DRAWGRIDLAYER_H__

#include <gd.h>

namespace gd {

    class GDH_DLL DrawGridLayer : public cocos2d::CCLayer {
        public:
            void* m_pCommonLines;       // 0x11c
            void* m_pYellowGuidelines;  // 0x120
            void* m_pGreenGuidelines;   // 0x124
            float m_fSongOffset1;       // 0x128
            float m_fSongOffset2;       // 0x12c
            float m_fLastMusicXPosition;// 0x130
            bool m_bEffectSortDirty;    // 0x134
            LevelEditorLayer* m_pEditor;// 0x138
            std::string m_sGuidelineString; // 0x13c
            cocos2d::CCNode* m_pGrid;       // 0x154
            cocos2d::CCArray* m_pGuidelines;// 0x158
            cocos2d::CCArray* m_pEffects;   // 0x15c
            cocos2d::CCArray* m_pGuides;    // 0x160
            cocos2d::CCArray* m_pSpeedObjects1;     // 0x164
            cocos2d::CCArray* m_pSpeedObjects2;     // 0x168
            cocos2d::CCArray* m_pPlayerNodePoints;  // 0x16c
            cocos2d::CCArray* m_pPlayer2NodePoints; // 0x170
            double UnkDouble;               // 0x174
            float m_fGuidelineSpacing;      // 0x17c
            float m_fSlowGuidelineSpacing;  // 0x180
            float m_fNormalGuidelineSpacing;// 0x184
            float m_fFastGuidelineSpacing;  // 0x188
            float m_fFasterGuidelineSpacing;// 0x18c
            float m_fFastestGuidelineSpacing;   // 0x190
            bool m_pUpdatingTimeMarkers;    // 0x194
            bool m_bTimeNeedsUpdate;        // 0x195
            float m_fActiveGridNodeSize;    // 0x198

            void draw() override {
                reinterpret_cast<void(__thiscall*)(DrawGridLayer*)>(
                    base + 0x16ce90
                )(this);
            }
    };

}

#endif
