#ifndef __OBJECTTOOLBOX_H__
#define __OBJECTTOOLBOX_H__

#include <gd.h>

namespace gd {
    class GDH_DLL ObjectToolbox : public cocos2d::CCNode {
    public:
        cocos2d::CCDictionary* m_frameToKey;
        cocos2d::CCDictionary* m_keyToFrame;
        PAD(4);

        static auto sharedState() {
            return reinterpret_cast<ObjectToolbox*(__stdcall*)()>(base + 0x198A80)();
        }

        // do these functions exist on windows? idk
        // theyre small enough where its easier to recreate them
        // than to try to find their addresses

        cocos2d::CCArray* allKeys() {
            return m_frameToKey->allKeys();
        }

        const char* frameToKey(const char* frame) {
            return reinterpret_cast<cocos2d::CCString*>(m_frameToKey->objectForKey(frame))->getCString();
        }

        const char* intKeyToFrame(int key) {
            return reinterpret_cast<cocos2d::CCString*>(m_keyToFrame->objectForKey(key))->getCString();
        }

        const char* keyToFrame(const char* key) {
            return intKeyToFrame(atoi(key));
        }

        static float gridNodeSizeForKey(int key) {
            float fRet;
            reinterpret_cast<void(__stdcall*)(int)>(base + 0x1dc920)(key);
            __asm movss fRet, xmm0
            return fRet;
        }

        static const char* perspectiveBlockFrame(int key) {
            return reinterpret_cast<const char*(__fastcall*)(int)>(base + 0x1dcd00)(key);
        }

        // custom funcs

        void addObject(int id, const char* frame) {
            m_frameToKey->setObject(cocos2d::CCString::createWithFormat("%i", id), frame);
            m_keyToFrame->setObject(cocos2d::CCString::create(frame), id);
        }
    };
}

#endif
