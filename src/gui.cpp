#include "gui.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

std::array<std::string, 5> tabs = {"Core", "Player", "Creator", "Replay", "About"};

HacksTab* HacksTab::create() {
    auto ret = new HacksTab();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void HacksTab::addToggle(const std::string& text, bool enabled, const std::function<void(bool)>& callback, float scale = 0.5f) {
    auto toggle = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(HacksTab::onToggle), 1.f);
    float y = (m_scrollLayer->m_contentLayer->getContentHeight() - 20) - (m_togglerCallbacks.size() * 35);
    
    toggle->setPosition({ 20, y });
    toggle->toggle(enabled);

    auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
    label->setAnchorPoint({0.f, 0.5f});
    label->setScale(scale);
    label->setPosition({ 45, y });

    m_togglesMenu->addChild(toggle);
    m_togglesMenu->addChild(label);

    m_togglerCallbacks[toggle] = callback;
}

bool HacksTab::init() {
    if (!CCMenu::init())
        return false;

    m_scrollLayer = ScrollLayer::create({225, 175});
    m_scrollLayer->setPosition({125, 11});
    m_scrollLayer->m_peekLimitTop = 15;
    m_scrollLayer->m_peekLimitBottom = 15;
    this->addChild(m_scrollLayer);

    m_togglesMenu = CCMenu::create();
    m_togglesMenu->setPosition({ 0, 0 });
    m_scrollLayer->m_contentLayer->addChild(m_togglesMenu);

    return true;
}

void HacksTab::onToggle(CCObject* sender) {
    auto toggler = static_cast<CCMenuItemToggler*>(sender);
    if (!toggler) return;

    auto it = m_togglerCallbacks.find(toggler);
    if (it != m_togglerCallbacks.end())
        it->second(!toggler->isToggled());
}

void HacksTab::setAmountOfHacks(int amount) {
    m_scrollLayer->m_contentLayer->setContentHeight(35 * amount + 5);
    m_scrollLayer->moveToTop();
}

bool HacksLayer::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->setTitle("GDH Mobile");

    float tabPositionY = 170.f;

    for (int i = 0; i < tabs.size(); i++) {
        auto button = ButtonSprite::create(tabs[i].c_str(), 90, true, "bigFont.fnt", (i == 0) ? "GJ_button_02.png" : "GJ_button_01.png", 30.f, 0.7f);
        auto buttonClick = CCMenuItemSpriteExtra::create(button, this, menu_selector(HacksLayer::onChangeTab));
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

    auto& hacks = Hacks::get();

    coreTab = HacksTab::create();
    coreTab->setPosition({0, 0});
    coreTab->setAmountOfHacks(5);
    coreTab->addToggle("Noclip", hacks.noclip, [&hacks](bool enabled) { hacks.noclip = enabled; });
    coreTab->addToggle("Unlock Items", hacks.unlock_items, [&hacks](bool enabled) { hacks.unlock_items = enabled; });
    coreTab->addToggle("No Respawn Blink", hacks.no_respawn_blink, [&hacks](bool enabled) { hacks.no_respawn_blink = enabled; });
    coreTab->addToggle("No Death Effect", hacks.no_death_effect, [&hacks](bool enabled) { hacks.no_death_effect = enabled; });
    coreTab->addToggle("Safe Mode", hacks.safe_mode, [&hacks](bool enabled) { hacks.safe_mode = enabled; });
    coreTab->setVisible(true);

    playerTab = HacksTab::create();
    playerTab->setPosition({0, 0});
    playerTab->setVisible(false);

    creatorTab = HacksTab::create();
    creatorTab->setPosition({0, 0});
    creatorTab->setVisible(false);

    replayTab = HacksTab::create();
    replayTab->setPosition({0, 0});
    replayTab->setVisible(false);

    aboutTab = HacksTab::create();
    aboutTab->setPosition({0, 0});
    aboutTab->setVisible(false);

    auto aboutLabel = CCLabelBMFont::create("GDH Mobile by TobyAdd\nVersion: 0.0\n\nSpecial Thanks:\nPrevter", "bigFont.fnt");
    aboutLabel->setAnchorPoint({0.f, 0.5f});
    aboutLabel->setScale(0.5f);
    aboutLabel->setPosition({130, 140});
    aboutTab->addChild(aboutLabel);

    m_mainLayer->addChild(coreTab);
    m_mainLayer->addChild(playerTab);
    m_mainLayer->addChild(creatorTab);
    m_mainLayer->addChild(replayTab);
    m_mainLayer->addChild(aboutTab);

    return true;
}

void HacksLayer::onChangeTab(CCObject* sender) {
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

HacksLayer* HacksLayer::create() {
    auto ret = new HacksLayer();
    if (ret->initAnchored(365.f, 220.f)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}
