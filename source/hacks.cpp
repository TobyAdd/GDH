#include "hacks.h"

json hacks::content;

void hacks::load()
{
    ifstream file("hacks.json");
    if (!file.is_open())
        return;
    string file_content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    hacks::content = json::parse(file_content);
    for (auto item : content.items())
    {
        json tabContent = item.value();
        for (size_t i = 0; i < tabContent.size(); i++)
        {
            json itemHack = tabContent.at(i);
            if (itemHack["enabled"])
            {
                json opcodes = itemHack["opcodes"];
                for (int j = 0; j < (int)opcodes.size(); j++)
                {
                    json opcode = opcodes.at(j);
                    uintptr_t address;
                    sscanf_s(opcode["addr"].get<string>().c_str(), "%x", &address);
                    DWORD base = (DWORD)GetModuleHandleA(0);
                    if (!opcode["lib"].is_null())
                    {
                        base = (DWORD)GetModuleHandleA(string(opcode["lib"]).c_str());
                    }
                    writemem(base + address, opcode["on"].get<string>());
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