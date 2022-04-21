#include "GameManager.h"
#include <Windows.h>

void GameManager::mem_init() {
	size_t base = reinterpret_cast<size_t>(GetModuleHandle(0));
	getSharedState = reinterpret_cast<decltype(getSharedState)>(base + 0xC4A50);
	getGameVariable = reinterpret_cast<decltype(getGameVariable)>(base + 0xC9D30);
}