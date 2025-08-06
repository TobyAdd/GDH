#pragma once

namespace hooksH {
    extern float color_dt;
    extern int selectedStartpos;
    extern std::vector<StartPosObject*> startPositions;
    void switchStartPos(int incBy, bool direction = true);
    bool startposExist();
}
