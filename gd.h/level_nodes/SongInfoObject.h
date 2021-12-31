#ifndef __SONGINFOOBJECT_H__
#define __SONGINFOOBJECT_H__

#include <gd.h>
#include <string>

namespace gd {
    class LevelSettingsObject;

    class GDH_DLL SongInfoObject : public cocos2d::CCNode {
    public:
        int m_nSongID;
        std::string m_sSongName;
        std::string m_sArtistName;
        std::string m_sVideoID;
        std::string m_sYouTubeChannelURL;
        std::string m_sSongLink;
        int m_nArtistID;
        float m_fFileSize;
        bool m_bIsUnloaded;
        bool m_bIsVerified;
        bool m_bIsBlocked;
        int m_nSongPriority;
        LevelSettingsObject *m_pLevelSettings;
    };
}
#endif
