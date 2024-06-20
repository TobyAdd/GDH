#include "includes.hpp"
#include "gui.hpp"
#include "memory.hpp"
#include "console.hpp"
#include "hacks.hpp"
#include "hooks.hpp"
#include <fstream>

extern "C" __declspec(dllexport) int __stdcall stub() { return 0; }

void CheckDir(const std::filesystem::path &path)
{
    if (!std::filesystem::is_directory(path) || !std::filesystem::exists(path))
    {
        std::filesystem::create_directory(path);
    }
}

void Main()
{
    CheckDir(hacks::folderPath);
    CheckDir(hacks::folderMacroPath);
    //CheckDir("GDH/recorder");

    hacks::load(hacks::fileDataPath, hacks::windows);

    //Console::Init();
    
    hacks::init();
    hacks::update_framerate();
    
    MH_Initialize();
    hooks::init();

    ImGuiHook::Load([](void *target, void *hook, void **trampoline)
                            { MH_CreateHook(target, hook, trampoline); });

    ImGuiHook::setRenderFunction(gui::RenderMain);
    ImGuiHook::setUnloadFunction(gui::Unload);

    ImGuiHook::setKeyPressHandler([](int keyCode)
    {
        switch (keyCode)
        {
        case VK_TAB:
            gui::Toggle();
            break;
        }
    });

    MH_EnableHook(MH_ALL_HOOKS);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        Main();
    }
    else if (fdwReason == DLL_PROCESS_DETACH) {
        hacks::save(hacks::windows, hacks::fileDataPath);
    }
    return true;
}
