#include <string>
#include <vector>
#include "memory.hpp"

struct opcode {    
    std::string pattern;
    std::string on;
    std::string module = "";

    std::string off;
    uintptr_t address;
};

struct hack {
    std::string name;
    std::string desc;
    std::vector<opcode> opcodes; 
    bool enabled = false;
};

struct window {
    std::string name;
    float x, y;
    float w, h;

    std::vector<hack> hacks;
};

namespace hacks {
    static const auto base = reinterpret_cast<uintptr_t>(GetModuleHandleA(0));
	static const auto cocos_base = reinterpret_cast<HMODULE>(GetModuleHandleA("libcocos2d.dll"));

    extern std::vector<window> windows;
    void init();

    extern bool unlock_items;
    extern bool ignore_esc;
    extern bool instant_complate;

    extern bool fps_enabled;
    extern float fps_value;
    
    extern float speed_value;

    void update_framerate();

    inline void serializeString(std::ofstream& ofs, const std::string& str) {
        size_t length = str.size();
        ofs.write(reinterpret_cast<const char*>(&length), sizeof(length));
        ofs.write(str.data(), length);
    }

    inline void deserializeString(std::ifstream& ifs, std::string& str) {
        size_t length;
        ifs.read(reinterpret_cast<char*>(&length), sizeof(length));
        str.resize(length);
        ifs.read(&str[0], length);
    }

    inline void saveWindows(const std::string& filename, const std::vector<window>& windows) {
        std::ofstream ofs(filename, std::ios::binary);
        if (!ofs.is_open()) {
            return;
        }

        size_t windowsSize = windows.size();
        ofs.write(reinterpret_cast<const char*>(&windowsSize), sizeof(windowsSize));
        for (const auto& win : windows) {
            serializeString(ofs, win.name);
            ofs.write(reinterpret_cast<const char*>(&win.x), sizeof(win.x));
            ofs.write(reinterpret_cast<const char*>(&win.y), sizeof(win.y));
            ofs.write(reinterpret_cast<const char*>(&win.w), sizeof(win.w));
            ofs.write(reinterpret_cast<const char*>(&win.h), sizeof(win.h));

            size_t hacksSize = win.hacks.size();
            ofs.write(reinterpret_cast<const char*>(&hacksSize), sizeof(hacksSize));
            for (const auto& hck : win.hacks) {
                serializeString(ofs, hck.name);
                serializeString(ofs, hck.desc);
                ofs.write(reinterpret_cast<const char*>(&hck.enabled), sizeof(hck.enabled));

                size_t opcodesSize = hck.opcodes.size();
                ofs.write(reinterpret_cast<const char*>(&opcodesSize), sizeof(opcodesSize));
                for (const auto& opc : hck.opcodes) {
                    serializeString(ofs, opc.pattern);
                    serializeString(ofs, opc.on);
                    serializeString(ofs, opc.module);
                    serializeString(ofs, opc.off);
                    ofs.write(reinterpret_cast<const char*>(&opc.address), sizeof(opc.address));
                }
            }
        }

        ofs.close();
    }

    inline void loadWindows(const std::string& filename, std::vector<window>& windows) {
        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs.is_open()) {
            return;
        }

        size_t windowsSize;
        ifs.read(reinterpret_cast<char*>(&windowsSize), sizeof(windowsSize));
        windows.resize(windowsSize);
        for (auto& win : windows) {
            deserializeString(ifs, win.name);
            ifs.read(reinterpret_cast<char*>(&win.x), sizeof(win.x));
            ifs.read(reinterpret_cast<char*>(&win.y), sizeof(win.y));
            ifs.read(reinterpret_cast<char*>(&win.w), sizeof(win.w));
            ifs.read(reinterpret_cast<char*>(&win.h), sizeof(win.h));

            size_t hacksSize;
            ifs.read(reinterpret_cast<char*>(&hacksSize), sizeof(hacksSize));
            win.hacks.resize(hacksSize);
            for (auto& hck : win.hacks) {
                deserializeString(ifs, hck.name);
                deserializeString(ifs, hck.desc);
                ifs.read(reinterpret_cast<char*>(&hck.enabled), sizeof(hck.enabled));

                size_t opcodesSize;
                ifs.read(reinterpret_cast<char*>(&opcodesSize), sizeof(opcodesSize));
                hck.opcodes.resize(opcodesSize);
                for (auto& opc : hck.opcodes) {
                    deserializeString(ifs, opc.pattern);
                    deserializeString(ifs, opc.on);
                    deserializeString(ifs, opc.module);
                    deserializeString(ifs, opc.off);
                    ifs.read(reinterpret_cast<char*>(&opc.address), sizeof(opc.address));
                }
            }
        }

        ifs.close();
    }
}