#include <Geode/Geode.hpp>
#include <gdr/gdr.hpp>
#include <vector>

enum state
{
    disable,
    record,
    play
};

struct FrameFix {
    uint32_t frame;
    float x, y;
    float rotation;
    double accelY;
    bool isPlayer2;
};

struct ReplayEngineMacro : gdr::Replay<ReplayEngineMacro> {
    std::vector<FrameFix> frameFixes;

    ReplayEngineMacro() : Replay("ReplayEngine", 4) {}

    bool shouldParseExtension() const override {
        return botInfo.name == "ReplayEngine";
    }

    void parseExtension(binary_reader& reader) override {
        size_t count;
        reader >> count;
        frameFixes.resize(count);
        for (size_t i = 0; i < count; ++i) {
            FrameFix& fix = frameFixes[i];
            reader >> fix.frame
                   >> fix.x >> fix.y
                   >> fix.rotation
                   >> fix.accelY
                   >> fix.isPlayer2;
        }
    }

    void saveExtension(binary_writer& writer) const override {
        writer << frameFixes.size();
        for (const auto& fix : frameFixes) {
            writer << fix.frame
                   << fix.x << fix.y
                   << fix.rotation
                   << fix.accelY
                   << fix.isPlayer2;
        }
    }
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

    uint64_t get_frame();
    void remove_actions(uint64_t frame);

    size_t get_actions_size();
    int get_current_index();

    void save(std::string name);
    void load(std::string name);
    void clear();

    void update(GJBaseGameLayer* self);
    void reset();
    void handle_button(bool down, int button, bool isPlayer1);
private:
    ReplayEngine() = default;
    ReplayEngineMacro macro;

    size_t m_inputIndex = 0;
    size_t m_physicIndex = 0;
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