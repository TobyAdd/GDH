#include <chrono>
#include <iomanip>
#include <sstream>
#include "labels.hpp"
#include "popupSystem.hpp"
#include "hooks.hpp"

using namespace geode::prelude;

Label::Label(LabelCorner _corner, std::string _format_text) {
    format_text = _format_text;
    corner = _corner;
}

std::string Label::get_text() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm localTime = fmt::localtime(now_time_t);

    int hour12 = localTime.tm_hour;
    std::string period12 = (hour12 < 12) ? "AM" : "PM";
    if (hour12 == 0) hour12 = 12;
    if (hour12 > 12) hour12 -= 12;

    std::string result = format_text;

    auto pl = PlayLayer::get();
    bool platformer = pl->m_level->isPlatformer();
    float percentage = platformer ? static_cast<float>(pl->m_gameState.m_levelTime) : pl->getCurrentPercent();

    CpsCounter::get().update();

    result = replace_all(result, "{time:12}", fmt::format("{} {}", time_to_fmt_time(hour12, localTime.tm_min, localTime.tm_sec), period12));
    result = replace_all(result, "{time:24}", time_to_fmt_time(localTime.tm_hour, localTime.tm_min, localTime.tm_sec));
    result = replace_all(result, "{attempt}", std::to_string(Labels::get().attempts));
    result = replace_all(result, "{sessionTime}", seconds_to_fmt_time(Labels::get().session_time));
    result = replace_all(result, "{progress}", platformer ? seconds_to_fmt_time(percentage) : fmt::format("{:.2f}%", percentage, 2));
    result = replace_all(result, "{clicks}", std::to_string(CpsCounter::get().overall));
    result = replace_all(result, "{cps}", std::to_string(CpsCounter::get().cps));
    result = replace_all(result, "{cpsHigh}", std::to_string(CpsCounter::get().highscore));
    result = replace_all(result, "{levelName}", pl->m_level->m_levelName);
    result = replace_all(result, "{levelCreator}", pl->m_level->m_creatorName);
    result = replace_all(result, "{byLevelCreator}", pl->m_level->m_creatorName.empty() ? "" : fmt::format(" by {}", pl->m_level->m_creatorName));
    result = replace_all(result, "{levelId}", std::to_string(pl->m_level->m_levelID));
    result = replace_all(result, "{noclipAccuracy}", fmt::format("{:.2f}%", NoclipAccuracy::get().getPercentage()));
    result = replace_all(result, "{deaths}", std::to_string(NoclipAccuracy::get().deaths_full));
    result = replace_all(result, "{startposCurrentIDX}", std::to_string(hooksH::selectedStartpos+1));
    result = replace_all(result, "{startposAllIDX}", std::to_string(hooksH::startPositions.size()));
    result = replace_all(result, "{\\n}", "\n");

    return result;
}

std::string Label::replace_all(std::string src, std::string replace, std::string replacement) {
    if (replace.empty()) return src;
    size_t start_pos = 0;
    while ((start_pos = src.find(replace, start_pos)) != std::string::npos) {
        src.replace(start_pos, replace.length(), replacement);
        start_pos += replacement.length();
    }
    return src;
}

std::string Label::time_to_fmt_time(int hours, int minutes, int seconds) {
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << hours << ":" << std::setw(2) << minutes << ":" << std::setw(2) << seconds;
    return ss.str();
}

std::string Label::seconds_to_fmt_time(float seconds) {
    int time = static_cast<int>(seconds);
    int minutes = time / 60;
    int hour = minutes / 60;
    return time_to_fmt_time(hour % 60, minutes % 60, time % 60);
}

std::string Labels::get_label_string(LabelCorner corner) {
    std::string result = "";
    
    for (Label& l : labels) {
        if (l.corner != corner) continue;
        if (l.format_text.empty()) continue;

        result += l.get_text();
        result += "\n";
    }
    return result;
}

void Labels::add(Label label) {
    labels.push_back(label);
}

void Labels::remove(int index) {
    labels.erase(labels.begin()+index);
}

void Labels::move_up(int index) {
    if (index <= 0) return;
    if (index >= labels.size()) return;

    std::iter_swap(std::next(labels.begin(), index), std::next(labels.begin(), index - 1));
}

void Labels::move_down(int index) {
    if (index < 0) return;
    if (index >= labels.size()-1) return;

    std::iter_swap(std::next(labels.begin(), index), std::next(labels.begin(), index + 1));
}

