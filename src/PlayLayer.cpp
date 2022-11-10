#include "PlayLayer.h"
#include "fpsbypass.h"
#include "spamBot.h"
#include "toolsTab.h"

gd::GameObject* bg;
bool PlayLayer::layoutMode = false;
bool PlayLayer::rgbIcons = false;
bool PlayLayer::showallatt = false;
bool PlayLayer::hideatt = false;
bool PlayLayer::noclip = false;

namespace DashReplayEngine {
	int DashReplayEngine::frame = 0;
	bool onlydashreplay = false;
	int DashReplayEngine::mode = 0; //0 - Disable; 1 - Recording; 2 - Playing
	bool DashReplayEngine::DownP1;
	bool DashReplayEngine::DownP2;
	vector<replaydata> DashReplayEngine::replay_p1;
	vector<replaydata> DashReplayEngine::replay_p2;
	vector<replaydata> DashReplayEngine::checkpoints;
	vector<int> DashReplayEngine::invframes;

	void DashReplayEngine::clearMacro() {
		replay_p1.clear();
		replay_p2.clear();
		checkpoints.clear();
	}

	bool DashReplayEngine::saveMacro(string s) {
		if (replay_p1.empty()) return false;
		fstream file(s, std::ios::out);
        if (file.is_open()) {
            file << "DashReplay Engine v3.0.0 //frame, pos_x, pos_y, rotation, y_vel, isDown" << "\n";
			file << "Framerate: " << FPSMultiplier::g_target_fps << "\n";
			file << "1P Replay Size: " << replay_p1.size() << "\n";
			for (int i = 0; i < (int)replay_p1.size(); i++) {
				file << to_string(DashReplayEngine::replay_p1[i].frame) << " " << to_string(DashReplayEngine::replay_p1[i].pos_x) << " " <<
					to_string(DashReplayEngine::replay_p1[i].pos_y) << " " << to_string(DashReplayEngine::replay_p1[i].rotation) << " " <<
						to_string(DashReplayEngine::replay_p1[i].y_vel) << " " << uselessShit::boolToString(DashReplayEngine::replay_p1[i].down)  << "\n";
			}
			file << "2P Replay Size: " << replay_p2.size() << "\n";
			for (int i = 0; i < (int)replay_p2.size(); i++) {
				file << DashReplayEngine::replay_p2[i].frame << " " << to_string(DashReplayEngine::replay_p2[i].pos_x) << " " <<
					to_string(DashReplayEngine::replay_p2[i].pos_y) << " " << to_string(DashReplayEngine::replay_p2[i].rotation) << " " <<
						to_string(DashReplayEngine::replay_p2[i].y_vel) << " " << uselessShit::boolToString(DashReplayEngine::replay_p2[i].down) << "\n";
			}
        }
        return true;
	}

