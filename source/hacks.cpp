#include "hacks.hpp"

bool hacks::unlock_items = false;
bool hacks::ignore_esc = false;
bool hacks::instant_complate = false;

bool hacks::fps_enabled = true;
float hacks::fps_value = 240.f;

float hacks::speed_value = 1.f;
std::vector<window> hacks::windows = {
    {"Core", 10, 10, 200, 200, 
        {
            {"Free Window Resize", "Allows free window resizing",
                {
                    {"e8 ? ? ? ? b8 ? ? ? ? e9 ? ? ? ? 83 be", "90 90 90 90 90", "libcocos2d.dll"},
                    {"ff 15 ? ? ? ? 48 8b 74 24 ? 48 8b 5c 24", "90 90 90 90 90 90", "libcocos2d.dll"},
                    {"ff 15 ? ? ? ? 48 8b 8c 24 ? ? ? ? 48 33 cc e8 ? ? ? ? 48 81 c4", "90 90 90 90 90 90", "libcocos2d.dll"}
                }
            },
            {"Noclip", "The player will be invincible to obstacles",
                {
                    {"0f 85 ? ? ? ? 80 b9 ? ? ? ? ? 0f 85 ? ? ? ? 4d 85 c0", "90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90"}
                }
            },
            {"Unlock Items", "The following elements will be unlocked:\n- Icons + Colors\n- Practice Music Sync\n- Music Unlocker"},
            {"No Respawn Blink", "Upon respawning, the cube will not produce an unpleasant flicker",
                {
                    {"0f 85 ? ? ? ? ba ? ? ? ? ff 15 ? ? ? ? ba", "E9 F3 00"}
                }
            },
            {"No Death Effect", "Upon death, the cube will not emit an exploding effect",
                {
                    {"e8 ? ? ? ? 80 bf ? ? ? ? ? 74 ? 48 8b 9f ? ? ? ? 8b 83", "90 90 90 90 90"}
                }
            },
            {"Safe Mode", "Disables progress on levels",
                {
                    {"0f 85 ? ? ? ? 40 38 b7 ? ? ? ? 0f 85 ? ? ? ? 0f b6 97", "E9 B4 01 00 00 90"},
                    {"0f 85 ? ? ? ? 80 bf ? ? ? ? ? 0f 85 ? ? ? ? e8", "E9 78 03 00 00 90"}
                }
            }
        }
    },
    {"Bypass", 10, 220, 200, 150, 
        {  
            {"Text Lenght", "Removes the limit that deletes previous checkpoints after the 50th checkpoint",
                {
                    {"74 ? 41 8b 85", "90 90"},
                    {"7c ? 85 c0 7f", "EB"}
                }
            },
            {"Slider Limit", "Removes the limit on sliding beyond set limits",
                {
                    {"76 ? 84 c0 74 ? 0f 57 c9 eb", "EB"},
                    {"76 ? 84 c0 74 ? 0f 57 c9 0f 28 d0", "EB"},
                    {"77 ? f3 0f 10 83 ? ? ? ? 0f 28 ce", "90 90"},
                    {"77 ? 84 c9 74 ? 41 0f 28 cb", "90 90"},
                    {"f3 0f 10 3d ? ? ? ? 45 0f 57 db", "0F 57 FF 90 90 90 90 90"},
                    {"0f 87 ? ? ? ? 0f 2f c6 0f 87 ? ? ? ? f3 0f 59 83 ? ? ? ? 0f 2e 83 ? ? ? ? 7a ? 0f 84 ? ? ? ? 83 ff", "0F 84"},
                    {"0f 87 ? ? ? ? f3 0f 59 83 ? ? ? ? 0f 2e 83 ? ? ? ? 7a ? 0f 84 ? ? ? ? 83 ff", "90 90 90 90 90 90"},
                    {"0f 87 ? ? ? ? 0f 2f c6 0f 87 ? ? ? ? f3 0f 59 83 ? ? ? ? 0f 2e 83 ? ? ? ? 7a ? 0f 84 ? ? ? ? 49 8b ce", "0F 84"},
                    {"0f 87 ? ? ? ? f3 0f 59 83 ? ? ? ? 0f 2e 83 ? ? ? ? 7a ? 0f 84 ? ? ? ? 49 8b ce", "90 90 90 90 90 90"},
                }
            },
            {"Character Filter", "Lets you input any character in all text inputs",
                {
                    {"75 ? 48 8d 4d ? 48 83 7d ? ? 48 0f 47 4d ? 48 89 5c 24", "90 90"}
                }
            },            
            {"Checkpoint Limit", "Removes the limit that deletes previous checkpoints after the 50th checkpoint",
                {
                    {"76 ? 48 8b 8f ? ? ? ? ff 15 ? ? ? ? 85 c0", "EB"}
                }
            }
        }
    },
    {"Player", 220, 10, 200, 410, 
        {
            {"Allow Low Volume", "Removes the limit on minimum volume percentage",
                {
                    {"76 ? 0f 57 f6 48 8b 05 ? ? ? ? 48 85 c0 75 ? b9 ? ? ? ? e8 ? ? ? ? 48 89 44 24 ? 48 8b c8 e8 ? ? ? ? 90 48 89 05 ? ? ? ? 48 8b 10 48 8b c8 ff 52 ? 48 8b 05 ? ? ? ? f3 44 0f 10 80", "EB"},
                    {"76 ? 0f 57 f6 48 8b 05 ? ? ? ? 48 85 c0 75 ? b9 ? ? ? ? e8 ? ? ? ? 48 89 44 24 ? 48 8b c8 e8 ? ? ? ? 90 48 89 05 ? ? ? ? 48 8b 10 48 8b c8 ff 52 ? 48 8b 05 ? ? ? ? f3 0f 11 b0", "EB"}
                }
            },            
            {"Anticheat Bypass", "Disables level kicking at level completion",
                {
                    {"0f 85 ? ? ? ? 80 bf ? ? ? ? ? 0f 85 ? ? ? ? 80 bf ? ? ? ? ? 0f 85 ? ? ? ? e8", "90 90 90 90 90 90"},
                    {"0f 85 ? ? ? ? 80 bf ? ? ? ? ? 0f 85 ? ? ? ? 80 bf ? ? ? ? ? 0f 85 ? ? ? ? 80 bf ? ? ? ? ? 0f 85 ? ? ? ? 48 8b 8f ? ? ? ? ff 15", "90 90 90 90 90 90"}
                }
            },          
            {"Fast Chest Open", "Removes the delay for opening chests",
                {
                    {"f3 0f 10 05 ? ? ? ? ff 15 ? ? ? ? 48 89 44 24", "0F 57 C0 90 90 90 90 90"}
                }
            },
            {"Ignore ESC", "Prevents exiting the level"},
            {"Instant Complete", "Instant level completion"},
            {"Solid Wave Trail", "Disables wave blending",
                {
                    {"75 ? 80 bb ? ? ? ? ? 75 ? 48 8b 83", "90 90"},
                    {"75 ? 48 8b 83 ? ? ? ? c6 80", "90 90"}
                }
            },
            {"All Modes Platformer", "Removes the limit on all modes in the platformer",
                {
                    {"0f 85 ? ? ? ? 4c 8b c7 49 8b d6 48 8b ce e8 ? ? ? ? 84 c0 0f 84 ? ? ? ? 4c 8b c7 49 8b d6 48 8b ce e8 ? ? ? ? ba ? ? ? ? 49 8b ce e8 ? ? ? ? 48 8b df 48 8b 07 48 8b cf ff 90 ? ? ? ? 48 8b d0 48 8d 4d ? ff 15 ? ? ? ? 49 8d 8e ? ? ? ? 48 8b d0 ff 15 ? ? ? ? 49 89 be ? ? ? ? 80 be ? ? ? ? ? 74 ? 48 8b 86 ? ? ? ? 48 85 c0 74 ? 48 8b d8 eb ? 48 85 db 74 ? 48 89 9e ? ? ? ? 80 bb ? ? ? ? ? 74 ? 41 b0 ? eb ? 45 33 c0 b2 ? 49 8b ce e8 ? ? ? ? 48 8b 06 b2 ? 48 8b ce ff 90 ? ? ? ? e9 ? ? ? ? 4c 8b c7 49 8b d6 48 8b ce e8 ? ? ? ? 84 c0 0f 84 ? ? ? ? 4c 8b c7 49 8b d6 48 8b ce e8 ? ? ? ? 45 33 c0 33 d2 49 8b ce e8 ? ? ? ? 45 33 c0 33 d2 49 8b ce e8 ? ? ? ? 45 33 c0 33 d2 49 8b ce e8 ? ? ? ? 45 33 c0 33 d2 49 8b ce e8 ? ? ? ? 45 33 c0 33 d2 49 8b ce e8 ? ? ? ? 45 33 c0 33 d2 49 8b ce e8 ? ? ? ? 48 8b df 48 8b 07 48 8b cf ff 90 ? ? ? ? 48 8b d0 48 8d 4d ? ff 15 ? ? ? ? 49 8d 8e ? ? ? ? 48 8b d0 ff 15 ? ? ? ? 49 89 be ? ? ? ? 80 be ? ? ? ? ? 74 ? 48 8b 86 ? ? ? ? 48 85 c0 74 ? 48 8b d8 eb ? 48 85 db 74 ? 48 89 9e ? ? ? ? 80 bb ? ? ? ? ? 74 ? 41 b0 ? 41 0f b6 d0 49 8b ce e8 ? ? ? ? e9 ? ? ? ? 45 33 c0 b2 ? 49 8b ce e8 ? ? ? ? e9 ? ? ? ? 4c 8b c7", "90 90 90 90 90 90"},
                    {"0f 85 ? ? ? ? 4c 8b c7 49 8b d6 48 8b ce e8 ? ? ? ? 84 c0 0f 84 ? ? ? ? 4c 8b c7 49 8b d6 48 8b ce e8 ? ? ? ? ba ? ? ? ? 49 8b ce e8 ? ? ? ? 48 8b df 48 8b 07 48 8b cf ff 90 ? ? ? ? 48 8b d0 48 8d 4d ? ff 15 ? ? ? ? 49 8d 8e ? ? ? ? 48 8b d0 ff 15 ? ? ? ? 49 89 be ? ? ? ? 80 be ? ? ? ? ? 74 ? 48 8b 86 ? ? ? ? 48 85 c0 74 ? 48 8b d8 eb ? 48 85 db 74 ? 48 89 9e ? ? ? ? 80 bb ? ? ? ? ? 74 ? 41 b0 ? eb ? 45 33 c0 b2 ? 49 8b ce e8 ? ? ? ? 48 8b 06 b2 ? 48 8b ce ff 90 ? ? ? ? e9 ? ? ? ? 4c 8b c7 49 8b d6 48 8b ce e8 ? ? ? ? 84 c0 0f 84 ? ? ? ? 4c 8b c7 49 8b d6 48 8b ce e8 ? ? ? ? 45 33 c0 33 d2 49 8b ce e8 ? ? ? ? 45 33 c0 33 d2 49 8b ce e8 ? ? ? ? 45 33 c0 33 d2 49 8b ce e8 ? ? ? ? 45 33 c0 33 d2 49 8b ce e8 ? ? ? ? 45 33 c0 33 d2 49 8b ce e8 ? ? ? ? 45 33 c0 33 d2 49 8b ce e8 ? ? ? ? 48 8b df 48 8b 07 48 8b cf ff 90 ? ? ? ? 48 8b d0 48 8d 4d ? ff 15 ? ? ? ? 49 8d 8e ? ? ? ? 48 8b d0 ff 15 ? ? ? ? 49 89 be ? ? ? ? 80 be ? ? ? ? ? 74 ? 48 8b 86 ? ? ? ? 48 85 c0 74 ? 48 8b d8 eb ? 48 85 db 74 ? 48 89 9e ? ? ? ? 80 bb ? ? ? ? ? 74 ? 41 b0 ? 41 0f b6 d0 49 8b ce e8 ? ? ? ? e9 ? ? ? ? 45 33 c0 b2 ? 49 8b ce e8 ? ? ? ? e9 ? ? ? ? 80 be", "90 90 90 90 90 90"}
                }
            },
            {"Force Platformer", "Enables platformer mode in all levels",
                {
                    {"0f b6 90 ? ? ? ? 48 8b 81", "B2 01 90 90 90 90 90"},
                    {"74 ? 49 8b 80", "EB"}
                }
            },
            {"Hide Pause Button", "Removes the pause button when the cursor is shown",
                {
                    {"ff 15 ? ? ? ? 0f 28 f0 f3 0f 10 3d ? ? ? ? f3 0f 5c f7 48 8b 0d", "90 90 90 90 90 90"}
                }
            },
            {"No Camera Move", "Disables camera movement via trigger",
                {
                    {"0f 8f ? ? ? ? 0f 84 ? ? ? ? 81 e9 ? ? ? ? 0f 84 ? ? ? ? 83 e9 ? 74 ? 83 f9 ? 0f 85 ? ? ? ? 8b 83", "E9 20 03 00"}
                }
            },
            {"No Camera Zoom", "Disables camera movement via trigger",
                {
                    {"41 8b 86 ? ? ? ? 89 44 24 ? 41 8b 86 ? ? ? ? 89 44 24 ? f3 41 0f 10 86 ? ? ? ? f3 0f 11 44 24 ? 45 8b 8e ? ? ? ? f3 41 0f 10 96 ? ? ? ? f3 41 0f 10 8e ? ? ? ? 49 8b cf e8 ? ? ? ? e9 ? ? ? ? 49 8b d6", "EB 44 90 90 90 90"}
                }
            },
            {"No Particles", "Disables resuming the particle system",
                {
                    {"74 ? 44 0f b6 c7 48 8b d6 48 8b cb e8 ? ? ? ? 48 8b cb", "EB", "libcocos2d.dll"}
                }
            },
            {"No Short Numbers", "All numbers are displayed in full\n(For example, \"1.5M\" becomes \"1500000\")",
                {
                    {"7d ? 8b d3 48 8d 4c 24", "90 90"}
                }
            },
            {"No BG Flash", "Removes the unpleasant flicker when triggering the portal",
                {
                    {"0f 85 ? ? ? ? 41 81 fd ? ? ? ? 75", "E9 ED 01 00 00 90"},
                    {"0f 84 ? ? ? ? 48 8b 8e ? ? ? ? b2 ? 48 8b 01 ff 90 ? ? ? ? ff 15", "E9 88 00 00 00 90"}
                }
            },
            {"No Glow", "Disables glow on objects",
                {
                    {"0f 85 ? ? ? ? 48 8b 05 ? ? ? ? 48 85 c0 75 ? b9 ? ? ? ? e8 ? ? ? ? 48 89 45 ? 48 8b c8 e8 ? ? ? ? 90 48 89 05 ? ? ? ? 48 8b 10 48 8b c8 ff 52 ? 48 8b 05 ? ? ? ? 80 b8 ? ? ? ? ? 74 ? 80 bf", "E9 36 03 00 00 90"}
                }
            },//0f 85 ? ? ? ? 48 8d 15 ? ? ? ? 48 8d 4c 24 ? ff 15 ? ? ? ? 48 8b d8 48 8d 54 24 ? 48 8d 4c 24 ? ff 15 ? ? ? ? 48 8b c8 f3 0f 10 54 24
            {"No Mirror", "Disables level mirroring",
                {
                    {"0f 85 ? ? ? ? 4c 8b c7 49 8b d6 48 8b ce e8 ? ? ? ? 84 c0 0f 84 ? ? ? ? 48 8b 07 48 8b cf ff 90 ? ? ? ? 48 8b d0 48 8d 4d ? ff 15 ? ? ? ? 49 8d 8e ? ? ? ? 48 8b d0 ff 15 ? ? ? ? 49 89 be ? ? ? ? 44 0f b6 87 ? ? ? ? b2", "E9 E9 02 00 00 90"}
                }
            },
            {"No Portal Lighting", "Disables lightning when entering mini/large portal",
                {
                    {"0f 85 ? ? ? ? 48 8d 15 ? ? ? ? 48 8d 4c 24 ? ff 15 ? ? ? ? 48 8b d8 48 8d 54 24 ? 48 8d 4c 24 ? ff 15 ? ? ? ? 48 8b c8 f3 0f 10 54 24", "E9 A2 00 00 00 90"}
                }
            }
        }
    },
    {"Creator", 430, 10, 210, 200, 
        {
            {"Copy Hack", "Copy any online level without a password",
                {
                    {"0f 8f ? ? ? ? 41 8b 85", "E9 F6 00 00 00 90"}
                }
            },
            {"Custom Object Bypass", "Removes the limit restricted to 1000 objects",
                {
                    {"0f 84 ? ? ? ? ff 15 ? ? ? ? 3d", "E9 51 01 00 00 90"},
                    {"0f 87 ? ? ? ? 48 8d 45", "90 90 90 90 90 90"}
                }
            },
            {"Scale Snap Bypass", "Removes the slider snapping when stretched from 0.97 to 1.03",
                {
                    {"76 ? f3 0f 10 0d ? ? ? ? 0f 2f c8", "EB"}
                }
            },
            {"Verify Hack", "Publish a level without verification",
                {
                    {"0f 84 ? ? ? ? 80 be ? ? ? ? ? 0f 85 ? ? ? ? 0f 57 c0", "E9 47 01 00 00 90"}
                }
            },
            {"Smooth Editor Trail", "Makes the wave smoother in the editor",
                {
                    {"0f 82 ? ? ? ? 48 8b 8f ? ? ? ? 4c 89 74 24", "90 90 90 90 90 90"}
                }
            },
            {"Level Edit", "Edit any online level",
                {
                    {"0f 85 ? ? ? ? 48 8d 0d ? ? ? ? ff 15 ? ? ? ? 4c 8b f8 48 8b 38", "90 90 90 90 90 90"},
                    {"4c 0f 44 c8", "49 89 C1 90"},
                    {"0f 85 ? ? ? ? ff 15 ? ? ? ? 48 8b 10 48 8b c8 ff 52", "90 90 90 90 90 90"},
                    {"0f 87 ? ? ? ? 48 63 c2 4c 8d 05 ? ? ? ? 41 8b 94 80 ? ? ? ? 49 03 d0 ff e2 e8", "E9 A8 01 00 00 90"}
                }
            }
        }
    },
    {"Framerate", 430, 220, 210, 100},
    {"Replay Engine", 650, 10, 300, 200}
};

