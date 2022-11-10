#ifndef __LOADINGLAYER_H__
#define __LOADINGLAYER_H__

#include <gd.h>

namespace gd {
	class LoadingLayer : public cocos2d::CCLayer {
	protected:
		PAD(4);
		int m_nLoadIndex;
		cocos2d::CCLabelBMFont* m_pCaption;
		PAD(4);
		//artifacts of rob debugging something
		cocos2d::CCSprite* m_pSliderBar;
		float m_fSliderGrooveXPos;
		PAD(4);
		bool m_bFromRefresh;

	public:
		static LoadingLayer* create(bool fromReload) {
			return reinterpret_cast<LoadingLayer* (__fastcall*)(bool)>(
				base + 0x18BFE0
				)(fromReload);
		}
		void setFromRefresh(bool value) {
			m_bFromRefresh = value;
		}
	};
}

#endif