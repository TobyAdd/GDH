#ifndef __SIMPLEPLAYER_H__
#define __SIMPLEPLAYER_H__

#include <gd.h>

namespace gd {

	enum IconType;

	class GJRobotSprite;
	class GJSpiderSprite;

	class GDH_DLL SimplePlayer : public cocos2d::CCSprite {
	protected:
		cocos2d::CCSprite* m_pFirstLayer; // idk a good name for these, theyre not even layers
		cocos2d::CCSprite* m_pSecondLayer;
		cocos2d::CCSprite* m_pBirdDome; // the glass thingy on the ufo
		cocos2d::CCSprite* m_pOutlineSprite;
		cocos2d::CCSprite* m_pDetailSprite;
		GJRobotSprite* m_pRobotSprite;
		GJSpiderSprite* m_pSpiderSprite;
		PAD(4); // seems to be unused
		bool m_bHasGlowOutline;

		virtual bool init(int iconID) {
			return reinterpret_cast<bool (__thiscall*)(SimplePlayer*, int)>(
				base + 0x12be20)(this, iconID);
		}
	public:
		static auto create(int iconID) {
			return reinterpret_cast<SimplePlayer* (__fastcall*)(int)>(
				base + 0x12bd80)(iconID);
		}

		void updatePlayerFrame(int iconID, IconType iconType) {
			reinterpret_cast<void (__thiscall*)(SimplePlayer*, int, IconType)>(
				base + 0x12c650)(this, iconID, iconType);
		}

		void updateColors() {
			reinterpret_cast<void (__thiscall*)(SimplePlayer*)>(
				base + 0x12c440)(this);
		}

		void setFrames(const char* firstLayer, const char* secondLayer, const char* birdDome, const char* outlineSprite, const char* detailSprite) {
			reinterpret_cast<void (__thiscall*)(SimplePlayer*, const char*, const char*, const char*, const char*, const char*)>(
				base + 0x12c9e0)
				(this, firstLayer, secondLayer, birdDome, outlineSprite, detailSprite);
		}

		virtual void setColor(const cocos2d::ccColor3B& color) {
			reinterpret_cast<void (__thiscall*)(SimplePlayer*, const cocos2d::ccColor3B&)>(
				base + 0x12c410)
				(reinterpret_cast<SimplePlayer*>(reinterpret_cast<char*>(this) + 0xec), color);
		}

		void setSecondColor(const cocos2d::ccColor3B& color) {
			// this function is inlined on windows
			m_pSecondLayer->setColor(color);
			updateColors();
		}

		virtual void setOpacity(unsigned char opacity) {
			reinterpret_cast<void (__thiscall*)(SimplePlayer*, unsigned char)>(
				base + 0x12cb90)
				(reinterpret_cast<SimplePlayer*>(reinterpret_cast<char*>(this) + 0xec), opacity);
		}

		// custom functions

		bool hasGlowOutline() { return m_bHasGlowOutline; }
		void setGlowOutline(bool value) {
			m_bHasGlowOutline = value;
			updateColors();
		}
	};
}

#endif
