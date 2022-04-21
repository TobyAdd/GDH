#ifndef __CCINDEXPATH_H__
#define __CCINDEXPATH_H__

#include <gd.h>

namespace gd {
	class GDH_DLL CCIndexPath : public cocos2d::CCObject {
		public:
			int m_nPosition; // 0x20
			int m_nUnknown2; // 0x24

		public:
			static CCIndexPath* create(unsigned int idk1, int idk2) {
				return reinterpret_cast<CCIndexPath* (__fastcall*)(int, int)>(
					base + 0x30E40
				)(idk1, idk2);
			}
		};
}

#endif