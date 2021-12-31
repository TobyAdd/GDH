#ifndef __GJREWARDOBJECT_H__
#define __GJREWARDOBJECT_H__

#include <gd.h>

namespace gd {
  
enum SpecialRewardItem
{
  kSpecialRewardItemFireShard = 0x1,
  kSpecialRewardItemIceShard = 0x2,
  kSpecialRewardItemPoisonShard = 0x3,
  kSpecialRewardItemShadowShard = 0x4,
  kSpecialRewardItemLavaShard = 0x5,
  kSpecialRewardItemBonusKey = 0x6,
  kSpecialRewardItemOrbs = 0x7,
  kSpecialRewardItemDiamonds = 0x8,
  kSpecialRewardItemCustomItem = 0x9,
};  
  
enum UnlockType
{
  kItemTypeCube = 0x1,
  kItemTypeCol1 = 0x2,
  kItemTypeCol2 = 0x3,
  kItemTypeShip = 0x4,
  kItemTypeBall = 0x5,
  kItemTypeBird = 0x6,
  kItemTypeDart = 0x7,
  kItemTypeRobot = 0x8,
  kItemTypeSpider = 0x9,
  kItemTypeStreak = 0xA,
  kItemTypeDeath = 0xB,
  kItemTypeGJItem = 0xC,
};
    
class GDH_DLL GJRewardObject : public cocos2d::CCObject
{
  public:
    SpecialRewardItem m_eSpecialRewardItem;
    UnlockType m_eUnlockType;
    int m_nItemID;
    int m_nTotal;
 };
}

#endif
