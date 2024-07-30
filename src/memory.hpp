#pragma once
#include "includes.hpp"
#include <Windows.h>
#include <psapi.h>
#include <fstream>
#include <streambuf>
#include <vector>
#include <sstream>
#include <cctype>
#include <iomanip>

namespace memory {
    uintptr_t PatternScan(uintptr_t base, uintptr_t scanSize, const std::string signature);
    bool WriteBytes(uintptr_t address, std::string bytes);
    size_t CountBytesInHexStr(const std::string hexStr);
    std::string ReadMemoryHexStr(uintptr_t address, size_t size);
    bool WriteFloat(uintptr_t address, float value);
    bool WriteInt(uintptr_t address, int value);
}