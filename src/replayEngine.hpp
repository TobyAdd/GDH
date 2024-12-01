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

    state mode = state::disable;
    std::string replay_name;

    bool accuracy_fix = true;
    bool rotation_fix = false;

    unsigned get_frame();
    void remove_actions(unsigned frame);
    size_t get_actions_size();

    std::string save(std::string name);
    std::string load(std::string name);
    std::string clear();

    void handle_update(GJBaseGameLayer* self);
    void handle_reset(GJBaseGameLayer* self);
    void handle_button(bool down, int button, bool isPlayer1);

    void auto_button_release();
private:
    ReplayEngine() = default;

    std::vector<replay_data> m_physicFrames_p1;
    std::vector<replay_data> m_physicFrames_p2;
    size_t m_physicIndex_p1 = 0;
    size_t m_physicIndex_p2 = 0;

    std::vector<replay_data2> m_inputFrames_p1;
    std::vector<replay_data2> m_inputFrames_p2;
    size_t m_inputIndex_p1 = 0;
    size_t m_inputIndex_p2 = 0;
};
