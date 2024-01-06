#include "include.hpp"
#include <thread>

class CPSCounter {
public:
    bool enabled = true;

    std::vector<DWORD> clicks;
    int cps = 0;
    int highscore = 0;

    void init() {
        std::thread([&]() {
            while (true) {
                if (enabled)
                    update();
                std::this_thread::sleep_for(100ms);
            }
        }).detach();
    }

    void reset() {
        cps = 0;
        highscore = 0;
    }
    
    void recordClick() {
        DWORD millis = GetTickCount();
        clicks.push_back(millis);
    }

    void update() {        
        cps = 0;
        std::vector<int> toRemove;
        DWORD currentTime = GetTickCount();
        
        for (size_t i = 0; i < clicks.size(); i++) {
            if (currentTime - clicks[i] < 1000) {
                cps++;
            } else {
                toRemove.push_back(i);
            }
        }

        for (int j = static_cast<int>(toRemove.size()) - 1; j >= 0; j--) {
            clicks.erase(clicks.begin() + toRemove[j]);
        }

        if (highscore < cps) {
            highscore = cps;
        }
    }
};