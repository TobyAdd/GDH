#include "keybinds.hpp"
#include <filesystem>
#include <fstream>
#include <streambuf>
#include <vector>
#include <sstream>
#include <string>
#include "hacks.hpp"


namespace keybinds
{
    json binds;

    void AddKeybind(string hackName, int keycode)
    {
        Console::WriteLine("adding keybind");

        // *static_cast<bool*>(switcher) = true;

        binds[to_string(keycode)] = hackName;
    }

    void keypress(int keycode)
    {
        Console::WriteLine(to_string(binds));
        Console::WriteLine(to_string(keycode));
        if (binds.contains(to_string(keycode))) {
            for (auto item : hacks::content.items()) {
                auto &components = item.value()["components"];
                if (!components.is_null())
                {
                    for (auto &component : components)
                    {
                        std::string type = component["type"];
                        if (component.contains("name") == true){
                            // Console::WriteLine(component["name"]);
                            if (type == "hack" && component["name"] == binds[to_string(keycode)])
                            {
                                Console::WriteLine(component["name"]);
                                component["enabled"] = !component["enabled"];
                                component["bind"] = keycode;
                                bool enabled = component["enabled"];
                                json opcodes = component["opcodes"];
                                for (auto &opcode : opcodes)
                                {
                                    string addrStr = opcode["addr"];
                                    string bytesStr = enabled ? opcode["on"] : opcode["off"];

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
                                    if (enabled && reference["on"].is_null())
                                    {
                                        continue;
                                    }

                                    if (!enabled && reference["off"].is_null())
                                    {
                                        continue;
                                    }

                                    string addrStr = reference["addr"];
                                    string addRefStr = enabled ? reference["on"] : reference["off"];

                                    uintptr_t address, refAdress;
                                    sscanf_s(addrStr.c_str(), "%x", &address);
                                    sscanf_s(addRefStr.c_str(), "%x", &refAdress);

                                    DWORD baseAddr = (DWORD)GetModuleHandleA(0);
                                    if (!reference["libAddr"].is_null() && !string(reference["libAddr"]).empty())
                                    {
                                        baseAddr = (DWORD)GetModuleHandleA(string(reference["libAddr"]).c_str());
                                    }

                                    auto refSwitch = enabled ? reference["libON"] : reference["libOff"];

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
                    }
                }
            }
        }
    }
}