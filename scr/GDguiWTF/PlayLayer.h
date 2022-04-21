#pragma once
#include "cocos2d.h"
#include <gd.h>
#include <vector>
using namespace cocos2d;
namespace PlayLayer {


	extern bool noclipAcc;
	extern bool wouldDie;
	extern float prevX;
	extern int deaths;
	extern int frames;
	extern float totalDelta;

	extern bool practice;

	extern int player_rainbow;
	extern float player_rainbow_speed;
	extern float player_rainbow_speed2;
	extern int player2_rainbow;
	extern float player2_rainbow_speed;
	extern float player2_rainbow_speed2;
	extern int player3_rainbow;
	extern float player3_rainbow_speed;
	extern int player4_rainbow;
	extern float player4_rainbow_speed;
	extern bool CPScounter;
	extern int cpsint;
	extern int cpscount;
	extern bool FPScounter;

	extern int fpsint;



	inline void(__thiscall* update)(gd::PlayLayer* self, float dt);
	void __fastcall updateHook(gd::PlayLayer* self, void*, float dt);

	inline void(__thiscall* resetLevel)(gd::PlayLayer* self);
	void __fastcall resetLevelHook(gd::PlayLayer* self, void*);

	inline int(__thiscall* death)(void* self, void* go, void* powerrangers);
	int __fastcall hkDeath(void* self, void*, void* go, void* powerrangers);

	inline bool(__thiscall* init)(gd::PlayLayer* self, void* level);
	bool __fastcall initHook(gd::PlayLayer* self, void*, void* level);

	inline bool(__thiscall* pushButton)(gd::PlayerObject* self, void* PlayerButton);
	bool __fastcall pushButtonHook(gd::PlayerObject* self, int edx, void* PlayerButton);

	inline bool(__thiscall* releaseButton)(gd::PlayerObject* self, void* PlayerButton);
	bool __fastcall releaseButtonHook(gd::PlayerObject* self, int edx, void* PlayerButton);

	inline void(__thiscall* onQuit)(gd::PlayLayer* self);
	void __fastcall onQuitHook(gd::PlayLayer* self, void*);

	void update_rainbow(int idx, int type, float speed, float speed2);

	void mem_init();
}