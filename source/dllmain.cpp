#include "include.h"
#include <imgui-hook.hpp>
#include <imgui.h>
#include "gui.h"
#include "hooks.h"
#include "hacks.h"
#include "speedhackAudio.hpp"
#include "startposSwitcher.hpp"

DWORD WINAPI ThreadMain(LPVOID lpParam)
{
    ImGuiHook::setRenderFunction(gui::Render);
    ImGuiHook::setKeyPressHandler([](int keyCode) 
        {
            switch (keyCode)
            {
            case VK_TAB:
                gui::Toggle();
                break;

            case VK_LEFT:
            case VK_RIGHT:
                startposSwitcher::switchStartPos(keyCode == VK_RIGHT);
                break;
            }
        });
    if (MH_Initialize() == MH_OK)
    {
        ImGuiHook::Load([](void *target, void *hook, void **trampoline)
                            { MH_CreateHook(target, hook, trampoline); });
        hooks::init();
        hacks::load();
        speedhackAudio::init();
        startposSwitcher::init();
        MH_EnableHook(MH_ALL_HOOKS);
    }
    return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        CloseHandle(CreateThread(0, 0, &ThreadMain, 0, 0, 0));
    }
    return true;
}