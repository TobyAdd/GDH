#include "pch.h"

using namespace std;

namespace bypassTab {
    extern bool bypassInit;
    extern vector<bool> bypassConfig;
    extern json bypassData;

    void init();
    void draw();
    void close();
}