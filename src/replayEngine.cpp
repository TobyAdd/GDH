#include "replayEngine.hpp"
#include "config.hpp"
#include "gui.hpp"

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
