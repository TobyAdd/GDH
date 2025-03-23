#pragma once
#include <string>
#include <vector>
#include <Windows.h>

namespace KeyMappingUtils {
    struct KeyMapping {
        int glfwKey;
        int winApiKey;
        std::string name;
    };

    std::string GetNameFromGLFW(int glfwKey);
    std::string GetNameFromWinAPI(int winApiKey);
    int GetGLFWFromWinAPI(int winApiKey);
    int GetWinAPIFromGLFW(int glfwKey);
    int GetXcbFromGLFW(int glfwKey);
    const KeyMapping* GetMappingFromGLFW(int glfwKey);
    const KeyMapping* GetMappingFromWinAPI(int winApiKey);
    const std::vector<KeyMapping>& GetKeyMappings();
}