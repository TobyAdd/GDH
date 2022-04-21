#ifndef __LOCALLEVELMANAGER_H__
#define __LOCALLEVELMANAGER_H__

#include <gd.h>

namespace gd {

    class GDH_DLL LocalLevelManager /* 0x114 */ : public cocos2d::CCNode {
        public:
            PAD(0x1C);
            cocos2d::CCDictionary* m_pLoadData; // 0x108, seems to be just LoadData.plist
            cocos2d::CCDictionary* m_pLevelData; // 0x10c, level strings for all the main levels
            cocos2d::CCArray* m_pLevels;    // 0x110
            
            static LocalLevelManager* sharedState() {
                return reinterpret_cast<LocalLevelManager*(__stdcall*)()>(
                    base + 0x18d260
                )();
            }
    };

}

#endif
