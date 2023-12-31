#include "include.hpp"
#include "sdk.hpp"

inline uintptr_t get_function(const char* mod, const char* name) {
	return (uintptr_t)(GetProcAddress(GetModuleHandleA(mod), name));
}

void sdk::cc_scheduler::set_time_scale(float val) {
	static auto fn = get_function("libcocos2d.dll", "?setTimeScale@CCScheduler@cocos2d@@QAEXM@Z");
	((void(__thiscall*)(cc_scheduler*, float))(fn))(this, val);
}

sdk::cc_director* sdk::cc_director::shared_director() {
	static auto fn = get_function("libcocos2d.dll", "?sharedDirector@CCDirector@cocos2d@@SAPAV12@XZ");
	return ((cc_director*(*)())(fn))();
}

sdk::cc_scheduler* sdk::cc_director::get_scheduler() {
	static auto fn = get_function("libcocos2d.dll", "?getScheduler@CCDirector@cocos2d@@UAEPAVCCScheduler@2@XZ");
	return ((cc_scheduler*(__thiscall*)(cc_director*))(fn))(this);
}

sdk::channel_control* sdk::channel_control::get_gd_channel() {
	static auto main = (uintptr_t)(GetModuleHandleA(nullptr));
	return (channel_control*)(*(uintptr_t*)(*(uintptr_t*)(main + 0x4E82C0) + 0x18C));
}

void sdk::channel_control::set_pitch(float pitch) {
	static auto fn = get_function("fmod.dll", "?setPitch@ChannelControl@FMOD@@QAG?AW4FMOD_RESULT@@M@Z");
	return ((void(__stdcall*)(channel_control*, float))(fn))(this, pitch);
}