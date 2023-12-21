#include "include.h"
#include <fstream>
#include <streambuf>
#include <vector>
#include <sstream>
#include <string>

namespace hacks {
    extern json content;
    bool writemem(uintptr_t address, string bytes);
    void load();
}