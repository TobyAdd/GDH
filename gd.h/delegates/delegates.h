#ifndef DELEGATES_H
#define DELEGATES_h

#include <gd.h>

namespace gd {
    class GJGameLevel;
    class NumberInputLayer;
    class SetIDPopup;
    class CCTextInputNode;
    class GJDropDownLayer;
    class SongInfoObject;
    class GJSpecialColorSelect;
    class HSVWidgetPopup;
    class GJUserScore;
    class ListButtonBar;
    class DialogLayer;
    class CCCircleWave;
    class CCScrollLayerExt;

    enum GJSongError {
        kGJSongErrorUnknown_not_REd = 0,
        // dunno, didnt bother to RE
    };

    class GDH_DLL TextInputDelegate {
        public:
            virtual void textChanged(CCTextInputNode*) {}
            virtual void textInputOpened(CCTextInputNode*) {}
            virtual void textInputClosed(CCTextInputNode*) {}
            virtual void textInputShouldOffset(CCTextInputNode*, float) {}
            virtual void textInputReturn(CCTextInputNode*) {}
            virtual bool allowTextInput(CCTextInputNode*) { return true; }
    };

    class GDH_DLL GameRateDelegate {
        public:
            virtual void updateRate(void) {}
    };

    class GDH_DLL ListButtonBarDelegate {
        public:
            virtual void listButtonBarSwitchedPage(ListButtonBar *, int) {}
    };

    class GDH_DLL DialogDelegate {
        public:
            virtual void dialogClosed(DialogLayer *) {}
    };

    class GDH_DLL GooglePlayDelegate {
        virtual void googlePlaySignedIn(void) {}
    };

    class GDH_DLL ColorSelectDelegate {
        public:
            virtual void colorSelectClosed(cocos2d::CCNode*)  {}
    };

    class GDH_DLL ColorSetupDelegate {
        public:
            virtual void colorSetupClosed(int)  {}
    };

    class GDH_DLL ColorPickerDelegate {
        virtual void colorValueChanged(cocos2d::ccColor3B color) {}
    };

    class GDH_DLL GJSpecialColorSelectDelegate {
        public:
            virtual void colorSelectClosed(GJSpecialColorSelect*, int) {}
    };
    
    class GDH_DLL HSVWidgetPopupDelegate {
        public:
            virtual void hsvPopupClosed(HSVWidgetPopup *, cocos2d::ccHSVValue)  {}
    };

    class GDH_DLL GJRotationControlDelegate {
        public:
            virtual void angleChanged(float)  {}
            virtual void angleChangeBegin(void)  {}
            virtual void angleChangeEnded(void)  {}
    };

    class GDH_DLL GJScaleControlDelegate {
        public:
            virtual void scaleChanged(float)  {}
            virtual void scaleChangeBegin(void)  {}
            virtual void scaleChangeEnded(void)  {}
    };

    class GDH_DLL MusicDownloadDelegate {
        public:
            virtual void downloadSongFailed(int, GJSongError)  {}
            virtual void downloadSongFinished(SongInfoObject*)  {}
            virtual void loadSongInfoFailed(int, GJSongError)  {}
            virtual void loadSongInfoFinished(SongInfoObject*)  {}
            virtual void songStateChanged(void)  {}
    };

    enum UpdateResponse {
        kUpdateResponseUnknown,
        kUpdateResponseUpToDate,
        kUpdateResponseGameVerOutOfDate,
        kUpdateResponseUpdateSuccess,
    };

    enum CommentError {
        kCommentErrorUnknown_not_REd = 0x0,
    };

    enum LikeItemType {
        kLikeItemTypeUnknown_not_REd = 0x0,
    };

    class GDH_DLL LeaderboardManagerDelegate {
        virtual void updateUserScoreFinished(void)  {}
        virtual void updateUserScoreFailed(void)  {}
        virtual void loadLeaderboardFinished(cocos2d::CCArray *,char const*)  {}
        virtual void loadLeaderboardFailed(char const*)  {}
    };

    class GDH_DLL UserInfoDelegate {
        virtual void getUserInfoFailed(int)  {}
        virtual void getUserInfoFinished(GJUserScore *)  {}
        virtual void userInfoChanged(GJUserScore *)  {}
    };

