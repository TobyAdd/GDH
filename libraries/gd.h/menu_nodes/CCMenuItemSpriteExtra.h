#ifndef __CCMENUITEMSPRITEEXTRA_H__
#define __CCMENUITEMSPRITEEXTRA_H__

#include <gd.h>

namespace gdh {
	#pragma runtime_checks("s", off)
	class CCMenuItemSpriteExtra : public cocos2d::CCMenuItemSprite {
	protected:
		float m_fUnknown;
		float m_fUnknown2;

	public:
		static CCMenuItemSpriteExtra* create(cocos2d::CCNode* sprite,
			cocos2d::CCObject* target, cocos2d::SEL_MenuHandler callback) {
			auto pRet = reinterpret_cast<CCMenuItemSpriteExtra* (__thiscall*)(cocos2d::CCNode*,
				cocos2d::CCObject*, cocos2d::SEL_MenuHandler)>(
					gdh::base + 0x25830
					)(sprite, target, callback);
			//fix stack before returning
			__asm add esp, 0x8
			return pRet;
		}
		void setSizeMult(float mult) {
			__asm movss xmm1, mult
			return reinterpret_cast<void(__thiscall*)(CCMenuItemSpriteExtra*)>(
				gdh::base + 0x19080
				)(this);
		}
	};
	#pragma runtime_checks("s", restore)
}

#endif