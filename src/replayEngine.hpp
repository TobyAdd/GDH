#include <Geode/Geode.hpp>
#include <vector>

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