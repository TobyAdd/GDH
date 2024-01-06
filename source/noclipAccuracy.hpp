#include "include.hpp"

class NoclipAccuracy {
public:    
    bool wouldDie = false;
    bool prevDied = false;
    int frames = 0;
    int deaths = 0;
    int deaths_full = 0;
    float totalDelta = 0;
    float prevX = 0;

    void handle_update(void* self, float delta) {
        void* player1 = *(void**)((char*)self + 0x870);
        float x = *(float*)((size_t)player1 + 0x48);

        if (x != prevX) {
            frames += 1;
            totalDelta += delta;
        }

        if (wouldDie) {
            wouldDie = false;
            if (totalDelta >= 0.1 && x != prevX) {
                deaths += 1;
            }
            if (totalDelta >= 0.1 && x != prevX && !prevDied)
                deaths_full += 1;
            prevDied = true;
        } else
            prevDied = false;

        prevX = x;
    }

    void handle_reset(void* self) {
        void* player1 = *(void**)((char*)self + 0x870);
        prevX = *(float*)((size_t)player1 + 0x48);
        frames = 0;
        totalDelta = 0;
        deaths = 0;
        deaths_full = 0;
        wouldDie = false;
    }

    void handle_death() {
        wouldDie = true;
    }

    float getPercentage() {
        if (frames == 0) {
            return 100.f;
        }

        return (float(frames - deaths) / float(frames)) * 100.f;
    }
};