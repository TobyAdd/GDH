#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define GAME_NAME "GeometryDash.exe"
#define GAME_WINDOW "Geometry Dash"
#include "pch.h"
#include "imgui/imgui_hook.h"
#include "imgui/imgui/imgui.h"
#include <cocos2d.h>
#include <mutex>
#include "gdhacks.h"
#include <filesystem>
#include <shellapi.h>
#include <commdlg.h>
#include <iostream>
#include "GDguiWTF/speedhack/speedhackaudio.h"
#include "handler/handler.h"
#include <string>
#include <cstring>
#include <regex>
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "bools.cpp"
#include <gd.h>
#include "GDguiWTF/PlayLayer.h"

static uint32_t offsets[10] = { 0x1E8, 0x1F4, 0x200, 0x20C, 0x218, 0x224, 0x230, 0x23C, 0x248, 0x254 };
static uint32_t maxvals[10] = { 142, 51, 43, 35, 35, 26, 17, 41, 41, 7 };

std::string gdFolder = hacks.gdname;
std::string password = "Get password from level";

using std::cout; using std::endl;
using std::cin; using std::string;
using namespace std;
Handler gmd;

ImGuiIO& io = ImGui::GetIO();

bool tab = true;
const char* transitname[]{"Fade (default)", "CrossFade", "FadeBL", "FadeDown", "FadeTR", "FadeUp", "FlipAngular", "FlipX", "FlipY", "JumpZoom", "MoveInB", "MoveInL", "MoveInR", "MoveInT", "RotoZoom", "ShrinkGrow", "SlideInB", "SlideInL", "SlideInR", "SlideInT", "SplitCols", "SplitRows", "TurnOffTiles", "ZoomFlipAngular", "ZoomFlipX", "ZoomFlipY", "PageTurn", "ProgressHorizontal", "ProgressInOut", "ProgressOutIn", "ProgressRadialCCW", "ProgressRadialCW", "ProgressVertical"};
const std::vector<uint32_t> transitaddr = { 0xA53D0, 0xA5320, 0xA54F0, 0xA55C0, 0xA5690, 0xA5760, 0xA5830, 0xA5950, 0xA5A70, 0xA5B90, 0xA5C40, 0xA5D10, 0xA5DE0, 0xA5EB0, 0xA5F80, 0xA6170, 0xA6240, 0xA6310, 0xA63E0, 0xA64B0, 0xA6580, 0xA6650, 0xA6720, 0xA67F0, 0xA6910, 0xA6A30, 0xA8D50, 0xA91D0, 0xA92A0, 0xA9370, 0xA9440, 0xA9510, 0xA95E0 };

void FixTouch() { const auto show = true; const auto director = cocos2d::CCDirector::sharedDirector(); const bool enable_touch = !ImGui::GetIO().WantCaptureMouse || !show; director->getTouchDispatcher()->setDispatchEvents(enable_touch); }

bool OnCreate = true;
bool InitTabBypass = true;
bool InitTabCreator = true;
bool InitTabPlayer = true;
bool InitTabSpeedhack = true;
bool InitTabUniversal = true;
bool InitTabDisplay = true;
bool InitTabUtilities = true;
bool InitTabNong = true;

void CustomStyle() {
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2(8.00f, 8.00f);
		style.FramePadding = ImVec2(4.00f, 3.00f);
		style.CellPadding = ImVec2(4.00f, 2.00f);
		style.ItemSpacing = ImVec2(8.00f, 4.00f);
		style.ItemInnerSpacing = ImVec2(4.00f, 4.00f);
		style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
		style.IndentSpacing = 24.00f;
		style.ScrollbarSize = 14.00f;
		style.GrabMinSize = 10.00f;
		style.WindowBorderSize = 0.00f;
		style.ChildBorderSize = 1.00f;
		style.PopupBorderSize = 1.00f;
		style.FrameBorderSize = 0.00f;
		style.TabBorderSize = 1.00f;
		style.WindowRounding = 8.00f;
		style.ChildRounding = 6.00f;
		style.FrameRounding = 4.00f;
		style.PopupRounding = 4.00f;
		style.ScrollbarRounding = 4.00f;
		style.GrabRounding = 4.00f;
		style.LogSliderDeadzone = 4.00f;
		style.TabRounding = 4.00f;
		style.WindowTitleAlign = ImVec2(0.50f, 0.50f);
		style.WindowMenuButtonPosition = 0;
		style.ColorButtonPosition = 1;
		style.ButtonTextAlign = ImVec2(0.50f, 0.50f);
		style.SelectableTextAlign = ImVec2(0.00f, 0.00f);
		style.DisplaySafeAreaPadding = ImVec2(3.00f, 3.00f);
	}
void CustomColor() {
		auto* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.16f, 0.16f, 0.16f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
		colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.41f, 0.41f, 0.41f, 0.40f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.28f, 0.29f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.41f, 0.41f, 0.41f, 0.40f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.28f, 0.28f, 0.29f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.41f, 0.41f, 0.41f, 0.40f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.28f, 0.28f, 0.29f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.49f, 0.49f, 0.49f, 0.95f);
		colors[ImGuiCol_Tab] = ImVec4(0.41f, 0.41f, 0.41f, 0.40f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.28f, 0.28f, 0.29f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.35f, 0.35f, 0.35f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.35f, 0.35f, 0.35f, 0.35f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
		}

void IconRandomizer()
{
	srand(GetCurrentTime());
	uint32_t base = *reinterpret_cast<uint32_t*>(reinterpret_cast<uint32_t>(GetModuleHandleA("GeometryDash.exe")) + 0x3222D0);
	for (auto i = 0; i < 10; ++i)
	{
		*reinterpret_cast<uint32_t*>(base + offsets[i]) = rand() % maxvals[i] + (maxvals[i] != 41);
		*reinterpret_cast<uint32_t*>(base + offsets[i] + 4) = 0;
	}
}

void SaveSettings() {
	auto file = fopen(".gdh\\gdh-data.dat", "wb");
	if (file) {
		fwrite(&hacks, sizeof(hacks), 1, file);
		fclose(file);
	}
}
void InitSettings() {
	auto file = fopen(".gdh\\gdh-data.dat", "rb");
	if (file) {
		fseek(file, 0, SEEK_END);
		auto size = ftell(file);

		if (size == sizeof(hacks)) {
			fseek(file, 0, SEEK_SET);
			fread(&hacks, sizeof(hacks), 1, file);
			fclose(file);
		}
	}
}

