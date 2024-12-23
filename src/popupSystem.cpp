#include "popupSystem.hpp"

using namespace geode::prelude;

PopupSystem* PopupSystem::create() {
    auto ret = new PopupSystem();
    if (ret->initAnchored(200.f, 150.f, "GDH_square.png"_spr)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool PopupSystem::setup() {
    auto closeBtnSprite = cocos2d::CCSprite::create("GDH_closeBtn.png"_spr);
    m_closeBtn->setSprite(closeBtnSprite);

    return true;
}

void PopupSystem::onExit() {
    geode::Popup<>::onExit();
}