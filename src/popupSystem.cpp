#include "popupSystem.hpp"
#include "utils.hpp"

using namespace geode::prelude;

bool popupSystem::setup() {
    this->setTitle("More Settings");

    return true;
}

popupSystem* popupSystem::create() {
    auto ret = new popupSystem();
    if (ret->initAnchored(300.f, 200.f, "GJ_square01.png")) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

void popupSystem::onExit() {
    geode::Popup<>::onExit();
}

void popupSystem::AddText(std::string text, float scale, float y_space) {
    auto label = cocos2d::CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
    label->setAnchorPoint({0.f, 0.5f});
    label->setScale(scale);
    label->setPosition({10, currentY});
    m_mainLayer->addChild(label);

    currentY -= y_space;
}

void popupSystem::AddIntInput(std::string hint, int value, std::function<void(int)> callback, float y_space) {
    auto input = TextInput::create(285, hint, "chatFont.fnt");
    input->setPosition({150.f, currentY});
    input->setString(fmt::format("{}", value));
    input->setFilter("1234567890");
    input->setCallback([callback](const std::string& text) {
        if (utilsH::isNumeric(text))
            callback(std::stoi(text));
    });

    m_mainLayer->addChild(input);

    currentY -= y_space;
}

void popupSystem::AddFloatInput(std::string hint, float value, std::function<void(float)> callback, float y_space) {
    auto input = TextInput::create(285, hint, "chatFont.fnt");
    input->setPosition({150.f, currentY});
    input->setString(fmt::format("{:.2f}", value));
    input->setFilter("1234567890.");
    input->setCallback([callback](const std::string& text) {
        if (utilsH::isNumeric(text))
            callback(std::stof(text));
    });

    m_mainLayer->addChild(input);

    currentY -= y_space;
}

void popupSystem::AddToggle(std::string text, bool value, std::function<void(bool)> callback, float y_space) {
    CCMenuItemToggler* toggler = CCMenuItemExt::createTogglerWithStandardSprites(0.75f, [this, callback](CCMenuItemToggler* sender) {
        callback(!sender->isOn());
    });

    toggler->setPosition({ 10, 0 });
    toggler->toggle(value);

    auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
    label->setAnchorPoint({0.f, 0.5f});
    label->setScale(0.5f);
    label->setPosition({ toggler->getPositionX() + 15.f, toggler->getPositionY() });

    auto hack = CCMenu::create();
    hack->setPosition({15, currentY});
    hack->addChild(toggler);
    hack->addChild(label);

    m_mainLayer->addChild(hack);

    currentY -= y_space;
}