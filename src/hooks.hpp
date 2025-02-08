#pragma once

namespace hooksH {
    extern int selectedStartpos;
    extern std::vector<StartPosObject*> startPositions;
    void switchStartPos(int incBy, bool direction = true);
}
