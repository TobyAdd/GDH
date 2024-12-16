#include "gui_mobile.hpp"
#include "config.hpp"
#include "hacks.hpp"

using namespace geode::prelude;

std::array<std::string, 6> tabs = {"Core", "Player", "Creator", "Labels", "Replay", "About"};

HacksTab* HacksTab::create() {
    auto ret = new HacksTab();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void HacksTab::addToggle(const std::string& text, bool enabled, const std::function<void(bool)>& callback) {
    auto toggle = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(HacksTab::onToggle), 0.75f);
    y_lastToggle = 203 - (m_togglerCallbacks.size() * 25);

    toggle->setPosition({ 145, y_lastToggle });
    toggle->toggle(enabled);

    auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
    label->setAnchorPoint({0.f, 0.5f});
    label->setScale(0.5f);
    label->setPosition({ toggle->getPositionX() + 15.f, y_lastToggle });

    addChild(toggle);
    addChild(label);

    m_togglerCallbacks[toggle] = callback;
}

bool HacksTab::init() {
    if (!CCMenu::init())
        return false;

    setPosition({0, 0});

    return true;
}

void HacksTab::onToggle(CCObject* sender) {
    auto toggler = static_cast<CCMenuItemToggler*>(sender);
    if (!toggler) return;

    auto it = m_togglerCallbacks.find(toggler);
    if (it != m_togglerCallbacks.end())
        it->second(!toggler->isToggled());
}

bool HacksLayer::setup() {
    auto& config = Config::get();
    auto& hacks = Hacks::get();

    this->setTitle("GDH");
    this->m_title->setFntFile("Roboto.fnt"_spr);

    auto closeBtnSprite = CCSprite::create("GDH_closeBtn.png"_spr);
    m_closeBtn->setSprite(closeBtnSprite);

    auto background = extension::CCScale9Sprite::create("square02_small.png");
    background->setOpacity(100);
    background->setPosition({290.f, 115.f});
    background->setContentSize({325.f, 210.f});
    m_mainLayer->addChild(background);

    int i = 0;
    float tabPositionY = 205.f;
    for (auto& win : hacks.m_windows) {
        if (win.name == "Framerate" || win.name == "GDH Settings" || win.name == "Variables" || win.name == "Shortcuts")
            continue;

        auto button = ButtonSprite::create(win.name.c_str(), 90, true, "Roboto.fnt"_spr, (i == 0) ? "GDH_button_02.png"_spr : "GDH_button_01.png"_spr, 30.f, 0.7f);
        auto buttonClick = CCMenuItemExt::createSpriteExtra(button, [this](CCMenuItemSpriteExtra* sender) {
            index = sender->getTag();

            for (int i = 0; i < tabs.size(); i++) {
                auto* btn = m_buttonMenu->getChildByTag(i);
                auto* child = btn->getChildren()->objectAtIndex(0);
                if (auto* btnSprite = typeinfo_cast<ButtonSprite*>(child)) {
                    btnSprite->updateBGImage(i == index ? "GDH_button_02.png"_spr : "GDH_button_01.png"_spr);
                }
            }
        });
        buttonClick->setPosition({65.f, tabPositionY});
        buttonClick->setTag(i);
        m_buttonMenu->addChild(buttonClick);

        tabPositionY -= 35;
        i++;

        unsigned hacks_count = static_cast<int>(win.hacks.size()/8);
        for (auto& hck : win.hacks) {
            auto exampleTab = HacksTab::create();
            exampleTab->addToggle(hck.name.c_str(), config.get<bool>(hck.config, false), [&config, &hck](bool enabled) {
                config.set(hck.config, enabled);
                if (!hck.game_var.empty())
                    GameManager::get()->setGameVariable(hck.game_var.c_str(), enabled);
                if (hck.handlerFunc) hck.handlerFunc(enabled);
            });
            
        }
    }    

    auto exampleTab = HacksTab::create();
    exampleTab->addToggle("No Respawn Blink", false, [&config](bool enabled) { });
    m_mainLayer->addChild(exampleTab);

    return true;
}

HacksLayer* HacksLayer::create() {
    auto ret = new HacksLayer();
    if (ret->initAnchored(460.f, 260.f, "GDH_square.png"_spr)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

void HacksLayer::onExit() {
    geode::Popup<>::onExit();
}