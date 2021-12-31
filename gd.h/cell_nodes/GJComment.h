#ifndef __GJCOMMENT_H__
#define __GJCOMMENT_H__

#include <gd.h>

namespace gd {
    class TableViewCell;

    class GDH_DLL GJComment : public cocos2d::CCNode {
        public:
            std::string m_sCommentString;
            PAD(0x18)
            int m_nCommentID;
            PAD(4)
            int m_nLikeCount;
            PAD(8)
            int m_nAccountID;
            std::string m_sUploadDate;

            static GJComment* create(cocos2d::CCDictionary* dict) {
                return reinterpret_cast<GJComment*(__fastcall*)(cocos2d::CCDictionary*)>(
                    gd::base + 0xc3740
                )(dict);
            }
    };

    class GDH_DLL CommentCell : public gd::TableViewCell {
        public:
            PAD(0x4);
            cocos2d::CCSprite* m_pIconSprite;
            cocos2d::CCLabelBMFont* m_pLikeLabel;
            GJComment* m_pComment;
            PAD(0x4);
    };
}

#endif
