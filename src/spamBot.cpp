#include "spamBot.h"

extern bool spambot::enable = false;

extern bool spambot::player1 = false;
extern bool spambot::player2 = false;

int temppush = 0;
int temprelease = 0;

extern int spambot::push = 5;
extern int spambot::release = 5;

extern bool spambot::downed = false;

namespace spambot {
    bool spambot::NextFrame() {
        if (!downed) {
            if (temppush == push || temppush > push) {
                temppush = 0;
                downed = !downed;
                return true;
            }
            else {
                temppush++;
                return false;
            }
        }
        else {
            if (temprelease == release || temprelease > release) {
                temprelease = 0;
                downed = !downed;
                return true;
            }
            else {
                temprelease++;
                return false;
            }
        }
    }
}