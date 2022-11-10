#include "universalTab.h"

bool universalTab::universalInit = false;
vector<bool> universalTab::universalConfig;
json universalTab::universalData;

namespace universalTab {
    void universalTab::init() {
        fstream file("GDH\\universal.json");
        if (!file.is_open())
            return;	
        string file_content;
        getline(file, file_content, '\0');
        file.close();
        universalData = json::parse(file_content)["hacks"];

        fstream fileConfig("GDH\\universal.data", std::ios::in);
        if (!fileConfig.is_open()) {
            for (int i = 0; i < (int)universalData.size(); i++) {
                universalConfig.push_back(false);
            }
        }
        else {
            string line;
            for (int i = 0; i < (int)universalData.size(); i++) {
                getline(fileConfig, line);
                universalConfig.push_back(uselessShit::stringToBool(line));
                if (uselessShit::stringToBool(line)) {
                    json current_obj = universalData.at(i);
                    json opcodes = current_obj["opcodes"];
                    for (int j = 0; j < (int)opcodes.size(); j++) {
                        json opcode = opcodes.at(j);
                        uintptr_t address;
                        sscanf_s(string(opcode["addr"]).data(), "%x", &address);
                        string str_bytes(opcode["on"]);
                        str_bytes.push_back(' ');
                        vector<uint8_t> bytes_to_write;
                        string current_byte("");
                        for (int h = 0; h < (int)str_bytes.size(); h++) {
                            if (str_bytes.at(h) == ' ') {
                                int cur_bt;
                                sscanf_s(string("0x" + current_byte).data(), "%x", &cur_bt);
                                bytes_to_write.push_back(cur_bt);
                                current_byte.clear();
                                continue;
                            }
                            current_byte.push_back(str_bytes.at(h));
                        }
                        DWORD base = gd::base;
                        if (!opcode["lib"].is_null() && string(opcode["lib"]) == "libcocos2d.dll") {
                            base = (DWORD)GetModuleHandleA("libcocos2d.dll");
                        }
                        uint32_t addr = base + address;
                        Memory::MemoryWriteBytes(addr, bytes_to_write);                
                    }
                }
            }
        }
    }

    void universalTab::draw() {
        ImGui::Begin("Universal", 0, ImGuiWindowFlags_NoCollapse);
        if (!universalInit) {
            ImGui::SetWindowPos(ImVec2(610 * uselessShit::overlaysize, 10 * uselessShit::overlaysize));
            ImGui::SetWindowSize(ImVec2(190 * uselessShit::overlaysize, 300 * uselessShit::overlaysize));
            ImGui::SetWindowFontScale(uselessShit::overlaysize);
            universalInit = true;
        }        
        for (int i = 0; i < (int)universalData.size(); i++) {
            json current_obj = universalData.at(i);
            bool current = universalConfig[i];
            if (ImGui::Checkbox(string(current_obj["name"]).c_str(), &current)) {
                universalConfig[i] = current;
                json opcodes = current_obj["opcodes"];
                for (int j = 0; j < (int)opcodes.size(); j++) {
                    json opcode = opcodes.at(j);
                    uintptr_t address;
                    sscanf_s(string(opcode["addr"]).data(), "%x", &address);
                    string str_bytes(opcode[current ? "on" : "off"]);
                    str_bytes.push_back(' ');
                    vector<uint8_t> bytes_to_write;
                    string current_byte("");
                    for (int h = 0; h < (int)str_bytes.size(); h++) {
                        if (str_bytes.at(h) == ' ') {
                            int cur_bt;
                            sscanf_s(string("0x" + current_byte).data(), "%x", &cur_bt);
                            bytes_to_write.push_back(cur_bt);
                            current_byte.clear();
                            continue;
                        }
                        current_byte.push_back(str_bytes.at(h));
                    }
                    DWORD base = gd::base;
                    if (!opcode["lib"].is_null() && string(opcode["lib"]) == "libcocos2d.dll") {
                        base = (DWORD)GetModuleHandleA("libcocos2d.dll");
                    }
                    uint32_t addr = base + address;
                    Memory::MemoryWriteBytes(addr, bytes_to_write);
                }
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip(string(current_obj["desc"]).c_str());
        }
        ImGui::End();


    }

    void universalTab::close() {
        fstream file("GDH\\universal.data", std::ios::out);
        if (file.is_open()) {
            for (int i = 0; i < (int)universalConfig.size(); i++) {
                file << uselessShit::boolToString(universalConfig[i]) << "\n";
            }
        }
        file.close();
    }
}