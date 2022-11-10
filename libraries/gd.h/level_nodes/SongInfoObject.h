#ifndef __SONGINFOOBJECT_H__
#define __SONGINFOOBJECT_H__

#include <gd.h>
#include <string>

namespace gd {
    class LevelSettingsObject;

    class SongInfoObject : public cocos2d::CCNode {
    public:
        int m_songID;
        std::string m_songName;
        std::string m_artistName;
        std::string m_youtubeVideo; // unsure
        std::string m_youtubeChannel;
        std::string m_songURL;
        int m_artistID;
        float m_fileSize;
        bool m_unknownSong;
        int m_priority;
        LevelSettingsObject* m_levelSettings;
    };
}
#endif