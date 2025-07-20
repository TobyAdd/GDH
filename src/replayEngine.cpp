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

void ReplayEngine::remove_actions(uint64_t frame) {
    std::erase_if(macro.inputs, [frame](const gdr::Input<>& input) { return input.frame > frame; });
}

void ReplayEngine::update(GJBaseGameLayer* self) {
    uint64_t frame = get_frame();

    if (mode == state::play) {
        while (m_inputIndex < macro.inputs.size() && frame >= macro.inputs[m_inputIndex].frame)
        {
            self->handleButton(macro.inputs[m_inputIndex].down, macro.inputs[m_inputIndex].button, macro.inputs[m_inputIndex].player2);
            m_inputIndex++; 
        }
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

    uint64_t frame = get_frame();
    macro.inputs.emplace_back(frame, button, isPlayer1, down);
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
