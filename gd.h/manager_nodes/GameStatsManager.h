#include <gd.h>
namespace gd {
	class GDH_DLL GameStatsManager : cocos2d::CCNode
	{
	public:
		PAD(40);
		cocos2d::CCDictionary* m_pDailyChests;
		cocos2d::CCDictionary* m_pWorldAdvertChests;
		cocos2d::CCDictionary* m_pActiveChallenges;
		cocos2d::CCDictionary* m_pUpcomingChallenges;
		PAD(12);
		cocos2d::CCDictionary* m_pPlayerStats;
		PAD(16);
		cocos2d::CCDictionary* m_pCompletedLevels;
		cocos2d::CCDictionary* m_pVerifiedUserCoins;
		cocos2d::CCDictionary* m_pPendingUserCoins;
		cocos2d::CCDictionary* m_pPurchasedItems;
		cocos2d::CCDictionary* m_pOnlineCurrencyScores;
		cocos2d::CCDictionary* m_pMainCurrencyScores;
		cocos2d::CCDictionary* m_pGauntletCurrencyScores;
		cocos2d::CCDictionary* m_pTimelyCurrencyScores;
		cocos2d::CCDictionary* m_pOnlineStars;
		cocos2d::CCDictionary* m_pTimelyStars;
		cocos2d::CCDictionary* m_pGauntletDiamondScores;
		cocos2d::CCDictionary* m_pTimelyDiamondScores;
		cocos2d::CCDictionary* m_pUnusedCurrencyAwardDict;
		cocos2d::CCDictionary* m_pChallengeDiamonds;
		cocos2d::CCDictionary* m_pCompletedMappacks;
		cocos2d::CCDictionary* m_pWeeklyChest;
		cocos2d::CCDictionary* m_pTreasureRoomChests;
		int m_nBonusKeySeed;
		int m_nBonusKeyRand;
		int m_nBonusKey;
		cocos2d::CCDictionary* m_pMiscChests;

		static GameStatsManager* sharedState() {
			return reinterpret_cast<GameStatsManager* (__stdcall*)()>(
				base + 0xf1e50
				)();
		}
	};
}
