#include "replayEngine.hpp"

ReplayEngine engine;

void ReplayEngine::handle_action(void* self, bool hold, int player_button, bool player) {
    unsigned frame = getFrame(self);
    replay.push_back({
        frame,
        hold,
        player_button,
        player
    });
}

unsigned ReplayEngine::getFrame(void* self) {
    if (self) {
        auto time = *(double*)(((char*)self) + 0x328);
        return static_cast<unsigned>(time * engine.fps);
    }       

    return 0;
}