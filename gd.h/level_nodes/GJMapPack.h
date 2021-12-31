#ifndef __GJMAPPACK_H__
#define __GJMAPPACK_H__

#include <gd.h>

namespace gd {
  class GDH_DLL GJMapPack : public cocos2d::CCNode
  {
  public:
	  cocos2d::CCArray* m_pLevels;
	  int m_nPackID;
	  GJDifficulty m_eDifficulty;
	  int m_nStars;
	  int m_nCoins;
	  std::string m_sPackName;
	  std::string m_sLevels;
	  cocos2d::ccColor3B m_tTextColour;
	  cocos2d::ccColor3B m_tBarColour;
	  int m_nMId;
	  bool m_bIsGauntlet;
    };
}

#endif
