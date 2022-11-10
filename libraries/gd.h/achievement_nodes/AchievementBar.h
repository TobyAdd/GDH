#ifndef __ACHIEVEMENTBAR_H__
#define __ACHIEVEMENTBAR_H__

#include <gd.h>

namespace gd {
	#pragma runtime_checks("s", off)
	class AchievementBar : public cocos2d::CCNodeRGBA {
	protected:
		PAD(0x24);

	public:
		static AchievementBar* create(const char* title,
			const char* desc, const char* icon, bool quest) {
			auto pRet = reinterpret_cast<AchievementBar* (__fastcall*)(const char*,
				const char*, const char*, bool)>(
					base + 0x3B120
					)(title, desc, icon, quest);
			__asm add esp, 0x8
			return pRet;
		}
	};
	#pragma runtime_checks("s", restore)
}

#endif