void Labels::swap(int index_0, int index_1) {
    // if (!(0 >= index_0 && index_0 >= labels.size())) return;
    // if (!(0 >= index_1 && index_1 >= labels.size())) return;

    std::iter_swap(std::next(labels.begin(), index_0), std::next(labels.begin(), index_1));
}

void Labels::save() {
    std::ofstream file(labelsDataPath);
    if (!file.is_open()) {
        return;
    }

    for (const auto& label : labels) {
        file << label.format_text << "," << label.corner << "\n";
    }
    file.close();
}

void Labels::load() {
    std::ifstream file(labelsDataPath);
    if (!file.is_open()) {
        return;
    }

    labels.clear();

    std::string line;
    while (std::getline(file, line)) {
        size_t lastCommaPos = line.find_last_of(',');

        if (lastCommaPos != std::string::npos && lastCommaPos + 1 < line.size()) {
            std::string formatText = line.substr(0, lastCommaPos);
            std::string cornerString = line.substr(lastCommaPos + 1);

            LabelCorner corner = static_cast<LabelCorner>(std::stoi(cornerString));
            Label newLabel(corner, formatText);
            labels.push_back(newLabel);
        }
    }

    file.close();
}

bool setSubColor(cocos2d::CCLabelBMFont* label, int from, int to, const cocos2d::ccColor3B &color) {
    if (!label) return false;

    cocos2d::CCArray* children = label->getChildren();
    unsigned int size = children->count();

    if (from < 0 || to >= size || from > to) return false;

    for (int i = from; i <= to; ++i) {
        cocos2d::CCSprite* sprite = static_cast<cocos2d::CCSprite*>(children->objectAtIndex(i));
        if (sprite) {
            sprite->setColor(color);
        }
    }
    
    return true;
}

