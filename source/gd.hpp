#pragma once
#include "includes.hpp"

template <class R, class T>
R& from(T base, intptr_t offset) {
	return *reinterpret_cast<R*>(reinterpret_cast<uintptr_t>(base) + offset);
}

const int FMOD_OUTPUTTYPE_AUTODETECT = 0;
const int FMOD_OUTPUTTYPE_WAVWRITER = 3;

namespace FMOD {
    static auto base = GetModuleHandleA("fmod.dll");
	
	struct Sound {
		void getDefaults(float *frequency, int *priority) {
            static const auto addr = GetProcAddress(base, "?getDefaults@Sound@FMOD@@QAG?AW4FMOD_RESULT@@PAMPAH@Z");
            reinterpret_cast<void*(__stdcall*)(void*, float *, int *)>(addr)(this, frequency, priority);
        }
    };

    struct Channel {
		void setPitch(float pitch) {
            static const auto addr = GetProcAddress(base, "?setPitch@ChannelControl@FMOD@@QAG?AW4FMOD_RESULT@@M@Z");
            reinterpret_cast<void*(__stdcall*)(void*, float)>(addr)(this, pitch);
        }

		void setVolume(float volume) {
            static const auto addr = GetProcAddress(base, "?setVolume@ChannelControl@FMOD@@QAG?AW4FMOD_RESULT@@M@Z");
            reinterpret_cast<void*(__stdcall*)(void*, float)>(addr)(this, volume);
        }

		void setFrequency(float frequency) {
            static const auto addr = GetProcAddress(base, "?setFrequency@Channel@FMOD@@QAG?AW4FMOD_RESULT@@M@Z");
            reinterpret_cast<void*(__stdcall*)(void*, float)>(addr)(this, frequency);
        }

		void getCurrentSound(Sound **sound) {
            static const auto addr = GetProcAddress(base, "?getCurrentSound@Channel@FMOD@@QAG?AW4FMOD_RESULT@@PAPAVSound@2@@Z");
            reinterpret_cast<void*(__stdcall*)(void*, Sound **sound)>(addr)(this, sound);
        }
    };
	
    struct System {
		void getChannel(int index, Channel **channel) {
            static const auto addr = GetProcAddress(base, "?getChannel@System@FMOD@@QAG?AW4FMOD_RESULT@@HPAPAVChannel@2@@Z");
            reinterpret_cast<void*(__stdcall*)(void*, int, Channel **)>(addr)(this, index, channel);
        }

        void setOutput(int output) {
            static const auto addr = GetProcAddress(base, "?setOutput@System@FMOD@@QAG?AW4FMOD_RESULT@@W4FMOD_OUTPUTTYPE@@@Z");
            reinterpret_cast<void*(__stdcall*)(void*, int)>(addr)(this, output);
        }
    };
}

namespace gd {

	using namespace cocos2d;

	static const auto base = reinterpret_cast<uintptr_t>(GetModuleHandleA(0));
	static const auto cocos_base = reinterpret_cast<HMODULE>(GetModuleHandleA("libcocos2d.dll"));

	struct string {
		union {
			char inline_data[16];
			char* ptr;
		} m_data;
		size_t m_size = 0;
		size_t m_capacity = 15;

		// TODO:
		// ~string() = delete;

		string(const char* data, size_t size) {
			reinterpret_cast<void*(__thiscall*)(void*, const char*, size_t)>(base + 0xa3f0)(this, data, size);
		}

		explicit string(const std::string_view& str) : string(str.data(), str.size()) {}
		// string(const char* str) : string(std::string_view(str)) {}
		string(const std::string& str) : string(str.c_str(), str.size()) {}

		size_t size() const { return m_size; }

		string& operator=(const std::string& other) {
			if (m_capacity > 15) delete m_data.ptr;
			reinterpret_cast<void*(__thiscall*)(void*, const char*, size_t)>(base + 0xa3f0)(this, other.c_str(), other.size());
			return *this;
		}

		const char* c_str() const {
			if (m_capacity <= 15) return m_data.inline_data;
			return m_data.ptr;
		}

		std::string_view sv() const {
			return std::string_view(c_str(), m_size);
		}

