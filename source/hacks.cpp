#include "hacks.hpp"
#include <filesystem>

json hacks::content;

std::filesystem::path get_game_path()
{
    wchar_t path_str[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, path_str, MAX_PATH);
    const std::filesystem::path path(path_str);
    return path.parent_path();
}

std::filesystem::path get_gdh_path()
{
    const auto path = get_game_path() / "GDH/";
    // make sure it exists
    try {
        if (!std::filesystem::exists(path))
        {
            std::filesystem::create_directory(path);
        }
    } catch (...) {}
    return path;
}

// move hacks.json from game directory to GDH directory if it exists
void move_hacks_file()
{   
    const auto orig_path = get_game_path() / "hacks.json";
    try {
        if (std::filesystem::exists(orig_path)) {
            std::filesystem::rename(orig_path, get_gdh_path() / "hacks.json");
        }
    } catch (...) {}
}

void hacks::load()
{
    move_hacks_file();
    ifstream file(get_gdh_path() / "hacks.json");
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

void hacks::inject_extensions() {
    // create GDH/extensions folder if it doesn't exist
    const auto extensions_path = get_gdh_path() / "extensions";
    try {
        if (!std::filesystem::exists(extensions_path))
        {
            std::filesystem::create_directory(extensions_path);
        }
    } catch (...) {}

    // load all dlls from it
    for (const auto& file : std::filesystem::directory_iterator(extensions_path))
	{
        if (file.path().extension() == ".dll")
        {
		    LoadLibrary(file.path().generic_string().c_str());
        }
	}
}
