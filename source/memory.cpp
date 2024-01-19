#include "memory.hpp"

uintptr_t memory::PatternScan(uintptr_t base, uintptr_t scanSize, const std::string signature) {
    std::vector<char> patternBytes;

    std::istringstream byteStream(signature);
    std::string byteStr;

    while (getline(byteStream, byteStr, ' ')) {
        if (byteStr == "?") {
            patternBytes.push_back('\?');
        } else {
            patternBytes.push_back(static_cast<char>(std::stoul(byteStr, nullptr, 16)));
        }
    }

    const size_t patternLength = patternBytes.size();
    const char* patternData = patternBytes.data();

    std::vector<char> buffer(scanSize);

    if (!ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<LPCVOID>(base), buffer.data(), scanSize, 0)) {
        return 0;
    }

    for (uintptr_t i = 0; i < scanSize - patternLength; ++i) {
        bool found = true;
        for (size_t j = 0; j < patternLength; ++j) {
            found &= patternData[j] == '\?' || patternData[j] == buffer[i + j];
        }
        if (found) {
            return base + i;
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
    if (VirtualProtect(reinterpret_cast<void *>(address), byteVec.size(), PAGE_EXECUTE_READWRITE, &oldProtect))
    {
        memcpy(reinterpret_cast<void *>(address), byteVec.data(), byteVec.size());
        VirtualProtect(reinterpret_cast<void *>(address), byteVec.size(), oldProtect, &oldProtect);
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