uintptr_t framerate_address = 0;
void hacks::update_framerate() {
    if (framerate_address == 0) {
        MODULEINFO moduleInfo;
        if (GetModuleInformation(GetCurrentProcess(), (HMODULE)hacks::base, &moduleInfo, sizeof(MODULEINFO))) {
            framerate_address = memory::PatternScan(reinterpret_cast<uintptr_t>(moduleInfo.lpBaseOfDll), moduleInfo.SizeOfImage, "89 88 ? ? ? ? a3");
        }  
    }
    if (framerate_address != 0) {
        memory::WriteFloat(framerate_address, 1.f / hacks::fps_value);
    }        
}

void hacks::init() {
    for (auto& win : hacks::windows) {
        for (auto& hck : win.hacks) {
            for (auto& opc : hck.opcodes) {
                uintptr_t base = hacks::base;

                if (!opc.module.empty()) {
                    base = (uintptr_t)GetModuleHandleA(opc.module.c_str());
                }

                MODULEINFO moduleInfo;

                if (GetModuleInformation(GetCurrentProcess(), (HMODULE)base, &moduleInfo, sizeof(MODULEINFO))) {
                    opc.address = memory::PatternScan(reinterpret_cast<uintptr_t>(moduleInfo.lpBaseOfDll), moduleInfo.SizeOfImage, opc.pattern);
                    if (opc.address != 0) {
                        opc.address -= uintptr_t(moduleInfo.lpBaseOfDll);
                        size_t opcode_size = memory::CountBytesInHexStr(opc.on);
                        opc.off = memory::ReadMemoryHexStr(base + opc.address, opcode_size);
                    }
                }
            }
        }

        for (auto& hck : win.hacks) {
            if (hck.enabled) {
                if (hck.name == "Unlock Items") { hacks::unlock_items = hck.enabled; }
                else if (hck.name == "Ignore ESC") { hacks::ignore_esc = hck.enabled; }
                else if (hck.name == "Instant Complete") { hacks::instant_complate = hck.enabled; }
                else {
                    for (auto& opc : hck.opcodes) {
                        std::string bytesStr = hck.enabled ? opc.on : opc.off;
                        if (opc.address != 0) {
                            uintptr_t base = (uintptr_t)GetModuleHandleA(0);
                            if (!opc.module.empty())
                            {
                                base = (uintptr_t)GetModuleHandleA(opc.module.c_str());
                            }
                            memory::WriteBytes(base + opc.address, bytesStr);
                        }                
                    }
                }
            }
        }
    }
}