void Labels::initMobileContext(geode::ScrollLayer* scrollLayer) {
    scrollLayer->m_contentLayer->removeAllChildren();

    for (size_t index = 0; index < labels.size(); index++) {
        Label& item = labels[index];
        auto labelMenu = CCMenu::create();
        labelMenu->setContentSize({325.f, 40.f});
        
        TextInput* replay_name_input = TextInput::create(195.f, "", "chatFont.fnt");
        replay_name_input->setPosition({105.f, 20.f});
        replay_name_input->setString(item.format_text);
        replay_name_input->setFilter("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~ ");
        replay_name_input->setCallback([&item](const std::string& text) {
            item.format_text = text;
        });
        labelMenu->addChild(replay_name_input);

        auto labelUp = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
        labelUp->setScale(0.65f);
        labelUp->setRotation(90.f);
        auto labelUpClick = CCMenuItemExt::createSpriteExtra(labelUp, [this, index, scrollLayer](CCMenuItemSpriteExtra* sender) {
            move_up(index);
            initMobileContext(scrollLayer);
        });
        labelUpClick->setPosition({220.f, 20.f});
        labelMenu->addChild(labelUpClick);

        auto labelDown = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
        labelDown->setScale(0.65f);
        labelDown->setRotation(-90.f);
        auto labelDownClick = CCMenuItemExt::createSpriteExtra(labelDown, [this, index, scrollLayer](CCMenuItemSpriteExtra* sender) {
            move_down(index);
            initMobileContext(scrollLayer);
        });
        labelDownClick->setPosition({245.f, 20.f});
        labelMenu->addChild(labelDownClick);

        auto moreSettings = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        moreSettings->setScale(0.5f);
        auto moreSettingsClick = CCMenuItemExt::createSpriteExtra(moreSettings, [this, &item](CCMenuItemSpriteExtra* sender) {
            std::array<std::string, 6> corners = {"Top Left", "Top Right", "Top", "Bottom Left", "Bottom Right", "Bottom"};
            FLAlertLayer::create("Info", fmt::format("Corner: {}", corners[(int)item.corner - 1]), "OK")->show();
        });
        moreSettingsClick->setPosition({275.f, 20.f});
        labelMenu->addChild(moreSettingsClick);

        auto deleteButton = CCSprite::createWithSpriteFrameName("GJ_resetBtn_001.png");
        deleteButton->setScale(1.f);
        auto deleteButtonClick = CCMenuItemExt::createSpriteExtra(deleteButton, [this, index, scrollLayer](CCMenuItemSpriteExtra* sender) {
            remove(index);
            initMobileContext(scrollLayer);
        });
        deleteButtonClick->setPosition({305.f, 20.f});
        labelMenu->addChild(deleteButtonClick);

        scrollLayer->m_contentLayer->addChild(labelMenu);
    }

    auto addLabel = CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png");
    auto addLabelClick = CCMenuItemExt::createSpriteExtra(addLabel, [this, scrollLayer](CCMenuItemSpriteExtra* sender) {
        LabelsCreateLayer::create(scrollLayer)->show();
    });
    addLabelClick->setPosition({15.f, 15.f});

    auto labelSettings = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    labelSettings->setScale(0.5f);
    auto labelSettingsClick = CCMenuItemExt::createSpriteExtra(labelSettings, [this](CCMenuItemSpriteExtra* sender) {
        auto popup = popupSystem::create();
        auto& config = Config::get();

        popup->AddText("Opacity (0 - 1.0):", 0.35f);
        popup->AddFloatInput("Opacity", opacity, [this, &config](float value) 
        {
            value = std::clamp(value, 0.f, 1.f);
            opacity = value;
            config.set<float>("label-opacity", value);
        }, 30.f);
        
        popup->AddText("Size:", 0.35f);
        popup->AddFloatInput("Size", size, [this, &config](float value) 
        {
            size = value;
            config.set<float>("label-size", value);
        }, 30.f);     

        popup->AddText("Label Padding:", 0.35f);
        popup->AddFloatInput("Label Padding", padding, [this, &config](float value) 
        {
            padding = value;
            config.set<float>("label-padding", value);
        }, 30.f);

        popup->show();
    });
    labelSettingsClick->setPosition({40.f, 15.f});

    auto infoButton = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    infoButton->setScale(0.75f);
    auto infoButtonClick = CCMenuItemExt::createSpriteExtra(infoButton, [this](CCMenuItemSpriteExtra* sender) {
        FLAlertLayer::create("Info",
        "<cg>{time:12}</c> - time 12-hour\n"
        "<cg>{time:24}</c> - time 24-hour\n"
        "<cg>{attempt}</c> - attempt\n"
        "<cg>{sessionTime}</c> - session time\n"
        "<cg>{progress}</c> - progress %\n"
        "<cg>{clicks}</c> - clicks\n"
        "<cg>{cps}</c> - cps counter\n"
        "<cg>{cpsHigh}</c> - cps counter highscore"
        , "OK")->show();
    });
    infoButtonClick->setPosition({65.f, 15.f});

    auto infoButton2Click = CCMenuItemExt::createSpriteExtra(infoButton, [this](CCMenuItemSpriteExtra* sender) {
        FLAlertLayer::create("Info",
        "<cg>{levelName}</c> - level name\n"
        "<cg>{levelCreator}</c> - creator name\n"
        "<cg>{levelId}</c> - level id\n"
        "<cg>{noclipAccuracy}</c> - noclip acc %\n"
        "<cg>{deaths}</c> - total deaths in noclip\n"
        "<cg>{\\n}</c> - new line"
        , "OK")->show();
    });
    infoButton2Click->setPosition({90.f, 15.f});

    auto addButtonMenu = CCMenu::create();
    addButtonMenu->setContentSize({325.f, 30.f});
    addButtonMenu->addChild(addLabelClick);
    addButtonMenu->addChild(labelSettingsClick);
    addButtonMenu->addChild(infoButtonClick);
    addButtonMenu->addChild(infoButton2Click);
    scrollLayer->m_contentLayer->addChild(addButtonMenu);

    scrollLayer->m_contentLayer->updateLayout();
    scrollLayer->moveToTop();
}

LabelsCreateLayer* LabelsCreateLayer::create(geode::ScrollLayer* scrollLayer) {
    auto ret = new LabelsCreateLayer();
    if (ret->initAnchored(360.f, 260.f, "GJ_square01.png")) {
        ret->autorelease();
        ret->m_scrollLayer = scrollLayer;
        return ret;
    }

    delete ret;
    return nullptr;
}

