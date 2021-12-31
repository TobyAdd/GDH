#ifndef __PLAYLAYER_H__
#define __PLAYLAYER_H__

#include <gd.h>

namespace gd {

	class GJBaseGameLayer;
	class CCCircleWaveDelegate;
	class GameObject;
	// class EndPortalObject;
	using EndPortalObject = GameObject;
	// class AudioEffectsLayer;
	using AudioEffectsLayer = cocos2d::CCLayerColor;
	// class GJGroundLayer;
	class GDH_DLL GJGroundLayer : public cocos2d::CCLayer {
		public:
			void updateGroundWidth() {
				reinterpret_cast<void(__thiscall*)(GJGroundLayer*)>(
					base + 0x12dda0
				)(this);
			}
	};
	class GJGameLevel;
	class GDH_DLL UILayer : public cocos2d::CCLayerColor {
		public:
			PAD(8)
			cocos2d::CCMenu* m_pCheckPointMenu;		// 0x1a0

			void onCheck(CCObject* pSender) {
				reinterpret_cast<void(__thiscall*)(UILayer*, CCObject*)>(
					base + 0x25fb60
				)(this, pSender);
			}

			void onDeleteCheck(CCObject* pSender) {
				reinterpret_cast<void(__thiscall*)(UILayer*, CCObject*)>(
					base + 0x25fc90
				)(this, pSender);
			}

			void onPause(CCObject* pSender) {
				reinterpret_cast<void(__thiscall*)(UILayer*, CCObject*)>(
					base + 0x25fad0
				)(this, pSender);
			}
	};

	using StartPosObject = gd::GameObject;
	class CheckpointObject;

