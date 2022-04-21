#ifndef __CCCIRCLEWAVE_H__
#define __CCCIRCLEWAVE_H__

#include <gd.h>

namespace gd {
	#pragma runtime_checks("s", off)
	class GDH_DLL CCCircleWave : public cocos2d::CCNode {
	protected:
		cocos2d::CCArray* m_pArray; //idk what this is tho
		PAD(4);
		float m_fFrom1; //?
		float m_fFrom2; //?
		cocos2d::ccColor3B m_cColor;
		cocos2d::CCPoint m_obUnknown;
		//more that im too lazy to figure out lol

	public:
		static CCCircleWave* create(bool fade1, bool fade2, float from, float to, float duration) {
			__asm {
				movss xmm0, from
				movss xmm1, to
				movss xmm2, duration
			}
			return reinterpret_cast<CCCircleWave* (__fastcall*)(bool, bool)>(
				base + 0x16C00
				)(fade1, fade2);
		}
		void setColor(cocos2d::ccColor3B color) { m_cColor = color; }
		void followObject(cocos2d::CCNode* obj, bool unk) {
			reinterpret_cast<void(__thiscall*)(CCCircleWave*, CCNode*, bool)>(base + 0x16f20)(this, obj, unk);
		}
	};
	#pragma runtime_checks("s", restore)
}

#endif