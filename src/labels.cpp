#include <chrono>
#include <iomanip>
#include <sstream>
#include "labels.hpp"
#include "popupSystem.hpp"
#include "replayEngine.hpp"
#include "hooks.hpp"
#include <algorithm>
#include "hacks.hpp"

using namespace geode::prelude;

Label::Label(LabelCorner _corner, std::string _format_text) {
    format_text = _format_text;
    corner = _corner;
}

std::string Label::get_text() {
    auto& hacks = Hacks::get();
    auto& engine = ReplayEngine::get();
    auto now = std::chrono::system_clock::now();
    auto steady_now = std::chrono::steady_clock::now();
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
    result = replace_all(result, "{attempt}", std::to_string(pl->m_attempts));
    result = replace_all(result, "{fps}", fmt::format("{:.0f}", Labels::get().fps));
    float session_time_seconds = std::chrono::duration<float>(steady_now - Labels::get().session_time).count();
    result = replace_all(result, "{sessionTime}", seconds_to_fmt_time(session_time_seconds));    
    for (int i = 0; i <= 6; ++i) {
        std::string placeholder = fmt::format("{{progress:{}f}}", i);
        std::string replacement = platformer ? seconds_to_fmt_time(percentage) : fmt::format("{:.{}f}%", percentage, i);
        result = replace_all(result, placeholder, replacement);
    }    
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
    result = replace_all(result, "{testmode}", pl->m_isTestMode ? "Testmode" : "");
    result = replace_all(result, "{cheat_indicator}", fmt::format("{}+{}", 
        (hacks.cheatState == legit) ? "<cg>" : (hacks.cheatState == unwanted) ? "<co>" : (hacks.cheatState == safe_mode) ? "<cy>" : (hacks.cheatState == cheating) ? "<cr>" : "",
        (hacks.cheatState == legit) ? "<cg/>" : (hacks.cheatState == unwanted) ? "<co/>" : (hacks.cheatState == safe_mode) ? "<cy/>" : (hacks.cheatState == cheating) ? "<cr/>" : ""));
    result = replace_all(result, "{re_state}", engine.mode == state::disable ? "<co>Disable<co/>" : engine.mode == state::record ? "<cr>Record<cr/>" : "<cg>Play<cg/>");
    
    static geode::Mod* cbfMod = geode::Loader::get()->getLoadedMod("syzzi.click_between_frames");
    static bool hasCBF = cbfMod != nullptr && cbfMod->isEnabled();
    result = replace_all(result, "{cbf_enabled}", hasCBF ? "<cg>Enabled<cg/>" : "<cr>Disabled<cr/>");
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

        auto text = l.get_text();

        result += processSpecialText(text);
        if (!text.empty()) result += "\n";
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

void Labels::setStringColored(cocos2d::CCLabelBMFont* label, std::string format_text) {
    if (!label) return;
    auto label_children = label->getChildren();
    std::vector<cocos2d::ccColor3B> colors(label_children->count(), {255, 255, 255});
    std::string parsed_text;
    const cocos2d::ccColor3B defaultColor = {255, 255, 255};
    cocos2d::ccColor3B currentColor = defaultColor;
    size_t index = 0;
    
    const std::unordered_map<std::string, cocos2d::ccColor3B> tagColors = {
        {"cr", {255, 0, 64}},      // Red
        {"cg", {0, 255, 0}},       // Green
        {"co", {255, 106, 0}},     // Orange
        {"cy", {255, 242, 0}},     // Yellow
        {"cb", {0, 128, 255}},     // Blue
        {"cp", {170, 0, 255}},     // Purple
        {"cw", {255, 255, 255}},   // White
        {"ck", {0, 0, 0}},         // Black
        {"cm", {255, 0, 255}},     // Magenta
        {"cc", {0, 255, 255}},     // Cyan
        {"cs", {192, 192, 192}},   // Silver
        {"cl", {128, 0, 255}},     // Lavender
        {"cn", {255, 192, 203}},   // Pink
        {"ca", {255, 215, 0}},     // Amber/Gold
        {"cd", {139, 69, 19}}      // Brown
    };    
    
    for (size_t i = 0; i < format_text.size(); i++) {
        if (format_text[i] == '<' && i + 2 < format_text.size()) {
            std::string potentialTag = format_text.substr(i + 1, 2);
            
            bool isClosingTag = false;
            if (i + 4 < format_text.size() && format_text[i + 3] == '/' && format_text[i + 4] == '>') {
                isClosingTag = true;
            }
            
            bool isOpeningTag = !isClosingTag && i + 3 < format_text.size() && format_text[i + 3] == '>';
            
            auto tagIt = tagColors.find(potentialTag);
            if (tagIt != tagColors.end()) {
                if (isOpeningTag) {
                    currentColor = tagIt->second;
                    i += 3;
                    continue;
                } else if (isClosingTag) {
                    currentColor = defaultColor;
                    i += 4;
                    continue;
                }
            }
        }
        
        if (format_text[i] == '\n') {
            parsed_text += '\n';
        } else {
            parsed_text += format_text[i];
            if (index < colors.size()) {
                colors[index] = currentColor;
                index++;
            }
        }
    }
    
    label->removeAllChildren();
    label->setCString(parsed_text.c_str());
    for (size_t i = 0; i < colors.size(); i++) {
        auto child = label_children->objectAtIndex(i);
        if (auto bmFont = geode::prelude::typeinfo_cast<cocos2d::CCSprite*>(child)) {
            bmFont->setColor(colors[i]);
        }
    }
}

std::string Labels::processSpecialText(std::string text) {
    const std::string cpsTag = "ColoredCPS(";
    size_t start = text.find(cpsTag);
    if (start != std::string::npos) {
        size_t contentStart = start + cpsTag.length();
        size_t end = text.find(')', contentStart);
        
        if (end != std::string::npos) {
            std::string content = text.substr(contentStart, end - contentStart);
            std::string replacement = push ? "<cg>" + content + "<cg/>" : content;
            text.replace(start, end - start + 1, replacement);
        }
    }
    
    const std::string deathTag = "ColoredDeath(";
    start = text.find(deathTag);
    if (start != std::string::npos) {
        size_t contentStart = start + deathTag.length();
        size_t end = text.find(')', contentStart);
        
        if (end != std::string::npos) {
            std::string content = text.substr(contentStart, end - contentStart);
            std::string replacement = NoclipAccuracy::get().prevDied ? "<cr>" + content + "<cr/>" : content;
            text.replace(start, end - start + 1, replacement);
        }
    }
    
    return text;
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
        "<cg>{fps}</c> - fps counter\n"
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
    std::array<std::string, 15> label_types = {"Time (24H)", "Time (12H)", "Session Time", "Cheat Indicator", "FPS Counter", "Level Progress", "Attempt", "CPS Counter", "Level Info", "Noclip Accuracy", "Death Counter", "Testmode", "Replay Engine State", "CBF Status", "Custom Text"};
    
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
        else if (m_labelTypeIndex == 3) {
            text = "{cheat_indicator}";
            Config::get().set<bool>("cheat_indicator", true);
        }
        else if (m_labelTypeIndex == 4) text = "{fps} FPS";
        else if (m_labelTypeIndex == 5) text = "{progress:2f}";
        else if (m_labelTypeIndex == 6) text = "Attempt {attempt}";
        else if (m_labelTypeIndex == 7) text = "ColoredCPS({cps}/{cpsHigh}/{clicks})";
        else if (m_labelTypeIndex == 8) text = "{levelName}{byLevelCreator} ({levelId})";
        else if (m_labelTypeIndex == 9) text = "ColoredDeath({noclipAccuracy})";
        else if (m_labelTypeIndex == 10) text = "ColoredDeath({deaths} Deaths)";
        else if (m_labelTypeIndex == 11) text = "{testmode}";
        else if (m_labelTypeIndex == 12) text = "{re_status}";
        else if (m_labelTypeIndex == 13) text = "CBF: {cbf_status}";
        else if (m_labelTypeIndex == 14) text = "here your text";
        
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