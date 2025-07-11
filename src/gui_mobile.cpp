#include "gui_mobile.hpp"
#include "config.hpp"
#include "hacks.hpp"
#include "labels.hpp"
#include "replayEngine.hpp"
#include "utils.hpp"

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

void HacksTab::addToggle(const std::string& text, const std::string& desc, bool enabled, const std::function<void(bool)>& callback, std::function<void()> handlerCustomWindow) {
    auto toggle = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(HacksTab::onToggle), 0.75f);

    toggle->setPosition({ 15, 10 });
    toggle->toggle(enabled);

    auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
    label->setAnchorPoint({0.f, 0.5f});
    label->setScale(0.5f);
    label->setPosition({ toggle->getPositionX() + 15.f, 10 });

    auto hack = CCMenu::create();
    hack->setContentSize({325, 25});
    hack->addChild(toggle);
    hack->addChild(label);

    if (!desc.empty()) {
        auto descSprite = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        descSprite->setScale(0.4f);
        auto descClick = CCMenuItemExt::createSpriteExtra(descSprite, [this, desc](CCMenuItemSpriteExtra* sender) {
            FLAlertLayer::create("Desc", desc, "OK")->show();
        });
        descClick->setPosition(label->getPositionX() + label->getScaledContentWidth() + 5.f, label->getScaledContentHeight());
        hack->addChild(descClick);
    }

    if (handlerCustomWindow != nullptr) {
        auto moreSettingsSprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        moreSettingsSprite->setScale(0.5f);
        auto moreSettingsSpriteClick = CCMenuItemExt::createSpriteExtra(moreSettingsSprite, [this, handlerCustomWindow](CCMenuItemSpriteExtra* sender) {
            handlerCustomWindow();
        });
        moreSettingsSpriteClick->setPosition(label->getPositionX() + label->getScaledContentWidth() + 25.f, label->getPositionY());
        hack->addChild(moreSettingsSpriteClick);
    }

    m_scrollLayer->m_contentLayer->addChild(hack);

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

CCLabelBMFont* AddTextToToggle(const char *str, CCMenuItemToggler* toggler, float x_space = 15.f) {
    auto label = CCLabelBMFont::create(str, "bigFont.fnt");
    label->setAnchorPoint({0.f, 0.5f});
    label->setPosition({toggler->getPositionX() + x_space, toggler->getPositionY()});
    label->setScale(0.5f);
    return label;
}

bool HacksLayer::setup() {
    auto& config = Config::get();
    auto& hacks = Hacks::get();

    this->setTitle("GDH");

    auto warningButton = CCSprite::createWithSpriteFrameName("GJ_reportBtn_001.png");
    warningButton->setScale(0.65f);
    auto warningButtonClick = CCMenuItemExt::createSpriteExtra(warningButton, [this](CCMenuItemSpriteExtra* sender) {
        FLAlertLayer::create("GDH Beta Testing", "<cr>Note: GDH is currently in beta testing, so some elements may be unstable/unfinished</c>\n\n<cg>In case of any issues/crashes, please report the problem in the GDH issues section on GitHub</c>", "OK")->show();
    });
    warningButtonClick->setPosition({455.f, 255.f});
    m_buttonMenu->addChild(warningButtonClick);

    auto githubButton = CCSprite::create("github.png"_spr);
    auto githubButtonClick = CCMenuItemExt::createSpriteExtra(githubButton, [this](CCMenuItemSpriteExtra* sender) {
        CCApplication::sharedApplication()->openURL("https://github.com/TobyAdd/GDH");
    });
    githubButtonClick->setPosition({420.f, 255.f});
    m_buttonMenu->addChild(githubButtonClick);

    auto background = extension::CCScale9Sprite::create("square02_small.png");
    background->setPosition({290.f, 115.f});
    background->setContentSize({325.f, 210.f});
    background->setOpacity(100);
    m_mainLayer->addChild(background);

    int i = 0;
    float tabPositionY = 205.f;

    for (auto& win : hacks.m_windows) {
        if (win.name == "Framerate" || win.name == "GDH Settings" || win.name == "Variables" || win.name == "Shortcuts")
            continue;

        auto button = ButtonSprite::create(win.name.c_str(), 90, true, "bigFont.fnt", (i == m_index) ? "GJ_button_02.png" : "GJ_button_01.png", 30.f, 0.7f);

        auto buttonClick = CCMenuItemExt::createSpriteExtra(button, [this, i](CCMenuItemSpriteExtra* sender) {
            switchTab(i);
        });
        buttonClick->setPosition({65.f, tabPositionY});
        m_buttonTabs.push_back(buttonClick);
        m_buttonMenu->addChild(buttonClick);

        auto tab = HacksTab::create();
        tab->setVisible(i == 0);
        tab->setID(fmt::format("{}"_spr, win.name));
        m_mainLayer->addChild(tab);

        if (win.name == "Labels") {
            auto& labels = Labels::get();
            labels.initMobileContext(tab->m_scrollLayer);
        }

        for (auto& hck : win.hacks) {
            tab->addToggle(hck.name, hck.desc, config.get<bool>(hck.config, false), [&config, &hck](bool enabled) {
                config.set(hck.config, enabled);
                if (!hck.game_var.empty())
                    GameManager::get()->setGameVariable(hck.game_var.c_str(), enabled);
                if (hck.handlerFunc) hck.handlerFunc(enabled);
            }, hck.handlerCustomWindow);          
        }

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

    for (int i = 0; i < m_buttonTabs.size(); i++) {
        auto* button = m_buttonTabs[i];
        if (button) {
            auto* btnSprite = static_cast<ButtonSprite*>(button->getChildren()->objectAtIndex(0));
            if (btnSprite) {
                btnSprite->updateBGImage(i == m_index ? "GJ_button_02.png" : "GJ_button_01.png");
            }
        }
    }
}

HacksLayer* HacksLayer::create() {
    auto ret = new HacksLayer();
    if (ret->initAnchored(460.f, 260.f, "GJ_square01.png")) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

void HacksLayer::onExit() {
    Config::get().save(fileDataPath);
    Labels::get().save();
    RGBIcons::get().save();
    geode::Popup<>::onExit();
}