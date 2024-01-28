#pragma once
#include "includes.hpp"

namespace VkCodes {
    std::string key_name(uint32_t keycode);
    uint32_t decode_key(std::string key);
}
