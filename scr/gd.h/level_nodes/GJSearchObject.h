#ifndef __GJSEARCHOBJECT_H__
#define __GJSEARCHOBJECT_H__

#include <gd.h>

namespace gd {

 enum SearchType {
    kGJSearchTypeLiked = 0,
    kGJSearchTypeDownloaded = 1,
    kGJSearchTypeSearch = 2,
    kGJSearchTypeTrending = 3,
    kGJSearchTypeRecent = 4,
    kGJSearchTypeUsersLevels = 5,
    kGJSearchTypeFeatured = 6,
    kGJSearchTypeMagic = 7,
    kGJSearchTypeSends = 8,
    kGJSearchTypeMapPack = 9,
    kGJSearchTypeMapPackOnClick = 10,
    kGJSearchTypeAwarded = 11,
    kGJSearchTypeFollowed = 12,
    kGJSearchTypeFriends = 13,
    kGJSearchTypeUsers = 14,
    kGJSearchTypeLikedGDW = 15,
    kGJSearchTypeHallOfFame = 16,
    kGJSearchTypeFeaturedGDW = 17,
    kGJSearchTypeSimilar = 18,
    kGJSearchTypeMyLevels = 98,
    kGJSearchTypeSavedLevels = 99,
    kGJSearchTypeFavouriteLevels = 100
};
    

    class GDH_DLL GJSearchObject : public cocos2d::CCNode {
    public:
            SearchType m_nScreenID;
            std::string m_sSearchQuery;
            std::string m_sDifficulty;
            std::string m_sLength;
            int m_nPage;
            bool m_bStarFilter;
            bool m_bNoStarFilter;
            char PAD1[2];
            int unknownMember;
            bool m_bUncompletedFilter;
            bool m_bCompletedFilter;
            bool m_bFeaturedFilter;
            bool m_bOriginalFilter;
            bool m_bTwoPlayerFilter;
            bool m_bCoinsFilter;
            bool m_bEpicFilter;
            char PAD2;
            GJDifficulty m_eDemonFilter;
            int m_nUnk;
            int m_nSongID;
            bool m_bCustomSongFilter;
            bool m_bSongFilter;
            char PAD3[2];

        static GJSearchObject* create(SearchType nID) {
            return reinterpret_cast<GJSearchObject*(__fastcall*)(SearchType)>(
                gd::base + 0xc2b90
            )(nID);
        }

        SearchType getType() {
            return this->m_nScreenID;
        }
    };

}

#endif
