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
    void openReplayMultishit();
public:
    int version_engine = 1;

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

    bool settings_openned = false;

    int secret = 5;

    unsigned get_frame();
    void handle_recording(GJBaseGameLayer* self, bool player);
    void handle_recording2(bool hold, int button, bool player);
    void handle_playing(GJBaseGameLayer* self);
    void handle_reseting(PlayLayer* self);

    std::string save(std::string name);
    std::string load(std::string name);

    void remove_actions(unsigned frame);

    bool containsRussianLetters(const std::filesystem::path& p);
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
    bool player_p2 = false;

    bool downed = false;

    bool next_frame();
    void handle_spambot(GJBaseGameLayer *self);
    void reset_temp();
};

extern SpamBot spamBot;

class StraightFly
{
public:
    bool enabled = false;
    int accuracy = 40;

    void handle_straightfly(GJBaseGameLayer *self);
    void start(GJBaseGameLayer *self);

private:
    float start_y = 0.0f;
};

extern StraightFly straightFly;