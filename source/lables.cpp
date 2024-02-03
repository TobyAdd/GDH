#include "lables.hpp"
#include "console.hpp"

std::vector<Label> labels;

void Labels::SetupLabel(CCLayer* self, std::string labelName, std::string placeholder)
{
    Label lbl;

    auto label = CCLabelBMFont::create(placeholder.c_str(), "bigFont.fnt");
    label->setZOrder(1000);
	label->setScale(0.4f);
	label->setOpacity(50);
    label->setAnchorPoint({ .0f, .5f });

    lbl.pointer = label;
    lbl.name = labelName;
    labels.push_back(lbl);

    self->addChild(lbl.pointer);
}

void Labels::ClearLabels()
{
    labels = {};
}

void Labels::setPositions()
{
    auto size = cocos2d::CCDirector::sharedDirector()->getWinSize();

    int counter = 0;
    for (Label& lbl : labels)
    {
        if (!lbl.pointer) continue;

        lbl.pointer->setPositionX(5.f);
        lbl.pointer->setPositionY(size.height - 10 - 15 * counter);
        counter++;
    }
}

DWORD bestrun = 0; 

void Labels::updateBestRun(DWORD newbest, bool isreset)
{
    CCLabelBMFont* bestrunptr = nullptr;

    for (Label &lbl : labels)
    {
        if (lbl.name == "bestrun")
        {
            bestrunptr = lbl.pointer;
        }
    }

    if (bestrunptr == nullptr) return;

    if (isreset == true)
    {
        bestrun = 0;
    } else if (newbest > bestrun)
    {
        bestrun = newbest;
    } else return;
    bestrunptr->setString(CCString::createWithFormat("Best Run: %i%%", newbest)->getCString());
}


//i dont want to make this one
// void Labels::updateAccuracy(std::string accuracy)
// {
//     CCLabelBMFont* accuracyptr = nullptr;

//     for (Label &lbl : labels)
//     {
//         if (lbl.name == "naccuracy")
//         {
//             accuracyptr = lbl.pointer;
//         }
//     }

//     if (accuracyptr == nullptr) return;

//     accuracyptr->setString(CCString::createWithFormat("Noclip Accuracy: %i%%", accuracy.c_str())->getCString());
// }

DWORD ndeaths = -1;

const ccColor3B deathColor = {255, 0, 0};
const ccColor3B notdeathColor = {255, 255, 255};

void Labels::updateNoclipDeaths(bool clear)
{
    CCLabelBMFont* ndeathsptr = nullptr;

    for (Label &lbl : labels)
    {
        if (lbl.name == "ndeaths")
        {
            ndeathsptr = lbl.pointer;
        }
    }

    if (ndeathsptr == nullptr) return;

    if (clear == true)
    {
        ndeaths = -1;
    } else
    {
        ndeaths += 1;
    }
    ndeathsptr->setColor(deathColor);
    ndeathsptr->setString(CCString::createWithFormat("Noclip Deaths: %i", ndeaths)->getCString());
}

void Labels::endNoclipDeath()
{
    CCLabelBMFont* ndeathsptr = nullptr;

    for (Label &lbl : labels)
    {
        if (lbl.name == "ndeaths")
        {
            ndeathsptr = lbl.pointer;
        }
    }

    if (ndeathsptr == nullptr) return;

    ndeathsptr->setColor(notdeathColor);
}

void Labels::updateAttempts(robtop::GJGameLevel_201 level)
{
    CCLabelBMFont* attptr = nullptr;

    for (Label &lbl : labels)
    {
        if (lbl.name == "attempts")
        {
            attptr = lbl.pointer;
        }
    }

    if (attptr == nullptr) return;

    attptr->setString(CCString::createWithFormat("Attempts: %i", level.tail.m_attempts.value())->getCString());
}