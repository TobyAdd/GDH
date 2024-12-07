#include "hitboxNode.hpp"

HitboxNode* HitboxNode::create()
{
	HitboxNode* ret = new HitboxNode();
	if (ret && ret->init())
	{
		ret->autorelease();
		return ret;
	}
    else
	    CC_SAFE_RELEASE(ret);

	return nullptr;
}

void drawRect(cocos2d::CCDrawNode *node, const cocos2d::CCRect &rect, const cocos2d::_ccColor4F &color, float borderWidth, const cocos2d::_ccColor4F &borderColor) {

}

void HitboxNode::drawForPlayer(PlayerObject* player) {
    clear();
    auto rect = player->getObjectRect();
    std::vector<cocos2d::CCPoint> vertices = {
            cocos2d::CCPoint(rect.getMinX(), rect.getMinY()),
            cocos2d::CCPoint(rect.getMinX(), rect.getMaxY()),
            cocos2d::CCPoint(rect.getMaxX(), rect.getMaxY()),
            cocos2d::CCPoint(rect.getMaxX(), rect.getMinY())
    };
    drawPolygon(vertices.data(), vertices.size(), {0, 0, 0, 0}, 0.25f, {1.f, 1.f, 0.f, 1.0f});
}