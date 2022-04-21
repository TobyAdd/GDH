#ifndef __GAMESOUNDMANAGER_H__
#define __GAMESOUNDMANAGER_H__

#include <gd.h>
#include <filesystem>

namespace gd {
	class GDH_DLL GameSoundManager : public cocos2d::CCNode {
	protected:
		cocos2d::CCDictionary* m_pDictionary1;
		cocos2d::CCDictionary* m_pDictionary2;
		PAD(12);
		bool m_bPreloaded;
		PAD(4);
		std::string m_sFilePath;

	public:
		static GameSoundManager* sharedState() {
			return reinterpret_cast<GameSoundManager* (__stdcall*)()>(
				base + 0x24800
				)();
		}
		static void playSound(std::string sName) {
			reinterpret_cast<void(__stdcall*)(
				std::string
			)>(
				base + 0x25450
			)(
				sName
			);
		}
		void playBackgroundMusic(std::string path, bool idk, bool idk2) {
			reinterpret_cast<void(__thiscall*)(GameSoundManager*, std::string, bool, bool)>(base + 0x252b0)(this, path, idk, idk2);
		}
		// penis
		void playBackgroundMusic(bool idk, std::string path) {
			reinterpret_cast<void(__thiscall*)(GameSoundManager*, bool, std::string)>(base + 0x4bf00)(this, idk, path);
		}
		void stopBackgroundMusic() {
			reinterpret_cast<void(__thiscall*)(GameSoundManager*)>(base + 0x253a0)(this);
		}
	};
}

#endif