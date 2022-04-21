#ifndef __GMANAGER_H__
#define __GMANAGER_H__

#include <gd.h>

class DS_Dictionary;

namespace gd {

	class GDH_DLL GManager : public cocos2d::CCNode {
		protected:
			std::string m_sFileName; // 0xec
			bool m_bSetup;			 // 0x104
			bool m_bSaved;			 // 0x105

		public:
			void save() {
				return reinterpret_cast<void(__thiscall*)(GManager*, std::string)>(
					base + 0x29250
					)(this, m_sFileName);
			}
			virtual void setup() {
				return reinterpret_cast<void(__thiscall*)(GManager*)>(
					base + 0x28F60
					)(this);
			}
			virtual void encodeDataTo(DS_Dictionary* data) {}
			virtual void dataLoaded(DS_Dictionary* data) {}
			virtual void firstLoad() {}
	};
}

#endif