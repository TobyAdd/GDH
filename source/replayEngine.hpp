#include "include.hpp"

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
    vector<replay_data> replay;

    char replay_name[128];

    bool fps_enabled = true;
    bool realtime = true;
    
    float fps = 60.f;
    float speed = 1.f;

    void handle_action(void* self, bool hold, int player_button, bool player);
    void handle_playing(void* self);
    unsigned getFrame(void* self);
};

extern ReplayEngine engine;