    class GDH_DLL CommentUploadDelegate {
        virtual void commentUploadFinished(int)  {}
        virtual void commentUploadFailed(int, CommentError)  {}
        virtual void commentDeleteFailed(int, int)  {}
    };

    class GDH_DLL LevelCommentDelegate {
        virtual void loadCommentsFinished(cocos2d::CCArray *, const char*)  {}
        virtual void loadCommentsFailed(const char*)  {}
        virtual void updateUserScoreFinished(void)  {}
        virtual void setupPageInfo(std::string, const char*)  {}
    };

    class GDH_DLL LevelDownloadDelegate {
        public:
            virtual void levelDownloadFinished(GJGameLevel *)  {}
            virtual void levelDownloadFailed(int)  {}
    };

    class GDH_DLL LevelDeleteDelegate {
        public:
            virtual void levelDeleteFinished(int)  {}
            virtual void levelDeleteFailed(int)  {}
    };

    class GDH_DLL LevelUpdateDelegate {
        public:
            virtual void levelUpdateFinished(GJGameLevel*, UpdateResponse)  {}
            virtual void levelUpdateFailed(int)  {}
    };

    class GDH_DLL UploadActionDelegate {
        public:
            virtual void uploadActionFinished(int, int) {};
            virtual void uploadActionFailed(int, int) {};
    };

    class GDH_DLL UploadPopupDelegate {
        public:
            virtual void onClosePopup(void) {};
    };
    
    class GDH_DLL LikeItemDelegate {
        public:
            virtual void likedItem(LikeItemType, int, bool)  {}
    };

    class GDH_DLL RateLevelDelegate {
        public:
            virtual void rateLevelClosed(void)  {}
    };

    class GDH_DLL NumberInputDelegate {
        public:
            virtual void numberInputClosed(NumberInputLayer*)  {}
    };

    class GDH_DLL SetIDPopupDelegate {
        public:
            virtual void setIDPopupClosed(SetIDPopup*, int)  {}
    };

    class GDH_DLL GJDropDownLayerDelegate {
        public:
            virtual void dropDownLayerWillClose(GJDropDownLayer*)  {}
    };

    class GDH_DLL CCCircleWaveDelegate {
        void circleWaveWillBeRemoved(CCCircleWave*) {}
    };

    enum AccountError {
        kAccountErrorUnknown = 0,
    };

    enum BackupAccountError {
        kBackupAccountErrorUnknown = 0,
    };

    struct GJAccountRegisterDelegate {
        virtual void registerAccountFailed(AccountError error) {}
        virtual void registerAccountFinished(void) {}
    };

    struct GJAccountLoginDelegate {
        virtual void loginAccountFailed(AccountError error) {}
        virtual void loginAccountFinished(int, int) {}
    };

    struct GJAccountDelegate {
        virtual void accountStatusChanged(void) {}
    };

    struct GJAccountBackupDelegate {
        virtual void backupAccountFailed(BackupAccountError) {}
        virtual void backupAccountFinished(void) {}
    };
    
    struct GJAccountSyncDelegate {
        virtual void syncAccountFailed(BackupAccountError) {}
        virtual void syncAccountFinished(void) {}
    };

    struct GJAccountSettingsDelegate {
        virtual void updateSettingsFailed(void) {}
        virtual void updateSettingsFinished(void) {}
    };

	class GDH_DLL CCScrollLayerExtDelegate {
		public:
			//lol nice typo rob
			virtual void scrllViewWillBeginDecelerating(CCScrollLayerExt*) {}
			virtual void scrollViewDidEndDecelerating(CCScrollLayerExt*) {}
			virtual void scrollViewTouchMoving(CCScrollLayerExt*) {}
			virtual void scrollViewDidEndMoving(CCScrollLayerExt*) {}
			virtual void scrollViewTouchBegin(CCScrollLayerExt*) {}
			virtual void scrollViewTouchEnd(CCScrollLayerExt*) {}
	};

    struct DynamicScrollDelegate {
        void updatePageWithObject(cocos2d::CCObject*, cocos2d::CCObject*) {}
    };
}

#endif
