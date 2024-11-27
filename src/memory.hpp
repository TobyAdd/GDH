#pragma once
#include <fstream>
#include <streambuf>
#include <vector>
#include <sstream>

namespace memory {
    uintptr_t PatternScan(uintptr_t base, uintptr_t scanSize, const std::string signature);
}