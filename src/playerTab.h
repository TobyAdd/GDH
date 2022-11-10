#include "pch.h"

using namespace std;

namespace playerTab {
    extern bool playerInit;
    extern vector<bool> playerConfig;
    extern json playerData;

    void init();
    void draw();
    void close();
}