#ifndef __INFOLAYER_H__
#define __INFOLAYER_H__

#include <gd.h>

namespace gd {
    class GDH_DLL GJCommentListLayer : public cocos2d::CCLayerColor {
        public:
            BoomListView* m_pList; // 0x198
    };

    class GDH_DLL InfoLayer :
        public FLAlertLayer,
        public LevelCommentDelegate,
        public CommentUploadDelegate,
        public FLAlertLayerProtocol /* 0x1d8 */
    {
        public:
            PAD(44)
            GJCommentListLayer* m_pList; // 0x204
    };    
}

#endif
