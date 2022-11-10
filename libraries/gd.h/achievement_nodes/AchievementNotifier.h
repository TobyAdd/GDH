#ifndef __ACHIEVEMENTNOTIFIER_H__
#define __ACHIEVEMENTNOTIFIER_H__

#include <gd.h>

namespace gd {

	class AchievementBar;

	class AchievementNotifier : public cocos2d::CCNode {
	protected:
		cocos2d::CCScene* m_pCurrentScene;
		cocos2d::CCArray* m_pQueue;
		AchievementBar* m_pCurrentAchievement;

	public:
		static AchievementNotifier* sharedState() {
			return reinterpret_cast<AchievementNotifier* (__stdcall*)()>(
				base + 0xFC90
				)();
		}
		void showNextAchievement() {
			return reinterpret_cast<void(__thiscall*)(AchievementNotifier*)>(
				base + 0xFD60
				)(this);
		}
		//this is inlined on win32 so let's reconstruct it
		void notifyAchievement(const char* title, const char* desc, const char* icon, bool quest) {
			m_pQueue->addObject(AchievementBar::create(title, desc, icon, quest));
			if (!m_pCurrentAchievement) {
				this->showNextAchievement();
			}
		}
	};
}

#endif