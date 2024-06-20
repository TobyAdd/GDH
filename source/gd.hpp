#pragma once
#include "includes.hpp"
#include <cocos2d.h>
#include "console.hpp"

template <class R, class T>
inline R& from(T base, intptr_t offset) {
    return *reinterpret_cast<R*>(reinterpret_cast<uintptr_t>(base) + offset);
}

namespace cocos2d {
    static auto base = GetModuleHandleA("libcocos2d.dll");
}

namespace gd {
    using namespace cocos2d;

    static auto base = reinterpret_cast<uintptr_t>(GetModuleHandleA(0));
    static auto cocos_base = reinterpret_cast<uintptr_t>(GetModuleHandleA("libcocos2d.dll"));

    
	class GameObject : public CCSprite {
    public:
		int m_nObjectID() {
            return from<int>(this, 1036);
        }
    };

    class StartPosObject : public GameObject {
    public:

    };

    class PlayerObject : public GameObject
	{
	public:
		auto& m_position() {
			return from<cocos2d::CCPoint>(this, 2704);
		}

		auto& m_yAccel() {
			return from<double>(this, 2464);
		}

		auto m_waveTrail() {
			return from<cocos2d::CCSprite*>(this, 0x7b0); 
		}

        cocos2d::CCSprite* m_pSecondarySprite() {
            return from<cocos2d::CCSprite*>(this, 1856);
        }
	};

    
	class PlayLayer : public CCLayer {
	public:
		PlayerObject*& m_pPlayer1() {
			return from<PlayerObject*>(this, 3480);
		}

		PlayerObject*& m_pPlayer2() {
			return from<PlayerObject*>(this, 3488);
		}

		double m_time() {
            return from<double>(this, 968);
		}

        bool m_bTwoPlayerMode() {
			return from<bool>(this, 1050);
		}

        bool m_isPracticeMode() {
			return from<bool>(this, 12776);
		}

        void resetLevel() {
            reinterpret_cast<void(__thiscall*)(void*)>(base + 0x395710)(this);
        }

        void resetCamera() {
            reinterpret_cast<void(__thiscall*)(void*)>(base + 0x22ec30)(this);
        }

        void startMusic() {
            reinterpret_cast<void(__thiscall*)(void*)>(base + 0x3973f0)(this);
        }

        auto &m_startPosObject() {
            return from<StartPosObject*>(this, 12672);
        }

        void setStartPosObject(StartPosObject* startPos) {
            if (startPos != m_startPosObject()) {
                if (startPos) {
                    startPos->retain();
                }
                if (m_startPosObject()) {
                    m_startPosObject()->release();
                }
                m_startPosObject() = startPos;
            }
        }

        auto &m_currentCheckpoint() {
            return from<CCNode*>(this, 14232);
        }

        auto &m_unknow() {
            return from<int>(this, 3798);
        }
	};

    namespace FMOD {
        static auto base = GetModuleHandleA("fmod.dll");
        
        struct Sound {
            void getDefaults(float *frequency, int *priority) {
                static const auto addr = GetProcAddress(base, "?getDefaults@Sound@FMOD@@QEAA?AW4FMOD_RESULT@@PEAMPEAH@Z");
                reinterpret_cast<void*(__stdcall*)(void*, float *, int *)>(addr)(this, frequency, priority);
            }
        };

        struct Channel {
            void setPitch(float pitch) {
                static const auto addr = GetProcAddress(base, "?setPitch@ChannelControl@FMOD@@QEAA?AW4FMOD_RESULT@@M@Z");
                reinterpret_cast<void*(__stdcall*)(void*, float)>(addr)(this, pitch);
            }

            void setVolume(float volume) {
                static const auto addr = GetProcAddress(base, "?setVolume@ChannelControl@FMOD@@QEAA?AW4FMOD_RESULT@@M@Z");
                reinterpret_cast<void*(__stdcall*)(void*, float)>(addr)(this, volume);
            }

            void setFrequency(float frequency) {
                static const auto addr = GetProcAddress(base, "?setFrequency@Channel@FMOD@@QEAA?AW4FMOD_RESULT@@M@Z");
                reinterpret_cast<void*(__stdcall*)(void*, float)>(addr)(this, frequency);
            }

            void getCurrentSound(Sound **sound) {
                static const auto addr = GetProcAddress(base, "?getCurrentSound@Channel@FMOD@@QEAA?AW4FMOD_RESULT@@PEAPEAVSound@2@@Z");
                reinterpret_cast<void*(__stdcall*)(void*, Sound **sound)>(addr)(this, sound);
            }
        };
        
        struct System {
            void getChannel(int index, Channel **channel) {
                static const auto addr = GetProcAddress(base, "?getChannel@System@FMOD@@QEAA?AW4FMOD_RESULT@@HPEAPEAVChannel@2@@Z");
                reinterpret_cast<void*(__stdcall*)(void*, int, Channel **)>(addr)(this, index, channel);
            }

            void setOutput(int output) {
                static const auto addr = GetProcAddress(base, "?setOutput@System@FMOD@@QEAA?AW4FMOD_RESULT@@W4FMOD_OUTPUTTYPE@@@Z");
                reinterpret_cast<void*(__stdcall*)(void*, int)>(addr)(this, output);
            }
        };
    }

    class FMODAudioEngine : public cocos2d::CCNode {
    public:
        static FMODAudioEngine* sharedEngine() {
            auto** instancePtr = reinterpret_cast<FMODAudioEngine**>(base + 0x687dc0);
            if (!*instancePtr) {
                *instancePtr = new FMODAudioEngine();
                (*instancePtr)->init();
            }
            return *instancePtr;
        }

        auto& m_system() {
            return from<FMOD::System*>(this, 0x230);
        }
    };
}
