#include "hooks.hpp"
#include "hacks.hpp"
#include "startposSwitcher.hpp"
#include "smartStartpos.hpp"

bool hooks::musicUnlocker = true;
int hooks::transitionSelect = 0;
unsigned hooks::frame;

typedef void(__stdcall* zcblive_action_callback)(bool, bool);
typedef void(__stdcall* zcblive_set_playlayer)(void*);

float left_over = 0.f;
void(__thiscall *CCScheduler_update)(void *, float);
void __fastcall CCScheduler_update_H(void *self, int, float dt)
{
    dt *= engine.speed;

    if (!engine.fps_enabled)    
        return CCScheduler_update(self, dt);
    
    float newdt = 1.f / engine.fps; 
    if (!engine.realtime)
        return CCScheduler_update(self, newdt);
    
    unsigned times = static_cast<int>((dt + left_over) / newdt);  
    auto start = chrono::high_resolution_clock::now();

    for (unsigned i = 0; i < times; ++i) {
        CCScheduler_update(self, newdt);
        using namespace literals;
        if (chrono::high_resolution_clock::now() - start > 33.333ms) {
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
        startposSwitcher::playLayer = self;

        // try to call zcblive callback
        HMODULE zcblive = GetModuleHandleA("zcblive.dll");
        if (zcblive) {
            auto set_playlayer = (zcblive_set_playlayer)GetProcAddress(zcblive, "zcblive_set_playlayer");
            if (set_playlayer) {
                set_playlayer(self);
            }
        }
    }
        
    return res;
}

void __fastcall hooks::playLayer_update_H(void* self, int edx, float deltaTime)
{
    playLayer_update(self, deltaTime);

    startposSwitcher::playLayer = self;
    frame = engine.getFrame(self);
    
    if (engine.mode == state::play)
    {
        while (engine.index < static_cast<int>(engine.replay.size()) &&
               engine.getFrame(self) >= engine.replay[engine.index].frame)
        {
            auto& replayData = engine.replay[engine.index];
            hooks::GJBaseGameLayer_HandleButton(self, replayData.hold, replayData.player_button, replayData.player);

            // try to call zcblive callback
            HMODULE zcblive = GetModuleHandleA("zcblive.dll");
            if (zcblive) {
                auto action_callback = (zcblive_action_callback)GetProcAddress(zcblive, "zcblive_action_callback");
                if (action_callback) {
                    action_callback(replayData.hold, !replayData.player);
                }
            }

            engine.index++;
        }
    }
}

void __fastcall hooks::PlayLayer_resetLevel_H(void *self)
{
    PlayLayer_resetLevel(self);
    startposSwitcher::playLayer = self;

    if (engine.mode == state::play) {
        engine.index = 0;        
    }
    else if (engine.mode == state::record) {
        unsigned frame = engine.getFrame(self);
        //m_isPracticeMode
        if (*(bool*)(((char*)self) + 0x2a74)) {
            auto check = [&](replay_data& action) -> bool {
                return action.frame >= frame;
            };

            engine.replay.erase(remove_if(engine.replay.begin(), engine.replay.end(), check), engine.replay.end());

            if (!engine.replay.empty() && engine.replay.back().hold) {
                engine.replay.push_back({frame, false, engine.replay.back().player_button, engine.replay.back().player});
            }
        }
        else {
            engine.replay.clear();
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
    startposSwitcher::playLayer = nullptr;
    if (engine.mode == state::record)
        engine.mode = state::disable;    
}

int __fastcall hooks::GJBaseGameLayer_HandleButton_H(void *self, uintptr_t, int push, int player_button, BOOL is_player1)
{
    auto ret = GJBaseGameLayer_HandleButton(self, push, player_button, is_player1);
    if (engine.mode == state::record) {
        engine.handle_action(self, push, player_button, is_player1);
    }        
    return ret;
}


bool(__thiscall *nose)(void *, int, int);
bool __fastcall noseH(void *self, int, int a, int b)
{
    auto n = nose(self, a, b);

    if (b == 17 && a == 12 && hooks::musicUnlocker)
    {
        return true;
    }

    return n;
}

void *(__cdecl *CCTransitionFade_create)(float, void *);
void *__cdecl CCTransitionFade_createH(float duration, void *scene)
{
    auto cocos = GetModuleHandleA("libcocos2d.dll");

    switch (hooks::transitionSelect)
    {
    case 1:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionCrossFade@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 2:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionFadeBL@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 3:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionFadeDown@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 4:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionFadeTR@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 5:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionFadeUp@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 6:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionFlipAngular@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 7:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionFlipX@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 8:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionFlipY@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 9:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionJumpZoom@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 10:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionMoveInB@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 11:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionMoveInL@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 12:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionMoveInR@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 13:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionMoveInT@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 14:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *, bool)>(
            GetProcAddress(cocos, "?create@CCTransitionPageTurn@cocos2d@@SAPAV12@MPAVCCScene@2@_N@Z"))(duration, scene, false);
    }
    case 15:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionProgressHorizontal@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 16:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionProgressInOut@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 17:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionProgressOutIn@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 18:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionProgressRadialCCW@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 19:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionProgressRadialCW@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 20:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionProgressVertical@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 21:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionRotoZoom@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 22:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionShrinkGrow@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 23:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionSlideInB@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 24:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionSlideInL@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 25:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionSlideInR@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 26:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionSlideInT@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 27:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionSplitCols@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 28:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionSplitRows@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 29:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionTurnOffTiles@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 30:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionZoomFlipAngular@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 31:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionZoomFlipX@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    case 32:
    {
        return reinterpret_cast<void *(__cdecl *)(float, void *)>(
            GetProcAddress(cocos, "?create@CCTransitionZoomFlipY@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"))(duration, scene);
    }
    default:
        return CCTransitionFade_create(duration, scene);
    }
}

void hooks::init()
{
    auto gd_base = GetModuleHandleA(0);
    auto cocos = GetModuleHandleA("libcocos2d.dll");
    MH_CreateHook((void *)(base + 0x2D69A0), PlayLayer_init_H, (void **)&PlayLayer_init);
    MH_CreateHook((void *)(base + 0x1B75E0), playLayer_update_H, (void **)&playLayer_update);
    MH_CreateHook((void *)(base + 0x2D6580), PlayLayer_destructor_H, (void **)&PlayLayer_destructor);
    MH_CreateHook((void *)(base + 0x2E42B0), PlayLayer_resetLevel_H, (void **)&PlayLayer_resetLevel);
    MH_CreateHook((void *)(base + 0x1B2880), GJBaseGameLayer_HandleButton_H, (void **)&GJBaseGameLayer_HandleButton);
    MH_CreateHook((void *)(base + 0x2D7F30), playLayer_levelComplate_H, (void **)&playLayer_levelComplate);

    MH_CreateHook(GetProcAddress(cocos, "?create@CCTransitionFade@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"), CCTransitionFade_createH, (void **)&CCTransitionFade_create);
    MH_CreateHook(GetProcAddress(cocos, "?update@CCScheduler@cocos2d@@UAEXM@Z"), CCScheduler_update_H, (void **)&CCScheduler_update);
    MH_CreateHook((void *)(base + 0x173D10), noseH, (void **)&nose);
}
