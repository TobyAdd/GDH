#ifndef __GAMEMANAGER_H__
#define __GAMEMANAGER_H__

#include <gd.h>

#define PLAYER_ICON_FUNC(x) void setPlayer##x(int v) { m_nPlayer##x = v; __STR_CAT__(m_nPlayer##x,Rand1) = v + __STR_CAT__(m_nPlayer##x,Rand2); }

namespace gd {
	enum IconType {
		kIconTypeCube           = 0,
		kIconTypeShip           = 1,
		kIconTypeBall           = 2,
		kIconTypeUfo            = 3,
		kIconTypeWave           = 4,
		kIconTypeRobot          = 5,
		kIconTypeSpider         = 6,
		kIconTypeDeathEffect    = 98,
		kIconTypeSpecial        = 99,
	};
	
	enum UnlockType {
		kUnlockTypeUnknown = 0,
		kUnlockTypeCube = 1,
		kUnlockTypeColor1 = 2,
		kUnlockTypeColor2 = 3,
		kUnlockTypeShip = 4,
		kUnlockTypeBall = 5,
		kUnlockTypeUfo = 6,
		kUnlockTypeWave = 7,
		kUnlockTypeRobot = 8,
		kUnlockTypeSpider = 9,
		kUnlockTypeSpecial = 10,
		kUnlockTypeDeathEffect = 11,
		kUnlockTypeGlow = 12,
	};

	class PlayLayer;
	class LevelEditorLayer;

	class GDH_DLL GameManager : public GManager {
	public:
		bool m_bSwitchModes;
		bool m_bToFullscreen;
		bool m_bReloading;
		bool m_bUnknown;
		PAD(4);
		cocos2d::CCDictionary* m_pValueKeeper;
		cocos2d::CCDictionary* m_pUnlockValueKeeper;
		cocos2d::CCDictionary* m_pCustomObjectDict;
		PAD(4);
		double m_dUnknown;
		PAD(16);
		double m_dUnknown2;
		PAD(8);
		bool m_bLoaded; // 0x148
		std::string m_sUnknown;	// 0x14c
		PlayLayer* m_pPlayLayer;	// 0x164
		LevelEditorLayer* m_pLevelEditorLayer;	// 0x168
		PAD(20);
		std::string m_sPlayerUDID;	// 0x180
		std::string m_sPlayerName;	// 0x198
		bool m_bCommentsEnabled;	// 0x1b0
		int m_nPlayerUserIDRand1;	// 0x1b4
		int m_nPlayerUserIDRand2;	// 0x1b8
		int m_nPlayerUserID;		// 0x1bc
		float m_fBackgroundMusicVolume;	// 0x1c0
		float m_fEffectsVolume;			// 0x1c4
		int m_nTimeOffset;			// 0x1c8
		PAD(28);
		int m_nPlayerFrameRand1;	// 0x1e8
		int m_nPlayerFrameRand2;	// 0x1ec
		int m_nPlayerFrame;			// 0x1f0
		int m_nPlayerShipRand1;		// 0x1f4
		int m_nPlayerShipRand2;		// 0x1f8
		int m_nPlayerShip;			// 0x1fc
		int m_nPlayerBallRand1;		// 0x200
		int m_nPlayerBallRand2;		// 0x204
		int m_nPlayerBall;			// 0x208
		int m_nPlayerBirdRand1;		// 0x20c
		int m_nPlayerBirdRand2;		// 0x210
		int m_nPlayerBird;			// 0x214
		int m_nPlayerDartRand1;		// 0x218
		int m_nPlayerDartRand2;		// 0x21c
		int m_nPlayerDart;			// 0x220
		int m_nPlayerRobotRand1;	// 0x224
		int m_nPlayerRobotRand2;	// 0x228
		int m_nPlayerRobot;			// 0x22c
		int m_nPlayerSpiderRand1;	// 0x230
		int m_nPlayerSpiderRand2;	// 0x234
		int m_nPlayerSpider;		// 0x238
		int m_nPlayerColorRand1;	// 0x23c
		int m_nPlayerColorRand2;	// 0x240
		int m_nPlayerColor;			// 0x244
		int m_nPlayerColor2Rand1;	// 0x248
		int m_nPlayerColor2Rand2;	// 0x24c
		int m_nPlayerColor2;		// 0x250
		int m_nPlayerStreakRand1;	// 0x254
		int m_nPlayerStreakRand2;	// 0x258
		int m_nPlayerStreak;		// 0x25c
		int m_nPlayerDeathEffectRand1;	// 0x260
		int m_nPlayerDeathEffectRand2;	// 0x264
		int m_nPlayerDeathEffect;		// 0x268
		PAD(8);
		int m_nSecretNumberRand1;	// 0x270
		int m_nSecretNumberRand2;	// 0x274
		bool m_bPlayerGlow;			// 0x278
		IconType m_nPlayerIconType;	// 0x27c
		bool m_bUnknown2;			// 0x280
		bool m_bShowSongMarkers;	// 0x281
		bool m_bShowBPMMarkers;		// 0x282
		bool m_bRecordGameplay;		// 0x283
		bool m_bShowProgressBar;	// 0x284
		bool m_bPerformanceMode;	// 0x285
		bool m_bClickedGarage;		// 0x286
		bool m_bClickedEditor;		// 0x287
		bool m_bClickedName;		// 0x288
		bool m_bClickedPractice;	// 0x289
		bool m_bShowedEditorGuide;	// 0x28a
		bool m_bShowedRateDiffDialog;	// 0x28b
		bool m_bShowedRateStarDialog;	// 0x28c
		bool m_bShowedLowDetailDialog;	// 0x291
		PAD(48);
		int m_nBootups;
		bool m_bHasRatedGame;
		PAD(3);
		bool m_bGameCenterEnabled;
		bool m_bSmoothFix;
		PAD(16);
		int m_nResolution;
		cocos2d::TextureQuality m_eQuality; //more after that i havent re'd