	class GDH_DLL PlayLayer : public GJBaseGameLayer, public CCCircleWaveDelegate {
		public:
			PAD(8);
			unsigned int unk2D8;
			bool unk2DC;
			bool m_bHasCheated; // 0x2DD by taking less than 30s to beat lvl
			int unk2E0; // random value between 0 and 100 + unk2E8
			int unk2E4; // random value between 0 and 100
			int unk2E8;
			bool unk2EC; // opposite of isFrozen
			bool unk2ED;
			PAD(10);
			cocos2d::CCDrawNode* unk2F8;
			float unk2FC; // seems to be camera y
			float unk300;
			float unk304;
			float unk308; // bottom ground y maybe
			float unk30C; // last y or smth idk
			bool unk310;
			bool unk311;
			bool unk312;
			PAD(25);
			StartPosObject* m_startPos; // 0x32C
			CheckpointObject* unk330;
			EndPortalObject* m_endPortal; // 0x334
			cocos2d::CCArray* m_checkpoints; // 0x338
			cocos2d::CCArray* unk33C;
			cocos2d::CCArray* unk340;
			cocos2d::CCArray* unk344;
			cocos2d::CCSprite* unk348;
			PAD(8);
			cocos2d::CCArray* unk354;
			cocos2d::CCArray* unk358;
			cocos2d::CCArray* unk35C;
			cocos2d::CCArray* unk360;
			bool m_isMute; // 0x364 has to do with fmod
			PAD(7);
			cocos2d::CCArray* m_screenRingObjects; // 0x36C ring objects that are on screen, yes this is weird
			cocos2d::CCParticleSystemQuad* unk370;
			cocos2d::CCDictionary* unk374;
			cocos2d::CCArray* m_circleWaves; // 0x378
			cocos2d::CCArray* unk37C;
			AudioEffectsLayer* m_audioEffectsLayer; // 0x380
			PAD(8);
			GJGroundLayer* m_bottomGround; // 0x38C
			GJGroundLayer* m_topGround; // 0x390
			PAD(8);
			bool m_isDead; // 0x39C
			bool unk39D; // smth to do with the camera
			bool unk39E; // set to true when cameraMoveX is called
			bool unk39F; // set to true when cameraMoveY is called
			PAD(4);
			int unk3A4;
			PAD(12);
			float m_levelLength; // 0x3B4
			float unk3B8;
			cocos2d::CCLabelBMFont* m_attemptLabel; // 0x3BC
			cocos2d::CCLabelBMFont* m_percentLabel; // 0x3C0
			bool m_isCameraShaking; // 0x3C4
			float m_currentShakeStrength; // 0x3C8
			float m_currentShakeInterval; // 0x3CC
			double m_lastShakeTime; // 0x3D0
			cocos2d::CCPoint unk3D8;
			PAD(16);
			int unk3F0; // always 40
			cocos2d::CCDictionary* m_particleSystemsPlist; // 0x3F4 i really need a better name	 
			cocos2d::CCDictionary* unk3F8;
			cocos2d::CCArray* m_particleSystems; // 0x3FC
			cocos2d::CCNode* unk400;
			cocos2d::CCSprite* m_sliderGrooveSprite; // 0x404
			cocos2d::CCSprite* m_sliderBarSprite; // 0x408
			float m_sliderWidth; // 0x40C sliderbar2 width (?) minus 4
			float unk410; // always 8
			PAD(4);
			int unk418; // always 4
			PAD(8);
			cocos2d::CCArray* m_gravityEffectSprites; // 0x424 contains GravityEffectSprite s	   
			bool unk428;
			bool m_shouldRecordActions; // 0x429 for the hidden rob bot
			bool unk42A;
			bool unk42B; // 0x42B
			bool unk42C;
			bool m_isPlayer2Frozen; // 0x42D wat
			std::string m_previousRecords; // 0x430
			PAD(8);
			double m_time; // 0x450
			PAD(9);
			bool unk461;
			cocos2d::CCDictionary* unk464;
			PAD(8);
			bool m_isPlaybackMode; // 0x470
			PAD(11);
			bool m_isFlipped; // 0x47C
			PAD(7);
			UILayer* m_uiLayer; // 0x484
			GJGameLevel* m_level; // 0x488
			float m_cameraX; // 0x48C
			float m_cameraY; // 0x490
			bool m_isTestMode; // 0x494
			bool m_isPracticeMode; // 0x495
			bool unk496;
			bool unk497;
			cocos2d::CCArray* unk498; // animations array maybe
			bool unk49C;
			PAD(11);
			int m_currentAttempt; // 0x4A8
			int m_jumpCount; // 0x4AC
			bool unk4B0;
			float m_totalTime; // 0x4B4
			int m_attemptJumpCount; // 0x4B8 resets when you die
			bool unk4BC;
			bool m_hasLevelCompleteMenu; // 0x4BD true when the level complete menu shows up, so slightly after completing the lvl
			bool m_hasCompletedLevel; // 0x4BE
			int m_lastDeathPercent; // 0x4C0
			bool unk4C4;
			PAD(11);
			bool unk4D0;
			cocos2d::CCArray* unk4D4;
			cocos2d::CCDictionary* unk4D8;
			PAD(4);
			double unk4E0;
			double unk4E8; // seems to be time modulo 2 for some reason
			PAD(24);
			double unk508; // time again?
			PAD(31);
			bool m_bIsPaused; // 0x52f
			GameObject* unk530;
			bool unk534;
			bool unk535; // gets set to false when you cheat
			bool m_disableGravityEffect; // 0x536 comes from gv_0072
			PAD(1);

			static PlayLayer* get() { return GameManager::sharedState()->getPlayLayer(); }

			static PlayLayer* create(GJGameLevel* lvl) {
				return reinterpret_cast<PlayLayer*(__fastcall*)(GJGameLevel*)>(
					base + 0x1fb6d0
				)(lvl);
			}

			static cocos2d::CCScene* scene(GJGameLevel* lvl) {
				return reinterpret_cast<cocos2d::CCScene*(__fastcall*)(GJGameLevel*)>(
					base + 0x1fb690
				)(lvl);
			}

			void togglePracticeMode(bool on) {
				reinterpret_cast<void(__thiscall*)(PlayLayer*, bool)>(
					base + 0x20d0d0
				)(this, on);
			}
	};
}

#endif