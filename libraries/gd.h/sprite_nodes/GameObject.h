#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <gd.h>

namespace gd {

	class CCSpritePlus;

	// jesus fucking christ (painfully written by @hjfod)
	enum GameObjectType {
		kGameObjectTypeSolid = 0,
		kGameObjectTypeHazard = 2,
		kGameObjectTypeInverseGravityPortal = 3,
		kGameObjectTypeNormalGravityPortal = 4,
		kGameObjectTypeShipPortal = 5,
		kGameObjectTypeCubePortal = 6,
		kGameObjectTypeDecoration = 7,
		kGameObjectTypeYellowJumpPad = 8,
		kGameObjectTypePinkJumpPad = 9,
		kGameObjectTypeGravityPad = 10,
		kGameObjectTypeYellowJumpRing = 11,
		kGameObjectTypePinkJumpRing = 12,
		kGameObjectTypeGravityRing = 13,
		kGameObjectTypeInverseMirrorPortal = 14,
		kGameObjectTypeNormalMirrorPortal = 15,
		kGameObjectTypeBallPortal = 16,
		kGameObjectTypeRegularSizePortal = 17,
		kGameObjectTypeMiniSizePortal = 18,
		kGameObjectTypeUfoPortal = 19,
		kGameObjectTypeModifier = 20,
		kGameObjectTypeSecretCoin = 22,
		kGameObjectTypeDualPortal = 23,
		kGameObjectTypeSoloPortal = 24,
		kGameObjectTypeSlope = 25,
		kGameObjectTypeWavePortal = 26,
		kGameObjectTypeRobotPortal = 27,
		kGameObjectTypeTeleportPortal = 28,
		kGameObjectTypeCollectible = 30,
		kGameObjectTypeUserCoin = 31,
		kGameObjectTypeDropRing = 32,
		kGameObjectTypeSpiderPortal = 33,
		kGameObjectTypeRedJumpPad = 34,
		kGameObjectTypeRedJumpRing = 35,
		kGameObjectTypeCustomRing = 36,
		kGameObjectTypeDashRing = 37,
		kGameObjectTypeGravityDashRing = 38,
		kGameObjectTypeCollisionObject = 39,
		kGameObjectTypeSpecial = 40,
	};

	class GJSpriteColor;
	class ColorActionSprite;
	class GJEffectManager;

	class GJSpriteColor;
	class ColorActionSprite;
	class GJEffectManager;

