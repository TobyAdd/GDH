#pragma once
#include <string>
#include <cctype>

namespace utilsH {
    bool isNumeric(const std::string& str);
    void UncompleteLevel();
    std::string GetKeyName(int key);
    void getFolder();
    void setPitchShifter(float pitch);
}

static void unimplemented() {}
inline void logMessage(const std::string& message) {
    std::ofstream logFile(folderPath / "h264_encode.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << message << std::endl;
    }
    logFile.close();
}