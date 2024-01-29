#pragma once
#include "includes.hpp"

template <class R, class T>
R& from(T base, intptr_t offset) {
	return *reinterpret_cast<R*>(reinterpret_cast<uintptr_t>(base) + offset);
}

namespace gd {

	using namespace cocos2d;

	static const auto base = reinterpret_cast<uintptr_t>(GetModuleHandleA(0));
	static const auto cocos_base = reinterpret_cast<uintptr_t>(GetModuleHandleA("libcocos2d.dll"));

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

	class GameObject : public CCSprite
	{
	public:
		auto m_glow() {
			return from<CCSprite*>(this, 0x2B00);
		}

		auto m_objectType() {
			return from<int>(this, 0x31c);
		}
	};

	class PlayerObject : public GameObject
	{
	public:
		auto m_position() {
			return from<CCPoint>(this, 0x64);
		}

		auto m_yAccel() {
			return from<double>(this, 0x790);
		}

		auto m_xAccel() {
			return from<double>(this, 0x630); 
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

		PlayLayer* getPlayLayer() {
			return from<PlayLayer*>(this, 0x198);
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

		PlayerObject* m_pPlayer1() {
			return from<PlayerObject*>(this, 0x878);
		}

		PlayerObject* m_pPlayer2() {
			return from<PlayerObject*>(this, 0x87C);
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
	
}

namespace FMOD {
	static auto base = GetModuleHandleA("fmod.dll");
	struct Channel {
		void setPitch(float pitch) {
			static const auto addr = GetProcAddress(base, "?setPitch@ChannelControl@FMOD@@QAG?AW4FMOD_RESULT@@M@Z");
			reinterpret_cast<void*(__stdcall*)(void*, float)>(addr)(this, pitch);
		}
	};
}

