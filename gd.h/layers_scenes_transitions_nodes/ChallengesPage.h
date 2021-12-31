#ifndef __CHALLENGESPAGE_H__
#define __CHALLENGESPAGE_H__

#include <gd.h>

namespace gd {
    class ChallengesPage : public FLAlertLayer {
        public:
            static ChallengesPage* create() {
                return reinterpret_cast<ChallengesPage*(__fastcall*)()>(
                    base + 0x3e050
                )();
            }
    };
}

#endif
