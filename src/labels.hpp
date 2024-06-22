#include "includes.hpp"
#include "noclip_accuracy.hpp"
#include "cps_counter.hpp"

namespace labels {
    extern bool time24_enabled;
    extern bool time12_enabled;
    extern bool noclip_accuracy_enabled;
    extern bool cps_counter_enabled;
    extern bool death_enabled;
    extern bool custom_text_enabled;
    extern std::string custom_text;

    extern int pos; //0 - lefttop; 1 - topbottom; 2 - righttop; 3 - rightbottom

    std::string get_labels();
}