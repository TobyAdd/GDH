#pragma once
#include "PlayLayer.h"
#include "GameManager.h"
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
int PlayLayer::player_rainbow;
float PlayLayer::player_rainbow_speed;
float PlayLayer::player_rainbow_speed2;
int PlayLayer::player2_rainbow;
float PlayLayer::player2_rainbow_speed;
float PlayLayer::player2_rainbow_speed2;
int PlayLayer::player3_rainbow;
float PlayLayer::player3_rainbow_speed;
int PlayLayer::player4_rainbow;
float PlayLayer::player4_rainbow_speed;
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

int fpsposition;
int cpsposition;
int noclipaccuracyposition;

void WriteBytes(void* location, std::vector<BYTE> bytes) {
	DWORD old_prot;
	VirtualProtect(location, bytes.size(), PAGE_EXECUTE_READWRITE, &old_prot);
	memcpy(location, bytes.data(), bytes.size());	
	VirtualProtect(location, bytes.size(), old_prot, &old_prot);
}

CCAction* create_rgb(float speed, bool is_reverse = false) {
	return is_reverse ?
		CCRepeatForever::create(CCSequence::create(
			CCTintTo::create(speed, 0, 255, 255),
			CCTintTo::create(speed, 0, 0, 255),
			CCTintTo::create(speed, 255, 0, 255),
			CCTintTo::create(speed, 255, 0, 0),
			CCTintTo::create(speed, 255, 255, 0),
			CCTintTo::create(speed, 0, 255, 0),
			nullptr
		)) : CCRepeatForever::create(CCSequence::create(
			CCTintTo::create(speed, 255, 0, 0),
			CCTintTo::create(speed, 255, 255, 0),
			CCTintTo::create(speed, 0, 255, 0),
			CCTintTo::create(speed, 0, 255, 255),
			CCTintTo::create(speed, 0, 0, 255),
			CCTintTo::create(speed, 255, 0, 255),
			nullptr
		));
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
	if (player2_rainbow == 1) {
		CCAction* rgb1 = create_rgb(player2_rainbow_speed);
		CCAction* rgb2 = create_rgb(player2_rainbow_speed, true);
		((CCNode*)self->m_pPlayer2)->runAction((CCAction*)rgb2->copy());
		((CCNode*)((CCNode*)self->m_pPlayer2->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0))->runAction((CCAction*)rgb1->copy());
		((CCNode*)self->m_pPlayer2->getChildren()->objectAtIndex(1))->runAction((CCAction*)rgb2->copy());
		((CCNode*)((CCNode*)self->m_pPlayer2->getChildren()->objectAtIndex(1))->getChildren()->objectAtIndex(0))->runAction((CCAction*)rgb1->copy());
	}
	}

	void __fastcall PlayLayer::onQuitHook(gd::PlayLayer* self, void*) {
		cpscount = 0; //cps
		PlayLayer::onQuit(self);
	}

	bool __fastcall PlayLayer::initHook(gd::PlayLayer* self, void*, void* level) {
		bool result = PlayLayer::init(self, level);
		play_layer = self;
		
		size_t base = (size_t)GetModuleHandle(0);
	
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

		//Rainbow
		if (PlayLayer::player_rainbow == 1) {
			CCAction* rgb1 = create_rgb(PlayLayer::player_rainbow_speed);
			CCAction* rgb2 = create_rgb(PlayLayer::player_rainbow_speed, true);
			((CCNode*)self->m_pPlayer1)->runAction((CCAction*)rgb1->copy());
			((CCNode*)((CCNode*)self->m_pPlayer1->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0))->runAction((CCAction*)rgb2->copy());
			((CCNode*)self->m_pPlayer1->getChildren()->objectAtIndex(1))->runAction((CCAction*)rgb1->copy());
			((CCNode*)((CCNode*)self->m_pPlayer1->getChildren()->objectAtIndex(1))->getChildren()->objectAtIndex(0))->runAction((CCAction*)rgb2->copy());
		}

		if (PlayLayer::player3_rainbow) {
			((CCNode*)self->m_pObjectLayer->getChildren()->objectAtIndex(49))->runAction(create_rgb(player3_rainbow_speed));
		}
		if (PlayLayer::player4_rainbow) {
			((CCNode*)self->m_pObjectLayer->getChildren()->objectAtIndex(50))->runAction(create_rgb(player4_rainbow_speed));
		}
		//Rainbow

		return result;
	}


	void PlayLayer::update_rainbow(int idx, int type, float speed, float speed2) {
		if (idx == 0) {
			PlayLayer::player_rainbow = type;
			PlayLayer::player_rainbow_speed = speed;
			PlayLayer::player_rainbow_speed2 = speed2;
			return;
		}
		if (idx == 1) {
			PlayLayer::player2_rainbow = type;
			PlayLayer::player2_rainbow_speed = speed;
			PlayLayer::player2_rainbow_speed2 = speed2;
			return;
		}
		if (idx == 2) {
			PlayLayer::player3_rainbow = type;
			PlayLayer::player3_rainbow_speed = speed;
			return;
		}
		if (idx == 3) {
			PlayLayer::player4_rainbow = type;
			PlayLayer::player4_rainbow_speed = speed;
			return;
		}
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


		return PlayLayer::pushButton(self, PlayerButton);
	}

	bool __fastcall PlayLayer::releaseButtonHook(gd::PlayerObject* self, int edx, void* PlayerButton) {
		if (CPScounter) { cpscount++; cpsint++; }
		return PlayLayer::releaseButton(self, PlayerButton);
	}


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