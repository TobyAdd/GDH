#pragma once
#include <Geode/Geode.hpp>

class HitboxNode : public cocos2d::CCDrawNode {
public:
    static HitboxNode *create();

    void drawForPlayer(PlayerObject* player);
};