#ifndef __CCNODECONTAINER_H__
#define __CCNODECONTAINER_H__

#include <gd.h>

namespace gd {
    class GDH_DLL CCNodeContainer : public cocos2d::CCNode {
        protected:
            // literally no extra fields or anything, just
            // these 3 methods

            bool init() override {
                return reinterpret_cast<bool(__fastcall*)(CCNodeContainer*)>(
                    base + 0x33b40
                )(this);
            }

        public:
            void visit() override {
                reinterpret_cast<void(__fastcall*)(CCNodeContainer*)>(
                    base + 0x112420
                )(this);
            }

            static CCNodeContainer* create() {
                return reinterpret_cast<CCNodeContainer*(__stdcall*)(void)>(
                    base + 0x112370
                )();
            }
    };
}


#endif
