#ifndef __LOCALLEVELMANAGER_H__
#define __LOCALLEVELMANAGER_H__

#include <gd.h>

namespace gd {

    class LocalLevelManager : public cocos2d::CCNode {
    public:
        PAD(0x1C);
        cocos2d::CCDictionary* m_loadData; // seems to be just LoadData.plist
        cocos2d::CCDictionary* m_levelData; // level strings for all the main levels
        cocos2d::CCArray* m_localLevels;
    };

}

#endif
