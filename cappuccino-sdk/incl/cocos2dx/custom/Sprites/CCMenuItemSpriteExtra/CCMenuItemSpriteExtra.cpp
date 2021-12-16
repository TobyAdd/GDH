#include "CCMenuItemSpriteExtra.h"

void CCMenuItemSpriteExtra::selected(){
	CCMenuItemSprite::selected();
	auto resize = cocos2d::CCScaleTo::create(0.3, m_sizeMult * m_origScale);
	auto bounce = cocos2d::CCEaseBounceOut::create(resize);
	this->runAction(bounce);
}

void CCMenuItemSpriteExtra::unselected(){
	CCMenuItemSprite::unselected();
	auto resize = cocos2d::CCScaleTo::create(0.3, m_origScale);
	auto bounce = cocos2d::CCEaseBounceOut::create(resize);
	this->runAction(bounce);
}

void CCMenuItemSpriteExtra::activate () {
	CCMenuItemSprite::activate();
	this->stopAllActions();
	this->setScale(m_origScale);
}

void CCMenuItemSpriteExtra::setScale(float scale) {
	CCMenuItemSprite::setScale(scale);	
	m_origScale = scale;
}

CCMenuItemSpriteExtra* CCMenuItemSpriteExtra::create(CCNode *normalSprite, CCNode *selectedSprite, CCObject *target, cocos2d::SEL_MenuHandler selector){
	auto spriteItem = new CCMenuItemSpriteExtra;
	if (spriteItem && spriteItem->initWithNormalSprite(normalSprite, selectedSprite, nullptr, target, selector)) 
		spriteItem->autorelease();
	else {
		delete spriteItem;
		spriteItem = nullptr;
	}
	return spriteItem;

}
