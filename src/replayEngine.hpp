#include <Geode/Geode.hpp>
#include <vector>

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
    bool down;
    int button;
    bool isPlayer1;
};

class ReplayEngine {
public:
    static ReplayEngine& get() {
        static ReplayEngine instance;
        return instance;
    }

    ReplayEngine& operator=(const ReplayEngine&) = delete;
    ReplayEngine(const ReplayEngine&) = delete;

    bool engine_v2 = false;

    state mode = state::disable;
    std::string replay_name;

    bool accuracy_fix = true;
    bool rotation_fix = false;

    bool settings_openned = false;

    bool frame_advance = false;
    bool next_frame = false;

    unsigned get_frame();
    void remove_actions(unsigned frame);

    size_t get_actions_size();
    int get_current_index();

    std::string save(std::string name);
    std::string load(std::string name, bool only_p1 = true, bool only_p2 = true);

    std::string save2(std::string name);
    std::string load2(std::string name, bool only_p1 = true, bool only_p2 = true);

    std::string clear();

    void handle_update(GJBaseGameLayer* self);
    void handle_reset();
    void handle_button(bool down, int button, bool isPlayer1);

    void auto_button_release();

    void renderUI();
private:
    ReplayEngine() = default;

    std::string load_v1(std::string name);
    std::string load_v2(std::string name);
    std::string load_v3(std::string name, bool only_p1 = true, bool only_p2 = true);

    std::vector<replay_data> m_physicFrames_p1;
    std::vector<replay_data> m_physicFrames_p2;
    size_t m_physicIndex_p1 = 0;
    size_t m_physicIndex_p2 = 0;

    std::vector<replay_data2> m_inputFrames_p1;
    std::vector<replay_data2> m_inputFrames_p2;
    size_t m_inputIndex_p1 = 0;
    size_t m_inputIndex_p2 = 0;
};

class SpamBot
{
public:
    static SpamBot& get() {
        static SpamBot instance;
        return instance;
    }

    SpamBot& operator=(const SpamBot&) = delete;
    SpamBot(const SpamBot&) = delete;

    int hold_current = 0;
    int release_current = 0;

    bool downed = false;

    bool next_frame();
    void handle_spambot(GJBaseGameLayer *self);
    void reset_temp();
private:
    SpamBot() = default;
};

class StraightFly
{
public:
    static StraightFly& get() {
        static StraightFly instance;
        return instance;
    }

    StraightFly& operator=(const StraightFly&) = delete;
    StraightFly(const StraightFly&) = delete;

    float accuracy = 40.f;

    void handle_straightfly(GJBaseGameLayer *self);
    void start(GJBaseGameLayer *self);

private:
    StraightFly() = default;

    float start_y = 0.0f;
};