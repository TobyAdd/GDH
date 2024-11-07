#include "labels.hpp"
#include "replayEngine.hpp"

std::vector<labels::Label> labels::labels_top_left = {};
std::vector<labels::Label> labels::labels_top_right = {};
std::vector<labels::Label> labels::labels_bottom_left = {};
std::vector<labels::Label> labels::labels_bottom_right = {};
std::vector<labels::Label> labels::labels_bottom = {};
std::vector<labels::Label> labels::labels_top = {};

float labels::label_size = 0.4f;
float labels::label_opacity = 0.5f;
float labels::label_padding = 5.f;
float labels::start_percent = 150;
float labels::best_percent = 5.f;

const char* labels::label_types[COUNT_LABELS] = {
    "Time (24h)",
    "Time (12h)",
    "Noclip Accuracy",
    "CPS Counter",
    "Death Counter",
    "Attempt Counter",
    "Session Time",
    "Level Progress",
    "Level Info",
    "Best Run",
    "Replay Engine Meta",
    "Custom Text"
};

const char* labels::label_slugs[COUNT_LABELS] = {
    "time24",
    "time12",
    "noclip_accuracy",
    "cps_counter",
    "death_counter",
    "attempt_counter",
    "session_time",
    "level_progress",
    "level_info",
    "best_run",
    "re_meta",
    "custom_text"
};

// i couldnt figure out where to put these so here they are
int labels::attempts = 0;
float labels::attempt_time = 0;
float labels::progress = 0;
bool labels::platformer = false;
std::string labels::level_name = {};
std::string labels::level_creator = {};
float labels::player1_x = 0;
float labels::player1_y = 0;
float labels::player2_x = 0;
float labels::player2_y = 0;
bool labels::gravity_p1_flipped = false;
bool labels::gravity_p2_flipped = false;
bool labels::two_players = false;

std::string time_to_fmt_time(int hours, int minutes, int seconds) {
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << hours << ":" << std::setw(2) << minutes << ":" << std::setw(2) << seconds;
    return ss.str();
}

std::string seconds_to_fmt_time(float seconds) {
    int time = (int) seconds;
    int minutes = seconds / 60;
    int hour = minutes / 60;
    return time_to_fmt_time(hour % 60, minutes % 60, time % 60);
}

std::string round_float(float value, int decimal_places) {
    std::stringstream fmtFloat;
    fmtFloat << std::fixed << std::setprecision(decimal_places) << value;
    return fmtFloat.str();
}

std::string labels::get_label_string_repr(labels::Label const& label) {
    if (label.type == labels::LABEL_TIME12) {
        SYSTEMTIME localTime;
        GetLocalTime(&localTime);

        int hour = localTime.wHour;
        std::string period = (hour < 12) ? "AM" : "PM";
        if (hour == 0) hour = 12;
        if (hour > 12) hour -= 12;
        
        return time_to_fmt_time(hour, localTime.wMinute, localTime.wSecond) + " " + period;
	
    } else if (label.type == labels::LABEL_TIME24) {
        SYSTEMTIME localTime;
        GetLocalTime(&localTime);
	
        return time_to_fmt_time(localTime.wHour, localTime.wMinute, localTime.wSecond);
	
    } else if (label.type == labels::LABEL_NOCLIP_ACCURACY) {
       return round_float(noclip_accuracy.getPercentage(), 2) + "%";

    } else if (label.type == labels::LABEL_CPS_COUNTER) {
       cps_counter.update();
       return std::to_string(cps_counter.cps) + "/" + std::to_string(cps_counter.overall) + "/" + std::to_string(cps_counter.highscore);
	
    } else if (label.type == labels::LABEL_DEATH_COUNTER) {
	return std::to_string(noclip_accuracy.deaths_full) + " Deaths";
        
    } else if (label.type == labels::LABEL_ATTEMPTS) {
	return "Attempt " + std::to_string(labels::attempts);
        
    } else if (label.type == labels::LABEL_ATTEMPT_TIME) {
	 return "Session: " + seconds_to_fmt_time(labels::attempt_time);
        
    } else if (label.type == labels::LABEL_PROGRESS) {
        if (labels::platformer) return seconds_to_fmt_time(labels::progress) + "." + std::to_string((int)((labels::progress - std::floor(labels::progress))*100.f));
        else return round_float(labels::progress, 2) + "%";
        
    } else if (label.type == labels::LABEL_LEVEL_INFO) {
        if (labels::level_creator.length() == 0) return labels::level_name;
	 else return labels::level_name + " by " + labels::level_creator;
        
    } else if (label.type == labels::LABEL_BEST_RUN) {
        if (labels::start_percent == labels::best_percent) return "Best Run: None"; 
        if (labels::start_percent == 0) return "Best Run: " + round_float(labels::best_percent, 2) + "%";
        return "Best Run: " + round_float(labels::start_percent, 2) + "-" + round_float(labels::best_percent, 2) + "%";
        
    } else if (label.type == labels::LABEL_RE_META) {
        unsigned int frame = engine.get_frame();
        std::string str = {};
        str += "Frame: " + std::to_string(frame) + "\n";
        if (labels::two_players) {
            str += "P1 X: " + std::to_string(labels::player1_x) +
                 "\nP1 Y: " + std::to_string(labels::player1_y) +
                 "\nP1 G: " + (std::string) (labels::gravity_p1_flipped ? "Flipped" : "Normal") + "\n";
            str += "P2 X: " + std::to_string(labels::player2_x) +
                 "\nP2 Y: " + std::to_string(labels::player2_y) +
                 "\nP2 G: " + (std::string) (labels::gravity_p2_flipped ? "Flipped" : "Normal");
        } else {
            str += "X: " + std::to_string(labels::player1_x) +
                 "\nY: " + std::to_string(labels::player1_y) + "\n";
            str += "G: " + (std::string) (labels::gravity_p1_flipped ? "Flipped" : "Normal");
        }
        return str;
        
    } else if (label.type == labels::LABEL_CUSTOM_TEXT) {
	 return label.text;
        
    }
    return (std::string) "unreachable";
}

std::string labels::get_labels(int position) {
    std::string label;
    std::vector<labels::Label> label_vector;
    
    if (position == 0) label_vector = labels_top_left;
    if (position == 1) label_vector = labels_top_right;
    if (position == 2) label_vector = labels_bottom_left;
    if (position == 3) label_vector = labels_bottom_right;
    if (position == 4) label_vector = labels_bottom;
    if (position == 5) label_vector = labels_top;
    
    for (auto& l : label_vector) {
	label += labels::get_label_string_repr(l);
	label += "\n";
    }
    
    return label;
}
