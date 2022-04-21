#ifndef __SONGINFOLAYER_H__
#define __SONGINFOLAYER_H__

#include <gd.h>

namespace gd {
    class FLAlertLayer;

    class GDH_DLL SongInfoLayer : public FLAlertLayer {
    public:
        std::string m_downloadLink;
        std::string m_artistNewgrounds;
        std::string m_artistYoutube;
        std::string m_artistFacebook;

        static SongInfoLayer* create(int songID) {
            return reinterpret_cast<SongInfoLayer*(__fastcall*)(int)>(base + 0x250520)(songID);
        }
        static SongInfoLayer* create(std::string songName, std::string artistName, std::string downloadLink, std::string artistNG, std::string artistYT, std::string artistFB) {
            return reinterpret_cast<SongInfoLayer*(__stdcall*)(std::string, std::string, std::string, std::string, std::string, std::string)>(
                base + 0x250830)(songName, artistName, downloadLink, artistNG, artistYT, artistFB);
        }
    };
}

#endif