		operator std::string() const { return std::string(sv()); }
	};

	class MenuLayer : public CCLayer {
	public:
	};

	class PauseLayer : public CCLayer {
	public:
	};

	class GJGameLevel : public cocos2d::CCNode {
	public:
		auto m_lvlLength() {
        	return from<int>(this, 0x2D0);
		}

		bool isPlatformer() {
			return m_lvlLength() == 5;
		}

		auto m_levelCreator() {
			return from<std::string>(this, 0x160);
		}

		auto m_lvlID() {
			return from<int>(this, 0x10c) - from<int>(this, 0x110);
		}

		int getAverageDifficulty() {
			return reinterpret_cast<int(__thiscall*)(
				GJGameLevel*
			)>(
				base + 0x114180
			)(this);
		}

		auto m_levelName() {
			return from<std::string>(this, 0x118);
		}

		auto m_autoLvl() {
			return from<bool>(this, 0x30C);
		}

		auto m_demonDifficulty() {
			return from<int>(this, 0x2FC);
		}

		auto m_stars() {
			return from<int>(this, 0x300) - from<int>(this, 0x304);
		}

		auto m_isDemon() {
			return from<bool>(this, 0x2F0) - from<bool>(this, 0x2F4);
		}

		auto& m_lowDetail() {
			return from<bool>(this, 0x24D);
		}
	};

	class CustomSongWidget : public CCNode {
	public:
		auto m_downloadBtn() {
			return from<CCSprite*>(this, 0x128);
		}

		void onDownload() {
			return reinterpret_cast<void(__thiscall*)(CustomSongWidget*)>(base + 0x94660)(this);
		}
	};

	class GameObject : public CCSprite
	{
	public:
		auto m_glow() {
			return from<CCSprite*>(this, 0x2B00);
		}

		auto m_objectType() {
			return from<int>(this, 0x31c);
		}

		int m_nObjectID() {
            return from<int>(this, 0x384);
        }
	};

	class PlayerObject : public GameObject
	{
	public:
		auto& m_position() {
			return from<CCPoint>(this, 0x854);
		}

		auto& m_yAccel() {
			return from<double>(this, 0x790);
		}

		auto m_xAccel() {
			return from<double>(this, 0x630); 
		}

		auto& m_isUpsideDown() {
			return from<bool>(this, 0x7AF); 
		}

		auto& m_isHolding() {
			return from<bool>(this, 0x775); 
		}

		cocos2d::CCSprite* m_pSecondarySprite() {
            return from<cocos2d::CCSprite*>(this, 0x5f0);
        }

		auto m_waveTrail() {
			return from<cocos2d::CCSprite*>(this, 0x628);
		}
	};

	class PlayLayer;

	class GameManager : public CCNode {
	public:
		static GameManager* sharedState() {
			return reinterpret_cast<GameManager*(__stdcall*)()>(base + 0x121540)();
		}

		auto playerName() {
			return from<std::string>(this, 0x1D0);
		}

		PlayLayer* getPlayLayer() {
			return from<PlayLayer*>(this, 0x198);
		}

		bool getGameVariable(char const* var) {
			return reinterpret_cast<bool(__thiscall*)(GameManager*, char const*)>(base + 0x128730)(this, var);
		}
	};

	class LevelSettingsObject : public CCNode {
	public:
		auto& m_startDual() {
			return from<bool>(this, 0x115);
		}

		auto& m_flipGravity() {
			return from<bool>(this, 0x135);
		}

		auto& m_reverseGameplay() {
			return from<bool>(this, 0x136);
		}

		auto& m_startMode() {
			return from<int>(this, 0x10C);
		}

		auto& m_startMini() {
			return from<bool>(this, 0x114);
		}

		auto& m_startSpeed() {
			return from<int>(this, 0x110);
		}
	};

	class StartPosObject : public GameObject {
	public:
		auto m_pLevelSettingsObject() {
			return from<LevelSettingsObject*>(this, 0x678);
		}
	};

	class PlayLayer : public CCLayer {
		public:

		//practiceMode 0x2a7c