	bool DashReplayEngine::loadMacro(string s) {
		string line;
		string splitword;
		int replaysize;
		vector<string> splitwords;

		fstream file(s, std::ios::in);
        if (!file.is_open())
            return false;
		getline(file, line);
		if (line.substr(0, 24) != "DashReplay Engine v3.0.0") {
			if (MessageBoxA(0, "Replay was recorded with old version of DashReplay. In this case, the replay may be broken. Continue?", "DashReplayAPI", MB_YESNO) == IDNO) 
				return false;
		}
		getline(file, line);
		if (line.substr(0, 11) == "Framerate: ") {
			line.erase(0, 11);
			FPSMultiplier::g_target_fps = stof(line);
		}
		getline(file, line);
		if (line.substr(0, 16) == "1P Replay Size: ") {
			line.erase(0, 16);
			replaysize = stoi(line);
		}
		for (int i = 0; i < replaysize; i++) {
			getline(file, line);
			istringstream splitstr(line);
			splitwords.clear();
            while (getline(splitstr, splitword, ' ')) {
                splitwords.push_back(splitword);
            }
			replaydata newdata = {stoi(splitwords[0]), stof(splitwords[1]), stof(splitwords[2]), stof(splitwords[3]),
				stof(splitwords[4]), uselessShit::stringToBool(splitwords[5])};
			DashReplayEngine::replay_p1.push_back(newdata);
		}
		getline(file, line);
		if (line.substr(0, 16) == "2P Replay Size: ") {
			line.erase(0, 16);
			replaysize = stoi(line);
		}
		for (int i = 0; i < replaysize; i++) {
			getline(file, line);
			istringstream splitstr(line);
			splitwords.clear();
            while (getline(splitstr, splitword, ' ')) {
                splitwords.push_back(splitword);
            }
			replaydata newdata = {stoi(splitwords[0]), stof(splitwords[1]), stof(splitwords[2]), stof(splitwords[3]),
				stof(splitwords[4]), uselessShit::stringToBool(splitwords[5])};
			DashReplayEngine::replay_p2.push_back(newdata);
		}
		return true;
	}
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

namespace PlayLayer {
	bool __fastcall PlayLayer::initHook(gd::PlayLayer* self, int edx, gd::GJGameLevel* level) {
		bool ret = PlayLayer::init(self, level);
		for (int i = 0; i < (int)self->getChildrenCount(); i++) {
			CCSprite* child = (CCSprite*)self->getChildren()->objectAtIndex(i);
			if (child->getZOrder() == -1) {
				bg = (gd::GameObject*)child;
				break;
			}
		}

		if (rgbIcons) {
			CCAction* rgb1 = create_rgb(1.0f);
			CCAction* rgb2 = create_rgb(1.0f, true);
			((CCNode*)self->m_pPlayer1)->runAction((CCAction*)rgb1->copy());
			((CCNode*)((CCNode*)self->m_pPlayer1->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0))->runAction((CCAction*)rgb2->copy());
			((CCNode*)self->m_pPlayer1->getChildren()->objectAtIndex(1))->runAction((CCAction*)rgb1->copy());
			((CCNode*)((CCNode*)self->m_pPlayer1->getChildren()->objectAtIndex(1))->getChildren()->objectAtIndex(0))->runAction((CCAction*)rgb2->copy());
			((CCNode*)self->m_pObjectLayer->getChildren()->objectAtIndex(49))->runAction(create_rgb(1.0f));
			((CCNode*)self->m_pObjectLayer->getChildren()->objectAtIndex(50))->runAction(create_rgb(1.0f));
		}
		return ret;
	}

	void __fastcall PlayLayer::updateHook(gd::PlayLayer* self, int edx, float deltaTime) {
		PlayLayer::update(self, deltaTime);	
		if (spambot::enable) {
			if (spambot::NextFrame()) {
				if (spambot::downed) {
					if (spambot::player1) PlayLayer::releaseButton(self, 0, true);
					if (spambot::player2) PlayLayer::releaseButton(self, 0, false);
				}
				else {
					if (spambot::player1) PlayLayer::pushButton(self, 0, true);
					if (spambot::player2) PlayLayer::pushButton(self, 0, false);
				}
			}
		}

		if (layoutMode) {
			for (int i = 0; i < (int)self->m_pObjects->count(); i++) {
				gd::GameObject* obj = (gd::GameObject*)self->m_pObjects->objectAtIndex(i);
				if (obj->m_nObjectType == gd::kGameObjectTypeDecoration) {
					obj->setVisible(false);
					obj->removeMeAndCleanup();
				} else {
					obj->setColor({255, 255, 255});
					obj->setOpacity(255);
					bg->setColor({40, 125, 255});
				}
			}
		}
		
		if (self->m_pPlayer1->m_position.x != 0 && !self->m_isDead) DashReplayEngine::frame++;
		if (DashReplayEngine::frame) {
			if (DashReplayEngine::mode == 1) {
				replaydata newdata_p1 = {DashReplayEngine::frame, self->m_pPlayer1->m_position.x, self->m_pPlayer1->m_position.y, self->m_pPlayer1->getRotation(), (float)self->m_pPlayer1->m_yAccel,
					DashReplayEngine::DownP1};
				DashReplayEngine::replay_p1.push_back(newdata_p1);

				replaydata newdata_p2 = {DashReplayEngine::frame, self->m_pPlayer2->m_position.x, self->m_pPlayer2->m_position.y, self->m_pPlayer2->getRotation(), (float)self->m_pPlayer1->m_yAccel,
					DashReplayEngine::DownP2};
				DashReplayEngine::replay_p2.push_back(newdata_p2);
			}
			else if (DashReplayEngine::mode == 2) {
				if (DashReplayEngine::frame > (int)DashReplayEngine::replay_p1.size()) return;
				if (std::find(DashReplayEngine::invframes.begin(), DashReplayEngine::invframes.end(), DashReplayEngine::frame) != DashReplayEngine::invframes.end()) noclip = true;
				else noclip = false;
				self->m_pPlayer1->m_position.x = DashReplayEngine::replay_p1[DashReplayEngine::frame - 1].pos_x;
				self->m_pPlayer1->m_position.y = DashReplayEngine::replay_p1[DashReplayEngine::frame - 1].pos_y;
				self->m_pPlayer1->setRotation(DashReplayEngine::replay_p1[DashReplayEngine::frame - 1].rotation);
				self->m_pPlayer1->m_yAccel = DashReplayEngine::replay_p1[DashReplayEngine::frame - 1].y_vel;

				if (DashReplayEngine::replay_p1[DashReplayEngine::frame].down && !DashReplayEngine::DownP1) {
					DashReplayEngine::DownP1 = true;
					PlayLayer::pushButton(self, 0, true);
				}

				if (!DashReplayEngine::replay_p1[DashReplayEngine::frame].down && DashReplayEngine::DownP1) {
					DashReplayEngine::DownP1 = false;
					PlayLayer::releaseButton(self, 0, true);
				}

				self->m_pPlayer2->m_position.x = DashReplayEngine::replay_p2[DashReplayEngine::frame - 1].pos_x;
				self->m_pPlayer2->m_position.y = DashReplayEngine::replay_p2[DashReplayEngine::frame - 1].pos_y;
				self->m_pPlayer2->setRotation(DashReplayEngine::replay_p2[DashReplayEngine::frame - 1].rotation);
				self->m_pPlayer2->m_yAccel = DashReplayEngine::replay_p2[DashReplayEngine::frame - 1].y_vel;

				if (DashReplayEngine::replay_p2[DashReplayEngine::frame].down && !DashReplayEngine::DownP2) {
					DashReplayEngine::DownP2 = true;
					PlayLayer::pushButton(self, 0, false);
				}

				if (!DashReplayEngine::replay_p2[DashReplayEngine::frame].down && DashReplayEngine::DownP2) {
					DashReplayEngine::DownP2 = false;
					PlayLayer::releaseButton(self, 0, false);
				}
			}
		}
	}

