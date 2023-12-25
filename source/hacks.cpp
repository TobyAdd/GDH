#include "hacks.hpp"

json hacks::content;

void hacks::load()
{
    ifstream file("hacks.json");
    if (!file.is_open())
        return;
    string file_content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    hacks::content = json::parse(file_content);

    for (auto item : hacks::content.items()) {
        std::string windowName = item.key();
        auto &components = item.value()["components"];
        if (!components.is_null())
        {
            for (auto &component : components)
            {
                std::string type = component["type"];
                if (type == "hack")
                {
                    bool enabled = component["enabled"];
                    if (enabled) {
                        json opcodes = component["opcodes"];
                        for (auto &opcode : opcodes)
                        {
                            string addrStr = opcode["addr"];
                            string bytesStr = opcode["on"];

                            uintptr_t address;
                            sscanf_s(addrStr.c_str(), "%x", &address);

                            DWORD base = (DWORD)GetModuleHandleA(0);
                            if (!opcode["lib"].is_null())
                            {
                                base = (DWORD)GetModuleHandleA(string(opcode["lib"]).c_str());
                            }

                            hacks::writemem(base + address, bytesStr);
                        }
                    }
                }
            }
        }
    }
}

bool hacks::writemem(uintptr_t address, std::string bytes)
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