		void resumeAndRestart(bool resetLevelFromStart){
			reinterpret_cast<void(__thiscall*)(PlayLayer*, bool)>(base + 0x2eafe0)(this, resetLevelFromStart);
		}

		void setStartPosObject(uint32_t* obj){
			reinterpret_cast<void(__thiscall*)(PlayLayer*, uint32_t*)>(base + 0x199e90)(this, obj);
		}

		void resetLevelFromStart(){
			reinterpret_cast<void(__thiscall*)(PlayLayer*)>(base + 0x2ea090)(this);
		}

		void resetLevel(){
			reinterpret_cast<void(__thiscall*)(PlayLayer*)>(base + 0x2ea130)(this);
		}

		void startMusic() {
			reinterpret_cast<void(__thiscall*)(PlayLayer*)>(base + 0x2eb3a0)(this);
		}

		void removeCheckpoint(bool firstCheckpoint) {
			reinterpret_cast<void(__thiscall*)(PlayLayer*, bool)>(base + 0x2e8d70)(this, firstCheckpoint);
		}

		PlayerObject*& m_pPlayer1() {
			return from<PlayerObject*>(this, 0x878);
		}

		PlayerObject*& m_pPlayer2() {
			return from<PlayerObject*>(this, 0x87C);
		}

		auto m_pLevelSettingsObject() {
			return from<LevelSettingsObject*>(this, 0x2a40);
		}

		double m_time() {
            return from<double>(this, 0x328);
		}

		auto m_pHitboxNode() {
			return from<CCDrawNode*>(this, 0x2d70);
		}

		auto m_flipped() {
			return from<bool>(this, 0x36e);
		}

		void updateDebugDraw() {
			reinterpret_cast<void(__fastcall*)(PlayLayer*)>(base + 0x1986F0)(this);
		}

		auto m_isDead() {
            return from<bool>(this, 0x2ac2);
        }

		bool m_bTwoPlayerMode() {
			return from<bool>(this, 0x36e);
		}

		auto m_isPracticeMode() {
			return from<bool>(this, 0x2A7C);
		}

		auto& m_respawnTime() {
            return from<float>(this, 0x410);
        }
	};

	class GameStatsManager : public CCNode {
	public:
		static GameStatsManager* sharedState() {
			return reinterpret_cast<GameStatsManager * (__stdcall*)()>(base + 0x167d90)();
		}

		bool isItemUnlocked(int unlocktype, int id) {
			return reinterpret_cast<bool(__thiscall*)(GameStatsManager*, int, int)>(base + 0x176570)(this, unlocktype, id);
		}

		bool isItemEnabled(int unlocktype, int id) {
			return reinterpret_cast<bool(__thiscall*)(GameStatsManager*, int, int)>(base + 0x1767f0)(this, unlocktype, id);
		}
	};

	class GJGroundLayer : public cocos2d::CCLayer {
	public:

		void updateGround01Color(_ccColor3B param_1) {
			reinterpret_cast<void(__fastcall*)(GJGroundLayer*, _ccColor3B)>(base + 0x1f7ab0)(this, param_1);
		}

		void updateGround02Color(_ccColor3B param_1) {
			reinterpret_cast<void(__fastcall*)(GJGroundLayer*, _ccColor3B)>(base + 0x1f7b10)(this, param_1);
		}

	};

	class GJMGLayer : public cocos2d::CCLayer {
	public:

		void updateGroundColor(_ccColor3B param_1, bool param_2) {
			reinterpret_cast<void(__thiscall*)(GJMGLayer*, _ccColor3B, bool)>(base + 0x1f8640)(this, param_1, param_2);
			
		}

		void updateGroundOpacity(unsigned char param_1, bool param_2) {
			reinterpret_cast<void(__thiscall*)(GJMGLayer*, unsigned char, bool)>(base + 0x1f86b0)(this, param_1, param_2);
		}

	};

    class FMODAudioEngine : public cocos2d::CCNode {
    public:
        static FMODAudioEngine* sharedEngine() {
            return reinterpret_cast<FMODAudioEngine * (__stdcall*)()>(base + 0x32bf0)();
        }

        auto& m_system() {
            return from<FMOD::System*>(this, 0x190);
        }
    };
	
}