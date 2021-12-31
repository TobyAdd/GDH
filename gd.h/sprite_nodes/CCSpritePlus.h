#ifndef __CCSPRITEPLUS_H__
#define __CCSPRITEPLUS_H__

#include <gd.h>

namespace gd {
	//credit to zmx (https://github.com/kyurime)
	class GDH_DLL CCSpritePlus : public cocos2d::CCSprite {
	protected:
		cocos2d::CCArray* m_pFollowers;
		CCSpritePlus* m_pFollowing;
		bool m_bHasFollowers;
		bool m_bScaleFollowers;
		bool m_bFlipFollowers;

	public:
		//CCNode vtable
		virtual void setScaleX(float scale) {
			return reinterpret_cast<void(__thiscall*)(CCSpritePlus*, float)>(
				base + 0x1C440
				)(this, scale);
		}
		virtual void setScaleY(float scale) {
			return reinterpret_cast<void(__thiscall*)(CCSpritePlus*, float)>(
				base + 0x1C4C0
				)(this, scale);
		}
		virtual void setScale(float scale) {
			return reinterpret_cast<void(__thiscall*)(CCSpritePlus*, float)>(
				base + 0x1C540
				)(this, scale);
		}
		virtual void setPosition(const cocos2d::CCPoint& pos) {
			return reinterpret_cast<void(__thiscall*)(CCSpritePlus*, const cocos2d::CCPoint&)>(
				base + 0x1C220
				)(this, pos);
		}
		virtual void setRotation(float rotation) {
			return reinterpret_cast<void(__thiscall*)(CCSpritePlus*, float)>(
				base + 0x1C280
				)(this, rotation);
		}
		virtual bool initWithTexture(cocos2d::CCTexture2D* texture) {
			return reinterpret_cast<bool(__thiscall*)(CCSpritePlus*, cocos2d::CCTexture2D*)>(
				base + 0x1C200
				)(this, texture);
		}
		virtual bool initWithSpriteFrameName(const char* name) {
			return reinterpret_cast<bool(__thiscall*)(CCSpritePlus*, const char*)>(
				base + 0x1C1E0
				)(this, name);
		}

		//own vtable
		virtual void setFlipX(bool flip) {
			return reinterpret_cast<void(__thiscall*)(CCSpritePlus*, bool)>(
				base + 0x1C300
				)(this, flip);
		}
		virtual void setFlipY(bool flip) {
			return reinterpret_cast<void(__thiscall*)(CCSpritePlus*, bool)>(
				base + 0x1C3A0
				)(this, flip);
		}
		CCSpritePlus* getFollowingSprite() { return m_pFollowing; }

		// note: this is not an actual gd function.
		// this should really not be used. it's just
		// because camden314 has it.
		void setFollowingSprite(CCSpritePlus* spr) { this->m_pFollowing = spr; }

		static CCSpritePlus* createWithSpriteFrame(cocos2d::CCSpriteFrame* frame) {
			return reinterpret_cast<CCSpritePlus* (__fastcall*)(cocos2d::CCSpriteFrame*)>(
				base + 0x1C130
				)(frame);
		}
	};
}

#endif