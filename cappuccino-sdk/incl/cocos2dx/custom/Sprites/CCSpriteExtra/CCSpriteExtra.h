#ifndef __CCSPRITE_EXTRA_H
#define __CCSPRITE_EXTRA_H

#include <sprite_nodes/CCSprite.h>
NS_CC_BEGIN

class CC_DLL CCSpriteExtra : public CCSprite{
public:
	ROB_CC_SYNTHESIZE(float, m_fXOffset, XOffset);
	ROB_CC_SYNTHESIZE(float, m_fYOffset, YOffset);
};

NS_CC_END
#endif