	#pragma runtime_checks("s", off)
	class GameObject : public CCSpritePlus {
	public:
		PAD(44);
		bool m_unk21C;
		bool m_bSomethingSawRelated; // 0x21D
		bool m_unk21E;
		PAD(13);
		cocos2d::CCPoint m_obStartPosOffset; //0x22C
		PAD(4);
		bool m_unk238;
		bool m_isFlippedX; //0x239
		bool m_isFlippedY; //0x23A
		PAD(1);
		cocos2d::CCPoint m_obBoxOffset; //0x23C
		bool m_isOriented; //0x244 idek what this is
		PAD(3);
		cocos2d::CCPoint m_unk248; // related to box offset
		PAD(4);
		bool m_unk254;
		PAD(11);
		cocos2d::CCAction* m_pMyAction; //0x260
		PAD(4);
		cocos2d::CCSize m_obObjectSize; //0x268
		bool m_unk270;
		bool m_unk271;
		bool m_unk272;
		PAD(1);
		cocos2d::CCParticleSystemQuad* m_pParticleSystem;
		std::string m_sEffectPlistName; //0x278
		PAD(32);
		bool m_unk2B0; // has custom scale or moved idfk
		PAD(7);
		cocos2d::CCRect m_objectRect2; //0x2B8
		bool m_bIsObjectRectDirty; //0x2C8
		bool m_bIsOrientedRectDirty; //0x2C9
		bool m_bHasBeenActivated; //0x2CA
		bool m_bHasBeenActivatedP2; //0x2CB
		PAD(16);
		bool m_bSawIsDisabled; // 0x2dc
		PAD(7);
		cocos2d::CCSprite* m_pDetailSprite; //0x2E4
		PAD(8);
		bool m_bIsRotatedSide; //0x2F0 for 90 and 270 degrees rotations
		PAD(3);
		float m_unk2F4;
		float m_unk2F8;
		int m_nUniqueID; //0x2FC
		GameObjectType m_nObjectType; //0x300
		int m_nSection; //0x304
		bool m_bTouchTriggered; //0x308
		bool m_bSpawnTriggered; //0x309
		PAD(2)
		cocos2d::CCPoint m_obStartPosition; //0x30C
		std::string m_sTextureName; //0x314
		bool m_unk32C;
		bool m_unk32D;
		PAD(14);
		float m_unk33C;
		float m_unk340;
		PAD(16);
		bool m_bIsGlowDisabled; //0x354
		PAD(3);
		int m_nTargetColorID;	// 0x358 (for color triggers)
		float m_fScale; //0x35C
		int m_nObjectID; //0x360
		PAD(4);
		bool m_unk368;
		bool m_unk369;
		bool m_unk36A;
		bool m_bIsDontEnter; //0x36B
		bool m_bIsDontFade; //0x36C
		int m_nDefaultZOrder; // 0x370
		PAD(23);
		bool m_unk38C;
		bool m_unk38D;
		bool m_unk38E;
		PAD(1);
		float m_unk390;
		PAD(20);
		GJSpriteColor* m_pBaseColor; //0x3A8
		GJSpriteColor* m_pDetailColor; //0x3AC
		PAD(4);
		int m_nDefaultZLayer; // 0x03B4
		int m_nZLayer; //0x3B8
		int m_nGameZOrder; //0x3BC
		std::string m_unk3C0;
		bool m_unk3D8;
		bool m_unk3D9;
		bool m_bIsSelected; //0x3DA
		PAD(1);
		int m_nGlobalClickCounter; //0x3DC i have no idea what this is for
		PAD(12);
		float m_fMultiScaleMultiplier;
		bool m_bIsGroupParent; //0x3F0
		PAD(3);
		short* m_pGroups; //0x3F4
		short m_nGroupCount; //0x3F8
		PAD(18);
		int m_nEditorLayer; //0x40C
		int m_nEditorLayer2; //0x410
		int m_unk414;
		PAD(12);
		cocos2d::CCPoint m_obFirstPosition; //0x424 first position from when its placed in the editor
		PAD(28);
		bool m_bHighDetail; //0x448
		PAD(3);
		ColorActionSprite* m_pColorActionSprite1; //0x44C
		ColorActionSprite* m_pColorActionSprite2; //0x450
		GJEffectManager* m_pEffectManager; //0x454
		PAD(16);

		//CCNode vtable
		virtual void setScaleX(float scale) {
			return reinterpret_cast<void(__thiscall*)(GameObject*, float)>(
				base + 0xE5050
				)(this, scale);
		}
		virtual void setScaleY(float scale) {
			return reinterpret_cast<void(__thiscall*)(GameObject*, float)>(
				base + 0xE50E0
				)(this, scale);
		}
		virtual void setScale(float scale) {
			return reinterpret_cast<void(__thiscall*)(GameObject*, float)>(
				base + 0xE5170
				)(this, scale);
		}
		virtual void setPosition(const cocos2d::CCPoint& pos) {
			return reinterpret_cast<void(__thiscall*)(GameObject*, const cocos2d::CCPoint&)>(
				base + 0xE4DE0
				)(this, pos);
		}
		virtual void setVisible(bool visible) {
			return reinterpret_cast<void(__thiscall*)(GameObject*, bool)>(
				base + 0xE57C0
				)(this, visible);
		}
		virtual void setRotation(float rotation) {
			return reinterpret_cast<void(__thiscall*)(GameObject*, float)>(
				base + 0xE4ED0
				)(this, rotation);
		}
		virtual bool initWithTexture(cocos2d::CCTexture2D* texture) {
			return reinterpret_cast<bool(__thiscall*)(GameObject*, cocos2d::CCTexture2D*)>(
				base + 0xCFA80
				)(this, texture);
		}
		virtual void setChildColor(const cocos2d::ccColor3B& color) {
			return reinterpret_cast<void(__thiscall*)(GameObject*, const cocos2d::ccColor3B&)>(
				base + 0xEE900
				)(this, color);
		}

