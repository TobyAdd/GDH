#pragma once
#include <cocos2d.h>

class CCMenuItemSpriteExtra : public cocos2d::CCMenuItemSprite{
private:
	float m_sizeMult = 1.25;
	float m_origScale = 1.f;
public:
	virtual void selected() override;
	virtual void unselected() override;
	virtual void activate() override;
	virtual void setScale(float scale) override;
	static CCMenuItemSpriteExtra* create(CCNode *normalSprite, CCNode *selectedSprite, CCObject *target, cocos2d::SEL_MenuHandler selector);
	void setSizeMult(float multiplier) { m_sizeMult = multiplier; }
};
