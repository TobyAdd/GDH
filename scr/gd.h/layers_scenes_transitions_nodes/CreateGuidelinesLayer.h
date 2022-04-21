#ifndef __CREATEGUIDELINESLAYER_H__
#define __CREATEGUIDELINESLAYER_H__

#include <gd.h>

namespace gd {
    class GDH_DLL CreateGuidelinesLayer : public FLAlertLayer, FLAlertLayerProtocol {
        public:
            PAD(36)
            std::string m_sGuidelineString;

            void onStop(CCObject* pSender) {
                reinterpret_cast<void(__thiscall*)(
                    CreateGuidelinesLayer*, CCObject*
                )>(
                    base + 0x4d2c0
                )(this, pSender);
            }
    };
}

#endif
