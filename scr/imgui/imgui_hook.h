#pragma once
#include <Windows.h>
#include <functional>

namespace ImGuiHook
{
	bool Load(std::function<void()> render);
	void Unload();
}
