#ifndef __FMODSOUND_H__
#define __FMODSOUND_H__

#include <gd.h>

namespace gd {
	class GDH_DLL FMODSound : public cocos2d::CCNode {
	public:
		FMOD::Sound* m_pSound;
	};
}

#endif