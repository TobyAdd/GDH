#define _CRT_SECURE_NO_DEPRECATE
#pragma once
#include "pch.h"
#include <cocos2d.h>
#include "imgui/imgui_hook.h"
#include "imgui/imgui/imgui.h"
#include <vector>
#include "imgui/kiero/minhook/include/MinHook.h"
#include <iostream>
#include <fstream>
#include "Detours/detours.h"

namespace Speedhack
{
	extern double speed;
	extern bool attatched;

	typedef DWORD(WINAPI* _tGetTickCount)(void);
	extern _tGetTickCount _GTC;
	extern DWORD _GTC_BaseTime;

	typedef ULONGLONG(WINAPI* _tGetTickCount64)(void);
	extern _tGetTickCount64 _GTC64;
	extern DWORD _GTC64_BaseTime;

	typedef BOOL(WINAPI* _tQueryPerformanceCounter)(LARGE_INTEGER*);
	extern _tQueryPerformanceCounter _QPC;
	extern LARGE_INTEGER _QPC_BaseTime;


	DWORD WINAPI _hGetTickCount();

	DWORD WINAPI _hGetTickCount64();

	DWORD WINAPI _hQueryPerformanceCounter(LARGE_INTEGER* lpPerformanceCount);


	void Setup();

	void Detach();

	void SetSpeed(float relSpeed);
}

namespace Speedhack
{
	double speed = 1.0;
	bool attatched = false;

	extern _tGetTickCount _GTC = nullptr;
	extern DWORD _GTC_BaseTime = 0;

	extern _tGetTickCount64 _GTC64 = nullptr;
	extern DWORD _GTC64_BaseTime = 0;

	extern _tQueryPerformanceCounter _QPC = nullptr;
	extern LARGE_INTEGER _QPC_BaseTime = LARGE_INTEGER();

	DWORD WINAPI _hGetTickCount()
	{
		return _GTC_BaseTime + ((_GTC() - _GTC_BaseTime) * speed);
	}

	DWORD WINAPI _hGetTickCount64()
	{
		return _GTC64_BaseTime + ((_GTC64() - _GTC64_BaseTime) * speed);
	}

	DWORD WINAPI _hQueryPerformanceCounter(LARGE_INTEGER* lpPerformanceCount)
	{
		LARGE_INTEGER x;
		_QPC(&x);
		lpPerformanceCount->QuadPart = _QPC_BaseTime.QuadPart + ((x.QuadPart - _QPC_BaseTime.QuadPart) * speed);
		return TRUE;
	}

	void Setup()
	{
		if (attatched)
			return;

		HMODULE hMod = GetModuleHandle(L"Kernel32.dll");

		if (!hMod)
			return;

		_GTC = (_tGetTickCount)GetProcAddress(hMod, "GetTickCount");
		_GTC_BaseTime = _GTC();

		_GTC64 = (_tGetTickCount64)GetProcAddress(hMod, "GetTickCount64");
		_GTC64_BaseTime = _GTC64();

		_QPC = (_tQueryPerformanceCounter)GetProcAddress(hMod, "QueryPerformanceCounter");
		_QPC(&_QPC_BaseTime);

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourAttach(&(PVOID&)_GTC, _hGetTickCount);
		DetourAttach(&(PVOID&)_GTC64, _hGetTickCount64);
		DetourAttach(&(PVOID&)_QPC, _hQueryPerformanceCounter);

		DetourTransactionCommit();

		attatched = true;
	}

	void Detach()
	{
		if (!attatched)
			return;

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourDetach(&(PVOID&)_GTC, _hGetTickCount);
		DetourDetach(&(PVOID&)_GTC64, _hGetTickCount64);
		DetourDetach(&(PVOID&)_QPC, _hQueryPerformanceCounter);

		DetourTransactionCommit();

		attatched = false;
	}

	void SetSpeed(float relSpeed)
	{
		if (attatched)
		{
			_GTC_BaseTime = _hGetTickCount();
			_GTC64_BaseTime = _hGetTickCount64();
			_hQueryPerformanceCounter(&_QPC_BaseTime);
		}

		speed = relSpeed;
	}
}

//ImGui Form Function
bool OnStartForm1 = true;
bool OnStartForm2 = true;
bool OnStartForm3 = true;
bool OnStartForm4 = true;
bool OnStartForm5 = true;
bool OnStartForm6 = true;
bool menu = true;
bool tab = true;
//ImGui Form Function

struct  {
	//hacks bool
	bool anticheat = false;
	bool noclip = false;
	bool unlockall = false;
	bool ac = false;
	bool noprogessbar = false;
	bool pmh = false;
	bool practicepulse = false;
	bool copy = false;
	bool lvledit = false;
	bool freewnd = false;
	bool NopeTranltn = false;
	bool nde = false;
	bool safemode = false;
	bool norespawnblink = false;
	bool soildwt = false;
	bool nospk = false;
	bool nohitbox = false;
	bool nosoilds = false;
	bool fbt = false;
	bool eh = false;
	bool imp = false;
	bool fz = false;
	bool jh = false;
	bool trailoff = false;
	bool trailon = false;
	bool it = false;
	bool ha = false;
	bool nopls = false;
	bool noflash = false;
	bool cop = false;
	bool asc = false;
	bool noesc = false;
	bool suicide = false;
	bool noparticles = false;
	bool instantcomplate = false;
	bool instantrespawn = false;
	bool nocmark = false;
	bool objbypass = false;
	bool zoomb = false;
	bool toolbox = false;
	bool veifyhack = false;
	bool dsb = false;
	bool editore = false;
	bool placeo = false;
	bool testmodeb = false;
	bool rotationhack = false;
	bool freescrool = false;
	bool hideui = false;
	bool zorderbypass = false;
	bool hidepausebtn = false;
	bool trailbug = false;
	bool cms = false;
	bool nsd = false;
	bool alm = false;
	bool ap = false;
	bool as = false;
	bool fpsC = false;
	bool fps = false;
	
	//hacks bool
}setting;




typedef void* (__cdecl* fSharedApplication)();
typedef void(__thiscall* fSetAnimationInterval)(void* instance, double delay);
fSharedApplication sharedApplication;
fSetAnimationInterval setAnimInterval;
float interval = 60;
void Fps(int FPS)
{
	interval = 1.0f / FPS;

	HMODULE hMod = LoadLibrary(L"libcocos2d.dll");

	sharedApplication = (fSharedApplication)GetProcAddress(hMod, "?sharedApplication@CCApplication@cocos2d@@SAPAV12@XZ");
	setAnimInterval = (fSetAnimationInterval)GetProcAddress(hMod, "?setAnimationInterval@CCApplication@cocos2d@@UAEXN@Z");

	void* application = sharedApplication();
	setAnimInterval(application, interval);
}



bool write_bytes(uintptr_t const address, std::vector<uint8_t> const& bytes) {
	return WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<LPVOID>(address), bytes.data(), bytes.size(), nullptr);
};

DWORD base = (DWORD)GetModuleHandleA(0);
DWORD base2 = (DWORD)GetModuleHandleA(0);
DWORD libcocosbase = (DWORD)GetModuleHandleA("libcocos2d.dll");
DWORD retJMP = base + 0x190BD5;
DWORD retJMP2 = base2 + 0x190BD5;