bool LabelsCreateLayer::setup() {
    std::array<std::string, 6> corners = {"Top Left", "Top Right", "Top", "Bottom Left", "Bottom Right", "Bottom"};
    std::array<std::string, 10> label_types = {"Time (24H)", "Time (12H)", "Session Time", "Level Progress", "Attempt", "CPS Counter", "Level Info", "Noclip Accuracy", "Death Counter", "Custom Text"};
    
    this->setTitle("Add Label");

    auto buttons = CCMenu::create();
    buttons->setPosition({0,0});

    float positionY = 210.f;

    for (int i = 0; i < corners.size(); i++) {
        auto cornerToggle = CCMenuItemExt::createTogglerWithStandardSprites(0.75f, [this, i](CCMenuItemToggler* sender) {
            for (int i = 0; i < m_toggles.size(); i++) {
                m_toggles[i]->toggle(false);
            }
            m_toggleIndex = i + 1;
        });
        if (i == 0) cornerToggle->toggle(true);
        cornerToggle->setPosition({25.f, positionY});
        m_toggles.push_back(cornerToggle);
        buttons->addChild(cornerToggle);

        auto label = CCLabelBMFont::create(corners[i].c_str(), "bigFont.fnt");
        label->setAnchorPoint({0.f, 0.5f});
        label->setPosition({cornerToggle->getPositionX() + 15.f, cornerToggle->getPositionY()});
        label->setScale(0.5f);
        buttons->addChild(label);

        positionY -= 25.f;
    }

    auto createButton = ButtonSprite::create("Create", 60, true, "bigFont.fnt", "GJ_button_01.png", 30.f, 0.7f);
    auto createButtonClick = CCMenuItemExt::createSpriteExtra(createButton, [this](CCMenuItemSpriteExtra* sender) {
        std::string text;
        if (m_labelTypeIndex == 0) text = "{time:24}";
        else if (m_labelTypeIndex == 1) text = "{time:12}";
        else if (m_labelTypeIndex == 2) text = "Session Time: {sessionTime}";
        else if (m_labelTypeIndex == 3) text = "{progress}";
        else if (m_labelTypeIndex == 4) text = "Attempt {attempt}";
        else if (m_labelTypeIndex == 5) text = "{cps}/{cpsHigh}/{clicks}";
        else if (m_labelTypeIndex == 6) text = "{levelName}{byLevelCreator} ({levelId})";
        else if (m_labelTypeIndex == 7) text = "{noclipAccuracy}";
        else if (m_labelTypeIndex == 8) text = "{deaths} Deaths";
        else if (m_labelTypeIndex == 9) text = "here your text";
        
        auto& labels = Labels::get();
        Label l((LabelCorner) (m_toggleIndex), text);
        labels.add(l);

        labels.initMobileContext(m_scrollLayer);

        m_closeBtn->activate();
    });
    createButtonClick->setPosition({50.f, positionY - 25.f});
    buttons->addChild(createButtonClick);

    auto background = extension::CCScale9Sprite::create("square02_small.png");
    background->setPosition({255.f, 125.f});
    background->setContentSize({180.f, 200.f});
    background->setOpacity(100);
    buttons->addChild(background);

    auto scrollLayer = ScrollLayer::create({180.f, 200.f});
    scrollLayer->m_contentLayer->setLayout(
        geode::ColumnLayout::create()
            ->setAutoScale(false)
            ->setAxisReverse(true)
            ->setAutoGrowAxis(scrollLayer->getContentHeight())
            ->setAxisAlignment(geode::AxisAlignment::End)
            ->setGap(0)
    );
    scrollLayer->setPosition({165.f, 25.f});
    scrollLayer->m_peekLimitTop = 15;
    scrollLayer->m_peekLimitBottom = 15;
    buttons->addChild(scrollLayer);
    
    for (int i = 0; i < label_types.size(); i++) {
        auto toggleMenu = CCMenu::create();
        toggleMenu->setContentSize({180.f, 25.f});
        auto levelTypeToggle = CCMenuItemExt::createTogglerWithStandardSprites(0.75f, [this, i](CCMenuItemToggler* sender) {
            for (int i = 0; i < m_labelTypeToggles.size(); i++) {
                m_labelTypeToggles[i]->toggle(false);
            }
            m_labelTypeIndex = i;
        });
        if (i == 0) levelTypeToggle->toggle(true);
        levelTypeToggle->setPosition({15.f, 10});
        m_labelTypeToggles.push_back(levelTypeToggle);
        toggleMenu->addChild(levelTypeToggle);

        auto label = CCLabelBMFont::create(label_types[i].c_str(), "bigFont.fnt");
        label->setAnchorPoint({0.f, 0.5f});
        label->setPosition({levelTypeToggle->getPositionX() + 15.f, levelTypeToggle->getPositionY()});
        label->setScale(0.5f);
        toggleMenu->addChild(label);

        scrollLayer->m_contentLayer->addChild(toggleMenu);
    }

    scrollLayer->m_contentLayer->updateLayout();  
    scrollLayer->moveToTop();

    m_mainLayer->addChild(buttons);

    return true;
}