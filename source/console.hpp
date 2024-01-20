#pragma once
#include <Windows.h>
#include <iostream>

namespace Console {
    void Init();
    void Write(const std::string text);
    void WriteLn(const std::string text);
    void WriteF(const char* format, ...);
    void Clear();
}