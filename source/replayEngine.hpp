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

        bool style_init = false;

        int sequence_index = 0;
        bool sequence_state = false, sequence_random = false, sequence_first = false;
        void sequence_work();

        int index = 0;
        vector<replay_data> replay;
        vector<std::string> sequence;

        bool fps_enabled = true;
        bool realtime = true;
        
        float fps = 60.f;
        float speed = 1.f;

        void save(std::string replay_name);
        void load(std::string replay_name);
        int random(int min, int max);

        void handle_action(void* self, bool hold, int player_button, bool player);
        unsigned getFrame(void* self);
};

extern ReplayEngine engine;