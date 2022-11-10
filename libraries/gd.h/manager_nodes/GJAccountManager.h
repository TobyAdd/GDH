#ifndef __GJACCOUNTMANAGER_H__
#define __GJACCOUNTMANAGER_H__

#include <gd.h>

namespace gd {
    class GJAccountManager : public cocos2d::CCNode {
        protected:
            PAD(0x4)
            std::string m_sPassword;
            std::string m_sUsername;

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