		//CCRGBAProtocol vtable
		virtual void setOpacity(GLubyte opacity) {
			return reinterpret_cast<void(__thiscall*)(GameObject*, GLubyte)>(
				base + 0xE53C0
				)(this, opacity);
		}

		static GameObject* createWithKey(int key) {
			return reinterpret_cast<GameObject* (__fastcall*)(int)>(
				base + 0xcf4f0
				)(key);
		}
		static GameObject* createWithFrame(const char* frame) {
			return reinterpret_cast<GameObject* (__fastcall*)(const char*)>(
				base + 0xCF8F0
				)(frame);
		}
		static GameObject* objectFromString(std::string str, bool unknown) {
			auto pRet = reinterpret_cast<GameObject* (__fastcall*)(std::string, bool)>(
				base + 0xEBE50
				)(str, unknown);
			__asm add esp, 0x18
			return pRet;
		}

		void deselectObject() {
			this->m_bIsSelected = false;

			return reinterpret_cast<void(__fastcall*)(GameObject*)>(
				base + 0xeee50
			)(this);
		}

		std::string getSaveString() {
			std::string ret;

			reinterpret_cast<void(__thiscall*)(GameObject*, std::string*)>(
				base + 0xed0c0
			)(
				this, &ret
			);

			return ret;
		}

		void addToGroup(int id) {
			reinterpret_cast<void(__thiscall*)(
				GameObject*, int
			)>(
				base + 0xeb8d0
			)(
				this, id
			);
		}

		cocos2d::CCPoint getStartPosition() { return m_obStartPosition; }
		void setStartPosition(cocos2d::CCPoint const& p) { m_obStartPosition = p; }
		
		unsigned int getUniqueID() { return m_nUniqueID; }
		short getGroupID(int ix) { return m_pGroups[ix]; }
		short getGroupIDCount() { return m_nGroupCount; }
		std::vector<short> getGroupIDs() {
			std::vector<short> res;

			if (m_pGroups && m_pGroups[0])
				for (auto i = 0; i < m_nGroupCount; i++)
					res.push_back(m_pGroups[i]);
		
			return res;
		}
		int getGameZOrder() { return m_nGameZOrder; }
		void setGameZOrder(int z) { m_nGameZOrder = z; }
		void setGameObjType(GameObjectType t) { m_nObjectType = t; }

		GJSpriteColor* getBaseColor() { return m_pBaseColor; }
		GJSpriteColor* getDetailColor() { return m_pDetailColor; }
		
		void updateCustomScale(float scale) {
			__asm movss xmm1, scale;
			reinterpret_cast<void(__thiscall*)(GameObject*)>(base + 0xe5340)(this);
		}
		// this is actually a virtual but i cant be bothered to get it in the right place
		void setRScale(float scale) {
			reinterpret_cast<void(__thiscall*)(GameObject*, float)>(base + 0xe5280)(this, scale);
		}
		void setObjectColor(const cocos2d::ccColor3B& color) {
			reinterpret_cast<void(__thiscall*)(GameObject*, const cocos2d::ccColor3B&)>(base + 0xee620)(this, color);
		}
		void selectObject(const cocos2d::ccColor3B color) {
			reinterpret_cast<void(__thiscall*)(GameObject*, const cocos2d::ccColor3B)>(base + 0xee960)(this, color);
		}

		cocos2d::CCRepeatForever* createRotateAction(float f, int n) {
			__asm movss xmm1, f

			auto pRet = reinterpret_cast<cocos2d::CCRepeatForever*(__thiscall*)(GameObject*, int)>(
				base + 0xe49b0
			)(this, n);

			return pRet;
		}

		void setMyAction(cocos2d::CCAction* pAction) {
			return reinterpret_cast<void(__thiscall*)(GameObject*, cocos2d::CCAction*)>(
				base + 0xd1b90
			)(this, pAction);
		}
	};

	#pragma runtime_checks("s", restore)
}

#endif