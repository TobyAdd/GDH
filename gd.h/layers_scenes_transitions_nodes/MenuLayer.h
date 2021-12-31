#ifndef __MENULAYER_H__
#define __MENULAYER_H__

#include <gd.h>

namespace gd {
	class FLAlertLayerProtocol;

	class GDH_DLL MenuLayer : public cocos2d::CCLayer, public FLAlertLayerProtocol, public GooglePlayDelegate {
	public:
		static MenuLayer* create() {
			return reinterpret_cast<MenuLayer*(__stdcall*)()>(base + 0x190550)();
		}
		static cocos2d::CCScene* scene(bool options) {
			return reinterpret_cast<cocos2d::CCScene*(__fastcall*)(bool)>(base + 0x190720)(options);
		}
		virtual bool init() {
			return reinterpret_cast<bool(__thiscall*)(void*)>(base + 0x1907b0)(this);
		}
		virtual void keyBackClicked() {
			reinterpret_cast<void(__thiscall*)(void*)>(base + 0x191f90)(this);
		}
		virtual void keyDown(cocos2d::enumKeyCodes key) {
			reinterpret_cast<void(__thiscall*)(void*, cocos2d::enumKeyCodes)>(base + 0x1922c0)(this, key);
		}
		virtual void googlePlaySignedIn() {
			reinterpret_cast<void(__thiscall*)(void*)>(base + 0x191b20)(this);
		}
		virtual void FLAlert_Clicked(gd::FLAlertLayer* alert, bool btn2) {
			// thx pie
			reinterpret_cast<void(__thiscall*)(char*, void*, bool)>(base + 0x192150)(reinterpret_cast<char*>(this) + sizeof(cocos2d::CCLayer), alert, btn2);
		}
	};
}

#endif