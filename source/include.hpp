#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <MinHook.h>
#include <json.hpp>
#include "console.hpp"
#include <cocos2d.h>
#include <set>


using namespace std;

using json = nlohmann::json;

#define MEMBERBYOFFSET(type, class, offset) *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(class) + offset)
#define MBO MEMBERBYOFFSET