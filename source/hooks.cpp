#include "hooks.hpp"
#include "hacks.hpp"
#include "console.hpp"
#include "replayEngine.hpp"
#include "labels.hpp"

std::string hooks::log;
gd::PlayLayer* pl = nullptr;
gd::StartPosObject* startpos = nullptr;
std::vector<gd::StartPosObject*> startPositions;

namespace startpos_switcher {
    int selectedStartpos = 0;

    void switchStartPos(int incBy, bool direction = true) {
        if (pl == nullptr || startPositions.empty()) return;

        selectedStartpos += incBy;

        if (selectedStartpos < -1)
            selectedStartpos = startPositions.size() - 1;

        if (selectedStartpos >= startPositions.size())
            selectedStartpos = -1;

        if (direction) {
            gd::StartPosObject* obj = selectedStartpos == -1 ? nullptr : startPositions[selectedStartpos];
            
            pl->m_currentCheckpoint() = nullptr;
            pl->setStartPosObject(obj);
            pl->resetLevel();

            if (hacks::startpos_switcher_reset_camera)
                pl->resetCamera();

            pl->startMusic();

            return;
        }
    }

    void handleKeyPress(int keyCode) {
        if (!pl) return;

        if (!hacks::use_a_s_d && (keyCode == VK_LEFT || keyCode == VK_RIGHT)) {
            int increment = (keyCode == VK_LEFT) ? -1 : 1;
            startpos_switcher::switchStartPos(increment);
        } else if (hacks::use_a_s_d && (keyCode == 'A' || keyCode == 'D')) {
            int increment = (keyCode == 'A') ? -1 : 1;
            startpos_switcher::switchStartPos(increment);
        }
    }
}

bool(__thiscall* StartPosObject_init)(gd::StartPosObject* self);
bool __fastcall StartPosObject_init_H(gd::StartPosObject* self) {
    auto ret = StartPosObject_init(self);
    if (ret)
        startpos = self;
    return ret;
}

void __fastcall hooks::PlayLayer_addObjectH(gd::PlayLayer *self, gd::GameObject *obj)
{
    PlayLayer_addObject(self, obj);
    if (obj->m_nObjectID() == 31) {
        startPositions.push_back(static_cast<gd::StartPosObject *>(obj));
    }
}

static double extraDeltaTime = 0;

void(__thiscall *CCScheduler_update)(void *, float);
void __fastcall CCScheduler_update_H(void *self, float dt)
{
    dt *= hacks::speed_value;
    if (!hacks::fps_enabled) return CCScheduler_update(self, dt);

    float newdt = 1.f / hacks::fps_value; 

    auto steps = static_cast<int32_t>(dt / newdt);
    double remainder = fmod(dt, newdt);

    for (int i = 0; i < steps; i++) {
        CCScheduler_update(self, newdt);
    }

    extraDeltaTime += remainder;

    if (extraDeltaTime >= newdt) {
        CCScheduler_update(self, newdt);
        extraDeltaTime -= newdt;
    }
}

bool *(__thiscall* dispatchKeyboardMSG)(void* self, int key, bool down, bool idk);
bool *__fastcall dispatchKeyboardMSGHook(void* self, int key, bool down, bool idk) {
    if (!idk && down && hacks::startpos_switcher) {
        startpos_switcher::handleKeyPress(key);
    }
    return dispatchKeyboardMSG(self, key, down, idk);
}

bool(__thiscall *GameStatsManager_isItemEnabled)(void *, int, int);
bool __fastcall GameStatsManager_isItemEnabledH(void *self, unsigned int a, unsigned int b)
{
    auto ret = GameStatsManager_isItemEnabled(self, a, b);

    if (hacks::unlock_items)
        return true;

    return ret;
}

bool __fastcall hooks::PlayLayer_initH(gd::PlayLayer* self, void* m_level, bool a3, bool a4)
{
    auto ret = PlayLayer_init(self, m_level, a3, a4);
    if (ret)
    {
        pl = self;

        CCLabelBMFont* labels = CCLabelBMFont::create("", "bigFont.fnt");

		labels->setZOrder(999);
		labels->setTag(999999);
		labels->setScale(0.4f);
        labels->setOpacity(150);
        labels->setAnchorPoint({1, 0});
        self->addChild(labels);
    }
    return ret;
}

