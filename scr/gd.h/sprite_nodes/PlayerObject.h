#ifndef __PLAYEROBJECT_H__
#define __PLAYEROBJECT_H__

#include <gd.h>

namespace gd {

	class GameObject;
	class AnimatedSpriteDelegate;
	class GJRobotSprite;
	class GJSpiderSprite;
	class HardStreak;
	class GhostTrailEffect;

	class GDH_DLL PlayerObject : public GameObject, public AnimatedSpriteDelegate {
	public:
		PAD(24);
		bool unk480;
		cocos2d::CCNode* unk484;
		cocos2d::CCDictionary* m_pCollisionLog; // 0x488
		cocos2d::CCDictionary* m_pCollisionLog1; // 0x48C
		PAD(32);
		bool unk4B0;
		cocos2d::CCSprite* unk4B4;
		PAD(28);
		bool unk4D4;
		cocos2d::CCArray* m_particleSystems; // 0x4D8
		bool unk4DC;
		bool m_isHidden; // 0x4DD
		int m_hasGhostTrail; // 0x4E0
		GhostTrailEffect* m_ghostTrail; // 0x4E4
		cocos2d::CCSprite* m_pIconSprite;
		cocos2d::CCSprite* m_pIconSpriteSecondary;
		cocos2d::CCSprite* m_pIconSpriteWhitener;
		cocos2d::CCSprite* m_pIconGlow;
		cocos2d::CCSprite* m_pVehicleSprite;
		cocos2d::CCSprite* m_pVehicleSpriteSecondary;
		cocos2d::CCSprite* unk500;
		cocos2d::CCSprite* m_pVehicleSpriteWhitener;
		cocos2d::CCSprite* m_pVehicleGlow;
		cocos2d::CCSprite* unk50C;
		cocos2d::CCMotionStreak* m_regularTrail; // 0x510
		HardStreak* m_waveTrail; // 0x514
		double m_xAccel; // 0x518
		double m_jumpAccel; // 0x520
		double m_gravity; // 0x528
		PAD(16);
		bool unk538;
		bool unk539;
		bool unk53A;
		bool unk53B;
		bool m_isInPlayLayer; // 0x53C
		bool unk53D;
		bool unk53E; // is holding on ship idk
		bool unk53F;
		PAD(16);
		double m_lastJumpTime; // 0x550
		double unk558;
		PAD(36);
		float unk584;
		PAD(20);
		GameObject* unk59C;
		PAD(8);
		GJRobotSprite* m_pRobot; //0x5A8
		GJSpiderSprite* m_pSpider; //0x5AC
		bool unk5B0;
		cocos2d::CCParticleSystemQuad* unk5B4;
		cocos2d::CCParticleSystemQuad* unk5B8;
		cocos2d::CCParticleSystemQuad* unk5BC;
		cocos2d::CCParticleSystemQuad* unk5C0;
		cocos2d::CCParticleSystemQuad* unk5C4;
		cocos2d::CCParticleSystemQuad* unk5C8;
		cocos2d::CCParticleSystemQuad* unk5CC;
		PAD(4);
		cocos2d::CCParticleSystemQuad* unk5D4;
		cocos2d::CCParticleSystemQuad* unk5D8;
		PAD(8);
		int m_nStreakID;	// 0x5e4
		float m_fWellIdk;	// 0x5e8
		PAD(16);
		bool unk5FC;
		bool unk5FD;
		bool unk5FE;
		PAD(17);
		bool unk610;
		bool m_isHolding; // 0x611
		bool m_hasJustHeld; // 0x612
		bool m_isHolding2; // 0x613 why are there 2 mr robert
		bool m_hasJustHeld2; // 0x614
		int unk618;
		float unk61C;
		int unk620; // always 213
		bool m_canRobotJump; // 0x624
		double m_yAccel; // 0x628
		bool unk630;
		bool unk631;
		float unk634;
		bool m_isShip; // 0x638
		bool m_isBird; // 0x639 ufo
		bool m_isBall; // 0x63A
		bool m_isDart; // 0x63B wave
		bool m_isRobot; // 0x63C
		bool m_isSpider; // 0x63D
		bool m_isUpsideDown; // 0x63E
		PAD(1);
		bool m_isOnGround; // 0x640
		bool m_isDashing; // 0x641
		float m_vehicleSize; // 0x644
		float m_playerSpeed; // 0x648
		cocos2d::CCPoint unk64C; // last pos idk
		cocos2d::CCPoint m_lastPortalPos; // 0x654
		cocos2d::CCLayer* unk65C;
		bool m_isSliding; // 0x660
		bool m_isRising; // 0x661
		bool unk662;
		cocos2d::CCPoint m_lastGroundedPos; // 0x664
		cocos2d::CCArray* m_touchingRings; // 0x66C
		GameObject* m_lastActivatedPortal; // 0x670
		bool unk674;
		PAD(7);
		cocos2d::CCPoint m_position; // 0x67C
		bool unk684;
		bool unk685;
		double unk688;
		PAD(8);
		float m_groundHeight; // 0x698
		float unk69C; // seems to be y vel, cant directly change it though
		PAD(4);
		float unk6A4[200]; // last 200 Y positions, tf
		PAD(28);

		virtual void setVisible(bool visible) {
			return reinterpret_cast<void(__thiscall*)(PlayerObject*, bool)>(
				base + 0x1FA860
				)(this, visible);
		}
		void pushButton(int button) {
			reinterpret_cast<void(__thiscall*)(PlayerObject*, int)>(base + 0x1F4E40)(this, button);
		}
		void releaseButton(int button) {
			reinterpret_cast<void(__thiscall*)(PlayerObject*, int)>(base + 0x1F4F70)(this, button);
		}
		bool playerIsFalling() {
			return reinterpret_cast<bool(__thiscall*)(PlayerObject*)>(base + 0x1f5d60)(this);
		}
		void runRotateAction() {
			reinterpret_cast<void(__thiscall*)(PlayerObject*)>(base + 0x1e9bf0)(this);
		}
		void playBurstEffect() {
			reinterpret_cast<void(__thiscall*)(PlayerObject*)>(base + 0x1f6790)(this);
		}
		void spiderTestJump(bool param1) {
			reinterpret_cast<void(__thiscall*)(PlayerObject*, bool)>(base + 0x1ed360)(this, param1);
		}
		void incrementJumps() {
			reinterpret_cast<void(__thiscall*)(PlayerObject*)>(base + 0x1e9a20)(this);
		}
		void flipGravity(bool param1, bool param2) {
			reinterpret_cast<void(__thiscall*)(PlayerObject*, bool, bool)>(base + 0x1f59d0)(this, param1, param2);
		}
		void runNormalRotation() {
			reinterpret_cast<void(__thiscall*)(PlayerObject*)>(base + 0x1e9c50)(this);
		}
		void runBallRotation() {
			reinterpret_cast<void(__thiscall*)(PlayerObject*)>(base + 0x1e9d10)(this);
		}
		void activateStreak() {
			reinterpret_cast<void(__thiscall*)(PlayerObject*)>(base + 0x1f9080)(this);
		}
	};
}

#endif