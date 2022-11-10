#include "fpsbypass.h"
#include "PlayLayer.h"
#include "toolsTab.h"
#include "spamBot.h"

typedef void* (__cdecl* fSharedApplication)();
typedef void(__thiscall* fSetAnimationInterval)(void* instance, double delay);
fSharedApplication sharedApplication;
fSetAnimationInterval setAnimInterval;

void FPSBypass(float fps)
{
	HMODULE hMod = LoadLibrary("libcocos2d.dll");
	sharedApplication = (fSharedApplication)GetProcAddress(hMod, "?sharedApplication@CCApplication@cocos2d@@SAPAV12@XZ");
	void* application = sharedApplication();
	setAnimInterval = (fSetAnimationInterval)GetProcAddress(hMod, "?setAnimationInterval@CCApplication@cocos2d@@UAEXN@Z");
	setAnimInterval(application, 1.0f / fps);
}

namespace FPSMultiplier{
    float FPSMultiplier::g_target_fps = 60.f;
    bool FPSMultiplier::g_fps_enabled = false;
    bool FPSMultiplier::g_enabled = false;
    bool FPSMultiplier::g_disable_render = false;
    float FPSMultiplier::g_left_over = 0.f;


    void (__thiscall* CCScheduler_update)(CCScheduler*, float);
    void __fastcall CCScheduler_update_H(CCScheduler* self, int, float dt) {
        if (g_fps_enabled)
            FPSBypass(g_target_fps);
        if (!g_enabled)
            return CCScheduler_update(self, dt);
        auto speedhack = self->getTimeScale();
        const float newdt = 1.f / g_target_fps / speedhack;
        g_disable_render = true;

        const int times = min(static_cast<int>((dt + g_left_over) / newdt), 100);
        for (int i = 0; i < times; ++i) {
            if (i == times - 1)
                g_disable_render = false;
            if (toolsTab::frameadvance && !toolsTab::nextframe) {return;}
            else {toolsTab::nextframe = false;}
            CCScheduler_update(self, newdt);
        }
        g_left_over += dt - newdt * times;
    }

    void (__thiscall* PlayLayer_updateVisibility)(void*);
    void __fastcall PlayLayer_updateVisibility_H(void* self) {
        if (!g_disable_render)
            PlayLayer_updateVisibility(self);
    }

    inline void(__thiscall* dispatchKeyboardMSG)(void* self, int key, bool down);
    void __fastcall dispatchKeyboardMSGHook(void* self, void*, int key, bool down) {
        if (gd::PlayLayer::get() && !gd::PlayLayer::get()->m_isPaused) {
            if (down) {
                if (key == 'C') {
                    toolsTab::frameadvance = true;
                    toolsTab::nextframe = true;
                    FPSMultiplier::g_fps_enabled = true;
                    FPSMultiplier::g_enabled = true;
                }
                if (key == 'F') {
                    toolsTab::frameadvance = false;
                }
                if (key == 'L') {
                    gd::PlayLayer::get()->m_pPlayer1->m_position.x = FLT_MAX;
                }
                if (key == 'R') {
                    PlayLayer::resetLevelHook(gd::PlayLayer::get());
                }
                if (key == 'J') {
                    spambot::enable = !spambot::enable;
                }
            }
        }
        dispatchKeyboardMSG(self, key, down);
    }

    void FPSMultiplier::Setup() {
        MH_CreateHook(
            (void*)(gd::base + 0x205460),
            PlayLayer_updateVisibility_H,
            (void**)&PlayLayer_updateVisibility
        );
        MH_CreateHook(
            GetProcAddress(GetModuleHandleA("libcocos2d.dll"), "?update@CCScheduler@cocos2d@@UAEXM@Z"),
            CCScheduler_update_H,
            (void**)&CCScheduler_update
        );
        MH_CreateHook(
            (PVOID)(GetProcAddress(GetModuleHandleA("libcocos2d.dll"), "?dispatchKeyboardMSG@CCKeyboardDispatcher@cocos2d@@QAE_NW4enumKeyCodes@2@_N@Z")),
            dispatchKeyboardMSGHook,
            (LPVOID*)&dispatchKeyboardMSG
        );
    }
}