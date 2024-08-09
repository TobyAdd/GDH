#pragma once
#include <map>
#include <string>

class Hacks {
public:
    static Hacks& get() {
        static Hacks instance;
        return instance;
    }

    Hacks& operator=(const Hacks&) = delete;
    Hacks(const Hacks&) = delete;
    
    bool noclip = false;
    bool unlock_items = false;
    bool no_respawn_blink = false;
    bool no_death_effect = false;
    bool safe_mode = false;

private:
    Hacks() = default;

protected:
    std::map<std::string, bool> m_hacks{};
};