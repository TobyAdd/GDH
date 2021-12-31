#ifndef __SETUPPICKUPTRIGGERPOPUP_H__
#define __SETUPPICKUPTRIGGERPOPUP_H__

#include <gd.h>

namespace gd {
    class GDH_DLL SetupPickupTriggerPopup : public FLAlertLayer, public TextInputDelegate {
        public:
            PAD(0xc)
            CCTextInputNode* m_pCountInput; // 0x1dc
            // too lazy to RE more, it should be
            // really easy to find the rest tho
    };
}

#endif
