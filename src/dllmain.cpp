#include "pch.h"
#include "bypassTab.h"
#include "playerTab.h"
#include "speedhackTab.h"
#include "creatorTab.h"
#include "SpeedhackAudio.h"
#include "fpsbypassTab.h"
#include "fpsbypass.h"
#include "universalTab.h"
#include "nongTab.h"
#include "toolsTab.h"
#include "PlayLayer.h"

using namespace std;

bool show = false;

inline int(__thiscall* MenuLayer_init)(gd::MenuLayer* self);
int __fastcall MenuLayer_initHook(gd::MenuLayer* self);
bool inited = false;

int __fastcall MenuLayer_initHook(gd::MenuLayer* self) {	
	if (!inited) {
		bypassTab::init();
		playerTab::init();
		speedhackTab::init();
		creatorTab::init();
		fpsbypassTab::init();
		universalTab::init();
		nongTab::init();
		toolsTab::init();
		inited = true;
	}
	return (int)MenuLayer_init(self);
}

void RenderMain() {
	if (inited) {
		ImGui::Begin("FPS", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);
		ImGui::SetWindowSize(ImVec2(NULL, NULL));
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
		ImGui::End();
		
		if (show) {
			if (!DashReplayEngine::onlydashreplay) {
				bypassTab::draw();
				playerTab::draw();
				speedhackTab::draw();
				creatorTab::draw();
				fpsbypassTab::draw();
				universalTab::draw();
				nongTab::draw();
			}
			toolsTab::draw();
		}
	}
}

DWORD WINAPI Main(void* hModule) {
	ImGuiHook::setRenderFunction(RenderMain);
	ImGuiHook::setToggleCallback([]() {
		show = !show;
	});
	MH_Initialize();
	ImGuiHook::setupHooks([](void* target, void* hook, void** trampoline) {
		MH_CreateHook(target, hook, trampoline);
	});
	SpeedhackAudio::init();
	MH_CreateHook(
		(PVOID)(gd::base + 0x1907B0),
		MenuLayer_initHook,
		(PVOID*)&MenuLayer_init
	);
	FPSMultiplier::Setup();
	PlayLayer::mem_init();
	MH_EnableHook(MH_ALL_HOOKS);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID) {
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH: {
			DisableThreadLibraryCalls(hModule);
			CreateThread(0, 0x1000, Main, hModule, 0, 0);
			break;
		}
		case DLL_PROCESS_DETACH: {
			bypassTab::close();
			playerTab::close();
			speedhackTab::close();
			creatorTab::close();
			fpsbypassTab::close();
			universalTab::close();
			nongTab::close();
			toolsTab::close();
			break;
		}
	}
	return TRUE;
}
