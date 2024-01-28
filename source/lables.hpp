#include "includes.hpp"
#include "bindings/GJGameLevel.h"

class Label
{
public:
	cocos2d::CCLabelBMFont* pointer = nullptr;
	std::string name = "";
    bool enabled = false;
};

namespace Labels {
    void SetupLabel(CCLayer* self, std::string labelName, std::string placeholder);
    void setPositions();
    void ClearLabels();
    void updateBestRun(DWORD newbest, bool isreset = false);
    void updateNoclipDeaths(bool clear = false);
    void endNoclipDeath();
    void updateAttempts(robtop::GJGameLevel_201 level);
    // void updateAccuracy(std::string accuracy);
}