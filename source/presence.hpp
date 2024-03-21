#pragma once

#include "includes.hpp"
#include "gd.hpp"
#include "hacks.hpp"

enum class Status {
    IN_LEVEL,
    IN_EDITOR,
    IN_MENU
};

namespace presence {
    static const char* APPLICATION_ID = "1055528380956672081";

    extern std::string player_name;
    extern long long rpcStartTime;

    void InitPresence();
    void UpdatePresence(Status status, gd::GJGameLevel* m_lvl = nullptr);
}