#include "labels.hpp"

bool labels::time24_enabled = true;
bool labels::time12_enabled = true;
bool labels::noclip_accuracy_enabled = true;
bool labels::cps_counter_enabled = true;
bool labels::death_enabled = true;
bool labels::custom_text_enabled = true;
std::string labels::custom_text = "test";
int labels::pos = 0;

std::string labels::get_labels() {
    std::string label;

    if (custom_text_enabled) {
        label += custom_text + "\n";
    }

    std::stringstream ss;

    if (time24_enabled) {
        SYSTEMTIME localTime;
        GetLocalTime(&localTime);
        ss << std::setfill('0') << std::setw(2) << localTime.wHour << ":" 
        << std::setw(2) << localTime.wMinute << ":" << std::setw(2) << localTime.wSecond << "\n";
        label += ss.str(); // Append formatted time to label
        ss.str(""); // Clear stringstream for reuse
    }

    if (time12_enabled) {
        SYSTEMTIME localTime;
        GetLocalTime(&localTime);

        int hour = localTime.wHour;
        std::string period = (hour < 12) ? "AM" : "PM";
        if (hour == 0) {
            hour = 12;
        } else if (hour > 12) {
            hour -= 12;
        }

        ss << std::setfill('0') << std::setw(2) << hour << ":" 
        << std::setw(2) << localTime.wMinute << ":" << std::setw(2) << localTime.wSecond << " " << period << "\n";
        label += ss.str(); // Append formatted time to label
    }

    if (noclip_accuracy_enabled) {
        std::stringstream formatedPercentage;
        formatedPercentage << std::fixed << std::setprecision(2) << noclip_accuracy.getPercentage();

        label += formatedPercentage.str() + "%\n";
    }

    if (cps_counter_enabled) {
        cps_counter.update();
        label += std::to_string(cps_counter.cps) + "/" + std::to_string(cps_counter.highscore) + "\n";
    }

    if (death_enabled) {
        label += std::to_string(noclip_accuracy.deaths_full) + " Deaths";
    }
    return label;
}