#include "hooks.hpp"

float speed = 1.0f;

void(__thiscall *CCScheduler_update)(void *, float);
void __fastcall CCScheduler_update_H(void* self, int, float dt) {
    dt *= speed;
    CCScheduler_update(self, dt);
}

void hooks::init() {
    auto cocos = GetModuleHandleA("libcocos2d.dll");
    MH_CreateHook(GetProcAddress(cocos, "?update@CCScheduler@cocos2d@@UAEXM@Z"), CCScheduler_update_H, (void **)&CCScheduler_update);
}

