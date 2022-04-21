#ifndef __LOADINGCIRCLE_H__
#define __LOADINGCIRCLE_H__

#include <gd.h>

namespace gd {
	class GDH_DLL LoadingCircle : public cocos2d::CCLayerColor {
	protected:
		cocos2d::CCSprite* m_pSprite;
		cocos2d::CCLayer* m_pParentLayer;
		bool m_bFade;

	public:
		static LoadingCircle* create() {
			return reinterpret_cast<LoadingCircle* (__stdcall*)()>(
				base + 0x2A0D0
				)();
		}
		void show() {
			return reinterpret_cast<void(__thiscall*)(LoadingCircle*)>(
				base + 0x2A290
				)(this);
		}
		void setParentLayer(cocos2d::CCLayer* layer) { m_pParentLayer = layer; }
		void setFade(bool fade) { m_bFade = fade; }
		void fadeAndRemove() {
			return reinterpret_cast<void(__thiscall*)(LoadingCircle*)>(
				base + 0x2A370
				)(this);
		}
	};
}

#endif