	void __fastcall PlayLayer::resetLevelHook(gd::PlayLayer* self) {
	    PlayLayer::resetLevel(self);
		if (!self->m_isPracticeMode) DashReplayEngine::frame = 0;
		else {
			if (!DashReplayEngine::checkpoints.empty()) {
				while (DashReplayEngine::checkpoints.back().frame < DashReplayEngine::replay_p1.back().frame) {
					if (!DashReplayEngine::replay_p1.empty()) DashReplayEngine::replay_p1.pop_back();
					else break;
				}

				while (DashReplayEngine::checkpoints.back().frame < DashReplayEngine::replay_p2.back().frame) {
					if (!DashReplayEngine::replay_p2.empty()) DashReplayEngine::replay_p2.pop_back();
					else break;
				}

				self->m_pPlayer1->setRotation(DashReplayEngine::replay_p1.back().rotation);
				self->m_pPlayer1->m_yAccel = DashReplayEngine::replay_p1.back().y_vel;
				self->m_pPlayer2->setRotation(DashReplayEngine::replay_p2.back().rotation);
				self->m_pPlayer2->m_yAccel = DashReplayEngine::replay_p2.back().y_vel;
			}
			else {
				DashReplayEngine::frame = 0;
			}
		}

		if (hideatt)
			self->m_attemptLabel->setVisible(false);
		if (showallatt) {
			string s = "Attempt " + to_string(self->m_level->attempts);
			self->m_attemptLabel->setString(s.c_str());
		}

		if (rgbIcons) {
			CCAction* rgb1 = create_rgb(1.0f);
			CCAction* rgb2 = create_rgb(1.0f, true);

			((CCNode*)self->m_pPlayer2)->runAction((CCAction*)rgb2->copy());
			((CCNode*)((CCNode*)self->m_pPlayer2->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0))->runAction((CCAction*)rgb1->copy());
			((CCNode*)self->m_pPlayer2->getChildren()->objectAtIndex(1))->runAction((CCAction*)rgb2->copy());
			((CCNode*)((CCNode*)self->m_pPlayer2->getChildren()->objectAtIndex(1))->getChildren()->objectAtIndex(0))->runAction((CCAction*)rgb1->copy());
		}

	}

	void __fastcall PlayLayer::onQuitHook(gd::PlayLayer* self) {
		DashReplayEngine::mode = 0;
		toolsTab::frameadvance = false;
		PlayLayer::onQuit(self);			
	}

	bool __fastcall PlayLayer::pushButtonHook(gd::PlayLayer* self, uintptr_t, int state, bool player) {
        auto ret = PlayLayer::pushButton(self, state, player);
		if (player) DashReplayEngine::DownP1 = true;
		if (!player) DashReplayEngine::DownP2 = true;
		return ret;
	}

