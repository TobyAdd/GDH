#ifndef __GJLISTLAYER_H__
#define __GJLISTLAYER_H__

#include "gd.h"

namespace gd {
	#pragma runtime_checks("s", off)
	class GDH_DLL GJListLayer : public cocos2d::CCLayerColor {
	public:
		BoomListView* m_pListView;

	public:
		static GJListLayer* create(
			BoomListView* list,
			const char* title,
			cocos2d::ccColor4B color,
			float width,
			float height
		) {
			__asm movss xmm3, width

			auto pRet = reinterpret_cast<GJListLayer * (__fastcall*)(
				BoomListView*, const char*, cocos2d::ccColor4B, float
			)>(
				base + 0x12E000
			)(
				list, title, color, height
			);

			__asm add esp, 0x8

			return pRet;
		}
	};
	#pragma runtime_checks("s", restore)
}

#endif