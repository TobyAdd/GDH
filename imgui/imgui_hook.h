/*
 * -----------------------------------------------------------------------------
 * Universal OpenGL2 ImGui Hook using Kiero by Adafcaefc
 * -----------------------------------------------------------------------------
 *
 * This project is inspired by rdbo's Universal DirectX 11 ImGui Hook using Kiero
 * https://github.com/rdbo/ImGui-DirectX-11-Kiero-Hook
 *
 * This project is using a modified version of Rebzzel's Kiero
 * https://github.com/Rebzzel/kiero
 *
 * This project is using ImGui as it's main GUI library
 * https://github.com/ocornut/imgui
 *
 * Special thanks to Sleepyut for helping me with the context rendering issue
 *
 * -----------------------------------------------------------------------------
 * Requirements
 * -----------------------------------------------------------------------------
 *
 * Visual Studio        https://visualstudio.microsoft.com/
 * Windows OpenGL SDK   https://www.microsoft.com/en-us/download/details.aspx?id=8279
 *
 */

#pragma once
#include <Windows.h>
#include <functional>

namespace ImGuiHook
{
	bool Load(std::function<void()> render);
	void Unload();
}