void writeByte(BYTE* address, BYTE content)
{
	DWORD oldProtect, newProtect;
	VirtualProtect(address, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
	*address = content;
	VirtualProtect(address, 1, oldProtect, &newProtect);


}

void LoadSettings() {
	auto file = fopen(".gdh\\gdh-data.dat", "rb");
	if (file) {
		fseek(file, 0, SEEK_END);
		auto size = ftell(file);

		if (size == sizeof(setting)) {
			fseek(file, 0, SEEK_SET);
			fread(&setting, sizeof(setting), 1, file);
			fclose(file);
		}
	}
}

void SaveSettings() {
	auto file = fopen(".gdh\\gdh-data.dat", "wb");
	if (file) {
		fwrite(&setting, sizeof(setting), 1, file);
		fclose(file);
	}
}

void placeJMP(BYTE* address, DWORD jumpTo, DWORD length)
{


	DWORD oldProtect, newProtect, relativeAddress;
	VirtualProtect(address, length, PAGE_EXECUTE_READWRITE, &oldProtect);
	relativeAddress = (DWORD)(jumpTo - (DWORD)address) - 5;
	*address = 0xE9;
	*((DWORD*)(address + 0x1)) = relativeAddress;
	for (DWORD x = 0x5; x < length; x++)
	{
		*(address + x) = 0x90;
	}
	VirtualProtect(address, length, oldProtect, &newProtect);
}

void ApplyStyleColor() {
	auto* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.12f, 0.21f, 0.94f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.00f, 0.32f, 1.00f, 0.97f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.32f, 1.00f, 0.97f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.32f, 1.00f, 0.40f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.00f, 0.32f, 1.00f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.01f, 0.00f, 1.00f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.18f, 0.20f, 0.29f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.18f, 0.20f, 0.29f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.13f, 0.17f, 0.40f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.25f, 0.25f, 0.25f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.35f, 0.58f, 0.86f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.41f, 0.68f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void ApplyStyleFlags() {
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowPadding = ImVec2(5.00f, 5.00f);
	style.FramePadding = ImVec2(4.00f, 3.00f);
	style.CellPadding = ImVec2(4.00f, 2.00f);
	style.ItemSpacing = ImVec2(8.00f, 4.00f);
	style.ItemInnerSpacing = ImVec2(4.00f, 4.00f);
	style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
	style.IndentSpacing = 21.00f;
	style.ScrollbarSize = 14.00f;
	style.GrabMinSize = 10.00f;
	style.WindowBorderSize = 1.00f;
	style.ChildBorderSize = 1.00f;
	style.PopupBorderSize = 1.00f;
	style.FrameBorderSize = 1.00f;
	style.TabBorderSize = 0.00f;
	style.WindowRounding = 5.00f;
	style.ChildRounding = 5.00f;
	style.FrameRounding = 5.00f;
	style.PopupRounding = 5.00f;
	style.ScrollbarRounding = 9.00f;
	style.GrabRounding = 5.00f;
	style.LogSliderDeadzone = 5.00f;
	style.TabRounding = 5.00f;
	style.WindowTitleAlign = ImVec2(0.00f, 0.50f);
	style.WindowMenuButtonPosition = 0;
	style.ColorButtonPosition = 1;
	style.ButtonTextAlign = ImVec2(0.50f, 0.50f);
	style.SelectableTextAlign = ImVec2(0.00f, 0.00f);
	style.DisplaySafeAreaPadding = ImVec2(3.00f, 3.00f);
}

void LoadHacks()
{
	if (setting.anticheat) {
		(BYTE*)write_bytes(base + 0x202AAA, { 0xEB, 0x2E }),
			(BYTE*)write_bytes(base + 0x15FC2E, { 0xEB }),
			(BYTE*)write_bytes(base + 0x20D3B3, { 0x90, 0x90, 0x90, 0x90, 0x90 }),
			(BYTE*)write_bytes(base + 0x1FF7A2, { 0x90, 0x90 }),
			(BYTE*)write_bytes(base + 0x18B2B4, { 0xB0, 0x01 }),
			(BYTE*)write_bytes(base + 0x20C4E6, { 0xE9, 0xD7, 0x00, 0x00, 0x00, 0x90 }),
			(BYTE*)write_bytes(base + 0x1FD557, { 0xEB, 0x0C }),
			(BYTE*)write_bytes(base + 0x1FD742, { 0xC7, 0x87, 0xE0, 0x02, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xC7, 0x87, 0xE4, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }),
			(BYTE*)write_bytes(base + 0x1FD756, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }),
			(BYTE*)write_bytes(base + 0x1FD79A, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }),
			(BYTE*)write_bytes(base + 0x1FD7AF, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
	}

	if (setting.noclip) { (BYTE*)write_bytes(base + 0x20A1DD, { 0xEB, 0x37 }); }
	if (setting.unlockall) {
		(BYTE*)write_bytes(base + 0xC50A8, { 0xB0, 0x01, 0x90, 0x90, 0x90 }),
			(BYTE*)write_bytes(base + 0xC54BA, { 0xB0, 0x01, 0x90, 0x90, 0x90 });
	}
	if (setting.ac) {
		(BYTE*)write_bytes(base + 0x2080FB, { 0xFF, 0x50, 0x64, 0xF3, 0x0F, 0x10, 0x00, 0x8B, 0x87, 0xC0, 0x03, 0x00, 0x00, 0x83, 0xEC, 0x08, 0x42 }),
			(BYTE*)write_bytes(base + 0x208114, { 0xF3, 0x0F, 0x5E, 0x87, 0xB4, 0x03, 0x00, 0x00, 0xC7, 0x02, 0x25, 0x2E, 0x32, 0x66, 0xC7, 0x42, 0x04, 0x25, 0x25, 0x00, 0x00, 0x8B, 0xB0, 0x04, 0x01, 0x00, 0x00, 0xF3, 0x0F, 0x5A, 0xC0, 0xF2, 0x0F, 0x11, 0x04, 0x24, 0x52 }),
			(BYTE*)write_bytes(base + 0x20813F, { 0x83, 0xC4, 0x0C });
	}
	if (setting.noprogessbar) {
		(BYTE*)write_bytes(base + 0x1FCE89, { 0x0F, 0x57, 0xC0, 0x90, 0x90, 0x90 }),
			(BYTE*)write_bytes(base + 0x1FCF38, { 0x0D }),
			(BYTE*)write_bytes(base + 0x1FCF6B, { 0x3F });
	}
	if (setting.pmh) {
		(BYTE*)write_bytes(base + 0x20C925, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }),
			(BYTE*)write_bytes(base + 0x20D143, { 0x90, 0x90 }),
			(BYTE*)write_bytes(base + 0x20A563, { 0x90, 0x90 }),
			(BYTE*)write_bytes(base + 0x20A595, { 0x90, 0x90 });
	}
	if (setting.practicepulse) {
		(BYTE*)write_bytes(base + 0x205536, { 0x90, 0x90 }),
			(BYTE*)write_bytes(base + 0x20555D, { 0x90, 0x90 }),
			(BYTE*)write_bytes(base + 0x20553E, { 0xEB });
	}
	if (setting.copy) {
		(BYTE*)write_bytes(base + 0x179B8E, { 0x90, 0x90 }),
			(BYTE*)write_bytes(base + 0x176F5C, { 0x8B, 0xCA, 0x90 }),
			(BYTE*)write_bytes(base + 0x176FE5, { 0xB0, 0x01, 0x90 });
	}
	if (setting.lvledit) { (BYTE*)write_bytes(base + 0x1E4A32, { 0x90, 0x90 }); }
	if (setting.freewnd) {
		(BYTE*)write_bytes(libcocosbase + 0x11388B, { 0x90, 0x90, 0x90, 0x90, 0x90 }),
			(BYTE*)write_bytes(libcocosbase + 0x11339D, { 0xB9, 0xFF, 0xFF, 0xFF, 0x7F, 0x90, 0x90 }),
			(BYTE*)write_bytes(libcocosbase + 0x1133C0, { 0x48 }),
			(BYTE*)write_bytes(libcocosbase + 0x1133C6, { 0x48 }),
			(BYTE*)write_bytes(libcocosbase + 0x112536, { 0xEB, 0x11, 0x90 });
	}
	if (setting.NopeTranltn) { (BYTE*)write_bytes(libcocosbase + 0xA5424, { 0x90, 0x90, 0x90, 0x90, 0x90 }); }
	if (setting.nde) { (BYTE*)write_bytes(base + 0x1EFBE0, { 0xC3 }); }
	if (setting.safemode) {
		(BYTE*)write_bytes(base + 0x20A3B2, { 0xE9, 0x9A, 0x01, 0x00, 0x00, 0x90, 0x90 }),
			(BYTE*)write_bytes(base + 0x1FD40F, { 0xE9, 0x13, 0x06, 0x00, 0x00, 0x90, 0x90 });
	}
	if (setting.norespawnblink) { (BYTE*)write_bytes(base + 0x1EF389, { 0xC7, 0x04, 0x24, 0x00, 0x00, 0x00, 0x00 }); }
	if (setting.soildwt) { (BYTE*)write_bytes(base + 0x1E8153, { 0x8B, 0x86, 0x14, 0x05, 0x00, 0x00, 0xC6, 0x80, 0x30, 0x01, 0x00, 0x00, 0x01, 0xEB, 0x2A }); }
	if (setting.nospk) { (BYTE*)write_bytes(base + 0x205347, { 0x75 }); }
	if (setting.nohitbox) { (BYTE*)write_bytes(base + 0x20456D, { 0xB8, 0x07, 0x00, 0x00, 0x00, 0x90 }); }
	if (setting.nosoilds) { (BYTE*)write_bytes(base + 0x20456D, { 0xB8, 0x14, 0x00, 0x00, 0x00, 0x90 }); }
	if (setting.fbt) { (BYTE*)write_bytes(base + 0x20456D, { 0x31, 0xC0, 0x83, 0x7B, 0x34, 0x00, 0xBA, 0x07, 0x00, 0x00, 0x00, 0x0F, 0x44, 0xC2, 0x90 }); }
	if (setting.eh) { (BYTE*)write_bytes(base + 0x20456D, { 0xB8, 0x02, 0x00, 0x00, 0x00, 0x90 }); }
	if (setting.imp) { (BYTE*)write_bytes(base + 0x20ACA3, { 0xC7, 0x04, 0x24, 0x00, 0x00, 0x00, 0x00 }); }
	if (setting.fz) { (BYTE*)write_bytes(base + 0x203519, { 0x90, 0x90, 0x90 }); }
	if (setting.jh) {
		(BYTE*)write_bytes(base + 0x1E9141, { 0x01 }),
			(BYTE*)write_bytes(base + 0x1E9498, { 0x01 });
	}
	if (setting.trailoff) { (BYTE*)write_bytes(libcocosbase + 0xAEDCC, { 0xE9, 0x69, 0x02, 0x00, 0x00, 0x90 }); }
	if (setting.trailon) { (BYTE*)write_bytes(libcocosbase + 0xAEDCC, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }); }
	if (setting.it) { (BYTE*)write_bytes(libcocosbase + 0xAEDCC, { 0x0F, 0x85, 0x68, 0x02, 0x00, 0x00 }); }
	if (setting.ha) { (BYTE*)write_bytes(base + 0x2D83B8, { 0x00 }); }
	if (setting.nopls) { (BYTE*)write_bytes(base + 0x2060D9, { 0xEB, 0x4A }); }
	if (setting.noflash) {
		(BYTE*)write_bytes(base + 0x1EEC50, { 0xC3 }),
			(BYTE*)write_bytes(base + 0x1E8365, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }),
			(BYTE*)write_bytes(base + 0x1EE31E, { 0xE9, 0x91, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90 });
	}
	if (setting.cop) { (BYTE*)write_bytes(base + 0x204F10, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }); }
	if (setting.asc) {
		(BYTE*)write_bytes(base + 0xE6706, { 0x8B, 0xC2, 0x90 });
		(BYTE*)write_bytes(base + 0xE720F, { 0x8B, 0xD9, 0x90 });
	}
	if (setting.noesc) { (BYTE*)write_bytes(base + 0x1E644C, { 0x90, 0x90, 0x90, 0x90, 0x90 }); }
	if (setting.suicide) {
		(BYTE*)write_bytes(base + 0x203DA2, { 0xE9, 0x57, 0x02, 0x00, 0x00, 0x90 });
		(BYTE*)write_bytes(base + 0x20401A, { 0xE9, 0x27, 0x02, 0x00, 0x00, 0x90 });
	}
	if (setting.noparticles) { (BYTE*)write_bytes(libcocosbase + 0xB8ED6, { 0x00 }); }
	if (setting.instantcomplate) { (BYTE*)write_bytes(base + 0x20350D, { 0xC7, 0x81, 0x7C, 0x06, 0x00, 0x00, 0x20, 0xBC, 0xBE, 0x4C, 0x90, 0x90, 0x90, 0x90, 0x90 }); }
	if (setting.instantrespawn) { (BYTE*)write_bytes(base + 0x20A677, { 0xE8 }); }
	if (setting.nocmark) {
		(BYTE*)write_bytes(base + 0xA6B8B, { 0x2B, 0x87, 0xCC, 0x02, 0x00, 0x00 });
		(BYTE*)write_bytes(base + 0x70E87, { 0xEB, 0x26 });
	}
	if (setting.objbypass) {
		(BYTE*)write_bytes(base + 0x73169, { 0xFF, 0xFF, 0xFF, 0x7F }),
			(BYTE*)write_bytes(base + 0x856A4, { 0xFF, 0xFF, 0xFF, 0x7F }),
			(BYTE*)write_bytes(base + 0x87B17, { 0xFF, 0xFF, 0xFF, 0x7F }),
			(BYTE*)write_bytes(base + 0x87BC7, { 0xFF, 0xFF, 0xFF, 0x7F }),
			(BYTE*)write_bytes(base + 0x87D95, { 0xFF, 0xFF, 0xFF, 0x7F }),
			(BYTE*)write_bytes(base + 0x880F4, { 0xFF, 0xFF, 0xFF, 0x7F }),
			(BYTE*)write_bytes(base + 0x160B06, { 0xFF, 0xFF, 0xFF, 0x7F });
	}
	if (setting.zoomb) {
		(BYTE*)write_bytes(base + 0x87801, { 0x90, 0x90, 0x90 });
		(BYTE*)write_bytes(base + 0x87806, { 0x90, 0x90, 0x90 });
		(BYTE*)write_bytes(base + 0x87871, { 0x90, 0x90, 0x90 });
		(BYTE*)write_bytes(base + 0x87876, { 0x90, 0x90, 0x90 });
	}
	if (setting.toolbox) {
		(BYTE*)write_bytes(base + 0x13A548, { 0x83, 0xF9, 0x01 });
		(BYTE*)write_bytes(base + 0x13A559, { 0xB8, 0x01, 0x00, 0x00, 0x00 });
		(BYTE*)write_bytes(base + 0x13A54D, { 0x83, 0xF9, 0x7F });
		(BYTE*)write_bytes(base + 0x13A552, { 0xB9, 0x7F, 0x00, 0x00, 0x00 });
		(BYTE*)write_bytes(base + 0x13A5D8, { 0x83, 0xF9, 0x01 });
		(BYTE*)write_bytes(base + 0x13A5E9, { 0xB8, 0x01, 0x00, 0x00, 0x00 });
		(BYTE*)write_bytes(base + 0x13A5dd, { 0x83, 0xF9, 0x7F });
		(BYTE*)write_bytes(base + 0x13A5E2, { 0xB9, 0x7F, 0x00, 0x00, 0x00 });
	}
	if (setting.veifyhack) { (BYTE*)write_bytes(base + 0x71D48, { 0xEB }); }
	if (setting.dsb) {
		(BYTE*)write_bytes(base + 0x174407, { 0x90, 0x90 }),
			(BYTE*)write_bytes(base + 0x174411, { 0x90, 0x90, 0x90 }),
			(BYTE*)write_bytes(base + 0x174456, { 0x90, 0x90 }),
			(BYTE*)write_bytes(base + 0x174460, { 0x90, 0x90, 0x90 });
	}
	if (setting.editore) {
		(BYTE*)write_bytes(base + 0x2E67A4, { 0x00, 0x60, 0xEA, 0x4B });
		(BYTE*)write_bytes(base + 0x8FA4D, { 0x0F, 0x60, 0xEA, 0x4B });
	}
	if (setting.placeo) {
		(BYTE*)write_bytes(base + 0x160EE1, { 0x8B, 0xC1, 0x90 });
		(BYTE*)write_bytes(base + 0x160EF2, { 0xE9, 0x23, 0x02, 0x00, 0x00, 0x90 });
	}
	if (setting.testmodeb) { (BYTE*)write_bytes(base + 0x1FD270, { 0xE9, 0xB7, 0x00, 0x00, 0x00, 0x90 }); }
	if (setting.rotationhack) {
		(BYTE*)write_bytes(base + 0x85CBC, { 0xB8, 0x01, 0x00, 0x00, 0x00, 0x90 });
		(BYTE*)write_bytes(base + 0x8BDDD, { 0xB8, 0x01, 0x00, 0x00, 0x00, 0x90 });
		(BYTE*)write_bytes(base + 0x8BE16, { 0xB8, 0x01, 0x00, 0x00, 0x00, 0x90 });
		(BYTE*)write_bytes(base + 0xECA3D, { 0xB8, 0x01, 0x00, 0x00, 0x00, 0x90 });
		(BYTE*)write_bytes(base + 0xEE5A9, { 0xB8, 0x01, 0x00, 0x00, 0x00, 0x90 });
		(BYTE*)write_bytes(base + 0x20181E, { 0xB8, 0x01, 0x00, 0x00, 0x00, 0x90 });
	}
	if (setting.freescrool) {
		(BYTE*)write_bytes(base + 0x8FAAC, { 0xEB });
		(BYTE*)write_bytes(base + 0x8FA95, { 0xEB });
		(BYTE*)write_bytes(base + 0x8FAC5, { 0xEB });
		(BYTE*)write_bytes(base + 0x8FADC, { 0xEB });
	}
	if (setting.hideui) { (BYTE*)write_bytes(base + 0x8720A, { 0xB3, 0x00, 0x90 }); }
	if (setting.zorderbypass) {
		(BYTE*)write_bytes(base + 0x22DEDE, { 0x90, 0x90, 0x90 });
		(BYTE*)write_bytes(base + 0x22DEE8, { 0x90, 0x90, 0x90 });
	}
	if (setting.hidepausebtn) { (BYTE*)write_bytes(base + 0x25F4AC, { 0x90, 0x90 }); }
	if (setting.trailbug) { (BYTE*)write_bytes(libcocosbase + 0xAE9BD, { 0xBB, 0xFF, 0x00, 0x00, 0x00, 0x90 }); }
	if (setting.cms) { (BYTE*)write_bytes(base + 0x208808, { 0xEB, 0x08 }); }
	if (setting.nsd) { (BYTE*)write_bytes(base + 0x1EDCE3, { 0xE9, 0x90, 0x01, 0x00, 0x00, 0x90 }); }
	if (setting.alm) {
		(BYTE*)write_bytes(base + 0x1E5D7F, { 0xEB, 0x08 });
		(BYTE*)write_bytes(base + 0x1DDEC1, { 0xEB, 0x08 });
	}
	if (setting.ap) { (BYTE*)write_bytes(base + 0x8DDE4, { 0x90, 0x8B, 0xCE, 0x90, 0x90, 0x90 }); }
	if (setting.as) { (BYTE*)write_bytes(base + 0x8F2F9, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }); }
	if (setting.fps) { setting.fps = true; }
}

