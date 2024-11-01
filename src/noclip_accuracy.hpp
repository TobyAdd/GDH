#include "includes.hpp"

class NoclipAccuracy {
public:    
    bool wouldDie = false;
    bool prevDied = false;
    int frames = 0;
    int deaths = 0;
    int deaths_full = 0;
    float totalDelta = 0;
    float prevX = 0;

    void handle_update(PlayLayer* self, float delta) {
        float x = self->m_player1->m_position.x;

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

    void handle_reset(PlayLayer* self) {
        prevX = self->m_player1->m_position.x;
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

inline NoclipAccuracy noclip_accuracy;