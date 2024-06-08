#include "includes.hpp"
#include "gui.hpp"
#include "memory.hpp"
#include "console.hpp"
#include "hacks.hpp"
#include "hooks.hpp"

extern "C" __declspec(dllexport) int __stdcall stub() { return 0; }

void CheckDir(std::string dir)
{
    if (!std::filesystem::is_directory(dir) || !std::filesystem::exists(dir))
    {
        std::filesystem::create_directory(dir);
    }
}

void Main()
{
    CheckDir("GDH");
    CheckDir("GDH/macros");
    //CheckDir("GDH/recorder");

    //Console::Init();

    hacks::loadWindows("gdh_data.dat", hacks::windows);

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
        hacks::saveWindows("gdh_data.dat", hacks::windows);
    }
    return true;
}
