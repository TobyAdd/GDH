#pragma once
#include "PlayLayer.h"
#include <vector>
#include "../imgui/kiero/minhook/include/MinHook.h"
#include "../bools.cpp"
#include <iomanip>

//Noclip Accuracy Var
bool PlayLayer::noclipAcc = false;
bool PlayLayer::wouldDie = false;
int PlayLayer::frames = 0;
int PlayLayer::deaths = 0;
float PlayLayer::totalDelta = 0;
float PlayLayer::prevX = 0;
//Noclip Accuracy Var

bool PlayLayer::practice;

//RainBow Var
gd::PlayLayer* play_layer;
//RainBow Var

//CPS Counter Var
bool PlayLayer::CPScounter;
int PlayLayer::cpscount;
int PlayLayer::cpsint;
std::chrono::system_clock::time_point start = std::chrono::system_clock::now(), now;
std::chrono::duration<double> cycleTime;
//CPS Counter Var

//FPS Var
bool PlayLayer::FPScounter;
int PlayLayer::fpsint;
//FPS Var

bool PlayLayer::hideattW;
bool PlayLayer::totalattW;

int fpsposition;
int cpsposition;
int noclipaccuracyposition;

void WriteBytes(void* location, std::vector<BYTE> bytes) {
	DWORD old_prot;
	VirtualProtect(location, bytes.size(), PAGE_EXECUTE_READWRITE, &old_prot);
	memcpy(location, bytes.data(), bytes.size());	
	VirtualProtect(location, bytes.size(), old_prot, &old_prot);
}

std::string getAccuracyText() {
	if (PlayLayer::frames == 0) return "Accuracy: 100.00%";
	float p = (float)(PlayLayer::frames - PlayLayer::deaths) / (float)PlayLayer::frames;
	std::stringstream stream;
	stream << "Accuracy: " << std::fixed << std::setprecision(2) << p * 100.f << "%";
	return stream.str();
}

std::string CPSText() {
	std::stringstream stream;
	stream << "CPS: " << PlayLayer::cpsint << "/" << PlayLayer::cpscount;
	return stream.str();
}

