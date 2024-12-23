#pragma once
#include <Geode/Geode.hpp>

class HacksTab : public cocos2d::CCMenu {
public:
    static HacksTab* create();
    void addToggle(const std::string& text, const std::string& desc, bool enabled, const std::function<void(bool)>& callback);
    float x_lastToggle;
    float y_lastToggle;

    geode::prelude::ScrollLayer* m_scrollLayer;
private:
    HacksTab() = default;

    bool init();
    void onToggle(cocos2d::CCObject* sender);
protected:
    std::map<CCMenuItemToggler*, std::function<void(bool)>> m_togglerCallbacks;
};

class HacksLayer : public geode::Popup<> {
private:
    std::vector<HacksTab*> m_tabs;
    int m_index = 0;
public:
    static HacksLayer* create(); 

    void switchTab(int newIndex);
    bool setup();
    void onExit();
};