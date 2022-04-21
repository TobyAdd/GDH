#ifndef __LEADERBOARDSLAYER_H__
#define __LEADERBOARDSLAYER_H__

#include <gd.h>

namespace gd {
    enum LeaderboardState {
        kLeaderboardStateTop100     = 1,
        kLeaderboardStateGlobal     = 2,
        kLeaderboardStateCreator    = 3,
        kLeaderboardStateFriends    = 4,
    };

    class LeaderboardsLayer : public cocos2d::CCLayer {
        public:
            static LeaderboardsLayer* create(LeaderboardState state) {
                return reinterpret_cast<LeaderboardsLayer*(__fastcall*)(
                    LeaderboardState
                )>(
                    base + 0x158710
                )(
                    state
                );
            }
    };
}

#endif
