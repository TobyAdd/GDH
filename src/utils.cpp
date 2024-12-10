#include "utils.hpp"

bool utils::isNumeric(const std::string& str) {
    bool hasDot = false;
    bool hasMinus = false;

    for (size_t i = 0; i < str.size(); ++i) {
        char ch = str[i];

        if (ch == '-') {
            if (hasMinus || i != 0) {
                return false;
            }
            hasMinus = true;
        } else if (ch == '.') {
            if (hasDot || i == 0 || i == str.size() - 1) {
                return false;
            }
            hasDot = true;
        } else if (!std::isdigit(static_cast<unsigned char>(ch))) {
            return false;
        }
    }

    return !str.empty();
}