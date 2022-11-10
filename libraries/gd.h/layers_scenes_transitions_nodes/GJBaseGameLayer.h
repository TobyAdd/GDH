#ifndef __GJBASEGAMELAYER_H__
#define __GJBASEGAMELAYER_H__

#include <gd.h>

namespace gd {
	class OBB2D;
	class GJEffectManager;
	class CCNodeContainer;
	class LevelSettingsObject;
	class PlayerObject;

	class GJBaseGameLayer /* 0x2cc */ : public cocos2d::CCLayer /* 0x11c */ {
		public:
			PAD(0x4)
			OBB2D* m_pOBB2D;					// 0x120
			GJEffectManager* m_pEffectManager;	// 0x124
			cocos2d::CCLayer* m_pObjectLayer;	// 0x128
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch0;	// 0x12c
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch1;	// 0x130
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch2;	// 0x134
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch3;	// 0x138
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch4;	// 0x13c
			CCNodeContainer* m_pNodeContainer0;				// 0x140
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch5;	// 0x144
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch6;	// 0x148
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch7;	// 0x14c
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch8;	// 0x150
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch9;	// 0x154
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch10;	// 0x158
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch11;	// 0x15c
			CCNodeContainer* m_pNodeContainer1;				// 0x160
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch12;	// 0x164
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch13;	// 0x168
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch14;	// 0x16c
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch15;	// 0x170
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch16;	// 0x174
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch17;	// 0x178
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch18;	// 0x17c
			CCNodeContainer* m_pNodeContainer2;				// 0x180
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch19;	// 0x184
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch20;	// 0x188
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch21;	// 0x18c
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch22;	// 0x190
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch23;	// 0x194
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch24;	// 0x198
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch25;	// 0x19c
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch26;	// 0x1a0
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch27;	// 0x1a4
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch28;	// 0x1a8
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch29;	// 0x1ac
			CCNodeContainer* m_pNodeContainer3;				// 0x1b0
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch30;	// 0x1b4
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch31;	// 0x1b8
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch32;	// 0x1bc
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch33;	// 0x1c0
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch34;	// 0x1c4
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch35;	// 0x1c8
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch36;	// 0x1cc
			CCNodeContainer* m_pNodeContainer4;				// 0x1d0
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch37;	// 0x1d4
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch38;	// 0x1d8
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch39;	// 0x1dc
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch40;	// 0x1e0
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch41;	// 0x1e4
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch42;	// 0x1e8
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch43;	// 0x1ec
			CCNodeContainer* m_pNodeContainer5;				// 0x1f0
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch44;	// 0x1f4
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch45;	// 0x1f8
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch46;	// 0x1fc
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch47;	// 0x200
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch48;	// 0x204
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch49;	// 0x208
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch50;	// 0x20c
			CCNodeContainer* m_pNodeContainer6;				// 0x210
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch51;	// 0x214
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch52;	// 0x218
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch53;	// 0x21c
			cocos2d::CCSpriteBatchNode* m_pSpriteBatch54;	// 0x220
			PlayerObject* m_pPlayer1;				// 0x224
			PlayerObject* m_pPlayer2;				// 0x228
			LevelSettingsObject* m_pLevelSettings;	// 0x22c
			cocos2d::CCDictionary* m_pDict0;	// 0x230
			cocos2d::CCArray* m_pObjects;	// 0x234
			cocos2d::CCArray* m_pObjectContainerArrays;	// 0x238
			cocos2d::CCArray* m_pArray2;	// 0x23c
			cocos2d::CCArray* m_pArray3;	// 0x240
			cocos2d::CCArray* m_pArray4;	// 0x244
			cocos2d::CCArray* m_pArray5;	// 0x248
			cocos2d::CCNode* m_pSomeNode;	// 0x24c
			PAD(0x1c)
			cocos2d::CCDictionary* m_pDict1;	// 0x26c
			cocos2d::CCDictionary* m_pDict2;	// 0x270
			cocos2d::CCDictionary* m_pDict3;	// 0x274
			PAD(0x28)
			cocos2d::CCArray* m_pArray6;	// 0x2a0
			cocos2d::CCArray* m_pArray7;	// 0x2a4
			cocos2d::CCDictionary* m_pDict4;	// 0x2a8
			cocos2d::CCDictionary* m_pDict5;	// 0x2ac
			PAD(0x1c)
		
		public:
			cocos2d::CCLayer* getObjectLayer() { return m_pObjectLayer; }

        	cocos2d::CCArray* getAllObjects() { return this->m_pObjects; }
	};
}

#endif