std::string GetFPSText() {
	std::stringstream stream;
	stream << "FPS: " << PlayLayer::fpsint;
	return stream.str();
}

	void __fastcall PlayLayer::updateHook(gd::PlayLayer* self, void* ff, float dt) {
	//Noclip Accuracy
	void* player1 = *(void**)((char*)self + 0x224);
	float x = *(float*)((size_t)player1 + 0x67c);

	if (x != prevX) {
		frames += 1;
		totalDelta += dt;
	}
	CCLabelBMFont* Fps = (CCLabelBMFont*)self->getChildByTag(200000);
	CCLabelBMFont* CpsText = (CCLabelBMFont*)self->getChildByTag(150000);
	CCLabelBMFont* noclipAccText = (CCLabelBMFont*)self->getChildByTag(100000);

	if (!CPScounter) { fpsposition = 12; }
	if (!noclipAcc) { fpsposition = 3; }
	if (!noclipAcc) { cpsposition = 3; }
	if (CPScounter && noclipAcc) { fpsposition = 21; }


	size_t base = (size_t)GetModuleHandle(0);
	if (FPScounter) {
		if (!noclipAcc) { fpsposition = 3; }
		if (CPScounter && noclipAcc) { fpsposition = 21; }
		auto FPSText = GetFPSText();
		Fps->setString(FPSText.c_str());
		auto cpssize = Fps->getScaledContentSize();
		if (CPScounter && !noclipAcc)
		{
			Fps->setPosition({ cpssize.width / 2 + 3, cpssize.height / 2 + 12 });
		}
		else {
			Fps->setPosition({ cpssize.width / 2 + 3, cpssize.height / 2 + fpsposition });
		}
		
		Fps->setVisible(true);
	}
	else {
		Fps->setVisible(false);
	}

	if (CPScounter) {
		if (noclipAcc)
		{
			cpsposition = 12; 
		}
		else {
			cpsposition = 3;
		}
		
		auto CpsTextText = CPSText();
		CpsText->setString(CpsTextText.c_str());
		auto cpssize = CpsText->getScaledContentSize();
		CpsText->setPosition({ cpssize.width / 2 + 3, cpssize.height / 2 + cpsposition });
		CpsText->setVisible(true);
	} else {
		CpsText->setVisible(false);
	}

	if (noclipAcc) {
		if (!CPScounter) { fpsposition = 12; }
		if (!noclipAcc) { fpsposition = 3; }
		if (!noclipAcc) { cpsposition = 3; }
		if (CPScounter && noclipAcc) { fpsposition = 21; }
		auto text = getAccuracyText();
		noclipAccText->setString(text.c_str());
		auto nasize = noclipAccText->getScaledContentSize();
		noclipAccText->setPosition({ nasize.width / 2 + 3, nasize.height / 2 + 3});
		noclipAccText->setVisible(true);
	}
	else {
		noclipAccText->setVisible(false);
	}

	if (CPScounter) {
		now = std::chrono::system_clock::now();
		cycleTime = now - start;
		if (cycleTime.count() > 1)
		{
			PlayLayer::cpsint = 0;
			start = now;
		}
	}

	if (wouldDie) {
		wouldDie = false;
		if (totalDelta >= 0.1 && x != prevX) {
			deaths += 1;
		}
	}

	prevX = x;
	//Noclip Accuracy
	return update(self, dt);
	}

	void __fastcall PlayLayer::resetLevelHook(gd::PlayLayer* self, void*) {
	//Noclip Accuracy
	void* player1 = *(void**)((char*)self + 0x224);
	prevX = *(float*)((size_t)player1 + 0x67c);
	frames = 0;
	totalDelta = 0;
	deaths = 0;
	wouldDie = false;
	//Noclip Accuracy

	cpscount = 0; //cps
	PlayLayer::resetLevel(self);
	self->m_attemptLabel->setVisible(!PlayLayer::hideattW);
		if (PlayLayer::totalattW) {
			std::string totalcount = "Attempt " + std::to_string(self->m_level->m_nAttempts);
			self->m_attemptLabel->setCString(totalcount.c_str());
		}	
	}

	void __fastcall PlayLayer::onQuitHook(gd::PlayLayer* self, void*) {
		cpscount = 0; //cps
		PlayLayer::onQuit(self);
	}

	bool __fastcall PlayLayer::initHook(gd::PlayLayer* self, void*, void* level) {
		play_layer = self;
	
		prevX = 0;
		fpsposition = 21;
		cpsposition = 12;
		noclipaccuracyposition = 3;

		CCLabelBMFont* noclipAccText = CCLabelBMFont::create("Accuracy: 100.00%", "chatFont.fnt");
		CCLabelBMFont* CpsText = CCLabelBMFont::create("CPS: 0/0", "chatFont.fnt");
		CCLabelBMFont* Fps = CCLabelBMFont::create("FPS: ", "chatFont.fnt");

		noclipAccText->setZOrder(1000);
		noclipAccText->setTag(100000);
		noclipAccText->setScale(0.5);
		
		Fps->setZOrder(2000);
		Fps->setTag(200000);
		Fps->setScale(0.5);

		CpsText->setZOrder(1500);
		CpsText->setTag(150000);
		CpsText->setScale(0.5);


		self->addChild(Fps);
		self->addChild(noclipAccText);
		self->addChild(CpsText);
		//Auto Toggle Practice
		if (practice)
			play_layer->togglePracticeMode(true);
		//Auto Toggle Practice	

		return PlayLayer::init(self, level);

		
	}
	
	int __fastcall PlayLayer::hkDeath(void* self, void*, void* go, void* powerrangers) {
	//Noclip Accuracy
	size_t base = (size_t)GetModuleHandle(0);

	if (noclipAcc) {
		wouldDie = true;
	}
	//Noclip Accuracy

	
	return PlayLayer::death(self, go, powerrangers);
}
	bool __fastcall PlayLayer::pushButtonHook(gd::PlayerObject* self, int edx, void* PlayerButton) {
		if (CPScounter) { cpscount++; cpsint++; }
		return PlayLayer::pushButton(self, PlayerButton);
	}

	bool __fastcall PlayLayer::releaseButtonHook(gd::PlayerObject* self, int edx, void* PlayerButton) {
		return PlayLayer::releaseButton(self, PlayerButton);
	}

	//0x20D3C0
	void PlayLayer::mem_init() {
		size_t base = (size_t)GetModuleHandle(0);

		MH_CreateHook(
			(PVOID)(base + 0x2029C0),
			PlayLayer::updateHook,
			(PVOID*)&PlayLayer::update
		);

		MH_CreateHook(
			(PVOID)(base + 0x20BF00),
			PlayLayer::resetLevelHook,
			(PVOID*)&PlayLayer::resetLevel
		);

		MH_CreateHook(
			(PVOID)(base + 0x20D810),
			PlayLayer::onQuitHook,
			(PVOID*)&PlayLayer::onQuit
		);

		MH_CreateHook(
			(PVOID)(base + 0x01FB780),
			PlayLayer::initHook,
			(PVOID*)&PlayLayer::init
		);


		MH_CreateHook(
			(PVOID)(base + 0x1F4E40),
			PlayLayer::pushButtonHook,
			(LPVOID*)&PlayLayer::pushButton
		);

		MH_CreateHook(
			(PVOID)(base + 0x1F4F70),
			PlayLayer::releaseButtonHook,
			(LPVOID*)&PlayLayer::releaseButton
		);
		
		MH_CreateHook(
			(PVOID)(base + 0x20A1A0),
			PlayLayer::hkDeath,
			(LPVOID*)&PlayLayer::death);
	}