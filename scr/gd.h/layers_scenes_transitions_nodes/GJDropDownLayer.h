#ifndef __GJDROPDOWNLAYER_H__
#define __GJDROPDOWNLAYER_H__

#include <gd.h>

namespace gd {

	class GJListLayer;

	#pragma runtime_checks("s", off)
	class GDH_DLL GJDropDownLayer : public cocos2d::CCLayerColor {
	public:
		cocos2d::CCPoint m_obEndPosition;
		cocos2d::CCPoint m_obStartPosition;
		cocos2d::CCMenu* m_pButtonMenu;
		GJListLayer* m_pListLayer;
		bool m_bControllerEnabled; //?
		cocos2d::CCLayer* m_pLayer;
		bool m_bHidden; //?
		PAD(7);

	public:
		//CCNode vtable
		virtual void registerWithTouchDispatcher() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				base + 0x16990
				)(this);
		}
		virtual void draw() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				base + 0x16A80
				)(this);
		}

		//CCTouchDelegate vtable
		virtual bool ccTouchBegan(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent) { return true; }
		virtual void ccTouchMoved(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent) {}
		virtual void ccTouchEnded(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent) {}
		virtual void ccTouchCancelled(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent) {}

		//CCKeypadDelegate vtable
		virtual void keyBackClicked() {
			return reinterpret_cast<void(__thiscall*)(char*)>(
				base + 0x113960
				)(reinterpret_cast<char*>(this) + 0xF4);
		}

		//vtable
		virtual void customSetup() {}
		virtual void enterLayer() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				base + 0x16970
				)(this);
		}
		virtual void exitLayer(cocos2d::CCObject* btn) {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*, cocos2d::CCObject*)>(
				base + 0x113980
				)(this, btn);
		}
		virtual void showLayer(bool noTransition) {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*, bool)>(
				base + 0x1139C0
				)(this, noTransition);
		}
		virtual void hideLayer(bool noTransition) {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*, bool)>(
				base + 0x113A90
				)(this, noTransition);
		}
		virtual void layerVisible() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				base + 0x16A40
				)(this);
		}
		virtual void layerHidden() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				base + 0x113B60
				)(this);
		}
		virtual void enterAnimFinished() {}
		virtual void disableUI() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				base + 0x113920
				)(this);
		}
		virtual void enableUI() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				base + 0x113940
				)(this);
		}
		GJDropDownLayer() {
			reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				base + 0x038470
				)(this);
		}
		bool init(const char* title, float height) {
			__asm movss xmm2, height
			return reinterpret_cast<bool(__thiscall*)(GJDropDownLayer*, const char*)>(
				base + 0x113530
				)(this, title);
		}

		static GJDropDownLayer* create(const char* title, float height) {
			GJDropDownLayer* pRet = new GJDropDownLayer();
			if (pRet && pRet->init(title, height)) {
				pRet->autorelease();
				return pRet;
			}
			else {
				CC_SAFE_DELETE(pRet);
				return nullptr;
			}
		}
	};
	#pragma runtime_checks("s", restore)
}

#endif