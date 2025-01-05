#pragma once
#include <string>
#include <cctype>

namespace utilsH {
    bool isNumeric(const std::string& str);
    void UncompleteLevel();
    std::string GetKeyName(int key);
}