void RenderMain()
{
	const auto show = true;
	const auto director = cocos2d::CCDirector::sharedDirector();
	const bool enable_touch = !ImGui::GetIO().WantCaptureMouse || !show;
	director->getTouchDispatcher()->setDispatchEvents(enable_touch);

	if (GetAsyncKeyState(VK_TAB) < 0 && tab == false)
	{
		menu = !menu;
		tab = true;
	}
	if (GetAsyncKeyState(VK_TAB) == 0 && tab == true)
	{
		tab = false;
	}

	if (menu)
	{
		ImGui::Begin("Core");
		if (OnStartForm1)
		{
			ImGui::SetWindowSize(ImVec2(210, 360));
			ImGui::SetWindowPos(ImVec2(10, 10));
			ApplyStyleFlags();
			ApplyStyleColor();
			OnStartForm1 = false;
		}




		if (ImGui::Checkbox("Disable Anticheat", &setting.anticheat))
		{
			(BYTE*)write_bytes(base + 0x202AAA, { 0xEB, 0x2E }),
				(BYTE*)write_bytes(base + 0x15FC2E, { 0xEB }),
				(BYTE*)write_bytes(base + 0x20D3B3, { 0x90, 0x90, 0x90, 0x90, 0x90 }),
				(BYTE*)write_bytes(base + 0x1FF7A2, { 0x90, 0x90 }),
				(BYTE*)write_bytes(base + 0x18B2B4, { 0xB0, 0x01 }),
				(BYTE*)write_bytes(base + 0x20C4E6, { 0xE9, 0xD7, 0x00, 0x00, 0x00, 0x90 }),
				(BYTE*)write_bytes(base + 0x1FD557, { 0xEB, 0x0C }),
				(BYTE*)write_bytes(base + 0x1FD742, { 0xC7, 0x87, 0xE0, 0x02, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xC7, 0x87, 0xE4, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }),
				(BYTE*)write_bytes(base + 0x1FD756, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }),
				(BYTE*)write_bytes(base + 0x1FD79A, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }),
				(BYTE*)write_bytes(base + 0x1FD7AF, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			if (!setting.anticheat)
			{

				{
					(BYTE*)write_bytes(base + 0x202AAA, { 0x74, 0x2E }),
						(BYTE*)write_bytes(base + 0x15FC2E, { 0x74 }),
						(BYTE*)write_bytes(base + 0x20D3B3, { 0xE8, 0x58, 0x04, 0x00, 0x00 }),
						(BYTE*)write_bytes(base + 0x1FF7A2, { 0x74, 0x6E }),
						(BYTE*)write_bytes(base + 0x18B2B4, { 0x88, 0xD8 }),
						(BYTE*)write_bytes(base + 0x20C4E6, { 0x0F, 0x85, 0xD6, 0x00, 0x00, 0x00 }),
						(BYTE*)write_bytes(base + 0x1FD557, { 0x74, 0x0C }),
						(BYTE*)write_bytes(base + 0x1FD742, { 0x80, 0xBF, 0xDD, 0x02, 0x00, 0x00, 0x00, 0x0F, 0x85, 0x0A, 0xFE, 0xFF, 0xFF, 0x80, 0xBF, 0x34, 0x05, 0x00, 0x00, 0x00, 0x0F, 0x84, 0xFD, 0xFD, 0xFF, 0xFF }),
						(BYTE*)write_bytes(base + 0x1FD557, { 0x74, 0x0C }),
						(BYTE*)write_bytes(base + 0x1FD756, { 0x0F, 0x84, 0xFD, 0xFD, 0xFF, 0xFF }),
						(BYTE*)write_bytes(base + 0x1FD79A, { 0x0F, 0x84, 0xB9, 0xFD, 0xFF, 0xFF }),
						(BYTE*)write_bytes(base + 0x1FD7AF, { 0x0F, 0x85, 0xA4, 0xFD, 0xFF, 0xFF });
				}
			}
		}
		if (ImGui::Checkbox("Noclip", &setting.noclip))
		{
			(BYTE*)write_bytes(base + 0x20A1DD, { 0xEB, 0x37 });
			if (!setting.noclip)
			{

				{
					(BYTE*)write_bytes(base + 0x20A1DD, { 0x8A, 0x80 });
				}
			}
		}

		if (ImGui::Checkbox("Unlock All Icons", &setting.unlockall))
		{
			(BYTE*)write_bytes(base + 0xC50A8, { 0xB0, 0x01, 0x90, 0x90, 0x90 }),
				(BYTE*)write_bytes(base + 0xC54BA, { 0xB0, 0x01, 0x90, 0x90, 0x90 });
			if (!setting.unlockall)
			{

				{
					(BYTE*)write_bytes(base + 0xC50A8, { 0xE8, 0x7A, 0xCD, 0x19, 0x00 }),
						(BYTE*)write_bytes(base + 0xC54BA, { 0xE8, 0x68, 0xC9, 0x19, 0x00 });
				}
			}
		}

		if (ImGui::Checkbox("Accurate Percentage", &setting.ac))
		{
			(BYTE*)write_bytes(base + 0x2080FB, { 0xFF, 0x50, 0x64, 0xF3, 0x0F, 0x10, 0x00, 0x8B, 0x87, 0xC0, 0x03, 0x00, 0x00, 0x83, 0xEC, 0x08, 0x42 }),
				(BYTE*)write_bytes(base + 0x208114, { 0xF3, 0x0F, 0x5E, 0x87, 0xB4, 0x03, 0x00, 0x00, 0xC7, 0x02, 0x25, 0x2E, 0x32, 0x66, 0xC7, 0x42, 0x04, 0x25, 0x25, 0x00, 0x00, 0x8B, 0xB0, 0x04, 0x01, 0x00, 0x00, 0xF3, 0x0F, 0x5A, 0xC0, 0xF2, 0x0F, 0x11, 0x04, 0x24, 0x52 }),
				(BYTE*)write_bytes(base + 0x20813F, { 0x83, 0xC4, 0x0C });
			if (!setting.ac)
			{

				{
					(BYTE*)write_bytes(base + 0x2080FB, { 0xFF, 0x50, 0x64, 0xF3, 0x0F, 0x10, 0x00, 0x8B, 0x87, 0xC0, 0x03, 0x00, 0x00, 0x83, 0xEC, 0x08, 0x42 }),
						(BYTE*)write_bytes(base + 0x208114, { 0xF3, 0x0F, 0x5E, 0x87, 0xB4, 0x03, 0x00, 0x00, 0xC7, 0x02, 0x25, 0x2E, 0x30, 0x66, 0xC7, 0x42, 0x04, 0x25, 0x25, 0x00, 0x00, 0x8B, 0xB0, 0x04, 0x01, 0x00, 0x00, 0xF3, 0x0F, 0x5A, 0xC0, 0xF2, 0x0F, 0x11, 0x04, 0x24, 0x52 }),
						(BYTE*)write_bytes(base + 0x20813F, { 0x83, 0xC4, 0x0C });
				}
			}
		}

		if (ImGui::Checkbox("No Progress Bar", &setting.noprogessbar))
		{
			(BYTE*)write_bytes(base + 0x1FCE89, { 0x0F, 0x57, 0xC0, 0x90, 0x90, 0x90 }),
				(BYTE*)write_bytes(base + 0x1FCF38, { 0x0D }),
				(BYTE*)write_bytes(base + 0x1FCF6B, { 0x3F });
			if (!setting.noprogessbar)
			{

				{
					(BYTE*)write_bytes(base + 0x1FCE89, { 0xF3, 0x0F, 0x10, 0x44, 0x24, 0x48 }),
						(BYTE*)write_bytes(base + 0x1FCF38, { 0x05 }),
						(BYTE*)write_bytes(base + 0x1FCF6B, { 0x00 });
				}
			}
		}

		if (ImGui::Checkbox("Practice Music Hack", &setting.pmh))
		{
			(BYTE*)write_bytes(base + 0x20C925, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }),
				(BYTE*)write_bytes(base + 0x20D143, { 0x90, 0x90 }),
				(BYTE*)write_bytes(base + 0x20A563, { 0x90, 0x90 }),
				(BYTE*)write_bytes(base + 0x20A595, { 0x90, 0x90 });
			if (!setting.pmh)
			{

				{
					(BYTE*)write_bytes(base + 0x20C925, { 0x0F, 0x85, 0xF7, 0x00, 0x00, 0x00 }),
						(BYTE*)write_bytes(base + 0x20D143, { 0x75, 0x41 }),
						(BYTE*)write_bytes(base + 0x20A563, { 0x75, 0x3E }),
						(BYTE*)write_bytes(base + 0x20A595, { 0x75, 0x0C });
				}
			}
		}

		if (ImGui::Checkbox("Practice Pulse", &setting.practicepulse))
		{
			(BYTE*)write_bytes(base + 0x205536, { 0x90, 0x90 }),
				(BYTE*)write_bytes(base + 0x20555D, { 0x90, 0x90 }),
				(BYTE*)write_bytes(base + 0x20553E, { 0xEB });
			if (!setting.practicepulse)
			{

				{
					(BYTE*)write_bytes(base + 0x205536, { 0x75, 0x08 }),
						(BYTE*)write_bytes(base + 0x20555D, { 0x75, 0x03 }),
						(BYTE*)write_bytes(base + 0x20553E, { 0x74 });
				}
			}
		}




		if (ImGui::Checkbox("Copy Hack", &setting.copy))
		{
			(BYTE*)write_bytes(base + 0x179B8E, { 0x90, 0x90 }),
				(BYTE*)write_bytes(base + 0x176F5C, { 0x8B, 0xCA, 0x90 }),
				(BYTE*)write_bytes(base + 0x176FE5, { 0xB0, 0x01, 0x90 });
			if (!setting.copy)
			{

				{
					(BYTE*)write_bytes(base + 0x179B8E, { 0x75, 0x0E }),
						(BYTE*)write_bytes(base + 0x176F5C, { 0x0F, 0x44, 0xCA }),
						(BYTE*)write_bytes(base + 0x176FE5, { 0x0F, 0x95, 0xC0 });
				}
			}
		}

		if (ImGui::Checkbox("Level Edit", &setting.lvledit))
		{
			(BYTE*)write_bytes(base + 0x1E4A32, { 0x90, 0x90 });
			if (!setting.lvledit)
			{

				{
					(BYTE*)write_bytes(base + 0x1E4A32, { 0x75, 0x6C });
				}
			}
		}


		if (ImGui::Checkbox("Free Window Resize", &setting.freewnd))
		{
			(BYTE*)write_bytes(libcocosbase + 0x11388B, { 0x90, 0x90, 0x90, 0x90, 0x90 }),
				(BYTE*)write_bytes(libcocosbase + 0x11339D, { 0xB9, 0xFF, 0xFF, 0xFF, 0x7F, 0x90, 0x90 }),
				(BYTE*)write_bytes(libcocosbase + 0x1133C0, { 0x48 }),
				(BYTE*)write_bytes(libcocosbase + 0x1133C6, { 0x48 }),
				(BYTE*)write_bytes(libcocosbase + 0x112536, { 0xEB, 0x11, 0x90 });
			if (!setting.freewnd)
			{

				{
					(BYTE*)write_bytes(libcocosbase + 0x11388B, { 0xE8, 0xB0, 0xF3, 0xFF, 0xFF }),
						(BYTE*)write_bytes(libcocosbase + 0x11339D, { 0xE8, 0xEE, 0xF6, 0xFF, 0xFF, 0x8B, 0xC8 }),
						(BYTE*)write_bytes(libcocosbase + 0x1133C0, { 0x50 }),
						(BYTE*)write_bytes(libcocosbase + 0x1133C6, { 0x50 }),
						(BYTE*)write_bytes(libcocosbase + 0x112536, { 0x50, 0x6A, 0x00 });
				}
			}
		}


		if (ImGui::Checkbox("No Transition", &setting.NopeTranltn))
		{
			(BYTE*)write_bytes(libcocosbase + 0xA5424, { 0x90, 0x90, 0x90, 0x90, 0x90 });
			if (!setting.NopeTranltn)
			{

				{
					(BYTE*)write_bytes(libcocosbase + 0xA5424, { 0xF3, 0x0F, 0x10, 0x45, 0x08 });
				}
			}
		}

		if (ImGui::Checkbox("No Death Effect", &setting.nde))
		{
			(BYTE*)write_bytes(base + 0x1EFBE0, { 0xC3 });
			if (!setting.nde)
			{

				{
					(BYTE*)write_bytes(base + 0x1EFBE0, { 0x55 });
				}
			}
		}

		if (ImGui::Checkbox("Safe Mode", &setting.safemode))
		{
			(BYTE*)write_bytes(base + 0x20A3B2, { 0xE9, 0x9A, 0x01, 0x00, 0x00, 0x90, 0x90 }),
				(BYTE*)write_bytes(base + 0x1FD40F, { 0xE9, 0x13, 0x06, 0x00, 0x00, 0x90, 0x90 });
			if (!setting.safemode)
			{

				{
					(BYTE*)write_bytes(base + 0x20A3B2, { 0x80, 0xBB, 0x94, 0x04, 0x00, 0x00, 0x00 }),
						(BYTE*)write_bytes(base + 0x1FD40F, { 0x83, 0xB9, 0x64, 0x03, 0x00, 0x00, 0x01 });
				}
			}
		}

		if (ImGui::Checkbox("No Respawn Blink", &setting.norespawnblink))
		{
			(BYTE*)write_bytes(base + 0x1EF389, { 0xC7, 0x04, 0x24, 0x00, 0x00, 0x00, 0x00 });
			if (!setting.norespawnblink)
			{

				{
					(BYTE*)write_bytes(base + 0x1EF389, { 0xC7, 0x04, 0x24, 0xCD, 0xCC, 0xCC, 0x3E });
				}
			}
		}

		ImGui::Begin("Player");
		if (OnStartForm2)
		{
			ImGui::SetWindowSize(ImVec2(210, 480));
			ImGui::SetWindowPos(ImVec2(230, 10));
			OnStartForm2 = false;
		}

		if (ImGui::Checkbox("Solid Wave Trail", &setting.soildwt))
		{
			(BYTE*)write_bytes(base + 0x1E8153, { 0x8B, 0x86, 0x14, 0x05, 0x00, 0x00, 0xC6, 0x80, 0x30, 0x01, 0x00, 0x00, 0x01, 0xEB, 0x2A });
			if (!setting.soildwt)
			{

				{
					(BYTE*)write_bytes(base + 0x1E8153, { 0x8B, 0x81, 0x3C, 0x02, 0x00, 0x00, 0x2B, 0x81, 0x40, 0x02, 0x00, 0x00, 0x83, 0xF8, 0x0F });
				}
			}
		}

		if (ImGui::Checkbox("No Spikes", &setting.nospk))
		{
			(BYTE*)write_bytes(base + 0x205347, { 0x75 });
			if (!setting.nospk)
			{

				{
					(BYTE*)write_bytes(base + 0x205347, { 0x74 });
				}
			}
		}

		if (ImGui::Checkbox("No Hitbox", &setting.nohitbox))
		{
			(BYTE*)write_bytes(base + 0x20456D, { 0xB8, 0x07, 0x00, 0x00, 0x00, 0x90 });
			if (!setting.nohitbox)
			{

				{
					(BYTE*)write_bytes(base + 0x20456D, { 0x8B, 0x83, 0x00, 0x03, 0x00, 0x00 });
				}
			}
		}

		if (ImGui::Checkbox("No Soilds", &setting.nosoilds))
		{
			(BYTE*)write_bytes(base + 0x20456D, { 0xB8, 0x14, 0x00, 0x00, 0x00, 0x90 });
			if (!setting.nosoilds)
			{

				{
					(BYTE*)write_bytes(base + 0x20456D, { 0x8B, 0x83, 0x00, 0x03, 0x00, 0x00 });
				}
			}
		}

		if (ImGui::Checkbox("Force Block Type", &setting.fbt))
		{
			(BYTE*)write_bytes(base + 0x20456D, { 0x31, 0xC0, 0x83, 0x7B, 0x34, 0x00, 0xBA, 0x07, 0x00, 0x00, 0x00, 0x0F, 0x44, 0xC2, 0x90 });
			if (!setting.fbt)
			{

				{
					(BYTE*)write_bytes(base + 0x20456D, { 0x8B, 0x83, 0x00, 0x03, 0x00, 0x00, 0x83, 0xF8, 0x07, 0x0F, 0x84, 0x7F, 0x0A, 0x00, 0x00 });
				}
			}
		}

		if (ImGui::Checkbox("Everything Hurts", &setting.eh))
		{
			(BYTE*)write_bytes(base + 0x20456D, { 0xB8, 0x02, 0x00, 0x00, 0x00, 0x90 });
			if (!setting.eh)
			{

				{
					(BYTE*)write_bytes(base + 0x20456D, { 0x8B, 0x83, 0x00, 0x03, 0x00, 0x00 });
				}
			}
		}


		if (ImGui::Checkbox("Instant Mirror Portal", &setting.imp))
		{
			(BYTE*)write_bytes(base + 0x20ACA3, { 0xC7, 0x04, 0x24, 0x00, 0x00, 0x00, 0x00 });
			if (!setting.imp)
			{

				{
					(BYTE*)write_bytes(base + 0x20ACA3, { 0xC7, 0x04, 0x24, 0x00, 0x00, 0x00, 0x3F });
				}
			}
		}

		if (ImGui::Checkbox("Freeze Player", &setting.fz))
		{
			(BYTE*)write_bytes(base + 0x203519, { 0x90, 0x90, 0x90 });
			if (!setting.fz)
			{

				{
					(BYTE*)write_bytes(base + 0x203519, { 0x50, 0xFF, 0xD6 });
				}
			}
		}

		if (ImGui::Checkbox("Jump Hack", &setting.jh))
		{
			(BYTE*)write_bytes(base + 0x1E9141, { 0x01 }),
				(BYTE*)write_bytes(base + 0x1E9498, { 0x01 });
			if (!setting.jh)
			{

				{
					(BYTE*)write_bytes(base + 0x1E9141, { 0x00 }),
						(BYTE*)write_bytes(base + 0x1E9498, { 0x00 });
				}
			}
		}

		if (ImGui::Checkbox("Trail Always Off", &setting.trailoff))
		{
			(BYTE*)write_bytes(libcocosbase + 0xAEDCC, { 0xE9, 0x69, 0x02, 0x00, 0x00, 0x90 });
			if (!setting.trailoff)
			{

				{
					(BYTE*)write_bytes(libcocosbase + 0xAEDCC, { 0x0F, 0x84, 0x68, 0x02, 0x00, 0x00 });
				}
			}
		}

		if (ImGui::Checkbox("Trail Always On", &setting.trailon))
		{
			(BYTE*)write_bytes(libcocosbase + 0xAEDCC, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			if (!setting.trailon)
			{

				{
					(BYTE*)write_bytes(libcocosbase + 0xAEDCC, { 0x0F, 0x84, 0x68, 0x02, 0x00, 0x00 });
				}
			}
		}

		if (ImGui::Checkbox("Trail Bug Fix", &setting.trailbug))
		{
			(BYTE*)write_bytes(libcocosbase + 0xAE9BD, { 0xBB, 0xFF, 0x00, 0x00, 0x00, 0x90 });
			if (!setting.trailbug)
			{

				{
					(BYTE*)write_bytes(libcocosbase + 0xAE9BD, { 0xF3, 0x0F, 0x2C, 0xC1, 0x2B, 0xD8 });
				}
			}
		}

		if (ImGui::Checkbox("Inversed Trail", &setting.it))
		{
			(BYTE*)write_bytes(libcocosbase + 0xAEDCC, { 0x0F, 0x85, 0x68, 0x02, 0x00, 0x00 });
			if (!setting.it)
			{

				{
					(BYTE*)write_bytes(libcocosbase + 0xAEDCC, { 0x0F, 0x84, 0x68, 0x02, 0x00, 0x00 });
				}
			}
		}

		if (ImGui::Checkbox("Hide Attempts", &setting.ha))
		{
			(BYTE*)write_bytes(base + 0x2D83B8, { 0x00 });
			if (!setting.ha)
			{

				{
					(BYTE*)write_bytes(base + 0x2D83B8, { 0x41 });
				}
			}
		}

		if (ImGui::Checkbox("No Pulse", &setting.nopls))
		{
			(BYTE*)write_bytes(base + 0x2060D9, { 0xEB, 0x4A });
			if (!setting.nopls)
			{

				{
					(BYTE*)write_bytes(base + 0x2060D9, { 0x74, 0x4A });
				}
			}
		}

		if (ImGui::Checkbox("No Flash", &setting.noflash))
		{
			(BYTE*)write_bytes(base + 0x1EEC50, { 0xC3 }),
				(BYTE*)write_bytes(base + 0x1E8365, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }),
				(BYTE*)write_bytes(base + 0x1EE31E, { 0xE9, 0x91, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90 });
			if (!setting.noflash)
			{

				{
					(BYTE*)write_bytes(base + 0x1EEC50, { 0x55 }),
						(BYTE*)write_bytes(base + 0x1E8365, { 0x50, 0x8B, 0xCB, 0xE8, 0x03, 0xF5, 0x00, 0x00 }),
						(BYTE*)write_bytes(base + 0x1EE31E, { 0x56, 0x8B, 0xCF, 0xE8, 0x4A, 0x95, 0x00, 0x00 });
				}
			}
		}

		if (ImGui::Checkbox("Coins on Practice", &setting.cop))
		{
			(BYTE*)write_bytes(base + 0x204F10, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			if (!setting.cop)
			{

				{
					(BYTE*)write_bytes(base + 0x204F10, { 0x80, 0xBE, 0x95, 0x04, 0x00, 0x00, 0x00, 0x0F, 0x85, 0xDE, 0x00, 0x00, 0x00 });
				}
			}
		}

		if (ImGui::Checkbox("Always Show Coins", &setting.asc))
		{
			(BYTE*)write_bytes(base + 0xE6706, { 0x8B, 0xC2, 0x90 });
			(BYTE*)write_bytes(base + 0xE720F, { 0x8B, 0xD9, 0x90 });
			if (!setting.asc)
			{

				{
					(BYTE*)write_bytes(base + 0xE6706, { 0x0F, 0x44, 0xC2 });
					(BYTE*)write_bytes(base + 0xE720F, { 0x0F, 0x44, 0xD9 });
				}
			}
		}

		if (ImGui::Checkbox("Ignore ESC", &setting.noesc))
		{
			(BYTE*)write_bytes(base + 0x1E644C, { 0x90, 0x90, 0x90, 0x90, 0x90 });
			if (!setting.noesc)
			{

				{
					(BYTE*)write_bytes(base + 0x1E644C, { 0xE8, 0xBF, 0x73, 0x02, 0x00 });
				}
			}
		}

		if (ImGui::Checkbox("Suicide", &setting.suicide))
		{
			(BYTE*)write_bytes(base + 0x203DA2, { 0xE9, 0x57, 0x02, 0x00, 0x00, 0x90 });
			(BYTE*)write_bytes(base + 0x20401A, { 0xE9, 0x27, 0x02, 0x00, 0x00, 0x90 });
			if (!setting.suicide)
			{

				{
					(BYTE*)write_bytes(base + 0x203DA2, { 0x0F, 0x86, 0x56, 0x02, 0x00, 0x00 });
					(BYTE*)write_bytes(base + 0x20401A, { 0x0F, 0x87, 0x26, 0x02, 0x00, 0x00 });
				}
			}
		}

		if (ImGui::Checkbox("No Particles", &setting.noparticles))
		{
			(BYTE*)write_bytes(libcocosbase + 0xB8ED6, { 0x00 });
			if (!setting.noparticles)
			{

				{
					(BYTE*)write_bytes(libcocosbase + 0xB8ED6, { 0x01 });
				}
			}
		}

		if (ImGui::Checkbox("Instant Respawn", &setting.instantrespawn))
		{
			(BYTE*)write_bytes(base + 0x20A677, { 0xE8 });
			if (!setting.instantrespawn)
			{

				{
					(BYTE*)write_bytes(base + 0x20A677, { 0xD8 });
				}
			}
		}

		if (ImGui::Checkbox("Instant Complete", &setting.instantcomplate))
		{
			(BYTE*)write_bytes(base + 0x20350D, { 0xC7, 0x81, 0x7C, 0x06, 0x00, 0x00, 0x20, 0xBC, 0xBE, 0x4C, 0x90, 0x90, 0x90, 0x90, 0x90 });
			if (!setting.instantcomplate)
			{

				{
					(BYTE*)write_bytes(base + 0x20350D, { 0x8B, 0x35, 0xE0, 0x23, 0x68, 0x00, 0x81, 0xC1, 0x7C, 0x06, 0x00, 0x00, 0x50, 0xFF, 0xD6 });
				}
			}
		}

		if (ImGui::Checkbox("Hide Pause Button", &setting.hidepausebtn))
		{
			(BYTE*)write_bytes(base + 0x25F4AC, { 0x90, 0x90 });
			if (!setting.hidepausebtn)
			{

				{
					(BYTE*)write_bytes(base + 0x25F4AC, { 0x75, 0x10 });
				}
			}
		}

		if (ImGui::Checkbox("Corrective Music Sync", &setting.cms))
		{
			(BYTE*)write_bytes(base + 0x208808, { 0xEB, 0x08 });
			if (!setting.cms)
			{

				{
					(BYTE*)write_bytes(base + 0x208808, { 0x75, 0x08 });
				}
			}
		}

		if (ImGui::Checkbox("No Spider Dash", &setting.nsd))
		{
			(BYTE*)write_bytes(base + 0x1EDCE3, { 0xE9, 0x90, 0x01, 0x00, 0x00, 0x90 });
			if (!setting.nsd)
			{

				{
					(BYTE*)write_bytes(base + 0x1EDCE3, { 0x0F, 0x84, 0x8F, 0x01, 0x00, 0x00 });
				}
			}
		}

		if (ImGui::Checkbox("Allow Low Volume", &setting.alm))
		{
			(BYTE*)write_bytes(base + 0x1E5D7F, { 0xEB, 0x08 });
			(BYTE*)write_bytes(base + 0x1DDEC1, { 0xEB, 0x08 });
			if (!setting.alm)
			{

				{
					(BYTE*)write_bytes(base + 0x1E5D7F, { 0x76, 0x08 });
					(BYTE*)write_bytes(base + 0x1DDEC1, { 0x76, 0x08 });
				}
			}
		}

		ImGui::End();

		ImGui::Begin("FPS Bypass");
		if (OnStartForm3)
		{
			ImGui::SetWindowSize(ImVec2(210, 110));
			ImGui::SetWindowPos(ImVec2(10, 380));
			OnStartForm3 = false;
		}

		static int fpslol = 60;
		if (ImGui::InputInt("", &fpslol, 0, 10000, 0))
		{
		}

		if (ImGui::Button("Apply"))
		{
			Fps(fpslol);
		}

		if (ImGui::Button("Default"))
		{
			Fps(60);
			fpslol = 60;
		}
		ImGui::End();

		ImGui::Begin("Creator");
		if (OnStartForm4)
		{
			ImGui::SetWindowSize(ImVec2(210, 360));
			ImGui::SetWindowPos(ImVec2(450, 10));
			OnStartForm4 = false;
		}

		if (ImGui::Checkbox("Absolute Scaling", &setting.as))
		{
			(BYTE*)write_bytes(base + 0x8F2F9, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			if (!setting.as)
			{

				{
					(BYTE*)write_bytes(base + 0x8F2F9, { 0x56, 0xE8, 0xB1, 0xEA, 0xFF, 0xFF });
				}
			}
		}

		if (ImGui::Checkbox("Absolute Position", &setting.ap))
		{
			(BYTE*)write_bytes(base + 0x8DDE4, { 0x90, 0x8B, 0xCE, 0x90, 0x90, 0x90 });
			if (!setting.ap)
			{

				{
					(BYTE*)write_bytes(base + 0x8DDE4, { 0x51, 0x8B, 0xCE, 0xFF, 0x50, 0x5C });
				}
			}
		}

		if (ImGui::Checkbox("No (C) Mark", &setting.nocmark))
		{
			(BYTE*)write_bytes(base + 0xA6B8B, { 0x2B, 0x87, 0xCC, 0x02, 0x00, 0x00 });
			(BYTE*)write_bytes(base + 0x70E87, { 0xEB, 0x26 });
			if (!setting.nocmark)
			{

				{
					(BYTE*)write_bytes(base + 0xA6B8B, { 0x2B, 0x87, 0xD0, 0x02, 0x00, 0x00 });
					(BYTE*)write_bytes(base + 0x70E87, { 0x74, 0x26 });
				}
			}
		}

		if (ImGui::Checkbox("Object Bypass", &setting.objbypass))
		{
			(BYTE*)write_bytes(base + 0x73169, { 0xFF, 0xFF, 0xFF, 0x7F }),
				(BYTE*)write_bytes(base + 0x856A4, { 0xFF, 0xFF, 0xFF, 0x7F }),
				(BYTE*)write_bytes(base + 0x87B17, { 0xFF, 0xFF, 0xFF, 0x7F }),
				(BYTE*)write_bytes(base + 0x87BC7, { 0xFF, 0xFF, 0xFF, 0x7F }),
				(BYTE*)write_bytes(base + 0x87D95, { 0xFF, 0xFF, 0xFF, 0x7F }),
				(BYTE*)write_bytes(base + 0x880F4, { 0xFF, 0xFF, 0xFF, 0x7F }),
				(BYTE*)write_bytes(base + 0x160B06, { 0xFF, 0xFF, 0xFF, 0x7F });
			if (!setting.objbypass)
			{

				{
					(BYTE*)write_bytes(base + 0x73169, { 0x80, 0x38, 0x01, 0x00 }),
						(BYTE*)write_bytes(base + 0x856A4, { 0x80, 0x38, 0x01, 0x00 }),
						(BYTE*)write_bytes(base + 0x87B17, { 0x80, 0x38, 0x01, 0x00 }),
						(BYTE*)write_bytes(base + 0x87BC7, { 0x80, 0x38, 0x01, 0x00 }),
						(BYTE*)write_bytes(base + 0x87D95, { 0x80, 0x38, 0x01, 0x00 }),
						(BYTE*)write_bytes(base + 0x880F4, { 0x80, 0x38, 0x01, 0x00 }),
						(BYTE*)write_bytes(base + 0x160B06, { 0x80, 0x38, 0x01, 0x00 });
				}
			}
		}

		if (ImGui::Checkbox("Zoom Bypass", &setting.zoomb))
		{
			(BYTE*)write_bytes(base + 0x87801, { 0x90, 0x90, 0x90 });
			(BYTE*)write_bytes(base + 0x87806, { 0x90, 0x90, 0x90 });
			(BYTE*)write_bytes(base + 0x87871, { 0x90, 0x90, 0x90 });
			(BYTE*)write_bytes(base + 0x87876, { 0x90, 0x90, 0x90 });
			if (!setting.zoomb)
			{

				{
					(BYTE*)write_bytes(base + 0x87801, { 0x0F, 0x2F, 0xC8 });
					(BYTE*)write_bytes(base + 0x87806, { 0x0F, 0x28, 0xC8 });
					(BYTE*)write_bytes(base + 0x87871, { 0x0F, 0x2F, 0xC8 });
					(BYTE*)write_bytes(base + 0x87876, { 0x0F, 0x28, 0xC8 });
				}
			}
		}

		if (ImGui::Checkbox("Toolbox Bypass", &setting.toolbox))
		{
			(BYTE*)write_bytes(base + 0x13A548, { 0x83, 0xF9, 0x01 });
			(BYTE*)write_bytes(base + 0x13A559, { 0xB8, 0x01, 0x00, 0x00, 0x00 });
			(BYTE*)write_bytes(base + 0x13A54D, { 0x83, 0xF9, 0x7F });
			(BYTE*)write_bytes(base + 0x13A552, { 0xB9, 0x7F, 0x00, 0x00, 0x00 });
			(BYTE*)write_bytes(base + 0x13A5D8, { 0x83, 0xF9, 0x01 });
			(BYTE*)write_bytes(base + 0x13A5E9, { 0xB8, 0x01, 0x00, 0x00, 0x00 });
			(BYTE*)write_bytes(base + 0x13A5dd, { 0x83, 0xF9, 0x7F });
			(BYTE*)write_bytes(base + 0x13A5E2, { 0xB9, 0x7F, 0x00, 0x00, 0x00 });
			if (!setting.toolbox)
			{

				{
					(BYTE*)write_bytes(base + 0x13A548, { 0x83, 0xF9, 0x06 });
					(BYTE*)write_bytes(base + 0x13A559, { 0xB8, 0x06, 0x00, 0x00, 0x00 });
					(BYTE*)write_bytes(base + 0x13A54D, { 0x83, 0xF9, 0x0C });
					(BYTE*)write_bytes(base + 0x13A552, { 0xB9, 0x0C, 0x00, 0x00, 0x00 });
					(BYTE*)write_bytes(base + 0x13A5D8, { 0x83, 0xF9, 0x02 });
					(BYTE*)write_bytes(base + 0x13A5E9, { 0xB8, 0x02, 0x00, 0x00, 0x00 });
					(BYTE*)write_bytes(base + 0x13A5dd, { 0x83, 0xF9, 0x73 });
					(BYTE*)write_bytes(base + 0x13A5E2, { 0xB9, 0x03, 0x00, 0x00, 0x00 });
				}
			}
		}

		if (ImGui::Checkbox("Verify Hack", &setting.veifyhack))
		{
			(BYTE*)write_bytes(base + 0x71D48, { 0xEB });
			if (!setting.veifyhack)
			{

				{
					(BYTE*)write_bytes(base + 0x71D48, { 0x74 });
				}
			}
		}

		if (ImGui::Checkbox("Default Song Bypass", &setting.dsb))
		{
			(BYTE*)write_bytes(base + 0x174407, { 0x90, 0x90 }),
				(BYTE*)write_bytes(base + 0x174411, { 0x90, 0x90, 0x90 }),
				(BYTE*)write_bytes(base + 0x174456, { 0x90, 0x90 }),
				(BYTE*)write_bytes(base + 0x174460, { 0x90, 0x90, 0x90 });
			if (!setting.dsb)
			{

				{
					(BYTE*)write_bytes(base + 0x174407, { 0x74, 0x2F }),
						(BYTE*)write_bytes(base + 0x174411, { 0x0F, 0x4F, 0xC6 }),
						(BYTE*)write_bytes(base + 0x174456, { 0x74, 0x2F }),
						(BYTE*)write_bytes(base + 0x174460, { 0x0F, 0x4F, 0xC6 });
				}
			}
		}

		if (ImGui::Checkbox("Editor Extension", &setting.editore))
		{
			(BYTE*)write_bytes(base + 0x2E67A4, { 0x00, 0x60, 0xEA, 0x4B });
			(BYTE*)write_bytes(base + 0x8FA4D, { 0x0F, 0x60, 0xEA, 0x4B });
			if (!setting.editore)
			{

				{
					(BYTE*)write_bytes(base + 0x2E67A4, { 0x00, 0x60, 0x6A, 0x48 });
					(BYTE*)write_bytes(base + 0x8FA4D, { 0x80, 0x67, 0x6A, 0x48 });
				}
			}
		}

		if (ImGui::Checkbox("Place Over", &setting.placeo))
		{
			(BYTE*)write_bytes(base + 0x160EE1, { 0x8B, 0xC1, 0x90 });
			(BYTE*)write_bytes(base + 0x160EF2, { 0xE9, 0x23, 0x02, 0x00, 0x00, 0x90 });
			if (!setting.placeo)
			{

				{
					(BYTE*)write_bytes(base + 0x160EE1, { 0x0F, 0x48, 0xC1 });
					(BYTE*)write_bytes(base + 0x160EF2, { 0x0F, 0x8F, 0x22, 0x02, 0x00, 0x00 });
				}
			}
		}

		if (ImGui::Checkbox("Testmode Bypass", &setting.testmodeb))
		{
			(BYTE*)write_bytes(base + 0x1FD270, { 0xE9, 0xB7, 0x00, 0x00, 0x00, 0x90 });
			if (!setting.testmodeb)
			{

				{
					(BYTE*)write_bytes(base + 0x1FD270, { 0x0F, 0x84, 0xB6, 0x00, 0x00, 0x00 });
				}
			}
		}

		if (ImGui::Checkbox("Rotation Hack", &setting.rotationhack))
		{
			(BYTE*)write_bytes(base + 0x85CBC, { 0xB8, 0x01, 0x00, 0x00, 0x00, 0x90 });
			(BYTE*)write_bytes(base + 0x8BDDD, { 0xB8, 0x01, 0x00, 0x00, 0x00, 0x90 });
			(BYTE*)write_bytes(base + 0x8BE16, { 0xB8, 0x01, 0x00, 0x00, 0x00, 0x90 });
			(BYTE*)write_bytes(base + 0xECA3D, { 0xB8, 0x01, 0x00, 0x00, 0x00, 0x90 });
			(BYTE*)write_bytes(base + 0xEE5A9, { 0xB8, 0x01, 0x00, 0x00, 0x00, 0x90 });
			(BYTE*)write_bytes(base + 0x20181E, { 0xB8, 0x01, 0x00, 0x00, 0x00, 0x90 });
			if (!setting.rotationhack)
			{

				{
					(BYTE*)write_bytes(base + 0x85CBC, { 0x8B, 0x80, 0x00, 0x03, 0x00, 0x00 });
					(BYTE*)write_bytes(base + 0x8BDDD, { 0x8B, 0x80, 0x00, 0x03, 0x00, 0x00 });
					(BYTE*)write_bytes(base + 0x8BE16, { 0x8B, 0x80, 0x00, 0x03, 0x00, 0x00 });
					(BYTE*)write_bytes(base + 0xECA3D, { 0x8B, 0x87, 0x00, 0x03, 0x00, 0x00 });
					(BYTE*)write_bytes(base + 0xEE5A9, { 0x8B, 0x86, 0x00, 0x03, 0x00, 0x00 });
					(BYTE*)write_bytes(base + 0x20181E, { 0x8B, 0x83, 0x00, 0x03, 0x00, 0x00 });
				}
			}
		}

		if (ImGui::Checkbox("Free Scroll", &setting.freescrool))
		{
			(BYTE*)write_bytes(base + 0x8FAAC, { 0xEB });
			(BYTE*)write_bytes(base + 0x8FA95, { 0xEB });
			(BYTE*)write_bytes(base + 0x8FAC5, { 0xEB });
			(BYTE*)write_bytes(base + 0x8FADC, { 0xEB });
			if (!setting.freescrool)
			{

				{
					(BYTE*)write_bytes(base + 0x8FAAC, { 0x75 });
					(BYTE*)write_bytes(base + 0x8FA95, { 0x75 });
					(BYTE*)write_bytes(base + 0x8FAC5, { 0x75 });
					(BYTE*)write_bytes(base + 0x8FADC, { 0x75 });
				}
			}
		}
		if (ImGui::Checkbox("Hide UI", &setting.hideui))
		{
			(BYTE*)write_bytes(base + 0x8720A, { 0xB3, 0x00, 0x90 });
			if (!setting.hideui)
			{

				{
					(BYTE*)write_bytes(base + 0x8720A, { 0x0F, 0x44, 0xD9 });
				}
			}
		}

		if (ImGui::Checkbox("Z Order Bypass", &setting.zorderbypass))
		{
			(BYTE*)write_bytes(base + 0x22DEDE, { 0x90, 0x90, 0x90 });
			(BYTE*)write_bytes(base + 0x22DEE8, { 0x90, 0x90, 0x90 });
			if (!setting.zorderbypass)
			{

				{
					(BYTE*)write_bytes(base + 0x22DEDE, { 0x0F, 0x4C, 0xC1 });
					(BYTE*)write_bytes(base + 0x22DEE8, { 0x0F, 0x4F, 0xC1 });
				}
			}
		}
		ImGui::End();

		ImGui::Begin("Speedhack");
		if (OnStartForm5)
		{
			ImGui::SetWindowSize(ImVec2(210, 110));
			ImGui::SetWindowPos(ImVec2(450, 380));
			OnStartForm5 = false;
		}

		static float speedhack = 1;
		if (ImGui::InputFloat("", &speedhack, 0, 500, 0))
		{
		}

		if (ImGui::Button("Apply"))
		{
			if (not(speedhack == 0))
			{
				Speedhack::Setup();
				Speedhack::SetSpeed(speedhack);
			}
		}

		if (ImGui::Button("Default"))
		{
			Speedhack::Setup();
			Speedhack::SetSpeed(1.0);
			speedhack = 1.0;
		}

		//if (ImGui::Button("Save Hacks"))
		//{
		//	SaveSettings();
		//}
		//if (ImGui::Button("Load Hacks"))
		//{
		//	LoadSettings();
		//}
		ImGui::End();

		ImGui::Begin("Tools");
		if (OnStartForm6)
		{
			ImGui::SetWindowSize(ImVec2(210, 480));
			ImGui::SetWindowPos(ImVec2(670, 10));
			OnStartForm6 = false;
		}

		if (ImGui::Checkbox("Show FPS Counter", &setting.fpsC))
		{
		setting.fps = !setting.fps;
		}
		ImGui::End();

	}
	
		if (setting.fps)
		{
			ImGui::Begin("FPS", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);
			ImGui::SetWindowSize(ImVec2(210, 6));
			ImGui::SetWindowPos(ImVec2(0, 0));
			ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
			ImGui::End();
		}
}



BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);
		ImGuiHook::Load(RenderMain);
		LoadSettings();
		LoadHacks();
		break;
	case DLL_PROCESS_DETACH:
		SaveSettings();
		ImGuiHook::Unload();
		break;
	}
	return TRUE;
}