#include "pch.h"

struct replaydata {
	int frame;

	float pos_x;
	float pos_y;
	float rotation;
	float y_vel;

	bool down;
};

namespace DashReplayEngine {
	extern bool onlydashreplay;
	extern int frame;
	extern int mode;
	extern bool DownP1;
	extern bool DownP2;
	extern vector<replaydata> replay_p1;
	extern vector<replaydata> replay_p2;
	extern vector<replaydata> checkpoints;
	extern vector<int> invframes;
	void clearMacro();
	bool saveMacro(string s);
	bool loadMacro(string s);
}

namespace PlayLayer {
	extern bool layoutMode;
	extern bool rgbIcons;
	extern bool hideatt;
	extern bool showallatt;
	extern bool noclip;

	extern string topleft;
	extern string bottomleft;
	extern string topright;
	extern string bottomright;

    inline bool(__thiscall* init)(gd::PlayLayer* self, gd::GJGameLevel* GJGameLevel);
	bool __fastcall initHook(gd::PlayLayer* self, int edx, gd::GJGameLevel* GJGameLevel);

	inline void(__thiscall* update)(gd::PlayLayer* self, float deltatime);
	void __fastcall updateHook(gd::PlayLayer* self, int edx, float deltatime);

	inline void(__thiscall* resetLevel)(gd::PlayLayer* self);
	void __fastcall resetLevelHook(gd::PlayLayer* self);

	inline bool(__thiscall* pushButton)(gd::PlayLayer* self, int state, bool player);
	bool __fastcall pushButtonHook(gd::PlayLayer* self, uintptr_t, int state, bool player);

	inline bool(__thiscall* releaseButton)(gd::PlayLayer* self, int state, bool player);
	bool __fastcall releaseButtonHook(gd::PlayLayer* self, uintptr_t, int state, bool player);

	inline void(__thiscall* onQuit)(gd::PlayLayer* self);
	void __fastcall onQuitHook(gd::PlayLayer* self);
       
    inline void(__thiscall* levelComplete)(void* self);
    void __fastcall levelCompleteHook(void* self);

	inline int(__thiscall* createCheckpoint)(gd::PlayLayer* self);
	int __fastcall createCheckpointHook(gd::PlayLayer* self);

	inline int(__thiscall* removeCheckpoint)(gd::PlayLayer* self);
	int __fastcall removeCheckpointHook(gd::PlayLayer* self);

	inline int(__thiscall* death)(void* self, void* go, void* powerrangers);
	int __fastcall hkDeath(void* self, void*, void* go, void* powerrangers);

	inline void(__thiscall* togglePractice)(gd::PlayLayer* self, bool practice);
    void __fastcall togglePracticeHook(gd::PlayLayer* self, int edx, bool practice);

    void mem_init();
}