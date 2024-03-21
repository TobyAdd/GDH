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

    for (uintptr_t i = base; i < base + scanSize; ++i) {
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

bool memory::WriteBytes(uintptr_t address, std::string bytes)
{
    std::vector<unsigned char> byteVec;
    std::stringstream byteStream(bytes);
    std::string byteStr;

    while (getline(byteStream, byteStr, ' '))
    {
        unsigned int byte = std::stoul(byteStr, nullptr, 16);
        byteVec.push_back(static_cast<unsigned char>(byte));
    }

    DWORD oldProtect;
    if (VirtualProtect(reinterpret_cast<void*>(address), byteVec.size(), PAGE_EXECUTE_READWRITE, &oldProtect))
    {
        memcpy(reinterpret_cast<void*>(address), byteVec.data(), byteVec.size());
        VirtualProtect(reinterpret_cast<void*>(address), byteVec.size(), oldProtect, &oldProtect);
        return true;
    }
    else
    {
        return false;
    }
}

size_t memory::CountBytesInHexStr(const std::string hexStr) {
    const std::string validChars = "0123456789ABCDEFabcdef";

    size_t count = 0;

    for (char ch : hexStr) {
        if (std::isspace(static_cast<unsigned char>(ch))) {
            continue;
        }

        if (validChars.find(ch) != std::string::npos) {
            count++;
        }
    }

    return (count != 0) ? count / 2 : 0;
}

std::string memory::ReadMemoryHexStr(uintptr_t address, size_t size) {
    std::vector<uint8_t> buffer(size);

    SIZE_T bytesRead;
    if (ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<LPCVOID>(address), buffer.data(), size, &bytesRead)) {
        std::stringstream hexString;

        for (size_t i = 0; i < bytesRead; ++i) {
            hexString << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(buffer[i]) << " ";
        }

        return hexString.str().substr(0, hexString.str().size() - 1);
    }
    return "";
}

bool memory::WriteFloat(uintptr_t address, float value) {
    DWORD oldProtect;
    if (VirtualProtect(reinterpret_cast<void*>(address), sizeof(float), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, &value, sizeof(float), nullptr);
        return VirtualProtect(reinterpret_cast<void*>(address), sizeof(float), oldProtect, &oldProtect);
    }
    else
    {
        return false;
    }
}