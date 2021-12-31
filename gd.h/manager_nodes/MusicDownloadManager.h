#ifndef __MUSICDOWNLOADMANAGER_H__
#define __MUSICDOWNLOADMANAGER_H__

#include <gd.h>

namespace gd {
    class GDH_DLL MusicDownloadManager : public cocos2d::CCNode {
    public:
        PAD(4);
        cocos2d::CCDictionary* m_unknownDict;
        cocos2d::CCArray* m_unknownArr; // list of MusicDelegateHandler s
        cocos2d::CCDictionary* m_songsDict;

        static MusicDownloadManager* sharedState() {
            return reinterpret_cast<MusicDownloadManager*(__stdcall*)()>(base + 0x1945b0)();
        }
        static std::string pathForSong(int id) {
            std::string res;

            reinterpret_cast<void(__stdcall*)(std::string*, int)>(base + 0x1960e0)(
                &res, id
            );

            return res;
        }

        cocos2d::CCArray* getDownloadedSongs() {
            return reinterpret_cast<cocos2d::CCArray*(__thiscall*)(MusicDownloadManager*)>(base + 0x195640)(this);
        }

        void songStateChanged() {
            return reinterpret_cast<void(__thiscall*)(MusicDownloadManager*)>(base + 0x194d90)(this);
        }
    };
}

#endif
