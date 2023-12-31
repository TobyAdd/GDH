#include "include.hpp"
#include <imgui-hook.hpp>
#include <imgui.h>
#include "gui.hpp"
#include "hacks.hpp"
#include "startposSwitcher.hpp"
#include "smartStartpos.hpp"

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
            }

            startposSwitcher::handleKeyPress(keyCode);
        });
    if (MH_Initialize() == MH_OK)
    {
        ImGuiHook::Load([](void *target, void *hook, void **trampoline)
                            { MH_CreateHook(target, hook, trampoline); });
        hacks::load();
        startposSwitcher::init();
        smartStartpos::init();
        MH_EnableHook(MH_ALL_HOOKS);
        hacks::inject_extensions();
    }
    return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    std::filesystem::path get_gdh_path();
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        CloseHandle(CreateThread(0, 0, &ThreadMain, 0, 0, 0));
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH) {
        if (!hacks::content.is_null()) {
            std::filesystem::path gdh_path = get_gdh_path();
            ofstream outputFile(gdh_path / "hacks.json");
            outputFile << hacks::content.dump(4);
            outputFile.close();
        }
    }
    return true;
}
