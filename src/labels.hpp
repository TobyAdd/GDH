#pragma once

#include "config.hpp"

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

        int attempts = 1;
        float session_time = 0.f;
        float progress = 0.f;
        bool platformer = false;
        
        void save();
        void load();

    private:
        Labels() = default;
};

class CpsCounter {
    public:
        static CpsCounter& get() {
            static CpsCounter instance;
            return instance;
        }

        CpsCounter& operator=(const CpsCounter&) = delete;
        CpsCounter(const CpsCounter&) = delete;

        std::vector<DWORD>
        void click();
        
    private:
        CpsCounter() = default;
};
