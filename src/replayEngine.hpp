#include "includes.hpp"

enum state
{
    disable,
    record,
    play,
    continue_record
};

struct ReplayData
{
    int tick;
    bool hold;
    int button;
    bool player;
};

struct ReplayData2
{
    int tick;
    float x;
    float y;
    float rotation;
    double y_accel;
    bool player;
};

class ReplayEngine
{
private:
    void openReplayMultishit();
    std::string log = "new era for re";
public:
    state mode = disable;
    std::string replay_name;

    void render();

    int index = 0;
    std::vector<ReplayData> replay;

    int index2 = 0;
    std::vector<ReplayData2> replay2;

    int get_tick();

    void handle_recording(bool hold, int button, bool player);
    void handle_recording2(bool player);

    void handle_playing();
    void handle_reseting();

    std::string save(std::string name);
    std::string load(std::string name);

    void remove_actions(int tick);
};

extern ReplayEngine engine;