#include "replayEngine.hpp"
#include "config.hpp"
#include "gui.hpp"

uint64_t ReplayEngine::get_frame() {
    auto &config = Config::get();

    auto gjbgl = GJBaseGameLayer::get();
    if (gjbgl)
        return static_cast<uint64_t>(gjbgl->m_gameState.m_levelTime * config.get<float>("tps_value", 240.f));
    return 0;
}

void ReplayEngine::remove_actions(uint64_t currentFrame, bool autoRelease) {
    std::erase_if(macro.inputs, [currentFrame](const gdr::Input<>& input) { return input.frame >= currentFrame; });
    std::erase_if(macro.frameFixes, [currentFrame](const FrameFix& frameFix) { return frameFix.frame >= currentFrame; });

    if (!autoRelease) return;

    std::array<bool, 6> released = {true, true, true, true, true, true};
    std::array<bool, 6> checked = {false, false, false, false, false, false};

    for (auto it = macro.inputs.rbegin(); it != macro.inputs.rend(); ++it) {
        int button_index = it->button - 1;
        button_index += it->player2 ? 3 : 0;

        if (!checked[button_index]) {
            checked[button_index] = true;
            released[button_index] = !it->down;
        }

        if (std::all_of(checked.begin(), checked.end(), [](bool v) { return v; })) break;
    }

    for (int i = 0; i < 3; ++i) {
        if (!released[i]) {
            macro.inputs.emplace_back(currentFrame, i + 1, false, false);
        }
        if (!released[i + 3]) {
            macro.inputs.emplace_back(currentFrame, i + 1, true, false);
        }
    }
}

void ReplayEngine::update(GJBaseGameLayer* self) {
    uint64_t frame = get_frame();

    if (mode == state::play) {
        while (m_inputIndex < macro.inputs.size() && frame >= macro.inputs[m_inputIndex].frame)
        {
            bool swapControls = GameManager::get()->getGameVariable("0010");
            bool isPlayer1 = !macro.inputs[m_inputIndex].player2;
            isPlayer1 = swapControls ? !isPlayer1 : isPlayer1;

            self->handleButton(macro.inputs[m_inputIndex].down, macro.inputs[m_inputIndex].button, isPlayer1);
            m_inputIndex++; 
        }

        while (m_physicIndex < macro.frameFixes.size() && frame >= macro.frameFixes[m_physicIndex].frame)
        {
            if (!macro.frameFixes[m_physicIndex].isPlayer2)
            {
                self->m_player1->setPosition({macro.frameFixes[m_physicIndex].x, macro.frameFixes[m_physicIndex].y});
                self->m_player1->m_position.x = macro.frameFixes[m_physicIndex].x;
                self->m_player1->m_position.y = macro.frameFixes[m_physicIndex].y;
                if (rotation_fix)
                    self->m_player1->setRotation(macro.frameFixes[m_physicIndex].rotation);
                self->m_player1->m_yVelocity = macro.frameFixes[m_physicIndex].accelY;
            }
            else
            {
                self->m_player2->setPosition({macro.frameFixes[m_physicIndex].x, macro.frameFixes[m_physicIndex].y});
                self->m_player2->m_position.x = macro.frameFixes[m_physicIndex].x;
                self->m_player2->m_position.y = macro.frameFixes[m_physicIndex].y;
                if (rotation_fix)
                    self->m_player2->setRotation(macro.frameFixes[m_physicIndex].rotation);
                self->m_player2->m_yVelocity = macro.frameFixes[m_physicIndex].accelY;
            }
            m_physicIndex++; 
        }
    }
    else if (mode == state::record) {
        if (!macro.frameFixes.empty() && macro.frameFixes.back().frame == frame)
            return;

        macro.frameFixes.push_back({
            frame,
            self->m_player1->m_position.x,
            self->m_player1->m_position.y,
            self->m_player1->getRotation(),
            self->m_player1->m_yVelocity,
            false
        });

        macro.frameFixes.push_back({
            frame,
            self->m_player2->m_position.x,
            self->m_player2->m_position.y,
            self->m_player2->getRotation(),
            self->m_player2->m_yVelocity,
            true
        });
    }
}

void ReplayEngine::reset() {
    if (mode == state::record) {
        int lastCheckpointFrame = get_frame();
        remove_actions(lastCheckpointFrame);
    }
    else if (mode == state::play) {
        m_inputIndex = 0;
        m_physicIndex = 0;
    }
}

void ReplayEngine::handle_button(bool down, int button, bool isPlayer1) {
    if (mode != state::record)
        return;

    bool swapControls = GameManager::get()->getGameVariable("0010");
    isPlayer1 = swapControls ? !isPlayer1 : isPlayer1;

    uint64_t frame = get_frame();
    macro.inputs.emplace_back(frame, button, !isPlayer1, down); // inverted !isPlayer1 because GDR accepts isPlayer2
}

bool ReplayEngine::save(const std::string& name) {
    auto res = macro.exportData(folderMacroPath / name);
    return res.isOk();
}

bool ReplayEngine::load(const std::string& name) {
    auto res = macro.importData(folderMacroPath / name);
    if (res.isOk()) macro = std::move(res).unwrap();
    return res.isOk();
}

void ReplayEngine::clear() {
    macro.inputs.clear();
}

size_t ReplayEngine::get_actions_size() {
    return macro.inputs.size();
}

int ReplayEngine::get_current_index() {
    return m_inputIndex;
}

bool SpamBot::next_frame()
{
    auto& config = Config::get();
    
    if ((downed && ++release_current >= config.get<int>("release_length", 5)) || (!downed && ++hold_current >= config.get<int>("hold_length", 5)))
    {
        downed = !downed;
        (downed) ? release_current = 0 : hold_current = 0;
        return true;
    }
    else
    {
        return false;
    }
}

void SpamBot::handle_spambot(GJBaseGameLayer *self)
{
    auto& config = Config::get();

    if (config.get<bool>("spambot_enabled", false) && next_frame())
    {
        bool action = downed;
        if (config.get<bool>("spambot_player1", true))
        {
            self->handleButton(action, 1, true);
        }
        if (config.get<bool>("spambot_player2", false) && self->m_gameState.m_isDualMode)
        {
            self->handleButton(action, 1, false);
        }
    }
}


void SpamBot::reset_temp()
{
    hold_current = 0;
    release_current = 0;
}

void StraightFly::handle_straightfly(GJBaseGameLayer *self)
{
    if (!Config::get().get<bool>("straight_fly_bot", false))
        return;

    float y = self->m_player1->m_position.y;
    double accel = self->m_player1->m_yVelocity;
    bool holding = self->m_player1->m_jumpBuffered;

    if (start_y == 0)
    {
        start(self);
    }

    if (self->m_player1->m_isUpsideDown)
    {
        float delta_y = y - start_y;
        y = start_y - delta_y;
        accel *= -1;
    }

    if (accel < 0 && y < start_y - accel - accuracy / 100 && !holding)
    {
        self->handleButton(true, 1, true);
    }
    else if (accel > 0 && y > start_y - accel + accuracy / 100 && holding)
    {
        self->handleButton(false, 1, true);
    }
}

void StraightFly::start(GJBaseGameLayer *self)
{
    start_y = self ? self->m_player1->m_position.y : 0.0f;
}
