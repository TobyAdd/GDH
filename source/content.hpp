#pragma once
#include "includes.hpp"
#include <fstream>
#include <iterator>
#include <filesystem>

namespace Content {
    extern json content;
    bool load();
    void save();
    void processCheckboxHack(const json &component);
    void processOpcode(json &opcode, bool enabled);
}