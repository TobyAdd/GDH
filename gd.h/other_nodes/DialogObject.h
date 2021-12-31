#ifndef __DIALOGOBJECT_H__
#define __DIALOGOBJECT_H__

#include <gd.h>

namespace gd {
    
class GDH_DLL DialogObject : public cocos2d::CCObject
{
  public:
    std::string m_sText;
    std::string m_sTitle;
    int m_nDialogType;
    cocos2d::ccColor3B m_pColour;
    float m_fTextWidth;
    bool m_bCanSkip;
 };
}

#endif
