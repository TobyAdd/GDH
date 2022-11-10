#ifndef __SCROLLINGLAYER_H__
#define __SCROLLINGLAYER_H__

#include <gd.h>

namespace gd {
	class ScrollingLayer : public cocos2d::CCLayerColor {
		public:
			PAD(0x28)
			float m_fLayerHeight;
			PAD(0x4)
			cocos2d::CCLayer * m_pScrollLayer;
			cocos2d::CCNode* m_pParent;

			static ScrollingLayer * create(cocos2d::CCSize _size, cocos2d::CCPoint _point, float _unknown) {
				auto pRet = reinterpret_cast<ScrollingLayer*(__stdcall*)(
					cocos2d::CCSize, cocos2d::CCPoint, float
				)>(
					base + 0x2d8e0
				)(
					_size, _point, _unknown
				);

				__asm add esp, 0x14

				return pRet;
			}
	};
}

#endif