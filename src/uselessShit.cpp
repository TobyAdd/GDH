#include "uselessShit.h"

float uselessShit::overlaysize = 1.0f;

namespace uselessShit {
    bool uselessShit::stringToBool(string s) {
        if (s == "true") 
            return true;
        else
            return false;
    }

    string uselessShit::boolToString(bool b) {
        if (b) 
            return "true";
        else
            return "false";
    }

    string GetFilenameFromPath(string s) {
        return filesystem::path(s).filename().u8string();
    }
}