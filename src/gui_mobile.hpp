#pragma once
#include <Geode/Geode.hpp>

class HacksLayer : public geode::Popup<> {
protected:
    bool setup();
    void onExit();
public:
    static HacksLayer* create();
    int index = 0;
};

class HacksTab : public cocos2d::CCMenu {
public:
    static HacksTab* create();
    void addToggle(const std::string& text, bool enabled, const std::function<void(bool)>& callback);
    float x_lastToggle;
    float y_lastToggle;
private:
    HacksTab() = default;

    bool init();
    void onToggle(cocos2d::CCObject* sender);
protected:
    std::map<CCMenuItemToggler*, std::function<void(bool)>> m_togglerCallbacks;
};