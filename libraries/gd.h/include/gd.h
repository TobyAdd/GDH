#ifndef __GD_H__
#define __GD_H__

#include <cocos2d.h>

namespace gdh {
	inline auto base = reinterpret_cast<char*>(GetModuleHandle(0));

	inline bool init() {
		//if the base address is valid, all other values should be valid.
		#if (__WARN_DEBUG__ == true)
		static_assert(sizeof(std::string) == 24,
			"Any classes containing member strings or using strings in functions will break!\n"
			"");
		#endif
		return base;
	}
}

#include "menu_nodes/CCMenuItemSpriteExtra.h"

#endif