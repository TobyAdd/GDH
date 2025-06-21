#pragma once
#include <string>
#include <vector> 

enum cheat_state
{
    safe_mode,
    unwanted,
    cheating,
    legit
};

struct hack {
    std::string name;
    std::string desc;
    std::string config;
    std::string game_var;
    
    std::function<void(bool)> handlerFunc = nullptr;
    void setHandler(std::function<void(bool)> func) { handlerFunc = func; }

    std::function<void()> handlerCustomWindow = nullptr;
    void setCustomWindowHandler(std::function<void()> func) { handlerCustomWindow = func; }

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

extern int m_exceptedTicks;

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

    void saveKeybinds();
    void loadKeybinds();
    void toggleKeybinds(int key);

    cheat_state preCheatState = cheat_state::legit;
    cheat_state cheatState = cheat_state::legit;
    cheat_state cheatingCheck();

    
    bool TPSBypass_Init(bool enable = true) {
        auto exceptedTicksAddr = (uintptr_t)&m_exceptedTicks;
        std::vector<uint8_t> patchBytes;
        
        // mov rax, exceptedTicks (48 B8 + 8 byte address)
        patchBytes.push_back(0x48);
        patchBytes.push_back(0xB8);
        for (int i = 0; i < 8; i++) {
            patchBytes.push_back((exceptedTicksAddr >> (i * 8)) & 0xFF);
        }

        // mov r11d, [rax] (44 8B 18)
        patchBytes.push_back(0x44);
        patchBytes.push_back(0x8B);
        patchBytes.push_back(0x18);

        for (int i = 0; i < 54; i++) {
            patchBytes.push_back(0x90); // NOP
        }
        
        uintptr_t offset = geode::base::get() + 0x232294;  
        static auto result1 = geode::Mod::get()->patch((void*)(offset), patchBytes);
        static auto patch1 = result1.isErr() ? nullptr : result1.unwrap();
        if (patch1) {
            if (enable) (void)patch1->enable();
            else (void)patch1->disable();
            return true;
        }

        return false;
    }

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

    bool SetCustomWindowHandlerByConfig(const std::string& configName, std::function<void()> handler) {
        for (auto& window : m_windows) {
            auto it = std::find_if(window.hacks.begin(), window.hacks.end(), [&](const hack& h) {
                return h.config == configName;
            });

            if (it != window.hacks.end()) {
                it->setCustomWindowHandler(handler);
                return true;
            }
        }
        return false;
    }
};