#ifndef __CCTEXTINPUTNODE_H__
#define __CCTEXTINPUTNODE_H__

#include <gd.h>

namespace gd {
	class TextInputDelegate;

	#pragma runtime_checks("s", off)
	class GDH_DLL CCTextInputNode : public cocos2d::CCLayer, public cocos2d::CCIMEDelegate, public cocos2d::CCTextFieldDelegate {
		public:
			int m_nUnknown0;
			std::string m_sCaption;
			int m_nUnknown1;
			int m_nUnknown2;
			std::string m_sFilter;
			float m_fWidth;
			float m_fMaxLabelScale;
			float m_fPlaceholderScale;
			cocos2d::ccColor3B m_cPlaceholderColor;
			cocos2d::ccColor3B m_cNormalColor;
			cocos2d::CCLabelBMFont* m_pCursor;
			cocos2d::CCTextFieldTTF* m_pTextField;
			TextInputDelegate* m_delegate;
			int m_nMaxLabelLength;
			cocos2d::CCLabelBMFont* m_pPlaceholderLabel;
			bool m_bUnknown;
			bool m_bUnknown2;
			bool m_bForceOffset;

			void visit() override {
				reinterpret_cast<void(__thiscall*)(CCTextInputNode*)>(
					base + 0x21000
				)(this);
			}

			void registerWithTouchDispatcher() override {
				reinterpret_cast<void(__thiscall*)(CCTextInputNode*)>(
					base + 0x220e0
				)(this);
			}

			virtual void onClickTrackNode(bool idk) {
				reinterpret_cast<void(__thiscall*)(CCTextInputNode*, bool)>(
					base + 0x216b0
				)(this, idk);
			}

			virtual bool ccTouchBegan(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent) override {
				return reinterpret_cast<bool(__thiscall*)(
					CCTextInputNode*, cocos2d::CCTouch*, cocos2d::CCEvent*
				)>(
					base + 0x1f20
				)(this, pTouch, pEvent);
			}

			bool init(
				const char* sCaption, cocos2d::CCObject* pTarget,
				const char* sFontFile, float fWidth, float fHeight
			) {
				__asm {
					movss xmm1, fWidth
					movss xmm2, fHeight
				}

				auto bRet = reinterpret_cast<CCTextInputNode* (__thiscall*)
					(CCTextInputNode*, const char*, cocos2d::CCObject*, const char*, const char*)>(
					base + 0x20e50
				)(this, sCaption, pTarget, "", sFontFile);

				return bRet;
			}

		public:
			static CCTextInputNode* create(const char* caption, cocos2d::CCObject* target, 
				const char* fntFile, float width, float height) {
				__asm {
					movss xmm0, width
					movss xmm1, height
				}
				auto pRet = reinterpret_cast<CCTextInputNode* (__thiscall*)
					(const char*, cocos2d::CCObject*, const char*)>(
					base + 0x20D90
				)(caption, target, fntFile);
				__asm add esp, 0x8
				return pRet;
			}
			void setLabelPlaceholderColor(cocos2d::ccColor3B color) {
				m_cPlaceholderColor = color;
				this->refreshLabel();
			}
			void setLabelPlaceholerScale(float scale) {
				m_fPlaceholderScale = scale;
				this->refreshLabel();
			}
			void setMaxLabelScale(float scale) {
				m_fMaxLabelScale = scale;
				this->refreshLabel();
			}
			void setMaxLabelLength(int length) { m_nMaxLabelLength = length; }
			void setAllowedChars(std::string filter) { m_sFilter = filter; }
			void refreshLabel() {
				return reinterpret_cast<void(__thiscall*)(CCTextInputNode*)>(
					base + 0x21330
					)(this);
			}
			cocos2d::CCTextFieldTTF* getTextField() { return m_pTextField; }
			void setString(const char* text) { m_pTextField->setString(text); }
			const char* getString() { return m_pTextField->getString(); }
			cocos2d::CCLabelBMFont* getPlaceholderLabel() { return m_pPlaceholderLabel; }
			void setDelegate(TextInputDelegate* delegate) { m_delegate = delegate; }
	};
	#pragma runtime_checks("s", restore)
}

#endif
