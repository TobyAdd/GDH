#include "include.hpp"

namespace smartStartpos
{
    extern bool enabled;
    extern std::vector<uint32_t*> coinsObject;

    void init();
    void resetObjects();
    void updateStartPos(void *playLayer);
}
