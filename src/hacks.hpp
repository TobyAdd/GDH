#pragma once
#include <string>
#include <vector>

struct hack {
    std::string name;
    std::string desc;
    std::string config;
    std::string game_var;
    
    std::function<void(bool)> handlerFunc = [](bool) {};
    void setHandler(std::function<void(bool)> func) { handlerFunc = func; }

    int keybind = 0;
};

struct window {
    std::string name;
    float x, y;
    float w, h;
    std::vector<hack> hacks;

    float orig_x, orig_y;
    float orig_w, orig_h;
};

class Hacks {
public:
    static Hacks& get() {
        static Hacks instance;
        return instance;
    }

    Hacks& operator=(const Hacks&) = delete;
    Hacks(const Hacks&) = delete;

    PauseLayer* pauseLayer;

    std::vector<window> m_windows;
    void Init();
private:
    Hacks() = default;

    bool SetHandlerByConfig(const std::string& configName, std::function<void(bool)> handler) {
        for (auto& window : m_windows) {
            auto it = std::find_if(window.hacks.begin(), window.hacks.end(), [&](const hack& h) {
                return h.config == configName;
            });

            if (it != window.hacks.end()) {
                it->setHandler(handler);
                return true;
            }
        }
        return false;
    }
};