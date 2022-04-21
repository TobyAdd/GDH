#ifndef __GJACCOUNTMANAGER_H__
#define __GJACCOUNTMANAGER_H__

#include <gd.h>

namespace gd {
    class GDH_DLL GJAccountManager : public cocos2d::CCNode {
        public:
            cocos2d::CCDictionary* m_pDLObject; // 0xec
            std::string m_sPassword;    // 0xf0
            std::string m_sUsername;    // 0x108
            int m_nPlayerAccountID;     // 0x120
            int m_nPlayerAccountIDSeed; // 0x124
            int m_nPlayerAccountIDRand; // 0x128
            GJAccountRegisterDelegate* m_pRegisterAccountDelegate;  // 0x12c
            GJAccountLoginDelegate* m_pLoginAccountDelegate;        // 0x130
            GJAccountDelegate* m_pAccountDelegate;                  // 0x134
            GJAccountBackupDelegate* m_pBackupAccountDelegate;      // 0x138
            GJAccountSyncDelegate* m_pSyncAccountDelegate;          // 0x13c
            GJAccountSettingsDelegate* m_pAccountSettingsDelegate;  // 0x140

        public:
            static GJAccountManager* sharedState() {
                return reinterpret_cast<GJAccountManager*(__stdcall*)()>(
                    base + 0x107d50
                )();
            }

            const char* getPassword() {
                return this->m_sPassword.c_str();
            }
            const char* getUsername() {
                return this->m_sUsername.c_str();
            }
    };
}

#endif
