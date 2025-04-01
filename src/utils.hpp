#pragma once
#include <string>
#include <cctype>
#include <fstream>

namespace utilsH {
    bool isNumeric(const std::string& str);
    void UncompleteLevel();
    void getFolder();
    void setPitchShifter(float pitch);
    bool isLinuxWine();
}

static void unimplemented() {}

#include "config.hpp"
static void logMessage(const std::string& message) {
    std::ofstream logFile(folderPath / "h264_encode.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << message << std::endl;
    }
    logFile.close();
}