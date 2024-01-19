#include "hooks.hpp"
#include "replayEngine.hpp"
#include <chrono>

int hooks::transitionSelect = 0;
unsigned hooks::frame = 0;

float left_over = 0.f;
void(__thiscall *CCScheduler_update)(void *, float);
void __fastcall CCScheduler_update_H(void *self, int, float dt)
{
    dt *= engine.speed;

    if (!engine.fps_enabled)    
        return CCScheduler_update(self, dt);
    
    float newdt = 1.f / engine.fps; 

    if (!engine.real_time)
        return CCScheduler_update(self, newdt);
 
    unsigned times = static_cast<int>((dt + left_over) / newdt);  
    auto start = std::chrono::high_resolution_clock::now();

    for (unsigned i = 0; i < times; ++i) {
        CCScheduler_update(self, newdt);
        using namespace std::literals;
        if (std::chrono::high_resolution_clock::now() - start > 33.333ms) {
            times = i + 1;
            break;
        }
    }
    left_over += dt - newdt * times; 
}

bool __fastcall hooks::PlayLayer_init_H(void *self, int edx, void *GJGameLevel, bool a3, bool a4)
{
    const auto res = PlayLayer_init(self, GJGameLevel, a3, a4);
    if (res) {
        //idk
    }
        
    return res;
}

void __fastcall hooks::playLayer_update_H(void* self, int edx, float deltaTime)
{
    playLayer_update(self, deltaTime);

    frame = engine.getFrame(self);

    if (engine.mode == state::play)
    {
        while (engine.index < static_cast<int>(engine.replay.size()) &&
               engine.getFrame(self) >= engine.replay[engine.index].frame)
        {
            auto& replayData = engine.replay[engine.index];
            hooks::GJBaseGameLayer_HandleButton(self, replayData.hold, replayData.player_button, replayData.player);
            engine.index++;
        }
    }
}

void __fastcall hooks::PlayLayer_resetLevel_H(void *self)
{
    PlayLayer_resetLevel(self);
    if (engine.mode == state::play) {
        engine.index = 0;        
    }
    else if (engine.mode == state::record) {
        unsigned frame = engine.getFrame(self);

        auto check = [&](replay_data& action) -> bool {
            return action.frame >= frame;
        };

        engine.replay.erase(remove_if(engine.replay.begin(), engine.replay.end(), check), engine.replay.end());

        if (!engine.replay.empty() && engine.replay.back().hold) {
            engine.replay.push_back({frame, false, engine.replay.back().player_button, engine.replay.back().player});
        }
    } 
}

void __fastcall hooks::playLayer_levelComplate_H(int *self) {
    playLayer_levelComplate(self);
    if (engine.mode == state::record)
        engine.mode = state::disable;
}

void __fastcall hooks::PlayLayer_destructor_H(void *self)
{
    PlayLayer_destructor(self);
    if (engine.mode == state::record)
        engine.mode = state::disable;    
}

int __fastcall hooks::GJBaseGameLayer_HandleButton_H(void *self, uintptr_t, int push, int player_button, BOOL is_player1)
{
    if (engine.mode == state::play) {
        return 0;
    }

    auto ret = GJBaseGameLayer_HandleButton(self, push, player_button, is_player1);
    if (engine.mode == state::record) {
        engine.handle_action(self, push, player_button, is_player1);
    }        
    return ret;
}

void* CreateTransition(float duration, void* scene, const char* functionName)
{
    auto cocos = GetModuleHandleA("libcocos2d.dll");
    return reinterpret_cast<void*(__cdecl*)(float, void*)>(
        GetProcAddress(cocos, functionName))(duration, scene);
}

void *(__cdecl *CCTransitionFade_create)(float, void *);
void *__cdecl CCTransitionFade_createH(float duration, void *scene)
{
    const char* transitionFunctions[] = {
    "?create@CCTransitionCrossFade@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionFadeBL@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionFadeDown@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionFadeTR@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionFadeUp@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionFlipAngular@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionFlipX@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionFlipY@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionJumpZoom@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionMoveInB@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionMoveInL@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionMoveInR@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionMoveInT@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionPageTurn@cocos2d@@SAPAV12@MPAVCCScene@2@_N@Z",
    "?create@CCTransitionProgressHorizontal@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionProgressInOut@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionProgressOutIn@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionProgressRadialCCW@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionProgressRadialCW@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionProgressVertical@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionRotoZoom@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionShrinkGrow@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionSlideInB@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionSlideInL@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionSlideInR@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionSlideInT@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionSplitCols@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionSplitRows@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionTurnOffTiles@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionZoomFlipAngular@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionZoomFlipX@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    "?create@CCTransitionZoomFlipY@cocos2d@@SAPAV12@MPAVCCScene@2@@Z",
    };

    if (hooks::transitionSelect >= 1 && hooks::transitionSelect <= sizeof(transitionFunctions) / sizeof(transitionFunctions[0]))
    {
        return CreateTransition(duration, scene, transitionFunctions[hooks::transitionSelect - 1]);
    }
    else
    {
        return CCTransitionFade_create(duration, scene);
    }
}

bool(__thiscall *GameStatsManager_isItemUnlocked)(void *, int, int);
bool __fastcall GameStatsManager_isItemUnlockedH(void *self, int, int a, int b)
{
    auto ret = GameStatsManager_isItemUnlocked(self, a, b);

    if (b == 17 && a == 12)
    {
        return true;
    }

    return ret;
}


void hooks::init() {
    HMODULE gd = GetModuleHandleA(0);
    HMODULE cocos = GetModuleHandleA("libcocos2d.dll");

    MH_CreateHook((void *)((DWORD)gd + 0x2dc4a0), PlayLayer_init_H, (void **)&PlayLayer_init);
    MH_CreateHook((void *)((DWORD)gd + 0x1bb780), playLayer_update_H, (void **)&playLayer_update);
    MH_CreateHook((void *)((DWORD)gd + 0x2ea130), PlayLayer_resetLevel_H, (void **)&PlayLayer_resetLevel);
    MH_CreateHook((void *)((DWORD)gd + 0x2dc080), PlayLayer_destructor_H, (void **)&PlayLayer_destructor);
    MH_CreateHook((void *)((DWORD)gd + 0x2ddb60), playLayer_levelComplate_H, (void **)&playLayer_levelComplate);
    MH_CreateHook((void *)((DWORD)gd + 0x1b69f0), GJBaseGameLayer_HandleButton_H, (void **)&GJBaseGameLayer_HandleButton);

    MH_CreateHook(GetProcAddress(cocos, "?update@CCScheduler@cocos2d@@UAEXM@Z"), CCScheduler_update_H, (void **)&CCScheduler_update);
    MH_CreateHook(GetProcAddress(cocos, "?create@CCTransitionFade@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"), CCTransitionFade_createH, (void **)&CCTransitionFade_create);
    MH_CreateHook((void *)((DWORD)gd + 0x176570), GameStatsManager_isItemUnlockedH, (void **)&GameStatsManager_isItemUnlocked);
}