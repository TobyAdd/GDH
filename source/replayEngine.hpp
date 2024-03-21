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
    std::string log = "New Replay System - Redefined";
    void openReplayMultishit();
public:
    state mode = disable;
    std::string replay_name;

    bool accuracy_fix = true;
    bool rotation_fix = false;

    bool real_time = true;
    bool ignore_inputs = false;

    bool frame_advance = false;

    bool continue_toggled = false;

    void render();
    void renderFull();

    int index = 0;
    std::vector<replay_data> replay;
    
    int index2 = 0;
    std::vector<replay_data2> replay2;

    unsigned get_frame();
    void handle_recording(gd::PlayLayer* self, bool player);
    void handle_recording2(bool hold, int button, bool player);
    void handle_playing(gd::PlayLayer* self);
    void handle_reseting(gd::PlayLayer* self);

    std::string save(std::string name);
    std::string load(std::string name);

    void remove_actions(unsigned frame);
};

extern ReplayEngine engine;

class SpamBot
{
public:
    bool enabled = false;
    int hold = 5;
    int release = 5;
    int hold_current = 0;
    int release_current = 0;
    bool player_p1 = true;
    bool player_p2 = true;

    bool downed = false;

    bool next_frame();
    void handle_spambot(gd::PlayLayer *self);
    void reset_temp();
};

extern SpamBot spamBot;

class StraightFly
{
public:
    bool enabled = false;
    int accuracy = 40;

    void handle_straightfly(gd::PlayLayer *self);
    void start(gd::PlayLayer *self);

private:
    float start_y = 0.0f;
};

extern StraightFly straightFly;

class Sequence
{
public:
    std::string replay_sq_name;
    std::vector<std::string> replays;
    int current_idx;
    bool enable_sqp;
    bool random_sqp;
    bool first_sqp;
    void do_some_magic();
};

extern Sequence sequence;