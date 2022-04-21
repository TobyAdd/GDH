#ifndef __CCMENUITEMTOGGLER_H__
#define __CCMENUITEMTOGGLER_H__

#include <gd.h>

namespace gd {

	class CCMenuItemSpriteExtra;

	#pragma runtime_checks("s", off)
	class GDH_DLL CCMenuItemToggler : public cocos2d::CCMenuItem {
	public:
		CCMenuItemSpriteExtra* m_pOnButton;		// 0x10c
		CCMenuItemSpriteExtra* m_pOffButton;	// 0x110
		bool m_bOn;				// 0x114
		bool m_bNotClickable;	// 0x115
		PAD(2)

		bool init(
			cocos2d::CCNode* off,
			cocos2d::CCNode* on,
			cocos2d::CCObject* target,
          	cocos2d::SEL_MenuHandler handler
		) {
			return reinterpret_cast<bool(__thiscall*)(
				CCMenuItemToggler*, cocos2d::CCNode*, cocos2d::CCNode*,
				cocos2d::CCObject*, cocos2d::SEL_MenuHandler
			)>(
				base + 0x196e0
			)(
				this, off, on, target, handler
			);
		}

		// inlined on windows
		CCMenuItemToggler() : cocos2d::CCMenuItem() {
			*reinterpret_cast<uintptr_t*>(this) =
				base + 0x2889d0;
			*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(this) + 0xec) =
				base + 0x288994;
			
			this->m_pOnButton = nullptr;
			this->m_pOffButton = nullptr;
			this->m_bOn = false;
			this->m_bNotClickable = false;
		}

	public:
		virtual void activate() override {
			reinterpret_cast<void(__thiscall*)(CCMenuItemToggler*)>(
				base + 0x198d0
			)(this);
		}

		virtual void selected() override {
			reinterpret_cast<void(__thiscall*)(CCMenuItemToggler*)>(
				base + 0x198a0
			)(this);
		}

		virtual void unselected() override {
			reinterpret_cast<void(__thiscall*)(CCMenuItemToggler*)>(
				base + 0x19900
			)(this);
		}

		virtual void setEnabled(bool enabled) override {
			reinterpret_cast<void(__thiscall*)(CCMenuItemToggler*, bool)>(
				base + 0x19930
			)(this, enabled);
		}

		static CCMenuItemToggler* create(cocos2d::CCNode* off, cocos2d::CCNode* on,
			cocos2d::CCObject* target, cocos2d::SEL_MenuHandler callback) {
			auto pRet = reinterpret_cast<CCMenuItemToggler* (__fastcall*)(cocos2d::CCNode*,
				cocos2d::CCNode*, cocos2d::CCObject*, cocos2d::SEL_MenuHandler)>(
					base + 0x19600
					)(off, on, target, callback);
			__asm add esp, 0x8
			return pRet;
		}
		static CCMenuItemToggler* createWithSize(const char* spr1, const char* spr2,
			cocos2d::CCObject* target, cocos2d::SEL_MenuHandler callback, float scale) {
			auto sprOff = cocos2d::CCSprite::createWithSpriteFrameName(spr1);
			auto sprOn = cocos2d::CCSprite::createWithSpriteFrameName(spr2);

			sprOff->setScale(scale);
			sprOn->setScale(scale);

			return create(sprOff, sprOn, target, callback);
		}
		static CCMenuItemToggler* createWithStandardSprites(cocos2d::CCObject* target, cocos2d::SEL_MenuHandler callback, float scale = 1.0f) {
			auto sprOff = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
			auto sprOn = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");

			sprOff->setScale(scale);
			sprOn->setScale(scale);

			return create(sprOff, sprOn, target, callback);
		}
		void setSizeMult(float mult) {
			__asm movss xmm1, mult
			return reinterpret_cast<void(__thiscall*)(CCMenuItemToggler*)>(
				base + 0x19850
				)(this);
		}
		//my own function
		inline bool isOn() { return m_bOn; }
		inline bool isToggled() { return m_bOn; }
		
		void setClickable(bool on) { m_bNotClickable = !on; }
		void toggle(bool on) {
			return reinterpret_cast<void(__thiscall*)(CCMenuItemToggler*, bool)>(
				base + 0x199B0
				)(this, on);
		}
		void toggleWithCallback(bool on) {
			this->activate();
			this->toggle(on);
		}
	};
	#pragma runtime_checks("s", restore)
}

#endif