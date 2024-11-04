#include "includes.hpp"
#include <thread>
#include <atomic>
#include <chrono>
#include <deque>

using namespace std::chrono_literals;

class CPSCounter {
public:
    bool enabled = true;

    std::vector<DWORD> clicks;
    int cps = 0;
    int highscore = 0;
    int overall = 0;

    void reset() {
        cps = 0;
        highscore = 0;
        overall = 0;
    }
    
    void recordClick() {
        DWORD millis = GetTickCount();
        overall++;
        clicks.push_back(millis);
    }

    void update() {        
        time_t currentTick = GetTickCount();

        clicks.erase(std::remove_if(clicks.begin(), clicks.end(), [currentTick](DWORD tick) {
            return currentTick - tick > 1000;
        }), clicks.end());

        cps = clicks.size();

        if (highscore < cps) {
            highscore = cps;
        }
    }
};

inline CPSCounter cps_counter;
