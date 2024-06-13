#include "includes.hpp"
#include "imgui-hook.hpp"
#include "gd.hpp"
#include <vector>

enum state
{
    disable,
    record,
    play,
    continue_record
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
    void FullRender();
    std::string log = "here is a log and yeah full re is dead";
    void openReplayMultishit();
public:
    state mode = disable;
    std::string replay_name;

    bool accuracy_fix = true;
    bool rotation_fix = false;

    bool real_time = true;
    bool ignore_inputs = false;

    bool frame_advance = false;

    void render();

    int index = 0;
    std::vector<replay_data> replay;
    
    int index2 = 0;
    std::vector<replay_data2> replay2;

    unsigned get_frame(gd::PlayLayer* self);
    void handle_recording(gd::PlayLayer* self, bool player);
    void handle_recording2(bool hold, int button, bool player, gd::PlayLayer* self = nullptr);
    void handle_playing(gd::PlayLayer* self);
    void handle_reseting(gd::PlayLayer* self);

    std::string save(std::string name);
    std::string load(std::string name);

    void remove_actions(unsigned frame);
};

extern ReplayEngine engine;