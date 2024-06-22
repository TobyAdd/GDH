#include "includes.hpp"

namespace hooks {
    extern PauseLayer* pauseLayer;
}

namespace startpos_switcher {
    void switchStartPos(int incBy, bool direction);
    void handleKeyPress(int keyCode);
}