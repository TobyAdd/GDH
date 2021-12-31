#ifndef __GJBASEGAMELAYER_H__
#define __GJBASEGAMELAYER_H__

#include <gd.h>
#include <vector>

namespace gd {
	class OBB2D;
	class GJEffectManager;
	class CCNodeContainer;
	class LevelSettingsObject;
	class PlayerObject;
	class GameObject;

	class GDH_DLL GJBaseGameLayer : public cocos2d::CCLayer {
	public:
		PAD(4);
		OBB2D* m_pBoundingBox; // 0x120 
		GJEffectManager* m_pEffectManager; // 0x124 
		cocos2d::CCLayer* m_pObjectLayer; // 0x128
		// these names come from robtop himself, so if you think theyre bad go bother him
		// more explained in depth here https://github.com/Wyliemaster/gddocs/blob/master/docs/resources/client/level-components/capacity-string.md
		cocos2d::CCSpriteBatchNode* m_pBatchNodeAddTop4; // 0x12C 
		cocos2d::CCSpriteBatchNode* m_pEffectBatchNodeAddTop4; // 0x130 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeTop3; // 0x134 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeAddTop3; // 0x138 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeAddGlowTop3; // 0x13C 
		CCNodeContainer* unk140;
		cocos2d::CCSpriteBatchNode* m_pBatchNodeTextTop3; // 0x144 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeAddTextTop3; // 0x148 
		cocos2d::CCSpriteBatchNode* m_pEffectBatchNodeTop3; // 0x14C 
		cocos2d::CCSpriteBatchNode* m_pEffectBatchNodeAddTop3; // 0x150 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeTop2; // 0x154 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeAddTop2; // 0x158 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeAddGlowTop2; // 0x15C 
		CCNodeContainer* unk160;
		cocos2d::CCSpriteBatchNode* m_pBatchNodeTextTop2; // 0x164 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeAddTextTop2; // 0x168 
		cocos2d::CCSpriteBatchNode* m_pEffectBatchNodeTop2; // 0x16C 
		cocos2d::CCSpriteBatchNode* m_pEffectBatchNodeAddTop2; // 0x170 
		cocos2d::CCSpriteBatchNode* m_pBatchNode; // 0x174 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeAdd; // 0x178 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeAddGlow; // 0x17C 
		CCNodeContainer* unk180;
		cocos2d::CCSpriteBatchNode* m_pBatchNodeTextTop1; // 0x184 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeAddTextTop1; // 0x188 
		cocos2d::CCSpriteBatchNode* m_pEffectBatchNodeTop1; // 0x18C 
		cocos2d::CCSpriteBatchNode* m_pEffectBatchNodeAddTop1; // 0x190 
		cocos2d::CCSpriteBatchNode* m_pBatchNodePlayer; // 0x194 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeAddPlayer; // 0x198 
		cocos2d::CCSpriteBatchNode* unk19C;
		cocos2d::CCSpriteBatchNode* m_pBatchNodeAddMid; // 0x1A0 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeBottom; // 0x1A4 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeAddBottom; // 0x1A8 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeAddBottomGlow; // 0x1AC 
		CCNodeContainer* unk1B0;
		cocos2d::CCSpriteBatchNode* m_pBatchNodeText; // 0x1B4 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeAddText; // 0x1B8 
		cocos2d::CCSpriteBatchNode* m_pEffectBatchNode; // 0x1BC 
		cocos2d::CCSpriteBatchNode* m_pEffectBatchNodeAdd; // 0x1C0 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeBottom2; // 0x1C4 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeAddBottom2; // 0x1C8 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeAddBottom2Glow; // 0x1CC 
		CCNodeContainer* unk1D0;
		cocos2d::CCSpriteBatchNode* m_pBatchNodeTextBot2; // 0x1D4 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeAddTextBot2; // 0x1D8 
		cocos2d::CCSpriteBatchNode* m_pEffectBatchNodeBot2; // 0x1DC 
		cocos2d::CCSpriteBatchNode* m_pEffectBatchNodeAddBot2; // 0x1E0 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeBottom3; // 0x1E4 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeAddBottom3; // 0x1E8 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeAddBottom3Glow; // 0x1EC 
		CCNodeContainer* unk1F0;
		cocos2d::CCSpriteBatchNode* m_pBatchNodeTextBot3; // 0x1F4 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeAddTextBot3; // 0x1F8 
		cocos2d::CCSpriteBatchNode* m_pEffectBatchNodeBot3; // 0x1FC 
		cocos2d::CCSpriteBatchNode* m_pEffectBatchNodeAddBot3; // 0x200 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeBottom4; // 0x204 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeAddBottom4; // 0x208 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeAddBottom4Glow; // 0x20C 
		CCNodeContainer* unk210;
		cocos2d::CCSpriteBatchNode* m_pBatchNodeTextBot4; // 0x214 
		cocos2d::CCSpriteBatchNode* m_pBatchNodeAddTextBot4; // 0x218 
		cocos2d::CCSpriteBatchNode* m_pEffectBatchNodeBot4; // 0x21C 
		cocos2d::CCSpriteBatchNode* m_pEffectBatchNodeAddBot4; // 0x220 
		PlayerObject* m_pPlayer1; // 0x224 
		PlayerObject* m_pPlayer2; // 0x228 
		LevelSettingsObject* m_pLevelSettings; // 0x22C 
		cocos2d::CCDictionary* unk230;
		cocos2d::CCArray* m_pObjects; // 0x234 
		cocos2d::CCArray* m_pObjectContainerArrays;	// 0x238
		cocos2d::CCArray* unk23C;
		cocos2d::CCArray* unk240;
		cocos2d::CCArray* m_pSpawnObjects; // 0x244 
		cocos2d::CCArray* unk248;
		cocos2d::CCNode* unk24C;
		std::vector<GameObject*> unk250;
		std::vector<void*> unk25C; // idk the types for the rest of the vectors
		cocos2d::CCDictionary* unk268;
		cocos2d::CCDictionary* unk26C;
		cocos2d::CCDictionary* unk270;
		std::vector<void*> unk274;
		std::vector<void*> unk280;
		std::vector<void*> unk28C;
		cocos2d::CCArray* unk298;
		cocos2d::CCArray* unk29C;
		cocos2d::CCDictionary* unk2A0;
		cocos2d::CCDictionary* unk2A4;
		bool m_bDidUpdateNormalCapacity; // 0x2A8 
		bool m_bIsDualMode; // 0x2A9 
		int unk2AC;
		bool unk2B0;
		int m_nAttemptClickCount; // 0x2B4 
		int m_nCurrectSection; // 0x2B8 
		int unk2BC; // probably what section gd has loaded up to
		bool unk2C0;
		bool unk2C1;
		PAD(10); // this should be 10 but im too lazy to fix playlayer rn

		cocos2d::CCLayer* getObjectLayer() { return m_pObjectLayer; }

		cocos2d::CCArray* getAllObjects() { return this->m_pObjects; }

		void pushButton(int uk, bool btn2) {
			reinterpret_cast<void(__thiscall*)(GJBaseGameLayer*, int, bool)>(
				base + 0x111500
			)(this, uk, btn2);
		}
		void releaseButton(int uk, bool btn2) {
			reinterpret_cast<void(__thiscall*)(GJBaseGameLayer*, int, bool)>(
				base + 0x111660
			)(this, uk, btn2);
		}
	};
}

#endif
