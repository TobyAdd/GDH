#ifndef __CCMENUITEMSPRITEEXTRA_H__
#define __CCMENUITEMSPRITEEXTRA_H__

#include <gd.h>

namespace gd {
	enum MenuAnimationType {
		kMenuAnimationTypeScale	= 0,
		kMenuAnimationTypeMove 	= 1,
	};

	#pragma runtime_checks("s", off)
	class GDH_DLL CCMenuItemSpriteExtra : public cocos2d::CCMenuItemSprite {
	public:
		float m_fScaleMultiplier;	// 0x118
		float m_fBaseScale;			// 0x11c
		bool m_bAnimationEnabled;	// 0x120
		bool m_bColorEnabled;		// 0x121
		PAD(2)
		PAD(52)
		float m_fColorDip;			// 0x158
		cocos2d::CCPoint m_obDestPosition;	// 0x15c
		cocos2d::CCPoint m_obOffset;	// 0x164
		MenuAnimationType m_nAnimationType = kMenuAnimationTypeScale;	// 0x16c
		cocos2d::CCPoint m_obStartPosition;	// 0x170
		PAD(0xc)

		bool init(cocos2d::CCNode* spr) {
			return reinterpret_cast<bool(__thiscall*)(
				CCMenuItemSpriteExtra*, cocos2d::CCNode*
			)>(
				base + 0x18fa0
			)(
				this, spr
			);
		}

		CCMenuItemSpriteExtra* destructor(bool rel) {
			return reinterpret_cast<CCMenuItemSpriteExtra*(__thiscall*)(
				CCMenuItemSpriteExtra*, bool
			)>(
				base + 0x18eb0
			)(
				this, rel
			);
		}

		CCMenuItemSpriteExtra() {
			reinterpret_cast<CCMenuItemSpriteExtra*(__thiscall*)(
				CCMenuItemSpriteExtra*
			)>(
				base + 0x18db0
			)(this);
		}

		virtual void activate() {
			reinterpret_cast<void(__fastcall*)(
				CCMenuItemSpriteExtra*
			)>(base + 0x191c0)(this);
		}

		virtual void selected() {
			reinterpret_cast<void(__fastcall*)(
				CCMenuItemSpriteExtra*
			)>(base + 0x19270)(this);
		}

		virtual void unselected() {
			reinterpret_cast<void(__fastcall*)(
				CCMenuItemSpriteExtra*
			)>(base + 0x19430)(this);
		}

	public:
		static CCMenuItemSpriteExtra* create(cocos2d::CCNode* sprite,
			cocos2d::CCObject* target, cocos2d::SEL_MenuHandler callback) {
			auto pRet = reinterpret_cast<CCMenuItemSpriteExtra* (__thiscall*)(cocos2d::CCNode*,
				cocos2d::CCObject*, cocos2d::SEL_MenuHandler)>(
					base + 0x18EE0
					)(sprite, target, callback);
			//fix stack before returning
			__asm add esp, 0x8
			return pRet;
		}
		void setSizeMult(float mult) {
			__asm movss xmm1, mult
			return reinterpret_cast<void(__thiscall*)(CCMenuItemSpriteExtra*)>(
				base + 0x19080
				)(this);
		}
		void useAnimationType(MenuAnimationType type) {
			this->m_obStartPosition = this->getNormalImage()->getPosition();
			this->m_nAnimationType = type;
		}
		void setDestination(cocos2d::CCPoint const& pos) {
			this->m_obDestPosition = pos;
		}
		void setOffset(cocos2d::CCPoint const& pos) {
			this->m_obOffset = pos;
		}
		void setScale(float scale) override {
			this->CCMenuItemSprite::setScale(scale);
			this->m_fBaseScale = scale;
		}
	};
	#pragma runtime_checks("s", restore)
}

#endif