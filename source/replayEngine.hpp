#pragma once
#include "includes.hpp"

struct replay_data
{
    unsigned frame;
    bool hold;
    int player_button;
    bool player;
};

enum state
{
    disable,
    record,
    play
};

class ReplayEngine
{
public:
    state mode = disable;

    int index = 0;
    std::vector<replay_data> replay;

    char replay_name[128];

    bool fps_enabled = false;
    bool real_time = true;
    
    float fps = 60.f;
    float speed = 1.f;

    void handle_action(void* self, bool hold, int player_button, bool player);
    void handle_playing(void* self);
    unsigned getFrame(void* self);

    std::string save(char *name);
    std::string load(char *name);
};

extern ReplayEngine engine;