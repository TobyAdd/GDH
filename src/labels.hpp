#pragma once

#include "config.hpp"
#include <algorithm>
#include <chrono>

enum LabelCorner {
    LabelCorner_None = 0,
    LabelCorner_TopLeft,
    LabelCorner_TopRight,
    LabelCorner_Top,
    LabelCorner_BottomLeft,
    LabelCorner_BottomRight,
    LabelCorner_Bottom,
};

class Label {
    public:
        std::string format_text;
        LabelCorner corner;
        
        Label(LabelCorner _corner, std::string _format_text);

        std::string get_text();

    private:        
        std::string replace_all(std::string src, std::string replace, std::string replacement);
        std::string time_to_fmt_time(int hours, int minutes, int seconds);
        std::string seconds_to_fmt_time(float seconds);
        std::string round_float(float value, int decimal_places);
};

class Labels {
    public:
        static Labels& get() {
            static Labels instance;
            return instance;
        }

        Labels& operator=(const Labels&) = delete;
        Labels(const Labels&) = delete;

        std::vector<Label> labels = {};
        float size = Config::get().get<float>("label-size", 0.3f);
        float opacity = Config::get().get<float>("label-opacity", 1.f);
        float padding = Config::get().get<float>("label-padding", 3.f);
        
        std::string get_label_string(LabelCorner corner);

        void add(Label label);
        void remove(int index);
        void move_up(int index);
        void move_down(int index);
        void swap(int index_0, int index_1);

        std::chrono::steady_clock::time_point session_time;
        float progress = 0.f;
        bool platformer = false;
        bool push = false;
        
        void save();
        void load();

        void setStringColored(cocos2d::CCLabelBMFont* label, std::string format_text);

        void initMobileContext(geode::ScrollLayer* scrollLayer);
    private:
        Labels() = default;

        std::string processSpecialText(std::string text);
        
};

class LabelsCreateLayer : public geode::Popup<> {
private:
    std::vector<CCMenuItemToggler*> m_toggles;
    std::vector<CCMenuItemToggler*> m_labelTypeToggles;
    int m_toggleIndex = 1;
    int m_labelTypeIndex = 0;

    geode::ScrollLayer* m_scrollLayer;
public:
    static LabelsCreateLayer* create(geode::ScrollLayer* scrollLayer); 
    bool setup();
};

class NoclipAccuracy {
public:    
    static NoclipAccuracy& get() {
        static NoclipAccuracy instance;
        return instance;
    }

    NoclipAccuracy& operator=(const NoclipAccuracy&) = delete;
    NoclipAccuracy(const NoclipAccuracy&) = delete;

    bool wouldDie = false;
    bool prevDied = false;
    int frames = 0;
    int deaths = 0;
    int deaths_full = 0;

    void handle_update(GJBaseGameLayer* self, float delta) {
        auto pl = PlayLayer::get();
        if (!pl) return;

        if (!self->m_player1->m_isDead && !pl->m_levelEndAnimationStarted)
            frames += 1;

        if (wouldDie && !self->m_player1->m_isDead && !pl->m_levelEndAnimationStarted) {
            wouldDie = false;
            deaths += 1;

            if (!prevDied)
                deaths_full += 1;
            prevDied = true;
        } else
            prevDied = false;
    }

    void handle_reset(PlayLayer* self) {
        frames = 0;
        deaths = 0;
        deaths_full = 0;
        wouldDie = false;
    }

    void handle_death() {
        wouldDie = true;
    }

    float getPercentage() {
        if (frames == 0) {
            return 100.f;
        }

        return (float(frames - deaths) / float(frames)) * 100.f;
    }
private:
    NoclipAccuracy() = default;
};


class CpsCounter {
public:
    static CpsCounter& get() {
        static CpsCounter instance;
        return instance;
    }

    CpsCounter& operator=(const CpsCounter&) = delete;
    CpsCounter(const CpsCounter&) = delete;

    std::vector<std::chrono::steady_clock::time_point> clicks = {};
    int cps = 0, highscore = 0, overall = 0;
    
    void reset() {
        cps = 0;
        highscore = 0;
        overall = 0;
        clicks.clear();
    }
    
    void click() {
        auto now = std::chrono::steady_clock::now();
        overall++;
        clicks.push_back(now);
    }
    
    void update() {
        auto now = std::chrono::steady_clock::now();
        auto one_second_ago = now - std::chrono::seconds(1);

        clicks.erase(std::remove_if(clicks.begin(), clicks.end(),
                                    [one_second_ago](const auto& clickTime) {
                                        return clickTime < one_second_ago;
                                    }),
                     clicks.end());

        cps = clicks.size();

        if (highscore < cps) {
            highscore = cps;
        }
    }

private:
    CpsCounter() = default;
};

class RGBIcons {
public:
    static RGBIcons& get() {
        static RGBIcons instance;
        return instance;
    }

    RGBIcons& operator=(const RGBIcons&) = delete;
    RGBIcons(const RGBIcons&) = delete;

    std::vector<cocos2d::ccColor3B> colors;

    std::vector<cocos2d::ccColor3B> rainbowColors = {
        cocos2d::ccColor3B(255, 0, 0),
        cocos2d::ccColor3B(255, 127, 0),
        cocos2d::ccColor3B(255, 255, 0),
        cocos2d::ccColor3B(0, 255, 0),
        cocos2d::ccColor3B(0, 191, 255),
        cocos2d::ccColor3B(0, 0, 255),
        cocos2d::ccColor3B(139, 0, 255)
    };

    cocos2d::ccColor3B interpolateColor(float dt, bool inverse = false) {
        std::vector<cocos2d::ccColor3B> _colors = colors;

        if (_colors.empty())
            return cocos2d::ccColor3B(0, 0, 0);

        if (inverse)
            std::reverse(_colors.begin(), _colors.end());

        size_t num_colors = _colors.size();
        dt = fmod(dt, 1.0f);
        float scaledDt = dt * (num_colors);
        size_t index1 = static_cast<size_t>(scaledDt);
        size_t index2 = (index1 + 1) % num_colors;
        float blend = scaledDt - index1;

        cocos2d::ccColor3B color1 = _colors[index1];
        cocos2d::ccColor3B color2 = _colors[index2];

        uint8_t r = static_cast<GLubyte>(color1.r + (color2.r - color1.r) * blend);
        uint8_t g = static_cast<GLubyte>(color1.g + (color2.g - color1.g) * blend);
        uint8_t b = static_cast<GLubyte>(color1.b + (color2.b - color1.b) * blend);

        return cocos2d::ccColor3B(r, g, b);
    }

    void save() {
        std::ofstream outFile(colorsDataPath);

        if (outFile.is_open()) {
            for (const auto& color : colors) {
                outFile << static_cast<int>(color.r) << " "
                        << static_cast<int>(color.g) << " "
                        << static_cast<int>(color.b) << "\n";
            }
            outFile.close();
        }
    }

    void load() {
        std::ifstream inFile(colorsDataPath);

        if (inFile.is_open()) {
            std::string line;
            while (std::getline(inFile, line)) {
                std::stringstream ss(line);
                int r, g, b;
                if (ss >> r >> g >> b) {
                    colors.push_back(cocos2d::ccColor3B(r, g, b));
                }
            }
            inFile.close();
        }
    }
private:
    RGBIcons() = default;
};