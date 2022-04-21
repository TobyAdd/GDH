#ifndef __FMODAUDIOENGINE_H__
#define __FMODAUDIOENGINE_H__

#include <gd.h>

namespace gd {
	class GDH_DLL FMODAudioEngine : public cocos2d::CCNode {
	public:
		cocos2d::CCDictionary* m_pDictionary;
		std::string m_sFilePath;
		float m_fBackgroundMusicVolume;
		float m_fEffectsVolume;
		float m_fPulse1;
		float m_fPulse2;
		float m_fPulse3;
		int m_nPulseCounter;
		bool m_bMetering;
		bool m_bFading;
		bool m_bFadeIn;
		float m_fFadeInDuration;
		FMOD::System* m_pSystem;
		FMOD::Sound* m_pSound;
		FMOD::Channel* m_pCurrentSoundChannel;
		FMOD::Channel* m_pGlobalChannel;
		FMOD::DSP* m_pDSP;
		FMOD_RESULT m_eLastResult;
		int m_nVersion;
		void* m_pExtraDriverData;
		int m_nMusicOffset;

	public:
		static FMODAudioEngine* sharedEngine() {
			return reinterpret_cast<FMODAudioEngine * (__stdcall*)()>(
				base + 0x239F0
				)();
		}
		void preloadEffect(std::string filename) {
			return reinterpret_cast<void(__thiscall*)(FMODAudioEngine*, std::string)>(
				base + 0x24240
				)(this, filename);
		}
		float getMusicVolume() { return m_fBackgroundMusicVolume; }
		float getSFXVolume() { return m_fEffectsVolume; }
		//my own function
		void reloadEffects() {
			using namespace std::filesystem;

			std::vector<std::string> oggs = {};
			path defaults = current_path() / "Resources";
			for (auto file : directory_iterator{ defaults }) {
				if (file.path().extension() == ".ogg")
					oggs.push_back(file.path().filename().string());
			}
			for (auto ogg : oggs) {
				m_pDictionary->removeObjectForKey(ogg);
				this->preloadEffect(ogg);
			}
		}
		// inlined on windows
		bool isBackgroundMusicPlaying() {
			const auto addr = GetProcAddress(GetModuleHandleA("fmod.dll"), "?isPlaying@ChannelControl@FMOD@@QAG?AW4FMOD_RESULT@@PA_N@Z");
			bool ret;
			reinterpret_cast<int(__stdcall*)(void*, bool*)>(addr)(this->m_pGlobalChannel, &ret);
			return ret;
		}
		void playBackgroundMusic(std::string const& path, bool idk0, bool idk1) {
			reinterpret_cast<void(__thiscall*)(FMODAudioEngine*, bool, bool, std::string)>(
				base + 0x23d80
			)(this, idk0, idk1, path);
		}
		bool isBackgroundMusicPlaying(const std::string& path) {
			return path == m_sFilePath && isBackgroundMusicPlaying();
		}
	};
}

#endif