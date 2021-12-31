#ifndef __PROFILEPAGE_H__
#define __PROFILEPAGE_H__

#include <gd.h>

namespace gd {
    class FLAlertLayer;

    class GDH_DLL ProfilePage : public FLAlertLayer,
        FLAlertLayerProtocol,
        LevelCommentDelegate,
        CommentUploadDelegate,
        UserInfoDelegate,
        UploadActionDelegate,
        UploadPopupDelegate,
        LeaderboardManagerDelegate
    {
        public:
            PAD(4);
            int m_nAccountID;

        public:
            static ProfilePage* create(int accountID, bool idk) {
                return reinterpret_cast<ProfilePage*(__fastcall*)(
                    int, bool
                )>(
                    base + 0x20ee50
                )(
                    accountID, idk
                );
            }
    };
}


#endif
