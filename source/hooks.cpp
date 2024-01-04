#include "hooks.hpp"
#include "hacks.hpp"
#include "startposSwitcher.hpp"
#include "smartStartpos.hpp"

bool hooks::musicUnlocker = true;
bool hooks::g_enabledEKG = false;
bool hooks::g_enabledAutoPickupCoins = false;
bool hooks::g_enabledNoEffectCircle = false;
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
    }
    // try to call zcblive callback
    HMODULE zcblive = GetModuleHandleA("zcblive.dll");
    if (zcblive) {
        auto set_playlayer = (zcblive_set_playlayer)GetProcAddress(zcblive, "zcblive_set_playlayer");
        if (set_playlayer) {
            set_playlayer(self);
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

     if(hooks::g_enabledAutoPickupCoins){
        auto gdBaseA = (DWORD)GetModuleHandleA(0);
            

        int coinCount = min(smartStartpos::coinsObject.size(), 3);

        //bool isCoinCollect[3] = { sharedStateA().a_autoPickupCoinsBooleanCoin1 , sharedStateA().a_autoPickupCoinsBooleanCoin2 , sharedStateA().a_autoPickupCoinsBooleanCoin3 };
        for (int i = 0; i < coinCount; i++) {
            if (smartStartpos::coinsObject[i]) {
                reinterpret_cast<void(__thiscall*)(void*, void*)>(gdBaseA + 0x199120)(self, smartStartpos::coinsObject[i]);//pickupCoin
                reinterpret_cast<void(__thiscall*)(void*, void*)>(gdBaseA + 0x198F70)(self, smartStartpos::coinsObject[i]);// desctoyObject
                //self->pickupItem(objsUniversal::coinsInVector[i]);
            }
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


bool(__thiscall *GameStatsManager_isItemUnlocked)(void *, int, int);
bool __fastcall GameStatsManager_isItemUnlockedH(void *self, int, int a, int b)
{
    auto ret = GameStatsManager_isItemUnlocked(self, a, b);

    if (b == 17 && a == 12 && hooks::musicUnlocker)
    {
        return true;
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

typedef enum {
    KEY_Enter = 0x0D,
    KEY_Shift = 0x10,
    KEY_OEMComma = 0xBC,
    KEY_OEMPeriod = 0xBE,
} enumKeyCodes;

bool should_key_jump(int key) {
    return (key >= 'A' && key <= 'Z') ||
    (key >= '0' && key <= '9') ||
    key == KEY_Shift ||
    key == KEY_OEMComma ||
    key == KEY_OEMPeriod ||
    key == KEY_Enter;
}

static const std::set<int> p1_keys = {'1','2','3','4','5','Q','W','E','R','T','A','S','D','F','G','Z','X','C','V','B'};

bool is_player1(int key) {
    return p1_keys.find(key) != p1_keys.end();
}


bool g_left_shift = false;
bool g_right_shift = false;


void*(__thiscall* dispatchKeyboardMSG)(void* self, int key, bool down, bool idk);
void* __fastcall dispatchKeyboardMSGHook(void* self, void*, int key, bool down, bool idk) {
    if (!hooks::g_enabledEKG) return dispatchKeyboardMSG(self, key, down, idk);
    auto base = reinterpret_cast<uintptr_t>(GetModuleHandle(0));
    auto gm = reinterpret_cast<void*(__fastcall*)()>(base + 0x11f720)();
    auto play_layer = MBO(void*, gm, 0x198);

    
    if (play_layer && should_key_jump(key)) {
        auto is_practice_mode = *(bool*)(((char*)play_layer) + 0x2a74);
        if (!is_practice_mode || (key != 'Z' && key != 'X')) {
            bool player1 = true;
            if (key == KEY_Shift) {
                bool left_shift = GetAsyncKeyState(VK_LSHIFT) < 0;
                bool right_shift = GetAsyncKeyState(VK_RSHIFT) < 0;
                if (down) {
                    if (right_shift && !g_right_shift) {
                        g_right_shift = true;
                    } else {
                        g_left_shift = true;
                        right_shift = false;
                    }
                } else {
                    if (g_right_shift && !right_shift) {
                        right_shift = true;
                        g_right_shift = false;
                    } else {
                        right_shift = false;
                        g_left_shift = false;
                    }
                }
                player1 = !right_shift;
            }
            else {
                player1 = is_player1(key);
            }


            if(!idk)
              reinterpret_cast<int(__thiscall*)(void*, int push, int player_button, bool is_player1)>(base + 0x1b2880)(play_layer, down, 1, player1);
           

        }
    }
    return dispatchKeyboardMSG(self, key, down, idk);
}

void*(__thiscall *CCCircle_init)(void *, bool, bool);
void* __fastcall CCCircle_initH(void *self, int, bool a, bool b) {

    auto ret = CCCircle_init(self, a , b);

    return ret;

}

void(__thiscall *UILayer_onCheck)(void *);
void __fastcall UILayer_onCheckH(void *self){
    auto base = reinterpret_cast<uintptr_t>(GetModuleHandle(0));
    auto gm = reinterpret_cast<void*(__fastcall*)()>(base + 0x11f720)();
    auto play_layer = MBO(void*, gm, 0x198);
    if(MBO(bool, play_layer, 0x2e91) || MBO(bool, play_layer, 0x2c20)){
        return;
    }

    UILayer_onCheck(self);
}

void(__thiscall *CCCircle_draw)(void *);
void __fastcall CCCircle_drawH(void *self){
    
    if(!MBO(bool, self, 0x124) && 
        hooks::g_enabledNoEffectCircle) {
        return;
    }

    CCCircle_draw(self);
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
    MH_CreateHook((void *)(base + 0x232f0), CCCircle_initH, (void **)&CCCircle_init);
    MH_CreateHook((void *)(base + 0x23840), CCCircle_drawH, (void **)&CCCircle_draw);
    MH_CreateHook((void *)(base + 0x2e2880), UILayer_onCheckH, (void **)&UILayer_onCheck);

    MH_CreateHook(GetProcAddress(cocos, "?create@CCTransitionFade@cocos2d@@SAPAV12@MPAVCCScene@2@@Z"), CCTransitionFade_createH, (void **)&CCTransitionFade_create);
    MH_CreateHook(GetProcAddress(cocos, "?dispatchKeyboardMSG@CCKeyboardDispatcher@cocos2d@@QAE_NW4enumKeyCodes@2@_N1@Z"), dispatchKeyboardMSGHook, (void **)&dispatchKeyboardMSG);
    MH_CreateHook(GetProcAddress(cocos, "?update@CCScheduler@cocos2d@@UAEXM@Z"), CCScheduler_update_H, (void **)&CCScheduler_update);
    MH_CreateHook((void *)(base + 0x173D10), GameStatsManager_isItemUnlockedH, (void **)&GameStatsManager_isItemUnlocked);
}
