#include <Windows.h>
#include "content.hpp"
#include "hooks.hpp"
#include "console.hpp"
#include "memory.hpp"
#include "gui.hpp"
#include "speedhackAudio.hpp"
#include "labels.hpp"

json Content::content;

bool Content::load(std::string path) {
    try {
        std::ifstream file(path);

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
                else if (type == "scale_gui") {
                    int index = component["index"];
                    const char* items[] = {"25%", "50%", "75%", "100%", "125%", "150%", "175%", "200%", "250%", "300%", "400%"};
                    gui::scalePercentage = float(atof(items[index])) / 100.0f;
                    ImGuiHook::scale = gui::scalePercentage;   
                }
                else if (type == "checkbox_discord_rpc") {
                    bool enabled = component["enabled"];
                    hacks::discord_rpc = enabled;
                }                
                else if (type == "checkbox_smart_startpos") {
                    bool enabled = component["enabled"];
                    hacks::smart_startpos = enabled;
                }
                else if (type == "checkbox_show_hitboxes") {
                    bool enabled = component["enabled"];
                    bool show_on_death = component["show_on_death"];

                    bool draw_trail_enabled = component["draw_trail"];
                    bool default_circle_hitboxes_size_enabled = component["default_circle_hitboxes_size"];

                    int border_alpha = component["border_alpha"];
                    int fill_alpha = component["fill_alpha"];
                    int circle_border_alpha = component["circle_border_alpha"];
                    int circle_fill_alpha = component["circle_fill_alpha"];

                    float hitbox_size = component["hitboxes_size"];
                    float circle_hitboxes_size = component["circle_hitboxes_size"];

                    hacks::show_hitboxes = enabled;
                    hacks::draw_trail = draw_trail_enabled;
                    hacks::border_alpha = border_alpha;
                    hacks::fill_alpha = fill_alpha;
                    hacks::circle_border_alpha = circle_border_alpha;
                    hacks::circle_fill_alpha = circle_fill_alpha;
                    hacks::hitbox_size = hitbox_size;
                    hacks::circle_hitbox_size = circle_hitboxes_size;
                    hacks::default_hitbox_size = default_circle_hitboxes_size_enabled;

                    hitboxes_mode::enabled_hitbox(enabled);
                    hitboxes_mode::enabled_draw_trail(draw_trail_enabled);
                }
                else if (type == "checkbox_instant_complete") {
                    bool enabled = component["enabled"];
                    hacks::instant_complate = enabled;
                }      
                else if (type == "checkbox_layout_mode") {
                    bool enabled = component["enabled"];
                    layout_mode::set_enabled(enabled);
                }                
                else if (type == "checkbox_startpos_switcher") {
                    bool enabled = component["enabled"];
                    bool use_a_d_enabled = component["use_a_d"];
                    startPosSwitcher::enabled = enabled;
                    startPosSwitcher::use_a_d = use_a_d_enabled;
                }               
                else if (type == "checkbox_auto_pickup_coins") {
                    bool enabled = component["enabled"];
                    hacks::auto_pickup_coins = enabled;
                }
                else if (type == "checkbox_auto_ldm") {
                    bool enabled = component["enabled"];
                    hacks::auto_ldm = enabled;
                }                         
                else if (type == "checkbox_auto_song_downloader") {
                    bool enabled = component["enabled"];
                    hacks::auto_song_downloader = enabled;
                }                                        
                else if (type == "checkbox_auto_practice_mode") {
                    bool enabled = component["enabled"];
                    hacks::auto_practice_mode = enabled;
                }
                else if (type == "fps_multiplier") {
                    bool enabled = component["tps_enabled"];
                    hacks::tps_enabled = enabled;

                    float tps_value = component["tps_value"];
                    hacks::tps = tps_value;
                }
                else if (type == "speed") {
                    float value = component["value"];
                    hacks::speed = value;

                    bool audio = component["audio"];
                }
                else if (type == "transition_customizer") {
                    bool enabled = component["enabled"];
                    hacks::transition_enabled = enabled;

                    int index = component["index"];
                    hacks::transition_index = index;

                    float duration = component["duration"];
                    hacks::transition_duration = duration;
                }
                else if (type == "checkbox_hide_player") {
                    bool enabled = component["enabled"];
                    hacks::hide_player_enabled = enabled;

                    bool first_player = component["first_player"];
                    hacks::hide_player_p1 = first_player;

                    bool second_player = component["second_player"];
                    hacks::hide_player_p2 = second_player;
                }
                else if (type == "checkbox_respawn_time") {
                    float duration = component["duration"];
                    hacks::respawnTime = duration;
                }
                else if (type == "labels") {
                    bool custom_text_enabled = component["custom_text_enabled"];
                    labels::custom_text_enabled = custom_text_enabled;

                    std::string custom_text = component["custom_text"];
                    labels::custom_text = custom_text;
                    
                    bool time_24hr = component["time_24hr"];
                    labels::time24_enabled = time_24hr;

                    bool time_12hr = component["time_12hr"];
                    labels::time12_enabled = time_12hr;
                    
                    bool noclip_accuracy = component["noclip_accuracy"];
                    labels::noclip_accuracy_enabled = noclip_accuracy;

                    bool cps_counter = component["cps_counter"];
                    labels::cps_counter_enabled = cps_counter;

                    bool death_counter = component["death_counter"];
                    labels::death_enabled = death_counter;

                    int pos = component["pos"];
                    labels::pos = pos;            
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