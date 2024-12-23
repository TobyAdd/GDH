#include "gui_mobile.hpp"
#include "config.hpp"
#include "hacks.hpp"

using namespace geode::prelude;

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
    y_lastToggle = 195 - (m_togglerCallbacks.size() * 25);

    toggle->setPosition({ 15, y_lastToggle });
    toggle->toggle(enabled);

    auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
    label->setAnchorPoint({0.f, 0.5f});
    label->setScale(0.5f);
    label->setPosition({ toggle->getPositionX() + 15.f, y_lastToggle });

    m_scrollLayer->m_contentLayer->addChild(toggle);
    m_scrollLayer->m_contentLayer->addChild(label);

    m_togglerCallbacks[toggle] = callback;
}

bool HacksTab::init() {
    if (!CCMenu::init())
        return false;

    setPosition({0, 0});

    m_scrollLayer = ScrollLayer::create({325.f, 210.f});
    m_scrollLayer->m_contentLayer->setLayout(
        geode::ColumnLayout::create()
            ->setAutoScale(false)
            ->setAxisReverse(true)
            ->setAutoGrowAxis(m_scrollLayer->getContentHeight())
            ->setAxisAlignment(geode::AxisAlignment::End)
            ->setGap(0)
    );
    m_scrollLayer->setPosition({128.f, 10.f});
    m_scrollLayer->m_peekLimitTop = 15;
    m_scrollLayer->m_peekLimitBottom = 15;
    
    addChild(m_scrollLayer);
    
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
    background->setPosition({290.f, 115.f});
    background->setContentSize({325.f, 210.f});
    m_mainLayer->addChild(background);

    int i = 0;
    float tabPositionY = 205.f;

    for (auto& win : hacks.m_windows) {
        if (win.name == "Framerate" || win.name == "GDH Settings" || win.name == "Variables" || win.name == "Shortcuts")
            continue;

        auto button = ButtonSprite::create(win.name.c_str(), 90, true, "Roboto.fnt"_spr, 
            (i == m_index) ? "GDH_button_02.png"_spr : "GDH_button_01.png"_spr, 30.f, 0.7f);
        auto buttonClick = CCMenuItemExt::createSpriteExtra(button, [this, i](CCMenuItemSpriteExtra* sender) {
            switchTab(i);
        });
        buttonClick->setPosition({65.f, tabPositionY});
        buttonClick->setTag(i);
        m_buttonMenu->addChild(buttonClick);

        auto tab = HacksTab::create();
        tab->setVisible(i == 0);
        tab->setID(fmt::format("{}"_spr, win.name));
        m_mainLayer->addChild(tab);

        for (auto& hck : win.hacks) {
            tab->addToggle(hck.name.c_str(), config.get<bool>(hck.config, false), [&config, &hck](bool enabled) {
                config.set(hck.config, enabled);
                if (!hck.game_var.empty())
                    GameManager::get()->setGameVariable(hck.game_var.c_str(), enabled);
                if (hck.handlerFunc) hck.handlerFunc(enabled);
            });          
        }

        tab->m_scrollLayer->m_contentLayer->setContentHeight(35 * win.hacks.size() + 5);
        tab->m_scrollLayer->m_contentLayer->updateLayout();
        tab->m_scrollLayer->moveToTop();

        m_tabs.push_back(tab);

        tabPositionY -= 35;
        i++;
    }

    return true;
}

void HacksLayer::switchTab(int newIndex) {
    if (newIndex < 0 || newIndex >= m_tabs.size()) return;

    m_index = newIndex;

    for (int i = 0; i < m_tabs.size(); i++) {
        m_tabs[i]->setVisible(i == m_index);
    }

    for (int i = 0; i < m_buttonMenu->getChildrenCount(); i++) {
        auto* button = static_cast<CCMenuItemSpriteExtra*>(m_buttonMenu->getChildByTag(i));
        if (button) {
            auto* btnSprite = dynamic_cast<ButtonSprite*>(button->getChildren()->objectAtIndex(0));
            if (btnSprite) {
                btnSprite->updateBGImage(i == m_index ? "GDH_button_02.png"_spr : "GDH_button_01.png"_spr);
            }
        }
    }
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