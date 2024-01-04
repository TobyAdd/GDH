#include "hacks.hpp"
#include "hooks.hpp"
#include <filesystem>
#include "startposSwitcher.hpp"
#include "smartStartpos.hpp"
#include "keybinds.hpp"

json hacks::content;

void CheckDir(string dir)
{
    if (!filesystem::is_directory(dir) || !filesystem::exists(dir))
    {
        filesystem::create_directory(dir);
    }
}

void hacks::load()
{
    CheckDir("GDH");
    CheckDir("GDH/extensions");
    CheckDir("GDH/replays");
    CheckDir("GDH/styles");
    
    ifstream file("GDH/hacks.json");

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
                    if (component.contains("bind")) 
                    {
                        keybinds::AddKeybind(component["name"], component["bind"]);
                    }
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

                        json references = component["references"];
                        
                        for (auto &reference : references)
                        {
                            if(reference["on"].is_null()){
                                continue;
                            }

                            string addrStr = reference["addr"];
                            string addRefStr = reference["on"];

                            uintptr_t address, refAdress;
                            sscanf_s(addrStr.c_str(), "%x", &address);
                            sscanf_s(addRefStr.c_str(), "%x", &refAdress);

                            DWORD baseAddr = (DWORD)GetModuleHandleA(0);
                            if (!reference["libAddr"].is_null() && !string(reference["libAddr"]).empty())
                            {
                                baseAddr = (DWORD)GetModuleHandleA(string(reference["libAddr"]).c_str());
                            }

                            auto refSwitch = reference["libON"];

                            DWORD baseRef = (DWORD)GetModuleHandleA(0);
                            if (!refSwitch.is_null() && !string(refSwitch).empty())
                            {
                                baseRef = (DWORD)GetModuleHandleA(string(refSwitch).c_str());
                            }

                            uintptr_t reference_address = baseAddr + address;
                            uintptr_t reference_value = baseRef + refAdress;

                            hacks::push_write(reference_address, reference_value);   
                            
                        }
                    }
                }
                else if (type == "speedhack") {
                    auto value = component["value"];
                    engine.speed = value.is_null() ? 1.0f : value;
                }
                else if (type == "fps_bypass") {
                    auto value = component["value"];
                    engine.fps = value.is_null() ? 60.0f : value;
                }
                else if (type == "transitionCustomizerCBX") {
                    auto value = component["value"];
                    hooks::transitionSelect = value;
                }
                else if (type == "startpos_checkbox")
                {
                    auto value = component["value"];
                    startposSwitcher::setEnabled(value);
                }
                else if (type == "a_d_startpos")
                {
                    auto value = component["value"];
                    startposSwitcher::setAlternateKeys(value);
                }                
                else if (type == "smart_startpos_checkbox")
                {
                    auto value = component["value"];
                    smartStartpos::enabled = value;
                }
                else if (type == "pmb_checkbox")
                {
                    auto value = component["value"];
                    hooks::musicUnlocker = value;
                }
            }
        }
    }
}

bool hacks::push_write(const uintptr_t address, const DWORD destination_address)
{
    DWORD oldProtect, newProtect;

    if (VirtualProtect(reinterpret_cast<LPVOID>(address), 4, PAGE_EXECUTE_READWRITE, &oldProtect))
    {
        if (WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<LPVOID>(address), &destination_address, 4, nullptr))
        {
            return VirtualProtect(reinterpret_cast<LPVOID>(address), 4, oldProtect, &newProtect);
        }
    }

    return false;
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

void hacks::inject_extensions() {  
    for (const auto& file : std::filesystem::directory_iterator("GDH/extensions"))
	{
        if (file.path().extension() == ".dll")
        {
		    LoadLibraryA(file.path().generic_string().c_str());
        }
	}
}