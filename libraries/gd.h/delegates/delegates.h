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

    enum GJSongError {
        kGJSongErrorUnknown_not_REd = 0,
        // dunno, didnt bother to RE
    };

    class TextInputDelegate {
        public:
            virtual void textChanged(CCTextInputNode*) {}
            virtual void textInputOpened(CCTextInputNode*) {}
            virtual void textInputClosed(CCTextInputNode*) {}
            virtual void textInputShouldOffset(CCTextInputNode*, float) {}
            virtual void textInputReturn(CCTextInputNode*) {}
            virtual bool allowTextInput(CCTextInputNode*) { return true; }
    };

    class GameRateDelegate {
        public:
            virtual void updateRate(void) {}
    };

    class ListButtonBarDelegate {
        public:
            virtual void listButtonBarSwitchedPage(ListButtonBar *, int) {}
    };

    class DialogDelegate {
        public:
            virtual void dialogClosed(DialogLayer *) {}
    };

    class GooglePlayDelegate {
        virtual void googlePlaySignedIn(void) {}
    };

    class ColorSelectDelegate {
        public:
            virtual void colorSelectClosed(cocos2d::CCNode*)  {}
    };

    class ColorSetupDelegate {
        public:
            virtual void colorSetupClosed(int)  {}
    };

    class ColorPickerDelegate {
        virtual void colorValueChanged(cocos2d::ccColor3B color);
    };

    class GJSpecialColorSelectDelegate {
        public:
            virtual void colorSelectClosed(GJSpecialColorSelect*, int)  {}
    };
    
    class HSVWidgetPopupDelegate {
        public:
            virtual void hsvPopupClosed(HSVWidgetPopup *, cocos2d::ccHSVValue)  {}
    };

    class GJRotationControlDelegate {
        public:
            virtual void angleChanged(float)  {}
            virtual void angleChangeBegin(void)  {}
            virtual void angleChangeEnded(void)  {}
    };

    class GJScaleControlDelegate {
        public:
            virtual void scaleChanged(float)  {}
            virtual void scaleChangeBegin(void)  {}
            virtual void scaleChangeEnded(void)  {}
    };

    class MusicDownloadDelegate {
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

    class LeaderboardManagerDelegate {
        virtual void updateUserScoreFinished(void)  {}
        virtual void updateUserScoreFailed(void)  {}
        virtual void loadLeaderboardFinished(cocos2d::CCArray *,char const*)  {}
        virtual void loadLeaderboardFailed(char const*)  {}
    };

    class UserInfoDelegate {
        virtual void getUserInfoFailed(int)  {}
        virtual void getUserInfoFinished(GJUserScore *)  {}
        virtual void userInfoChanged(GJUserScore *)  {}
    };

    class CommentUploadDelegate {
        virtual void commentUploadFinished(int)  {}
        virtual void commentUploadFailed(int, CommentError)  {}
        virtual void commentDeleteFailed(int, int)  {}
    };

    class LevelCommentDelegate {
        virtual void loadCommentsFinished(cocos2d::CCArray *, const char*)  {}
        virtual void loadCommentsFailed(const char*)  {}
        virtual void updateUserScoreFinished(void)  {}
        virtual void setupPageInfo(std::string, const char*)  {}
    };

    class LevelDownloadDelegate {
        public:
            virtual void levelDownloadFinished(GJGameLevel *)  {}
            virtual void levelDownloadFailed(int)  {}
    };

    class LevelDeleteDelegate {
        public:
            virtual void levelDeleteFinished(int)  {}
            virtual void levelDeleteFailed(int)  {}
    };

    class LevelUpdateDelegate {
        public:
            virtual void levelUpdateFinished(GJGameLevel*, UpdateResponse)  {}
            virtual void levelUpdateFailed(int)  {}
    };

    class UploadActionDelegate {
        public:
            virtual void uploadActionFinished(int, int) {};
            virtual void uploadActionFailed(int, int) {};
    };

    class UploadPopupDelegate {
        public:
            virtual void onClosePopup(void) {};
    };
    
    class LikeItemDelegate {
        public:
            virtual void likedItem(LikeItemType, int, bool)  {}
    };

    class RateLevelDelegate {
        public:
            virtual void rateLevelClosed(void)  {}
    };

    class NumberInputDelegate {
        public:
            virtual void numberInputClosed(NumberInputLayer*)  {}
    };

    class SetIDPopupDelegate {
        public:
            virtual void setIDPopupClosed(SetIDPopup*, int)  {}
    };

    class GJDropDownLayerDelegate {
        public:
            virtual void dropDownLayerWillClose(GJDropDownLayer*)  {}
    };

    class CCCircleWaveDelegate {
        void circleWaveWillBeRemoved(CCCircleWave*) {}
    };
}

#endif
