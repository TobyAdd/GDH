#include "includes.hpp"

enum state
{
    disable,
    record,
    play
};

struct replay_data
{
    unsigned frame;
    float x;
    float y;
    float rotation;
    double y_accel;
    bool player;
};

struct replay_data2
{
    unsigned frame;
    bool hold;
    int button;
    bool player;
};

class ReplayEngine
{
private:
    std::string log = "More settings";
    void openReplayMultishit();
public:
    state mode = disable;
    std::string replay_name;

    bool accuracy_fix = true;
    bool rotation_fix = false;

    bool real_time = true;
    bool ignore_inputs = false;

    bool frame_advance = false;
    bool next_frame = false;

    void render();

    int index = 0;
    std::vector<replay_data> replay;
    
    int index2 = 0;
    std::vector<replay_data2> replay2;

    unsigned get_frame();
    void handle_recording(GJBaseGameLayer* self, bool player);
    void handle_recording2(bool hold, int button, bool player);
    void handle_playing(GJBaseGameLayer* self);
    void handle_reseting(PlayLayer* self);

    std::string save(std::string name);
    std::string load(std::string name);

    void remove_actions(unsigned frame);
};

extern ReplayEngine engine;