#pragma once
#include <Geode/Geode.hpp>

class popupSystem : public geode::Popup<> {
private:
    float currentY = 160.f;
public:
    static popupSystem* create(); 
    bool setup();
    void onExit();

    void AddText(std::string text, float scale = 0.5f, float y_space = 25.f);
    void AddIntInput(std::string hint, int value, std::function<void(int)> callback, float y_space = 35.f);
    void AddFloatInput(std::string hint, float value, std::function<void(float)> callback, float y_space = 35.f);
    void AddToggle(std::string text, bool value, std::function<void(bool)> callback, float y_space = 25.f);
};