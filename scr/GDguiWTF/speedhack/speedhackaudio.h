#include <Windows.h>
#include "imgui/kiero/minhook/include/MinHook.h"

namespace SpeedhackAudio {
	void* channel;
	float speed;
	bool initialized = false;

	// setfrequency
	// setvolume

	void* (__stdcall* setVolume)(void* t_channel, float volume);
	void* (__stdcall* setFrequency)(void* t_channel, float frequency);

	void* __stdcall AhsjkabdjhadbjJHDSJ(void* t_channel, float volume) {
		channel = t_channel;

		if (speed != 1.f) {
			setFrequency(channel, speed);

		}
		return setVolume(channel, volume);
	}

	void init() {
		if (initialized)
			return;

		setFrequency = (decltype(setFrequency))GetProcAddress(GetModuleHandle("fmod.dll"), "?setPitch@ChannelControl@FMOD@@QAG?AW4FMOD_RESULT@@M@Z");
		DWORD hkAddr = (DWORD)GetProcAddress(GetModuleHandle("fmod.dll"), "?setVolume@ChannelControl@FMOD@@QAG?AW4FMOD_RESULT@@M@Z");

		MH_CreateHook(
			(PVOID)hkAddr,
			AhsjkabdjhadbjJHDSJ,
			(PVOID*)&setVolume
		);

		speed = 1.f;
		initialized = true;
	}

	void set(float frequency) {
		if (!initialized)
			init();

		if (channel == nullptr)
			return;

		speed = frequency;
		setFrequency(channel, frequency);
	}
}
