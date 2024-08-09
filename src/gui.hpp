#pragma once
#include <Geode/Geode.hpp>
#include <functional>
#include <map>
#include <array>
#include <string>
#include "hacks.hpp"

using namespace geode::prelude;

class HacksTab : public CCMenu {
public:
    static HacksTab* create();
    void addToggle(const std::string& text, bool enabled, const std::function<void(bool)>& callback, float scale);
    void setAmountOfHacks(int amount);
    ScrollLayer* m_scrollLayer;
private:
    HacksTab() = default;

    bool init();
    void onToggle(CCObject* sender);
protected:
    std::map<CCMenuItemToggler*, std::function<void(bool)>> m_togglerCallbacks;
    CCMenu* m_togglesMenu;
};

class HacksLayer : public geode::Popup<> {
private:
    HacksTab* coreTab;
    HacksTab* playerTab;
    HacksTab* creatorTab;
    HacksTab* replayTab;
    HacksTab* aboutTab;

protected:
    bool setup() override;
    void onChangeTab(CCObject* sender);

public:
    static HacksLayer* create();
};