		int getPlayerFrame() { return m_nPlayerFrame; }
		int getPlayerShip() { return m_nPlayerShip; }
		int getPlayerBall() { return m_nPlayerBall; }
		int getPlayerBird() { return m_nPlayerBird; }
		int getPlayerDart() { return m_nPlayerDart; }
		int getPlayerRobot() { return m_nPlayerRobot; }
		int getPlayerSpider() { return m_nPlayerSpider; }
		int getPlayerStreak() { return m_nPlayerStreak; }
		int getPlayerDeathEffect() { return m_nPlayerDeathEffect; }
		bool getPlayerGlow() { return m_bPlayerGlow; }
		int getPlayerColor() { return m_nPlayerColor; }
		int getPlayerColor2() { return m_nPlayerColor2; }
		IconType getPlayerIconType() { return m_nPlayerIconType; }

		PLAYER_ICON_FUNC(Frame)
		PLAYER_ICON_FUNC(Ship)
		PLAYER_ICON_FUNC(Ball)
		PLAYER_ICON_FUNC(Bird)
		PLAYER_ICON_FUNC(Dart)
		PLAYER_ICON_FUNC(Robot)
		PLAYER_ICON_FUNC(Spider)
		PLAYER_ICON_FUNC(Streak)
		PLAYER_ICON_FUNC(DeathEffect)
		PLAYER_ICON_FUNC(Color)
		PLAYER_ICON_FUNC(Color2)
		void setPlayerGlow(bool v) { m_bPlayerGlow = v; }
		void setPlayerIconType(IconType v) { m_nPlayerIconType = v; }

		#undef FUCK_THIS
		
		bool isColorUnlocked(int _id, bool _type) {
			return reinterpret_cast<bool(__thiscall*)(
				GameManager*, int, bool
			)>(
				base + 0xc53f0
			)(
				this, _id, _type
			);
		}

		bool isIconUnlocked(int _id, IconType _type) {
			return reinterpret_cast<bool(__thiscall*)(
				GameManager*, int, IconType
			)>(
				base + 0xc4fc0
			)(
				this, _id, _type
			);
		}

		cocos2d::ccColor3B colorForIdx(int _id) {
			auto ret = reinterpret_cast<cocos2d::ccColor3B(__stdcall*)(
				int
			)>(base + 0xc8d10)(_id);

			return ret;
		}

		static GameManager* sharedState() {
			return reinterpret_cast<GameManager* (__stdcall*)()>(
				base + 0xC4A50
				)();
		}
		void reloadAll(bool bSwitch, bool bFullscreen, bool bReloadedInSession) {
			return reinterpret_cast<void(__thiscall*)(GameManager*, bool, bool, bool)>(
				base + 0xCE950
				)(this, bSwitch, bFullscreen, bReloadedInSession);
		}
		void setQuality(cocos2d::TextureQuality quality) {
			m_eQuality = quality;
		}
		bool getGameVariable(const char* key) {
			return reinterpret_cast<bool(__thiscall*)(GameManager*, const char*)>(
				base + 0xC9D30
				)(this, key);
		}
		void setGameVariable(const char* key, bool value) {
			return reinterpret_cast<void(__thiscall*)(GameManager*, const char*, bool)>(
				base + 0xC9B50
				)(this, key, value);
		}
		int getIntGameVariable(const char* key) {
			return reinterpret_cast<int(__thiscall*)(GameManager*, const char*)>(
				base + 0xCA330
				)(this, key);
		}
		void setIntGameVariable(const char* key, int value) {
			reinterpret_cast<void(__thiscall*)(GameManager*, const char*, int)>(base + 0xCA230)(this, key, value);
		}
		static void toggleGameVariable(const char* key) {
			reinterpret_cast<void(__stdcall*)(const char*)>(base + 0xc9e90)(key);
		}
		bool getUGV(const char* key) {
			return reinterpret_cast<bool(__thiscall*)(GameManager*, const char*)>(
				base + 0xCA0D0
				)(this, key);
		}
		void fadeInMusic(const char* filename) {
			return reinterpret_cast<void(__thiscall*)(GameManager*, const char*)>(
				base + 0xC4BD0
				)(this, filename);
		}
		PlayLayer* getPlayLayer() { return m_pPlayLayer; }
		LevelEditorLayer* getEditorLayer() { return m_pLevelEditorLayer; }

		bool getGameVariableDefault(const char* key, bool defaultValue) {
			auto object = reinterpret_cast<cocos2d::CCString*>(m_pValueKeeper->objectForKey(std::string("gv_") + key));
			if (object == nullptr)
				return defaultValue;
			return object->boolValue();
		}

		int getIntGameVariableDefault(const char* key, int defaultValue) {
			auto object = reinterpret_cast<cocos2d::CCString*>(m_pValueKeeper->objectForKey(std::string("gv_") + key));
			if (object == nullptr)
				return defaultValue;
			return object->intValue();
		}

		void returnToLastScene(GJGameLevel* level) {
			reinterpret_cast<void(__thiscall*)(GameManager*, GJGameLevel*)>(
				base + 0xce6a0
			)(this, level);
		}
	};
}

#endif