void InitHacks() {
	//BypassInit
	if (hacks.unlockall) { GDhacks::bypass::UnlockAllIcons(1); }
	if (hacks.textbypass) { GDhacks::bypass::TextBypass(1); }
	if (hacks.characterf) { GDhacks::bypass::CharacterFilter(1); }
	if (hacks.scalebypass) { GDhacks::bypass::ScaleBypass(1); }
	if (hacks.mainlvlbypass) { GDhacks::bypass::MainLevelsBypass(1); }
	if (hacks.gvaultb) { GDhacks::bypass::GuardVault(1); }
	if (hacks.kmasterv) { GDhacks::bypass::KeymasterVault(1); }
	if (hacks.kmasterb) { GDhacks::bypass::KeymasterBasement(1); }
	if (hacks.bkeybypass) { GDhacks::bypass::BasementKeyBypass(1); }
	if (hacks.challengeb) { GDhacks::bypass::ChallengeBypass(1); }
	if (hacks.troom) { GDhacks::bypass::TreasureRoom(1); }
	if (hacks.pshop) { GDhacks::bypass::PotborShop(1); }
	if (hacks.sshop) { GDhacks::bypass::ScratchShop(1); }
	if (hacks.sitem) { GDhacks::bypass::ShopItems(1); }
	if (hacks.gvault) { GDhacks::bypass::GatekeeperVault(1); }
	if (hacks.backupsl) { GDhacks::bypass::BackupStarsLimit(1); }
	if (hacks.unlockall) { GDhacks::bypass::UnlockAllIcons(1); }
	if (hacks.unlockall) { GDhacks::bypass::UnlockAllIcons(1); }
	//BypassInit

	//PlayerInit
	if (hacks.noclip) { GDhacks::player::Noclip(1); }
	if (hacks.nospk) { GDhacks::player::NoSpikes(1); }
	if (hacks.nohitbox) { GDhacks::player::NoHitbox(1); }
	if (hacks.nosoilds) { GDhacks::player::NoSolids(1); }
	if (hacks.forceblocktype) { GDhacks::player::ForceBlockType(1); }
	if (hacks.everythinghurts) { GDhacks::player::EverythingHurts(1); }
	if (hacks.freezeplayer) { GDhacks::player::FreezePlayer(1); }
	if (hacks.jumphack) { GDhacks::player::JumpHack(1); }
	if (hacks.alwaystrailon) { GDhacks::player::TrailAlwaysOn(1); }
	if (hacks.alwaystrailoff) { GDhacks::player::TrailAlwaysOff(1); }
	if (hacks.inversedtrail) { GDhacks::player::InversedTrail(1); }
	if (hacks.trailbutfix) { GDhacks::player::TrailBugFix(1); }
	if (hacks.hideatt) { PlayLayer::hideattW = true; }
	if (hacks.totalatt) { PlayLayer::totalattW = true; }
	if (hacks.pmh) { GDhacks::player::PracticeMusicHack(1); }
	if (hacks.nopulse) { GDhacks::player::NoPulse(1); }
	if (hacks.ignoresc) { GDhacks::player::IgnoreESC(1); }
	if (hacks.suicide) { GDhacks::player::Suicide(1); }
	if (hacks.accuratepercentage) { GDhacks::player::AccuratePercentage(1); }
	if (hacks.noparticles) { GDhacks::player::NoParticles(1); }
	if (hacks.instantr) { GDhacks::player::InstantRespawn(1); }
	if (hacks.instantmp) { GDhacks::player::InstantMirrorPortal(1); }
	if (hacks.onlyper) { GDhacks::player::PercentageOnly(1); }
	if (hacks.hidepauseb) { GDhacks::player::HidePauseButton(1); }
	if (hacks.noglow) { GDhacks::player::NoGlow(1); }
	if (hacks.nomirror) { GDhacks::player::NoMirror(1); }
	if (hacks.minicube) { GDhacks::player::MiniCubeIcon(1); }
	if (hacks.nowavetrail) { GDhacks::player::NoWaveTrail(1); }
	if (hacks.samedualcolor) { GDhacks::player::SameDualColour(1); }
	if (hacks.nonewbestpopup) { GDhacks::player::NoNewBestPopup(1); }
	if (hacks.practiceusercoins) { GDhacks::player::PracticeUserCoins(1); }
	if (hacks.hideatt) { GDhacks::player::HideAttempts(1); }
	if (hacks.showuncollectcoins) { GDhacks::player::CoinsShowUncollected(1); }
	if (hacks.soildwave) { GDhacks::player::SolidWaveTrail(1); }
	if (hacks.deatheffect) { GDhacks::player::NoDeathEffect(1); }
	if (hacks.noforceglow) { GDhacks::player::NoForceGlow(1); }
	if (hacks.practicepulse) { GDhacks::player::PracticePulse(1); }
	if (hacks.correctsyncmusic) { GDhacks::player::CorrectiveMusicSync(1); }
	if (hacks.noportallight) { GDhacks::player::NoPortalLightning(1); }
	if (hacks.nospiderdash) { GDhacks::player::NoSpiderDash(1); }
	//PlayerInit

	//CreatorInit
	if (hacks.copyhack) { GDhacks::creator::CopyHack(1); }
	if (hacks.nocmark) { GDhacks::creator::NoCMark(1); }
	if (hacks.lvledit) { GDhacks::creator::LevelEdit(1); }
	if (hacks.objbypass) { GDhacks::creator::ObjectBypass(1); }
	if (hacks.cobjbypass) { GDhacks::creator::CustomObjectBypass(1); }
	if (hacks.zoomb) { GDhacks::creator::ZoomBypass(1); }
	if (hacks.toolbb) { GDhacks::creator::ToolboxBypass(1); }
	if (hacks.verifyhack) { GDhacks::creator::VerifyHack(1); }
	if (hacks.dsb) { GDhacks::creator::DefaultSongBypass(1); }
	if (hacks.editorex) { GDhacks::creator::EditorExtension(1); }
	if (hacks.placeover) { GDhacks::creator::PlaceOver(1); }
	if (hacks.testmodeb) { GDhacks::creator::TestmodeBypass(1); }
	if (hacks.rotatehack) { GDhacks::creator::RotationHack(1); }
	if (hacks.freescroll) { GDhacks::creator::FreeScroll(1); }
	if (hacks.zorderbypass) { GDhacks::creator::ZOrderBypass(1); }
	if (hacks.absolutes) { GDhacks::creator::AbsoluteScaling(1); }
	if (hacks.absolutep) { GDhacks::creator::AbsolutePosition(1); }
	if (hacks.gidbypass) { GDhacks::creator::GroupIDBypass(1); }
	if (hacks.scalesnapbypass) { GDhacks::creator::ScaleSnapBypass(1); }
	if (hacks.coloridbypass) { GDhacks::creator::ColourIDBypass(1); }
	if (hacks.playtestzoombypass) { GDhacks::creator::PlaytestZoomBypass(1); }
	if (hacks.smootheditortrail) { GDhacks::player::PercentageOnly(1); }
	//CreatorInit

	//InitSpeedhack
	CCDirector::sharedDirector()->getScheduler()->setTimeScale(hacks.speedvalue);
	if (hacks.speedhackaudio) { SpeedhackAudio::set(hacks.speedvalue); }
	//InitSpeedhack

	//InitUniversal
	if (hacks.anticheat) { GDhacks::universal::AnticheatBypass(1); }
	if (hacks.forcev) { GDhacks::universal::ForceVisibility(1); }
	if (hacks.norotation) { GDhacks::universal::NoRotation(1); }
	if (hacks.freewnd) { GDhacks::universal::FreeWindowResize(1); }
	if (hacks.notransition) { GDhacks::universal::NoTransition(1); }
	if (hacks.safemode) { GDhacks::universal::SafeMode(1); }
	if (hacks.transparentbg) { GDhacks::universal::TransparentBG(1); }
	if (hacks.transparentlists) { GDhacks::universal::TransparentLists(1); }
	if (hacks.fastalttab) { GDhacks::universal::FastAltTab(1); }
	if (hacks.allowlowvol) { GDhacks::universal::AllowLowVolume(1); }
	if (hacks.fpscounter) { PlayLayer::FPScounter = hacks.fpscounter; }
	if (hacks.cpscounter) { PlayLayer::CPScounter = hacks.cpscounter; }
	if (hacks.noclipaccuracy) { PlayLayer::noclipAcc = hacks.noclipaccuracy; }
	
	//InitUniversal

	//FPSInit
	//FPSInit

	//InitTransition
	if (hacks.transitsel) { gmd.WriteJump(libcocosbase + transitaddr[0], libcocosbase + transitaddr[hacks.transitsel]); }
	else { gmd.Write(libcocosbase + transitaddr[0], "\x55\x8B\xEC\x6A\xFF", 5); }
	//InitTransition

	//AUTO function
	if (hacks.autopractice) {
		PlayLayer::practice = hacks.autopractice;
	}
	//Auto Function
}

