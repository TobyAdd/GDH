#pragma once
#include "includes.hpp"
#include "noclip_accuracy.hpp"
#include "cps_counter.hpp"

namespace labels {
    typedef enum {
	LABEL_TIME24 = 0,
	LABEL_TIME12,
	LABEL_NOCLIP_ACCURACY,
	LABEL_CPS_COUNTER,
	LABEL_DEATH_COUNTER,
        LABEL_ATTEMPTS,
	LABEL_ATTEMPT_TIME,
	LABEL_PROGRESS,
        LABEL_LEVEL_INFO,
        LABEL_BEST_RUN,
        LABEL_RE_META,
	LABEL_CUSTOM_TEXT,
	COUNT_LABELS
    } LabelType;
    
    typedef struct {
	LabelType type;
	std::string text;
    } Label;

    extern std::vector<Label> labels_top_left;
    extern std::vector<Label> labels_top_right;
    extern std::vector<Label> labels_bottom_left;
    extern std::vector<Label> labels_bottom_right;
    extern std::vector<Label> labels_bottom;
    extern std::vector<Label> labels_top;

    extern const char* label_types[COUNT_LABELS];

    extern float label_size;
    extern int label_opacity;
    extern float label_padding;

    extern float start_percent;
    extern float best_percent;

    extern int attempts;
    extern float attempt_time;
    extern float progress;
    extern bool platformer;
    extern std::string level_name;
    extern std::string level_creator;
    extern float player1_x;
    extern float player1_y;
    extern float player2_x;
    extern float player2_y;
    extern bool gravity_p1_flipped;
    extern bool gravity_p2_flipped;
    extern bool two_players;

    std::string get_label_string_repr(Label const& label);
    std::string get_labels(int position);
}

