#include "memory.hpp"

struct PatternByte {
    bool isWildcard;
    uint8_t value;
};

uintptr_t memory::PatternScan(uintptr_t base, uintptr_t scanSize, const std::string signature) {
    std::vector<PatternByte> patternData;

    for (size_t i = 0; i < signature.size(); ++i) {
        if (signature[i] == ' ') {
            continue;
        }

        if (signature[i] == '?') {
            patternData.push_back({ true, 0 });
        }
        else {
            std::string byteStr = signature.substr(i, 2);
            patternData.push_back({ false, static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16)) });
            i++;
        }
    }

    for (uintptr_t i = base; /*i < base + scanSize*/; ++i) {
        bool found = true;

        for (size_t j = 0; j < patternData.size(); ++j) {
            if (patternData[j].isWildcard) {
                continue;
            }

            if (patternData[j].value != *reinterpret_cast<uint8_t*>(i + j)) {
                found = false;
                break;
            }
        }

        if (found) {
            return i;
        }
    }

    return 0;
}