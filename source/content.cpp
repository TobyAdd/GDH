#include "content.hpp"
#include "console.hpp"
#include "memory.hpp"
#include "hooks.hpp"
#include "gui.hpp"

json Content::content;

void CheckDir(std::string dir)
{
    if (!std::filesystem::is_directory(dir) || !std::filesystem::exists(dir))
    {
        std::filesystem::create_directory(dir);
    }
}

bool Content::load() {
    try {
        CheckDir("GDH");
        CheckDir("GDH/macros");
        std::ifstream file("GDH/content.json");

        if (!file.is_open()) {
            Console::WriteLn("Failed to open the file 'content.json'");
            return false;
        }

        std::string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        content = json::parse(file_content);        
        Console::WriteLn("The content.json file has been successfully parsed");

        gui::license_accepted = content.value("license_accepted", false);

        Console::WriteLn("Loading hacks...\n");

        int success = 0, skip = 0, fail = 0;

        for (auto &item : Content::content["Hacks"].items()) {
            auto &components = item.value()["components"];

            for (auto &component : components) {
                std::string type = component["type"];

                if (type == "checkbox_hack") {
                    std::string name = component["name"];
                    json &opcodes = component["opcodes"];
                    Console::WriteF("%s\n", name.c_str());

                    for (auto &opcode : opcodes) {
                        if (opcode["pattern"].is_null()) {
                            Console::WriteLn("- No pattern, skipping...\n");
                            skip++;
                            continue;
                        }

                        HMODULE base = GetModuleHandleA(0);

                        if (!opcode["lib"].is_null()) {
                            base = GetModuleHandleA(std::string(opcode["lib"]).c_str());
                        }

                        MODULEINFO moduleInfo;

                        if (GetModuleInformation(GetCurrentProcess(), base, &moduleInfo, sizeof(MODULEINFO))) {
                            std::string pattern = opcode["pattern"];
                            Console::WriteF("- Pattern: %s\n", pattern.c_str());
                            Console::WriteLn("- Searching offset by pattern");
                            uintptr_t address = memory::PatternScan(reinterpret_cast<uintptr_t>(moduleInfo.lpBaseOfDll), moduleInfo.SizeOfImage, pattern);

                            if (address != 0) {
                                Console::WriteF("- Success! (%p)\n", (void *)address);
                                address -= uintptr_t(moduleInfo.lpBaseOfDll);
                                char buffer_offset[10];
                                sprintf_s(buffer_offset, "%p", (void *)address);
                                opcode["address"] = std::string(buffer_offset);

                                size_t opcodeOnSize = memory::CountBytesInHexStr(opcode["on"]);
                                Console::WriteLn("- Updating original bytes\n");
                                std::string opcode_off = memory::ReadMemoryHexStr(DWORD(base) + address, opcodeOnSize);
                                opcode["off"] = opcode_off;
                                success++;
                            } else {
                                Console::WriteLn("Pattern not found (Hack can be broken)\n");
                                fail++;
                            }
                        }
                    }
                }
            }
        }

        /*I had to separate the patch function because if you
        apply a hack while the pattern scan is in progress,
        the pattern scanner breaks due to the patched bytes*/

        for (auto &item : Content::content["Hacks"].items()) {
            auto &components = item.value()["components"];

            for (auto &component : components) {
                std::string type = component["type"];

                if (type == "checkbox_hack") {                    
                    json &opcodes = component["opcodes"];

                    for (auto &opcode : opcodes) {
                        if (component["enabled"]) {
                            std::string addrStr = opcode["address"];
                            uintptr_t address;
                            sscanf_s(addrStr.c_str(), "%x", &address);
                            DWORD base = (DWORD)GetModuleHandleA(0);

                            if (!opcode["lib"].is_null()) {
                                base = (DWORD)GetModuleHandleA(std::string(opcode["lib"]).c_str());
                            }

                            if (address != 0) {
                                memory::WriteBytes(base + address, opcode["on"]);
                            }
                        }
                    }
                }                
                else if (type == "transitionCustomizerCBX") {
                    auto index = component["index"];
                    hooks::transitionSelect = index;
                }
            }
        }

        Console::WriteF("success: %i; skip: %i, fail: %i; loaded and updated opcodes\n", success, skip, fail);

        return true;
    } catch (const json::parse_error e) {
        Console::WriteF("JSON parse error: %s\n", e.what());
        return false;
    } catch (const std::exception e) {
        Console::WriteF("Error: %s\n", e.what());
        return false;
    }
}


void Content::save() {
    if (Content::content.is_null()) {
        return;
    }

    std::ofstream file("GDH/content.json");
    file << std::setw(4) << Content::content;
    file.close();
}