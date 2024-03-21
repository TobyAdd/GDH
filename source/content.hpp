#pragma once
#include "includes.hpp"
#include <fstream>
#include <iterator>
#include <filesystem>

namespace Content {
    extern json content;
    bool load(std::string path);
    void save();
    void processCheckboxHack(const json &component);
}