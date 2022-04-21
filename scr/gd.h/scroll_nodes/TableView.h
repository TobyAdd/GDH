#ifndef __TABLEVIEW_H__
#define __TABLEVIEW_H__

#include <gd.h>

namespace gd {

	class CCScrollLayerExt;
	class CCScrollLayerExtDelegate;
	class CCIndexPath;
	class TableView;
	
	typedef enum {
		//idk what goes here
	} TableViewCellEditingStyle;

	class GDH_DLL TableViewDelegate {
	public:
		virtual void willTweenToIndexPath(CCIndexPath&, TableViewCell*, TableView*) {}
		virtual void didEndTweenToIndexPath(CCIndexPath&, TableView*) {}
		virtual void TableViewWillDisplayCellForRowAtIndexPath(CCIndexPath&, TableViewCell*, TableView*) {}
		virtual void TableViewDidDisplayCellForRowAtIndexPath(CCIndexPath&, TableViewCell*, TableView*) {}
		virtual void TableViewWillReloadCellForRowAtIndexPath(CCIndexPath&, TableViewCell*, TableView*) {}
		virtual float cellHeightForRowAtIndexPath(CCIndexPath&, TableView*) = 0;
		virtual void didSelectRowAtIndexPath(CCIndexPath&, TableView*) = 0;
	};

	class GDH_DLL TableViewDataSource {
	public:
		virtual unsigned int numberOfRowsInSection(unsigned int, TableView*) = 0;
		virtual unsigned int numberOfSectionsInTableView(TableView*) { return 1; }
		virtual void TableViewCommitCellEditingStyleForRowAtIndexPath(TableView*, TableViewCellEditingStyle, CCIndexPath&) {}
		virtual TableViewCell* cellForRowAtIndexPath(CCIndexPath&, TableView*) = 0;
	};

	class GDH_DLL TableView : public CCScrollLayerExt, public CCScrollLayerExtDelegate {
		public:
			bool m_bTouchDown2;
			PAD(4)
			cocos2d::CCPoint m_obTouchStartPosition2;	// 0x170
			cocos2d::CCPoint m_obUnknown2;	// 0x178
			cocos2d::CCPoint m_obTouchPosition2;	// 0x180
			PAD(4)
			bool m_bTouchMoved;
			PAD(3)
			cocos2d::CCArray* m_pCellArray;	// 0x190
			cocos2d::CCArray* m_pArray2;	// 0x194
			cocos2d::CCArray* m_pArray3;	// 0x198
			TableViewDelegate* m_pDelegate;	// 0x19c
			TableViewDataSource* m_pDataSource;	// 0x1a0
			PAD(16)
			float m_fTouchLastY;	// 0x1ac
			PAD(4)

			TableView(cocos2d::CCRect rect) : CCScrollLayerExt(rect) {
				reinterpret_cast<TableView*(__thiscall*)(TableView*, cocos2d::CCRect)>(
					base + 0x30fb0
				)(this, rect);
			}

			void reloadData() {
				reinterpret_cast<void(__thiscall*)(TableView*)>(
					base + 0x317e0
				)(this);
			}

			static TableView* create(TableViewDelegate* delegate, TableViewDataSource* dataSource, cocos2d::CCRect rect) {
				auto pRet = reinterpret_cast<TableView*(__fastcall*)(TableViewDelegate*, TableViewDataSource*, cocos2d::CCRect)>(
					base + 0x30ed0
				)(delegate, dataSource, rect);

				__asm add esp, 0x10

				return pRet;
			}
	};
}

#endif