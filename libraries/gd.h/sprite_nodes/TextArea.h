#ifndef __TEXTAREA_H__
#define __TEXTAREA_H__

#include <gd.h>

namespace gd {
	#pragma runtime_checks("s", off)
	class TextArea : public cocos2d::CCSprite {
	protected:
		PAD(0x58);

	public:
		static TextArea* create(const char* font, bool unknown,
			std::string caption, float scale, float width, float height) {
			__asm {
				movss xmm1, scale
				movss xmm2, width
				movss xmm3, height
			}
			auto pRet = reinterpret_cast<TextArea* (__fastcall*)(const char*,
				bool, std::string)>(
					base + 0x33270
					)(font, unknown, caption);
			__asm add esp, 0x20
			return pRet;
		}

		void colorAllCharactersTo(cocos2d::ccColor3B color) {
			reinterpret_cast<void(__thiscall*)(
				TextArea*, cocos2d::ccColor3B
			)>(base + 0x33830)(this, color);
		}
	};
	#pragma runtime_checks("s", restore)
}

#endif