	bool __fastcall PlayLayer::releaseButtonHook(gd::PlayLayer* self, uintptr_t, int state, bool player) {
        auto ret = releaseButton(self, state, player);
		if (player) DashReplayEngine::DownP1 = false;
		if (!player) DashReplayEngine::DownP2 = false;
		return ret;
	}

	void __fastcall PlayLayer::levelCompleteHook(void* self) {
		if (DashReplayEngine::mode == 1) DashReplayEngine::mode = 0;
		PlayLayer::levelComplete(self);
	}

	int __fastcall PlayLayer::createCheckpointHook(gd::PlayLayer* self) {
		auto ret = PlayLayer::createCheckpoint(self);
		replaydata newdata = {DashReplayEngine::frame};
		DashReplayEngine::checkpoints.push_back(newdata);
		return ret;
	}

	int __fastcall PlayLayer::removeCheckpointHook(gd::PlayLayer* self) {
		auto ret = PlayLayer::removeCheckpoint(self);
		if (!DashReplayEngine::checkpoints.empty()) DashReplayEngine::checkpoints.pop_back();
		return ret;
	}

	int __fastcall PlayLayer::hkDeath(void* self, void*, void* go, void* powerrangers) {	
		int ret;
		if (!PlayLayer::noclip) ret = PlayLayer::death(self, go, powerrangers);
		return ret;
	}

	void __fastcall PlayLayer::togglePracticeHook(gd::PlayLayer* self, int edx, bool practice) {
		PlayLayer::togglePractice(self, practice);
		if (DashReplayEngine::mode == 1 && self->m_pPlayer1->m_position.x == 0) {
			DashReplayEngine::frame = 0;
			DashReplayEngine::clearMacro();
		}	
	}

	inline bool(__thiscall* LevelEditorLayer_init)(gd::LevelEditorLayer* self, gd::GJGameLevel* GJGameLevel);
	bool __fastcall LevelEditorLayer_initHook(gd::LevelEditorLayer* self, int edx, gd::GJGameLevel* level) {
		auto ret = LevelEditorLayer_init(self, level);
		toolsTab::frameadvance = false;
		DashReplayEngine::mode = 0;
		return ret;
	}
	void PlayLayer::mem_init() {
		size_t base = (size_t)GetModuleHandle(0);

		MH_CreateHook(
			(PVOID)(base + 0x01FB780),
			PlayLayer::initHook,
			(LPVOID*)&PlayLayer::init
		);

		MH_CreateHook(
			(PVOID)(base + 0x2029C0),
			PlayLayer::updateHook,
			(LPVOID*)&PlayLayer::update
		);

		MH_CreateHook(
			(PVOID)(base + 0x20BF00),
			PlayLayer::resetLevelHook,
			(LPVOID*)&PlayLayer::resetLevel
		);

		MH_CreateHook(
			(PVOID)(base + 0x111500),
			PlayLayer::pushButtonHook,
			(LPVOID*)&PlayLayer::pushButton
		);

		MH_CreateHook(
			(PVOID)(base + 0x111660),
			PlayLayer::releaseButtonHook,
			(LPVOID*)&PlayLayer::releaseButton
		);

		MH_CreateHook(
			(PVOID)(base + 0x20D810),
			PlayLayer::onQuitHook,
			(LPVOID*)&PlayLayer::onQuit
		);

		MH_CreateHook(
			(PVOID)(base + 0x20B050),
			PlayLayer::createCheckpointHook,
			(LPVOID*)&PlayLayer::createCheckpoint
		);

		MH_CreateHook(
			(PVOID)(base + 0x20B830),
			PlayLayer::removeCheckpointHook,
			(LPVOID*)&PlayLayer::removeCheckpoint
		);

		MH_CreateHook(
			(PVOID)(base + 0x1FD3D0),
			PlayLayer::levelCompleteHook,
			(LPVOID*)&PlayLayer::levelComplete
		);

		MH_CreateHook(
			(PVOID)(base + 0x20A1A0),
			PlayLayer::hkDeath,
			(LPVOID*)&PlayLayer::death
		);

		MH_CreateHook(
			(PVOID)(base + 0x20D0D0),
			PlayLayer::togglePracticeHook,
			(LPVOID*)&PlayLayer::togglePractice
		);

		MH_CreateHook(
			(PVOID)(base + 0x15EE00),
			LevelEditorLayer_initHook,
			(LPVOID*)&LevelEditorLayer_init
		);
	}
}