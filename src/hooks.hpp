#include "includes.hpp"

extern float left_over;

namespace hooks {
    extern PauseLayer* pauseLayer;
}

namespace startpos_switcher {
    void switchStartPos(int incBy, bool direction);
    void handleKeyPress(int keyCode);
}