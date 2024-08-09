#include "gui.hpp"
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "hacks.hpp"

using namespace geode::prelude;

class $modify(MyMenuLayer, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) {
			return false;
		}

		auto myButton = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
			this,
			menu_selector(MyMenuLayer::showPopup)
		);

		auto menu = this->getChildByID("bottom-menu");
		menu->addChild(myButton);

		myButton->setID("my-button"_spr);
		menu->updateLayout();

		return true;
	}

	void showPopup(CCObject* sender) {
		HacksLayer::create()->show();
	}
};

class $modify(MyPauseLayer, PauseLayer) {
	void customSetup() {
		PauseLayer::customSetup();

		auto myButton = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
			this,
			menu_selector(MyPauseLayer::showPopup)
		);
		myButton->setPosition({35, 35});

		CCMenu* menu = CCMenu::create();
		menu->setPosition({0, 0});
		
		menu->addChild(myButton);
		this->addChild(menu);
	}

	void showPopup(CCObject* sender) {
		HacksLayer::create()->show();
	}
};

