#pragma once


namespace hooksH {
    extern int selectedStartpos;

    extern std::vector<StartPosObject*> startPositions;
    void switchStartPos(int incBy, bool direction = true);

#ifdef GEODE_IS_WINDOWS
    bool isWine();
    void spawn_deafen_process(int xkeycode);
#endif
}
