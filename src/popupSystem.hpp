#pragma once
#include <Geode/Geode.hpp>

class PopupSystem : public geode::Popup<> {
public:
    float y_lastToggle;

    bool setup();

    static PopupSystem* create(); 
    void onExit();
};