#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>

std::array<std::string, 5> tabs = {"Core", "Player", "Creator", "Replay", "About"};


class HacksTab : public CCMenu {
public:
    static HacksTab* create() {
        auto ret = new HacksTab();
        if (ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    void addToggle(const std::string& text, bool enabled, const std::function<void(bool)>& callback) {
        auto toggle = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(HacksTab::onToggle), 1.f);
        float y = 280 - (m_togglerCallbacks.size() * 35);
        
        toggle->setPosition({ 20, y });
        toggle->toggle(enabled);

        auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
		label->setAnchorPoint({0.f, 0.5f});
        label->setScale(0.7f);
        label->setPosition({ 45, y });

        m_togglesMenu->addChild(toggle);
        m_togglesMenu->addChild(label);

        m_togglerCallbacks[toggle] = callback;
    }

private:
	HacksTab() = default;

	bool init() {
		if (!CCMenu::init())
			return false;

		auto scroll = ScrollLayer::create({225, 175});
		scroll->setPosition({125, 11});
		scroll->m_contentLayer->setContentHeight(300);
		scroll->moveToTop(); 
		scroll->m_peekLimitTop = 15;
		scroll->m_peekLimitBottom = 15;
		this->addChild(scroll);

		m_togglesMenu = CCMenu::create();
		m_togglesMenu->setPosition({ 0, 0 });
		scroll->m_contentLayer->addChild(m_togglesMenu);

		return true;
	}

	void onToggle(CCObject* sender) {
		auto toggler = static_cast<CCMenuItemToggler*>(sender);
		if (!toggler) return;

		auto it = m_togglerCallbacks.find(toggler);
		if (it != m_togglerCallbacks.end())
			it->second(!toggler->isToggled());
	}

protected:
    std::map<CCMenuItemToggler*, std::function<void(bool)>> m_togglerCallbacks;
    CCMenu* m_togglesMenu;
};

class MyPopup : public geode::Popup<> {
private:
	HacksTab* coreTab;
	HacksTab* playerTab;
	HacksTab* creatorTab;
	HacksTab* replayTab;
	HacksTab* aboutTab;
protected:
    bool setup() override {
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        this->setTitle("GDH Mobile");

		float tabPositionY = 170.f;

		for (int i = 0; i < tabs.size(); i++) {
			auto button = ButtonSprite::create(tabs[i].c_str(), 90, true, "bigFont.fnt", (i == 0) ? "GJ_button_02.png" : "GJ_button_01.png", 30.f, 0.7f);
			auto buttonClick = CCMenuItemSpriteExtra::create(button, this, menu_selector(MyPopup::onChangeTab));
			buttonClick->setPosition({65.f, tabPositionY});
			buttonClick->setTag(i);
			m_buttonMenu->addChild(buttonClick);

			tabPositionY -= 35;
		}

		auto background = extension::CCScale9Sprite::create("square02_small.png");
		background->setOpacity(100);
		background->setPosition({238, 98});
		background->setContentSize({230, 175});
		m_mainLayer->addChild(background);

		coreTab = HacksTab::create();
		coreTab->setPosition({0, 0});
		coreTab->addToggle("core", false, [](bool enabled) {});
		coreTab->setVisible(true);

		playerTab = HacksTab::create();
		playerTab->setPosition({0, 0});
		playerTab->addToggle("player", false, [](bool enabled) {});
		playerTab->setVisible(false);

		creatorTab = HacksTab::create();
		creatorTab->setPosition({0, 0});
		creatorTab->addToggle("creator", false, [](bool enabled) {});
		creatorTab->setVisible(false);

		replayTab = HacksTab::create();
		replayTab->setPosition({0, 0});
		replayTab->addToggle("replay", false, [](bool enabled) {});
		replayTab->setVisible(false);

		aboutTab = HacksTab::create();
		aboutTab->setPosition({0, 0});
		aboutTab->addToggle("about", false, [](bool enabled) {	});
		aboutTab->setVisible(false);

		m_mainLayer->addChild(coreTab);
		m_mainLayer->addChild(playerTab);
		m_mainLayer->addChild(creatorTab);
		m_mainLayer->addChild(replayTab);
		m_mainLayer->addChild(aboutTab);

        return true;
    }

	void onChangeTab(CCObject* sender) {
		int tag = sender->getTag();

		for (int i = 0; i < tabs.size(); i++) {
			auto* btn = m_buttonMenu->getChildByTag(i);
			auto* child = btn->getChildren()->objectAtIndex(0);
			if (auto* btnSprite = typeinfo_cast<ButtonSprite*>(child)) {
				btnSprite->updateBGImage(i == tag ? "GJ_button_02.png" : "GJ_button_01.png");
			}
		}

		coreTab->setVisible(tag == 0);
		playerTab->setVisible(tag == 1);
		creatorTab->setVisible(tag == 2);
		replayTab->setVisible(tag == 3);
		aboutTab->setVisible(tag == 4);
	}

public:
    static MyPopup* create() {
        auto ret = new MyPopup();
        if (ret->initAnchored(365.f, 220.f)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }
};


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
		MyPopup::create()->show();
	}
};