void RenderMain()
{
	auto getPlayLayer = gd::GameManager::sharedState()->getPlayLayer();

	FixTouch();
	

	if (GetAsyncKeyState(VK_TAB) < 0 && tab == false) {hacks.menu = !hacks.menu; tab = true;} if (GetAsyncKeyState(VK_TAB) == 0 && tab == true){tab = false;}
	if (OnCreate) {
		CustomStyle(); CustomColor(); gmd.AttemptAttach(GAME_WINDOW, GAME_NAME); InitHacks(); OnCreate = false; std::pair<int, int> windowSize = gmd.GetWindowSize();
		winsize.widthsize = windowSize.first; winsize.heightsize = windowSize.second;
	}
	if (hacks.fpsbypass) { GDhacks::FPSBypass(hacks.fpsvalue); }
	if (hacks.fpscounter) { PlayLayer::fpsint = ImGui::GetIO().Framerate; }

	if (hacks.menu)
	{
		ImGui::Begin("Bypass", 0, ImGuiWindowFlags_NoResize);
		if (InitTabBypass) { ImGui::SetWindowSize(ImVec2(210 * hacks.pixelsize, 360 * hacks.pixelsize)); ImGui::SetWindowPos(ImVec2(10 * hacks.pixelsize, 10 * hacks.pixelsize)); ImGui::SetWindowFontScale(hacks.pixelsize);  InitTabBypass = false; }
		if (ImGui::Checkbox("Unlock All Icons", &hacks.unlockall)) { if (hacks.unlockall) { GDhacks::bypass::UnlockAllIcons(1); } else { GDhacks::bypass::UnlockAllIcons(0); } }
		if (ImGui::Checkbox("Text Bypass", &hacks.textbypass)) { if (hacks.textbypass) { GDhacks::bypass::TextBypass(1); } else { GDhacks::bypass::TextBypass(0); } }
		if (ImGui::Checkbox("Character Filter", &hacks.characterf)) { if (hacks.characterf) { GDhacks::bypass::CharacterFilter(1); } else { GDhacks::bypass::CharacterFilter(0); } }
		if (ImGui::Checkbox("Scale Bypass", &hacks.scalebypass)) { if (hacks.scalebypass) { GDhacks::bypass::ScaleBypass(1); } else { GDhacks::bypass::ScaleBypass(0); } }
		if (ImGui::Checkbox("Main Levels Bypass", &hacks.mainlvlbypass)) { if (hacks.mainlvlbypass) { GDhacks::bypass::MainLevelsBypass(1); } else { GDhacks::bypass::MainLevelsBypass(0); } }
		if (ImGui::Checkbox("Guard Vault", &hacks.gvaultb)) { if (hacks.gvaultb) { GDhacks::bypass::GuardVault(1); } else { GDhacks::bypass::GuardVault(0); } }
		if (ImGui::Checkbox("Keymaster Vault", &hacks.kmasterv)) { if (hacks.kmasterv) { GDhacks::bypass::KeymasterVault(1); } else { GDhacks::bypass::KeymasterVault(0); } }
		if (ImGui::Checkbox("Keymaster Basement", &hacks.kmasterb)) { if (hacks.kmasterb) { GDhacks::bypass::KeymasterBasement(1); } else { GDhacks::bypass::KeymasterBasement(0); } }
		if (ImGui::Checkbox("Basement Key Bypass", &hacks.bkeybypass)) { if (hacks.bkeybypass) { GDhacks::bypass::BasementKeyBypass(1); } else { GDhacks::bypass::BasementKeyBypass(0); } }
		if (ImGui::Checkbox("Challenge Bypass", &hacks.challengeb)) { if (hacks.challengeb) { GDhacks::bypass::ChallengeBypass(1); } else { GDhacks::bypass::ChallengeBypass(0); } }
		if (ImGui::Checkbox("Treasure Room", &hacks.troom)) { if (hacks.troom) { GDhacks::bypass::TreasureRoom(1); } else { GDhacks::bypass::TreasureRoom(0); } }
		if (ImGui::Checkbox("Potbor Shop", &hacks.pshop)) { if (hacks.pshop) { GDhacks::bypass::PotborShop(1); } else { GDhacks::bypass::PotborShop(0); } }
		if (ImGui::Checkbox("Scratch Shop", &hacks.sshop)) { if (hacks.sshop) { GDhacks::bypass::ScratchShop(1); } else { GDhacks::bypass::ScratchShop(0); } }
		if (ImGui::Checkbox("Shop Items", &hacks.sitem)) { if (hacks.sitem) { GDhacks::bypass::ShopItems(1); } else { GDhacks::bypass::ShopItems(0); } }
		if (ImGui::Checkbox("Gatekeeper Vault", &hacks.gvault)) { if (hacks.gvault) { GDhacks::bypass::GatekeeperVault(1); } else { GDhacks::bypass::GatekeeperVault(0); } }
		if (ImGui::Checkbox("Backup Stars Limit", &hacks.backupsl)) { if (hacks.backupsl) { GDhacks::bypass::BackupStarsLimit(1); } else { GDhacks::bypass::BackupStarsLimit(0); } }
		ImGui::End();

		ImGui::Begin("Player", 0, ImGuiWindowFlags_NoResize);
		if (InitTabPlayer) { ImGui::SetWindowSize(ImVec2(210 * hacks.pixelsize, 580 * hacks.pixelsize)); ImGui::SetWindowPos(ImVec2(230 * hacks.pixelsize, 10 * hacks.pixelsize)); ImGui::SetWindowFontScale(hacks.pixelsize); InitTabPlayer = false; };
		if (ImGui::Checkbox("NoClip", &hacks.noclip)) { if (hacks.noclip) { GDhacks::player::Noclip(1); } else { GDhacks::player::Noclip(0); } }
		if (ImGui::Checkbox("No Spikes", &hacks.nospk)) { if (hacks.nospk) { GDhacks::player::NoSpikes(1); } else { GDhacks::player::NoSpikes(0); } }
		if (ImGui::Checkbox("No Hitbox", &hacks.nohitbox)) { if (hacks.nohitbox) { GDhacks::player::NoHitbox(1); } else { GDhacks::player::NoHitbox(0); } }
		if (ImGui::Checkbox("No Solids", &hacks.nosoilds)) { if (hacks.nosoilds) { GDhacks::player::NoSolids(1); } else { GDhacks::player::NoSolids(0); } }
		if (ImGui::Checkbox("Force Block Type", &hacks.forceblocktype)) { if (hacks.forceblocktype) { GDhacks::player::ForceBlockType(1); } else { GDhacks::player::ForceBlockType(0); } }
		if (ImGui::Checkbox("Everything Hurts", &hacks.everythinghurts)) { if (hacks.everythinghurts) { GDhacks::player::EverythingHurts(1); } else { GDhacks::player::EverythingHurts(0); } }
		if (ImGui::Checkbox("Freeze Player", &hacks.freezeplayer)) { if (hacks.freezeplayer) { GDhacks::player::FreezePlayer(1); } else { GDhacks::player::FreezePlayer(0); } }
		if (ImGui::Checkbox("Jump Hack", &hacks.jumphack)) { if (hacks.jumphack) { GDhacks::player::JumpHack(1); } else { GDhacks::player::JumpHack(0); } }
		if (ImGui::Checkbox("Trail Always On", &hacks.alwaystrailon)) { if (hacks.alwaystrailon) { GDhacks::player::TrailAlwaysOn(1); } else { GDhacks::player::TrailAlwaysOn(0); } }
		if (ImGui::Checkbox("Trail Always Off", &hacks.alwaystrailoff)) { if (hacks.alwaystrailoff) { GDhacks::player::TrailAlwaysOff(1); } else { GDhacks::player::TrailAlwaysOff(0); } }
		if (ImGui::Checkbox("Inversed Trail", &hacks.inversedtrail)) { if (hacks.inversedtrail) { GDhacks::player::InversedTrail(1); } else { GDhacks::player::InversedTrail(0); } }
		if (ImGui::Checkbox("Trail Bug Fix", &hacks.trailbutfix)) { if (hacks.trailbutfix) { GDhacks::player::TrailBugFix(1); } else { GDhacks::player::TrailBugFix(0); } }
		if (ImGui::Checkbox("Hide Attempts", &hacks.hideatt)) { if (hacks.hideatt) { PlayLayer::hideattW = true; } else { PlayLayer::hideattW = false; } }
		if (ImGui::Checkbox("Show Total Attempts", &hacks.totalatt)) { if (hacks.totalatt) { PlayLayer::totalattW = true; } else { PlayLayer::totalattW = false; } }
		if (ImGui::Checkbox("Practice Music Hack", &hacks.pmh)) { if (hacks.pmh) { GDhacks::player::PracticeMusicHack(1); } else { GDhacks::player::PracticeMusicHack(0); } }
		if (ImGui::Checkbox("No Pulse", &hacks.nopulse)) { if (hacks.nopulse) { GDhacks::player::NoPulse(1); } else { GDhacks::player::NoPulse(0); } }
		if (ImGui::Checkbox("Ignore ESC", &hacks.ignoresc)) { if (hacks.ignoresc) { GDhacks::player::IgnoreESC(1); } else { GDhacks::player::IgnoreESC(0); } }
		if (ImGui::Checkbox("Suicide", &hacks.suicide)) { if (hacks.suicide) { GDhacks::player::Suicide(1); } else { GDhacks::player::Suicide(0); } }
		if (ImGui::Checkbox("Accurate Percentage", &hacks.accuratepercentage)) { if (hacks.accuratepercentage) { GDhacks::player::AccuratePercentage(1); } else { GDhacks::player::AccuratePercentage(0); } }
		if (ImGui::Checkbox("No Particles", &hacks.noparticles)) { if (hacks.noparticles) { GDhacks::player::NoParticles(1); } else { GDhacks::player::NoParticles(0); } }
		if (ImGui::Checkbox("Instant Respawn", &hacks.instantr)) { if (hacks.instantr) { GDhacks::player::InstantRespawn(1); } else { GDhacks::player::InstantRespawn(0); } }
		if (ImGui::Checkbox("Instant Mirror Portal", &hacks.instantmp)) { if (hacks.instantmp) { GDhacks::player::InstantMirrorPortal(1); } else { GDhacks::player::InstantMirrorPortal(0); } }
		if (ImGui::Checkbox("Percentage Only", &hacks.onlyper)) { if (hacks.onlyper) { GDhacks::player::PercentageOnly(1); } else { GDhacks::player::PercentageOnly(0); } }
		if (ImGui::Checkbox("Hide Pause Button", &hacks.hidepauseb)) { if (hacks.hidepauseb) { GDhacks::player::HidePauseButton(1); } else { GDhacks::player::HidePauseButton(0); } }
		if (ImGui::Checkbox("No Glow", &hacks.noglow)) { if (hacks.noglow) { GDhacks::player::NoGlow(1); } else { GDhacks::player::NoGlow(0); } }
		if (ImGui::Checkbox("No Mirror", &hacks.nomirror)) { if (hacks.nomirror) { GDhacks::player::NoMirror(1); } else { GDhacks::player::NoMirror(0); } }
		if (ImGui::Checkbox("Mini Cube Icon", &hacks.minicube)) { if (hacks.minicube) { GDhacks::player::MiniCubeIcon(1); } else { GDhacks::player::MiniCubeIcon(0); } }
		if (ImGui::Checkbox("No Wave Trail", &hacks.nowavetrail)) { if (hacks.nowavetrail) { GDhacks::player::NoWaveTrail(1); } else { GDhacks::player::NoWaveTrail(0); } }
		if (ImGui::Checkbox("Same Dual Colour", &hacks.samedualcolor)) { if (hacks.samedualcolor) { GDhacks::player::SameDualColour(1); } else { GDhacks::player::SameDualColour(0); } }
		if (ImGui::Checkbox("No New Best Popup", &hacks.nonewbestpopup)) { if (hacks.nonewbestpopup) { GDhacks::player::NoNewBestPopup(1); } else { GDhacks::player::NoNewBestPopup(0); } }
		if (ImGui::Checkbox("Practice User-Coins", &hacks.practiceusercoins)) { if (hacks.practiceusercoins) { GDhacks::player::PracticeUserCoins(1); } else { GDhacks::player::PracticeUserCoins(0); } }
		if (ImGui::Checkbox("No Respawn Flash", &hacks.norespawnflash)) { if (hacks.norespawnflash) { GDhacks::player::NoRespawnFlash(1); } else { GDhacks::player::NoRespawnFlash(0); } }
		if (ImGui::Checkbox("Coins Show Uncollected", &hacks.showuncollectcoins)) { if (hacks.showuncollectcoins) { GDhacks::player::CoinsShowUncollected(1); } else { GDhacks::player::CoinsShowUncollected(0); } }
		if (ImGui::Checkbox("Solid Wave Trail", &hacks.soildwave)) { if (hacks.soildwave) { GDhacks::player::SolidWaveTrail(1); } else { GDhacks::player::SolidWaveTrail(0); } }
		if (ImGui::Checkbox("No Death Effect", &hacks.deatheffect)) { if (hacks.deatheffect) { GDhacks::player::NoDeathEffect(1); } else { GDhacks::player::NoDeathEffect(0); } }
		if (ImGui::Checkbox("No Force Glow", &hacks.noforceglow)) { if (hacks.noforceglow) { GDhacks::player::NoForceGlow(1); } else { GDhacks::player::NoForceGlow(0); } }
		if (ImGui::Checkbox("Practice Pulse", &hacks.practicepulse)) { if (hacks.practicepulse) { GDhacks::player::PracticePulse(1); } else { GDhacks::player::PracticePulse(0); } }
		if (ImGui::Checkbox("Corrective Music Sync", &hacks.correctsyncmusic)) { if (hacks.correctsyncmusic) { GDhacks::player::CorrectiveMusicSync(1); } else { GDhacks::player::CorrectiveMusicSync(0); } }
		if (ImGui::Checkbox("No Portal Lightning", &hacks.noportallight)) { if (hacks.noportallight) { GDhacks::player::NoPortalLightning(1); } else { GDhacks::player::NoPortalLightning(0); } }
		if (ImGui::Checkbox("No Spider Dash", &hacks.nospiderdash)) { if (hacks.nospiderdash) { GDhacks::player::NoSpiderDash(1); } else { GDhacks::player::NoSpiderDash(0); } }
		ImGui::Separator();
		if (ImGui::Button("Unlink Account", { 196.f * hacks.pixelsize,19.f * hacks.pixelsize })) { gmd.Write(gmd.GetPointerAddress({ 0x3222D8, 0x120 }, GAME_NAME), 0); }
		if (ImGui::Button("Copy User Icons", { 196.f * hacks.pixelsize,19.f * hacks.pixelsize })) {
			static const uint32_t offsets[9] = { 0x1AC, 0x1B0, 0x1B4, 0x1B8, 0x1BC, 0x1C0, 0x1C4, 0x148, 0x14C };
			if (gmd.GetPointerAddress({ 0x3222C8, 0x1DC, 0x10, 0x0 }, GAME_NAME))
			{
				for (auto i = 0; i < 9; ++i)
				{
					int val = gmd.Read<int>(gmd.GetPointerAddress({ 0x3222C8, 0x1DC, 0x10, offsets[i] }, GAME_NAME));
					uint32_t addr = gmd.GetPointerAddress({ 0x3222D0, 0x1E8 + static_cast<uint32_t>(0xC * i) }, GAME_NAME);
					gmd.Write(addr, val);
					gmd.Write(addr + 4, 0);
				}
			}
		}
		if (ImGui::Button("Random Icon", { 196.f * hacks.pixelsize,19.f * hacks.pixelsize })) { IconRandomizer(); }
		if (ImGui::Button("Refresh Textures", { 196.f * hacks.pixelsize,19.f * hacks.pixelsize })) { gd::GameManager::sharedState()->reloadAll(false, false, true);}
		ImGui::End();

		ImGui::Begin("Creator", 0, ImGuiWindowFlags_NoResize);
		if (InitTabCreator) { ImGui::SetWindowSize(ImVec2(210 * hacks.pixelsize, 360 * hacks.pixelsize)); ImGui::SetWindowPos(ImVec2(450 * hacks.pixelsize, 10 * hacks.pixelsize)); ImGui::SetWindowFontScale(hacks.pixelsize); InitTabCreator = false; }

		if (ImGui::Button(password.c_str(), { 196.f * hacks.pixelsize, 19.f * hacks.pixelsize })) {
			if (password == "Get password from level") {
				uint32_t addr = gmd.GetPointerAddress({ 0x003222D0, 0x164, 0x488, 0x2C4 }, GAME_NAME);
				int pswd = gmd.Read<int>(addr) - gmd.Read<int>(addr + 4);
				switch (pswd) {
				case 0:password = "Level not copyable"; break;
				case 1:password = "No password need"; break;
				default:password = std::to_string(pswd).substr(1, 6); break;
				}
			}
			else { password = "Get password from level"; }
		}
		ImGui::Separator();
		if (ImGui::Checkbox("Copy Hack", &hacks.copyhack)) { if (hacks.copyhack) { GDhacks::creator::CopyHack(1); } else { GDhacks::creator::CopyHack(0); } }
		if (ImGui::Checkbox("No (C) Mark", &hacks.nocmark)) { if (hacks.nocmark) { GDhacks::creator::NoCMark(1); } else { GDhacks::creator::NoCMark(0); } }
		if (ImGui::Checkbox("Level Edit", &hacks.lvledit)) { if (hacks.lvledit) { GDhacks::creator::LevelEdit(1); } else { GDhacks::creator::LevelEdit(0); } }
		if (ImGui::Checkbox("Object Bypass", &hacks.objbypass)) { if (hacks.objbypass) { GDhacks::creator::ObjectBypass(1); } else { GDhacks::creator::ObjectBypass(0); } }
		if (ImGui::Checkbox("Custom Object Bypass", &hacks.cobjbypass)) { if (hacks.cobjbypass) { GDhacks::creator::CustomObjectBypass(1); } else { GDhacks::creator::CustomObjectBypass(0); } }
		if (ImGui::Checkbox("Zoom Bypass", &hacks.zoomb)) { if (hacks.zoomb) { GDhacks::creator::ZoomBypass(1); } else { GDhacks::creator::ZoomBypass(0); } }
		if (ImGui::Checkbox("Toolbox Button Bypass", &hacks.toolbb)) { if (hacks.toolbb) { GDhacks::creator::ToolboxBypass(1); } else { GDhacks::creator::ToolboxBypass(0); } }
		if (ImGui::Checkbox("Verify Hack", &hacks.verifyhack)) { if (hacks.verifyhack) { GDhacks::creator::VerifyHack(1); } else { GDhacks::creator::VerifyHack(0); } }
		if (ImGui::Checkbox("Default Song Bypass", &hacks.dsb)) { if (hacks.dsb) { GDhacks::creator::DefaultSongBypass(1); } else { GDhacks::creator::DefaultSongBypass(0); } }
		if (ImGui::Checkbox("Editor Extension", &hacks.editorex)) { if (hacks.editorex) { GDhacks::creator::EditorExtension(1); } else { GDhacks::creator::EditorExtension(0); } }
		if (ImGui::Checkbox("Place Over", &hacks.placeover)) { if (hacks.placeover) { GDhacks::creator::PlaceOver(1); } else { GDhacks::creator::PlaceOver(0); } }
		if (ImGui::Checkbox("Testmode Bypass", &hacks.testmodeb)) { if (hacks.testmodeb) { GDhacks::creator::TestmodeBypass(1); } else { GDhacks::creator::TestmodeBypass(0); } }
		if (ImGui::Checkbox("Rotation Hack", &hacks.rotatehack)) { if (hacks.rotatehack) { GDhacks::creator::RotationHack(1); } else { GDhacks::creator::RotationHack(0); } }
		if (ImGui::Checkbox("Free Scroll", &hacks.freescroll)) { if (hacks.freescroll) { GDhacks::creator::FreeScroll(1); } else { GDhacks::creator::FreeScroll(0); } }
		if (ImGui::Checkbox("Hide UI", &hacks.hideui)) { if (hacks.hideui) { GDhacks::creator::HideUI(1); } else { GDhacks::creator::HideUI(0); } }
		if (ImGui::Checkbox("Z Order Bypass", &hacks.zorderbypass)) { if (hacks.zorderbypass) { GDhacks::creator::ZOrderBypass(1); } else { GDhacks::creator::ZOrderBypass(0); } }
		if (ImGui::Checkbox("Absolute Scaling", &hacks.absolutes)) { if (hacks.absolutes) { GDhacks::creator::AbsoluteScaling(1); } else { GDhacks::creator::AbsoluteScaling(0); } }
		if (ImGui::Checkbox("Absolute Position", &hacks.absolutep)) { if (hacks.absolutep) { GDhacks::creator::AbsolutePosition(1); } else { GDhacks::creator::AbsolutePosition(0); } }
		if (ImGui::Checkbox("Group ID Bypass", &hacks.gidbypass)) { if (hacks.gidbypass) { GDhacks::creator::GroupIDBypass(1); } else { GDhacks::creator::GroupIDBypass(0); } }
		if (ImGui::Checkbox("Scale Snap Bypass", &hacks.scalesnapbypass)) { if (hacks.scalesnapbypass) { GDhacks::creator::ScaleSnapBypass(1); } else { GDhacks::creator::ScaleSnapBypass(0); } }
		if (ImGui::Checkbox("Colour ID Bypass", &hacks.coloridbypass)) { if (hacks.coloridbypass) { GDhacks::creator::ColourIDBypass(1); } else { GDhacks::creator::ColourIDBypass(0); } }
		if (ImGui::Checkbox("Playtest Zoom Bypass", &hacks.playtestzoombypass)) { if (hacks.playtestzoombypass) { GDhacks::creator::PlaytestZoomBypass(1); } else { GDhacks::creator::PlaytestZoomBypass(0); } }
		if (ImGui::Checkbox("Smooth Editor Trail", &hacks.smootheditortrail)) { if (hacks.smootheditortrail) { GDhacks::creator::SmoothEditorTrail(1); } else { GDhacks::creator::SmoothEditorTrail(0); } }
		ImGui::End();
		ImGui::Begin("Speedhack", nullptr, ImGuiWindowFlags_NoResize);
		if (InitTabSpeedhack) { ImGui::SetWindowSize(ImVec2(210 * hacks.pixelsize, 130 * hacks.pixelsize)); ImGui::SetWindowPos(ImVec2(10 * hacks.pixelsize, 380 * hacks.pixelsize)); ImGui::SetWindowFontScale(hacks.pixelsize); InitTabSpeedhack = false; }
		ImGui::PushItemWidth(196.000000 * hacks.pixelsize); ImGui::InputFloat("##1", &hacks.speedvalue, 0, 500, 0);
		if (ImGui::Button("Apply", { 196.f * hacks.pixelsize, 19.f * hacks.pixelsize })) { if (not(hacks.speedvalue == 0)) { CCDirector::sharedDirector()->getScheduler()->setTimeScale(hacks.speedvalue); if (hacks.speedhackaudio) { SpeedhackAudio::set(hacks.speedvalue); } } }
		if (ImGui::Button("Default", { 196.f * hacks.pixelsize, 19.f * hacks.pixelsize })) { CCDirector::sharedDirector()->getScheduler()->setTimeScale(1.0); hacks.speedvalue = 1.0; if (hacks.speedhackaudio) { SpeedhackAudio::set(1.0); } }
		if (ImGui::Checkbox("Speedhack Audio", &hacks.speedhackaudio)) { if (hacks.speedhackaudio) { if (not(hacks.speedvalue == 0)) { SpeedhackAudio::set(hacks.speedvalue); } } else { SpeedhackAudio::set(1); } }
		ImGui::End();

		ImGui::Begin("Universal", nullptr, ImGuiWindowFlags_NoResize);
		if (InitTabUniversal) { ImGui::SetWindowSize(ImVec2(210 * hacks.pixelsize, 270 * hacks.pixelsize)); ImGui::SetWindowPos(ImVec2(670 * hacks.pixelsize, 10 * hacks.pixelsize)); ImGui::SetWindowFontScale(hacks.pixelsize); InitTabUniversal = false; }
		if (ImGui::Checkbox("Disable Anticheat", &hacks.anticheat)) { if (hacks.anticheat) { GDhacks::universal::AnticheatBypass(1); } else { GDhacks::universal::AnticheatBypass(0); } }
		if (ImGui::Checkbox("Force Visibility", &hacks.forcev)) { if (hacks.forcev) { GDhacks::universal::ForceVisibility(1); } else { GDhacks::universal::ForceVisibility(0); } }
		if (ImGui::Checkbox("No Rotation", &hacks.norotation)) { if (hacks.norotation) { GDhacks::universal::NoRotation(1); } else { GDhacks::universal::NoRotation(0); } }
		if (ImGui::Checkbox("Free Window Resize", &hacks.freewnd)) { if (hacks.freewnd) { GDhacks::universal::FreeWindowResize(1); } else { GDhacks::universal::FreeWindowResize(0); } }
		if (ImGui::Checkbox("No Transition", &hacks.notransition)) { if (hacks.notransition) { GDhacks::universal::NoTransition(1); } else { GDhacks::universal::NoTransition(0); } }
		if (ImGui::Checkbox("Safe Mode", &hacks.safemode)) { if (hacks.safemode) { GDhacks::universal::SafeMode(1); } else { GDhacks::universal::SafeMode(0); } }
		if (ImGui::Checkbox("Transparent BG", &hacks.transparentbg)) { if (hacks.transparentbg) { GDhacks::universal::TransparentBG(1); } else { GDhacks::universal::TransparentBG(0); } }
		if (ImGui::Checkbox("Transparent Lists", &hacks.transparentlists)) { if (hacks.transparentlists) { GDhacks::universal::TransparentLists(1); } else { GDhacks::universal::TransparentLists(0); } }
		if (ImGui::Checkbox("Fast Alt-Tab", &hacks.fastalttab)) { if (hacks.fastalttab) { GDhacks::universal::FastAltTab(1); } else { GDhacks::universal::FastAltTab(0); } }
		if (ImGui::Checkbox("Allow Low Volume", &hacks.allowlowvol)) { if (hacks.allowlowvol) { GDhacks::universal::AllowLowVolume(1); } else { GDhacks::universal::AllowLowVolume(0); } }
		ImGui::End();

		ImGui::Begin("FPS Bypass", nullptr, ImGuiWindowFlags_NoResize);
		if (InitTabDisplay) { ImGui::SetWindowSize(ImVec2(210 * hacks.pixelsize, 110 * hacks.pixelsize)); ImGui::SetWindowPos(ImVec2(450 * hacks.pixelsize, 380 * hacks.pixelsize)); ImGui::SetWindowFontScale(hacks.pixelsize); InitTabDisplay = false; }
		ImGui::PushItemWidth(196.000000 * hacks.pixelsize); ImGui::InputInt("##3", &hacks.fpslol, 0, 10000, 0);
		if (ImGui::Button("Apply", { 196.f * hacks.pixelsize, 19.f * hacks.pixelsize })) { hacks.fpsvalue = hacks.fpslol; if (!hacks.fpsbypass) { hacks.fpsbypass = true; } }
		if (ImGui::Button("Default", { 196.f * hacks.pixelsize, 19.f * hacks.pixelsize })) { hacks.fpsvalue = 60; hacks.fpslol = 60; if (!hacks.fpsbypass) { hacks.fpsbypass = true; }}
		ImGui::End();

		ImGui::Begin("Utilities", nullptr, ImGuiWindowFlags_NoResize);
		if (InitTabUtilities) { ImGui::SetWindowSize(ImVec2(210 * hacks.pixelsize, 380 * hacks.pixelsize)); ImGui::SetWindowPos(ImVec2(890 * hacks.pixelsize, 10 * hacks.pixelsize)); ImGui::SetWindowFontScale(hacks.pixelsize); InitTabUtilities = false; }
		
		if (ImGui::TreeNode("Show Info")) {
			if (ImGui::Checkbox("FPS Counter", &hacks.fpscounter)) { PlayLayer::FPScounter = hacks.fpscounter; }
			if (ImGui::Checkbox("CPS Counter", &hacks.cpscounter)) { PlayLayer::CPScounter = hacks.cpscounter; }
			if (ImGui::Checkbox("Noclip Accuracy", &hacks.noclipaccuracy)) { PlayLayer::noclipAcc = hacks.noclipaccuracy;  }
			
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Transition")) {
			ImGui::PushItemWidth(176.000000 * hacks.pixelsize);
			if (ImGui::Combo("##1", &hacks.transitsel, transitname, IM_ARRAYSIZE(transitname)))
			{
				gmd.AttemptAttach(GAME_WINDOW, GAME_NAME);
				if (hacks.transitsel)
				{
					gmd.WriteJump(libcocosbase + transitaddr[0], libcocosbase + transitaddr[hacks.transitsel]);
				}
				else
				{
					gmd.Write(libcocosbase + transitaddr[0], "\x55\x8B\xEC\x6A\xFF", 5);
				}
			}ImGui::TreePop();
		}

		if (ImGui::TreeNode("Other")) {
			ImGui::Checkbox("FPS Bypass", &hacks.fpsbypass);
			if (ImGui::Button("Inject DLL", { 160.f * hacks.pixelsize, 19.f * hacks.pixelsize })) { ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose DLL", ".dll,.exe,.*", "."); }
			if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
			{
				if (ImGuiFileDialog::Instance()->IsOk())
				{
					std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName(); const char* DllPath = filePath.c_str();
					if (!filePath.empty()) { if (gmd.Inject(DllPath)) { MessageBoxA(0, "Injected", "OK", MB_OK); } else { MessageBoxA(0, "Not Injected", "FAILED", MB_OK); } }
				}
				ImGuiFileDialog::Instance()->Close();
			}ImGui::TreePop();
		}

		if (ImGui::TreeNode("Change Windwo Size")) {
			std::pair<int, int> windowSize = gmd.GetWindowSize();
			ImGui::PushItemWidth(130.000000 * hacks.pixelsize);
			ImGui::InputInt("Width", &winsize.widthsize, 0, 50000, 0);
			ImGui::PushItemWidth(130.000000 * hacks.pixelsize);
			ImGui::InputInt("Heigth", &winsize.heightsize, 0, 50000, 0);
			if (ImGui::Button("Apply", { 160.f * hacks.pixelsize, 19.f * hacks.pixelsize })) { gmd.SetWindowSize(winsize.widthsize, winsize.heightsize); }
			ImGui::TreePop();
		}
	
		if (ImGui::TreeNode("Auto Functions")) {
			if (ImGui::Checkbox("Auto practice mode", &hacks.autopractice)) { PlayLayer::practice = hacks.autopractice; }
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("GDH Seetting")) {
			ImGui::Text("Pixel Size:");
			ImGui::PushItemWidth(175.000000 * hacks.pixelsize);
			if (ImGui::InputFloat("##4", &hacks.pixelsize, 0.1, 0, 0)) {
				InitTabBypass = true;
				InitTabCreator = true;
				InitTabPlayer = true;
				InitTabSpeedhack = true;
				InitTabUniversal = true;
				InitTabDisplay = true;
				InitTabUtilities = true;
				InitTabNong = true;
			}ImGui::TreePop();
		}

		if (ImGui::TreeNode("Bruteforce")) { if (ImGui::Button("Bruteforce")) { system("start .gdh\\bruteforce.exe"); }ImGui::TreePop(); };

		if (ImGui::TreeNode("GDH About")) {
			ImGui::Text("GDH by TobyAdd");
			if (ImGui::Button("Sourse code")) { ShellExecuteA(0, "open", "https://github.com/TobyAdd/GDH-Overlay", NULL, NULL, SW_SHOWMAXIMIZED); }
			if (ImGui::Button("Support project")) { ShellExecuteA(0, "open", "https://new.donatepay.ru/@TobyAdd", NULL, NULL, SW_SHOWMAXIMIZED); }
			if (ImGui::Button("Special thanks")) { ShellExecuteA(0, "open", "SpecialThx.txt", NULL, NULL, SW_SHOWMAXIMIZED); }
			ImGui::TreePop();
		}

		ImGui::End();

			ImGui::Begin("Nong Downloader", nullptr, ImGuiWindowFlags_NoResize);
			if (InitTabNong) { ImGui::SetWindowSize(ImVec2(210 * hacks.pixelsize, 200 * hacks.pixelsize));  ImGui::SetWindowPos(ImVec2(670 * hacks.pixelsize, 290 * hacks.pixelsize)); ImGui::SetWindowFontScale(hacks.pixelsize); InitTabNong = false; }
			ImGui::Text("ID:");
			ImGui::PushItemWidth(196.000000 * hacks.pixelsize);
			ImGui::InputInt("##1", &hacks.songid, 0, 10000, 0);
			ImGui::Text("YouTube Video ID:");
			ImGui::PushItemWidth(196.000000 * hacks.pixelsize);
			ImGui::InputText("##2", hacks.ytid, 64);
			ImGui::Text("GD Local AppData Name:");
			ImGui::PushItemWidth(196.000000 * hacks.pixelsize);
			ImGui::PushItemWidth(196.000000 * hacks.pixelsize);
			ImGui::InputText("##3", hacks.gdname, 64);
			if (ImGui::Button("Download", { 196.f * hacks.pixelsize, 19.f * hacks.pixelsize })) {
				std::string command = "start .gdh\\nong\\downloader.bat " + std::to_string(hacks.songid) + " " + hacks.ytid + " " + gdFolder;
				system(command.c_str());
			}
			if (ImGui::Button("Delete", { 196.f * hacks.pixelsize, 19.f * hacks.pixelsize })) {
				char* sAppDataPath; size_t iAppDataPathLength;
				_dupenv_s(&sAppDataPath, &iAppDataPathLength, "LOCALAPPDATA");
				std::string stdAppData(sAppDataPath);
				std::string delcommand = stdAppData + '\\' + gdFolder + '\\' + std::to_string(hacks.songid) + ".mp3";
				MessageBoxA(0, delcommand.c_str(), 0, MB_OK);
				std::filesystem::remove(delcommand.c_str());
			}
			ImGui::End();
	}
}

inline int(__thiscall* MenuLayer_init)(gd::MenuLayer* self);
int __fastcall MenuLayer_initHook(gd::MenuLayer* self);
bool was = true;

int __fastcall MenuLayer_initHook(gd::MenuLayer* self) {	
	if (was) {
		InitSettings();
		InitHacks();
		was = false;
	}
	return (int)MenuLayer_init(self);
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		ImGuiHook::Load(RenderMain);
		if (!std::filesystem::is_directory(".gdh") || !std::filesystem::exists(".gdh"))
		{
			std::filesystem::create_directory(".gdh");
		}
		MH_Initialize();
		SpeedhackAudio::init();
		PlayLayer::mem_init();
		MH_CreateHook(
			(PVOID)(base + 0x1907B0),
			MenuLayer_initHook,
			(PVOID*)&MenuLayer_init
		);
		MH_EnableHook(MH_ALL_HOOKS);
		break;
	case DLL_PROCESS_DETACH:
		SaveSettings();
		break;
	}
	return TRUE;
}