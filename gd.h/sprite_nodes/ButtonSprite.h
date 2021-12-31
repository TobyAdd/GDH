#ifndef __BUTTONSPRITE_H__
#define __BUTTONSPRITE_H__

#include <gd.h>

namespace cocos2d {
	namespace extension {
		class CCScale9Sprite;
	}
}

namespace gd {
	#pragma runtime_checks("s", off)
	class GDH_DLL ButtonSprite : public cocos2d::CCSprite {
		public:
			PAD(24)
			cocos2d::CCLabelBMFont* m_pLabel; // 0x1fc
			cocos2d::CCSprite* m_pSubSprite;	// 0x200
			cocos2d::CCSprite* m_pSubBGSprite;  // 0x204
			cocos2d::extension::CCScale9Sprite* m_pBGSprite;	// 0x208
			PAD(8)
			cocos2d::CCPoint m_obSpritePosition;	// 0x214
			
			/*PARAMS:
			* caption - the button's caption.
			* width - the width of the button. only used if absolute is TRUE.
			* absolute - whether or not to use provided width. if FALSE, game will auto-adjust.
			* font - font file for caption.
			* texture - texture file for button background.
			* height - height of button. put 0 for auto.
			* scale - scale of the caption.
			*/
			static ButtonSprite* create(const char* caption, int width, bool absolute,
				const char* font, const char* texture, float height, float scale) {
				//scale is passed in lower 4 bytes of xmm3
				__asm movss xmm3, scale
				//arg 3 is always 0. dunno why it's not optimized out as a param
				auto pRet = reinterpret_cast<ButtonSprite* (__fastcall*)(const char*,
					int, int, bool, const char*, const char*, float)>(
						base + 0x137D0
						)(caption, width, 0, absolute, font, texture, height);
				//clean stack before returning
				__asm add esp, 0x14
				return pRet;
			}

			static ButtonSprite* create(
				CCNode* subSpr, int width, bool absolute, float scale,
				int idk0, const char* texBG, bool idk1, float height
			) {
				__asm movss xmm3, height

				auto pRet = reinterpret_cast<ButtonSprite*(__fastcall*)(
					CCNode*, int, bool, float, int, const char*, bool
				)>(
					base + 0x134b0
				)(subSpr, width, absolute, scale, idk0, texBG, idk1);

				__asm add esp, 0x14

				return pRet;
			}

			void updateBGImage(const char* spr) {
				reinterpret_cast<void(__thiscall*)(ButtonSprite*, const char*)>(
					base + 0x13af0
				)(this, spr);
			}

			void setString(const char* txt) {
				reinterpret_cast<void(__thiscall*)(ButtonSprite*, const char*)>(
					base + 0x14030
				)(this, txt);
			}

			void updateSpriteBGSize(cocos2d::CCPoint const& offset) {
				reinterpret_cast<void(__thiscall*)(ButtonSprite*, cocos2d::CCPoint)>(
					base + 0x13c00
				)(this, offset);

				__asm add esp, 0x8
			}
	};
	#pragma runtime_checks("s", restore)
}

#endif