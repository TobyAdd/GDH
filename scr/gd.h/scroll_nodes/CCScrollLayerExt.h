#ifndef __CCSCROLLLAYEREXT_H__
#define __CCSCROLLLAYEREXT_H__

#include <gd.h>

namespace gd {

	class CCContentLayer;
	class TableView;

	class GDH_DLL TableViewCell : public cocos2d::CCLayer {
		public:
			bool m_bUnknown;	// 0x11c
			PAD(3)
			TableView* m_pTableView; 	// 0x120
			CCIndexPath m_iIndexPath;	// 0x124
			std::string m_sUnknownString;	// 0x14c
			PAD(4)
			float m_fWidth;		// 0x168
			float m_fHeight;	// 0x16c
			cocos2d::CCLayerColor* m_pBGLayer;	// 0x170
			cocos2d::CCLayer* m_pLayer;	// 0x174
			PAD(4)

			TableViewCell(const char* name, float width, float height) {
				__asm {
					movss xmm2, width
					movss xmm3, height
				}
				reinterpret_cast<void(__thiscall*)(TableViewCell*, const char*)>(
					base + 0x32E70
				)(this, name);
			}

			static TableViewCell* create(const char* name, float width, float height) {
				auto pRet = new TableViewCell(name, width, height);

				if (pRet) {
					pRet->autorelease();
					return pRet;
				}

				CC_SAFE_DELETE(pRet);
				return nullptr;
			}
	};

	class GDH_DLL StatsCell : public TableViewCell {
		public:
			void updateBGColor(unsigned int index) {
				reinterpret_cast<void(__thiscall*)(StatsCell*, unsigned int)>(
					base + 0x59cf0
				)(this, index);
			}
	};
	
	class GDH_DLL LevelCell /* 0x184 */ : public TableViewCell /* 0x17c */ {
		public:
			GJGameLevel* m_pLevel;  // 0x17c
			PAD(4)
	};

	class GDH_DLL CCScrollLayerExt : public cocos2d::CCLayer {
		public:
			cocos2d::CCTouch* m_pTouch;	// 0x11c
			cocos2d::CCPoint m_obTouchPosition;		 // 0x120
			cocos2d::CCPoint m_obTouchStartPosition; // 0x128
			PAD(8)
			bool m_bTouchDown;	// 0x138
			bool m_bNotAtEndOfScroll;	// 0x139
			PAD(2)
			cocos2d::CCLayerColor* m_pVerticalScrollbar;	// 0x13c
			cocos2d::CCLayerColor* m_pHorizontalScrollbar;	// 0x140
			CCScrollLayerExtDelegate* m_pDelete;	// 0x144
			CCContentLayer* m_pContentLayer;	// 0x148
			bool m_bCutContent; // 0x14c
			bool m_bVScrollbarVisible;	// 0x14d
			bool m_bHScrollbarVisible;	// 0x14e
			bool m_bDisableHorizontal; // 0x14f
			bool m_bDisableVertical; // 0x150
			bool m_bDisableMovement; // 0x151
			PAD(2)
			float m_fScrollLimitTop; // 0x154
			float m_fScrollLimitBottom; // 0x158
			float m_fPeekLimitTop; // 0x15c
			float m_fPeekLimitBottom; // 0x160

			CCScrollLayerExt(cocos2d::CCRect rect) {
				reinterpret_cast<void(__thiscall*)(CCScrollLayerExt*, cocos2d::CCRect)>(
					base + 0x1B020
				)(this, rect);
			}

			virtual void registerWithTouchDispatcher() override {
				return reinterpret_cast<void(__thiscall*)(CCScrollLayerExt*)>(
					base + 0x1b980
				)(this);
			}

			//own vtable
			virtual void preVisitWithClippingRect(cocos2d::CCRect rect) {
				return reinterpret_cast<void(__thiscall*)(CCScrollLayerExt*, cocos2d::CCRect)>(
					base + 0x1C000
				)(this, rect);
			}
			virtual void postVisit() {
				return reinterpret_cast<void(__thiscall*)(CCScrollLayerExt*)>(
					base + 0x1C090
				)(this);
			}

			static CCScrollLayerExt* create(cocos2d::CCRect rect, bool vertical = true) {
				auto pRet = new CCScrollLayerExt(rect);

				if (pRet) {
					pRet->autorelease();
					pRet->m_bDisableVertical = !vertical;
					pRet->m_bDisableHorizontal = vertical;
					pRet->m_bCutContent = true;
					return pRet;
				}

				CC_SAFE_DELETE(pRet);
				return nullptr;
			}

			void scrollLayer(float scroll) {
				__asm movss xmm1, scroll

				reinterpret_cast<void(__thiscall*)(CCScrollLayerExt*)>(
					base + 0x1be20
				)(this);
			}

			void moveToTop() {
				reinterpret_cast<void(__thiscall*)(CCScrollLayerExt*)>(
					base + 0x1b4a0
				)(this);
			}

			void moveToTopWithOffset(float fOffset) {
				__asm { movss xmm1, fOffset }

				reinterpret_cast<void(__thiscall*)(CCScrollLayerExt*)>(
					base + 0x1b420
				)(this);
			}

			float getMinY() {
				return this->getContentSize().height -
					this->m_pContentLayer->getContentSize().height -
					this->m_fScrollLimitTop;
			}

			float getMaxY() {
				return this->m_fScrollLimitBottom;
			}

			void addItem(CCNode* cell, bool addToTop = false) {
				float height = 0.0f;
				CCObject* obj;
				CCARRAY_FOREACH(this->m_pContentLayer->getChildren(), obj) {
					auto node = reinterpret_cast<CCNode*>(obj);

					height += node->getScaledContentSize().height;

					if (!addToTop)
					node->setPositionY(node->getPositionY() + cell->getScaledContentSize().height);
				}

				cell->setPosition(
					this->getScaledContentSize().width / 2,
					addToTop ? 
					height + cell->getScaledContentSize().height / 2 :
					cell->getScaledContentSize().height / 2
				);

				height += cell->getScaledContentSize().height;

				this->m_pContentLayer->addChild(cell);

				this->m_pContentLayer->setContentSize({
					cell->getScaledContentSize().width, height
				});
			}
	};
}

#endif