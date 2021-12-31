#ifndef __ANIMATEDSHOPKEEPER_H__
#define __ANIMATEDSHOPKEEPER_H__

#include <gd.h>

namespace gd {

	class CCAnimatedSprite;

	typedef enum {
		kShopTypeNormal,
		kShopTypeSecret,
		kShopTypeCommunity
	} ShopType;

	class GDH_DLL AnimatedShopKeeper : public CCAnimatedSprite {
	protected:
		float m_fUnknown1;
		float m_fUnknown2;
		bool m_bUnknown;

	public:
		static AnimatedShopKeeper* create(ShopType type) {
			return reinterpret_cast<AnimatedShopKeeper* (__fastcall*)(ShopType)>(
				base + 0x14C4D0
				)(type);
		}
		void startAnimating() {
			return reinterpret_cast<void(__thiscall*)(AnimatedShopKeeper*)>(
				base + 0x14C690
				)(this);
		}

		//own vtable
		virtual void animationFinished(const char*) {}
	};
}

#endif