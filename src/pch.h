#ifndef PCH_H
#define PCH_H

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define GAME_NAME "GeometryDash.exe"
#define GAME_WINDOW "Geometry Dash"

#include <windows.h>
#include <imgui-hook.hpp>
#include <imgui.h>
#include <cocos2d.h>
#include <MinHook.h>
#include <gd.h>
#include <psapi.h>
#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <shellapi.h>
#include "json.hpp"
#include <fstream>
#include <mutex>
#include "memory.h"
#include "uselessShit.h"
#include <filesystem>
#include <regex>

using namespace cocos2d;
using namespace nlohmann;

#endif