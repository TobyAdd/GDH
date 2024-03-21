#include "includes.hpp"
#include "gui.hpp"
#include "content.hpp"
#include "hooks.hpp"
#include "speedhackAudio.hpp"
#include "crash-handler.hpp"
#include "cps_counter.hpp"

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
    CheckDir("GDH/recorder");

    MH_Initialize();
    crashHandler::init();
    Console::Init();
    if (!Content::load("GDH/content.json")) {
        Console::WriteLn("Failed to parse content.json");
        return;
    }

    hacks::update_framerate();
    hacks::init_custom_respawn_time();
    SpeedhackAudio::speed = hacks::speed;
    
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
    ImGuiHook::Load([](void *target, void *hook, void **trampoline)
                            { MH_CreateHook(target, hook, trampoline); });

    hooks::init();
    
    MH_EnableHook(MH_ALL_HOOKS);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH) {
        Main();
    }
    else if (fdwReason == DLL_PROCESS_DETACH){
        Content::save();
    }
    return true;
}