void(__thiscall* PlayLayer_update)(gd::PlayLayer*, float);
void __fastcall PlayLayer_update_H(gd::PlayLayer* self, float dt) {    
    PlayLayer_update(self, dt);

    hacks::ricon_delta += dt;

    if (engine.mode == state::play)
    {
        engine.handle_playing(self);
    }
    else if (engine.mode == state::record) {
        engine.handle_recording(self, true);
        engine.handle_recording(self, false);
    }

    noclip_accuracy.handle_update(self, dt);

    if (hacks::rgb_icons) {
        float r1, g1, b1, r2, g2, b2;

        ImGui::ColorConvertHSVtoRGB((float) hacks::ricon_delta * hacks::ricon_coef, hacks::ricon_saturation, hacks::ricon_brightness, r1, g1, b1);
        ImGui::ColorConvertHSVtoRGB((float) hacks::ricon_delta * hacks::ricon_coef + hacks::ricon_shift, hacks::ricon_saturation, hacks::ricon_brightness, r2, g2, b2);

        cocos2d::_ccColor3B color1 = {(GLubyte)(r1 * 255.0f), (GLubyte)(g1 * 255.0f), (GLubyte)(b1 * 255.0f)};
        cocos2d::_ccColor3B color2 = {(GLubyte)(r2 * 255.0f), (GLubyte)(g2 * 255.0f), (GLubyte)(b2 * 255.0f)};

        self->m_pPlayer1()->setColor(color1);
        self->m_pPlayer2()->setColor(color2);
        self->m_pPlayer1()->m_pSecondarySprite()->setColor(color2);
        self->m_pPlayer2()->m_pSecondarySprite()->setColor(color1);


        reinterpret_cast<cocos2d::CCNodeRGBA *>(self->m_pPlayer1()->m_waveTrail())->setColor(color1);
        reinterpret_cast<cocos2d::CCNodeRGBA *>(self->m_pPlayer2()->m_waveTrail())->setColor(color2);
    }

    CCLabelBMFont* labelsS = (CCLabelBMFont*)self->getChildByTag(999999);
    if (labelsS) {
		auto screenLeft = CCDirector::sharedDirector()->getScreenLeft();
        auto screenTop = CCDirector::sharedDirector()->getScreenTop();
        auto screenBottom = CCDirector::sharedDirector()->getScreenBottom();
        auto screenRight = CCDirector::sharedDirector()->getScreenRight();

        labelsS->setCString(labels::get_labels().c_str());
        auto size = labelsS->getScaledContentSize();

        if (labels::pos == 0) {
            labelsS->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);
            labelsS->setPosition({screenLeft + size.width + 5, screenTop - size.height - 3});
        }
        else if (labels::pos == 1) {
            labelsS->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);
            labelsS->setPosition({screenLeft + size.width + 5, screenBottom + 3});
        }
        else if (labels::pos == 2) {
            labelsS->setAlignment(CCTextAlignment::kCCTextAlignmentRight);
            labelsS->setPosition({screenRight - 5, screenTop - size.height - 3});
        }
        else if (labels::pos == 3) {
            labelsS->setAlignment(CCTextAlignment::kCCTextAlignmentRight);
            labelsS->setPosition({screenRight - 5, screenBottom + 3});
        }
		
    }
}

void(__thiscall* PlayLayer_reset)(gd::PlayLayer*);
void __fastcall PlayLayer_reset_H(gd::PlayLayer* self) {    
    PlayLayer_reset(self);
    engine.handle_reseting(self);
    noclip_accuracy.handle_reset(self);
    cps_counter.reset();

    if (hacks::instant_complate) {
        float a = 0;
        reinterpret_cast<void(__thiscall *)(void* self, float *a2)>(hacks::base + 0x388570)(self, &a);
    }
        
}

void(__thiscall* PlayLayer_quit)(void*);
void __fastcall PlayLayer_quit_H(void* self) {    
    if (!hacks::ignore_esc)
        PlayLayer_quit(self);
}

void __fastcall hooks::PlayLayer_destructor_H(gd::PlayLayer *self)
{
    PlayLayer_destructor(self);

    if (engine.mode == record)
    {
        engine.mode = state::disable;
    }

    pl = nullptr;
    hacks::ricon_delta = 0;

    startPositions.clear();
}

int __fastcall hooks::GJBaseGameLayer_HandleButton_H(gd::PlayLayer*self, int hold, int player_button, bool is_player1)
{
    if (engine.mode == state::play && engine.ignore_inputs) {
        return 0;
    }

    if (hold) {
        cps_counter.recordClick();
    }

    auto ret = GJBaseGameLayer_HandleButton(self, hold, player_button, is_player1);
    if (engine.mode == state::record)
    {
        if (pl)
            engine.handle_recording2(hold, player_button, is_player1, self);
    }
    return ret;
}

void __fastcall hooks::PlayLayer_destroyPlayer_H(gd::PlayLayer* self, gd::PlayerObject* player, gd::GameObject* obj) {
    PlayLayer_destroyPlayer(self, player, obj);
    noclip_accuracy.handle_death();
}

void hooks::init() {
    MH_CreateHook(GetProcAddress((HMODULE)hacks::cocos_base, "?update@CCScheduler@cocos2d@@UEAAXM@Z"), CCScheduler_update_H, (void **)&CCScheduler_update);
    MH_CreateHook((void *)(hacks::base + 0x1DABE0), GameStatsManager_isItemEnabledH, (void **)&GameStatsManager_isItemEnabled);
    MH_CreateHook((void *)(hacks::base + 0x1F7DD0), PlayLayer_initH, (void **)&PlayLayer_init);
    MH_CreateHook((void *)(hacks::base + 0x2277D0), PlayLayer_update_H, (void **)&PlayLayer_update);
    MH_CreateHook((void *)(hacks::base + 0x3958b0), PlayLayer_reset_H, (void **)&PlayLayer_reset);
    MH_CreateHook((void *)(hacks::base + 0x35cd70), PlayLayer_quit_H, (void **)&PlayLayer_quit);
    MH_CreateHook((void *)(hacks::base + 0x2238a0), GJBaseGameLayer_HandleButton_H, (void **)&GJBaseGameLayer_HandleButton);
    MH_CreateHook((void *)(hacks::base + 0x3905a0), PlayLayer_destroyPlayer_H, (void **)&PlayLayer_destroyPlayer);
    MH_CreateHook((void *)(hacks::base + 0x38a990), PlayLayer_addObjectH, (void **)&PlayLayer_addObject);//382540
    MH_CreateHook((void *)(hacks::base + 0x382540), PlayLayer_destructor_H, (void **)&PlayLayer_destructor);
    MH_CreateHook((void *)(hacks::base + 0x485B00), StartPosObject_init_H, (void **)&StartPosObject_init);
    MH_CreateHook(GetProcAddress(hacks::cocos_base, "?dispatchKeyboardMSG@CCKeyboardDispatcher@cocos2d@@QEAA_NW4enumKeyCodes@2@_N1@Z"), dispatchKeyboardMSGHook, (void **)&dispatchKeyboardMSG);
}