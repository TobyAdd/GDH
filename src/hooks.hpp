#pragma once


namespace hooksH {
    extern int selectedStartpos;

    extern std::vector<StartPosObject*> startPositions;
    void switchStartPos(int incBy, bool direction = true);

    bool isWine();
    void spawn_deafen_process(int xkeycode);
}
