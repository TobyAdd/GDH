#pragma once
#include "includes.hpp"
#include <psapi.h>
#include <fstream>
#include <streambuf>
#include <vector>
#include <sstream>
#include <cctype>

namespace memory {
    uintptr_t PatternScan(uintptr_t base, uintptr_t scanSize, const std::string signature);
    bool WriteBytes(uintptr_t address, std::string bytes);
    size_t CountBytesInHexStr(const std::string hexStr);
    std::string ReadMemoryHexStr(uintptr_t address, size_t size);
}