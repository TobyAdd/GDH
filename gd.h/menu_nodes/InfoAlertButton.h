#ifndef __INFOALERTBUTTON_H__
#define __INFOALERTBUTTON_H__

#include <gd.h>

namespace gd {
    class GDH_DLL InfoAlertButton : public CCMenuItemSpriteExtra {
        public:
            bool init(std::string const& title, std::string const& text, float scale) {
                __asm movss xmm1, scale

                return reinterpret_cast<InfoAlertButton*(__thiscall*)(
                    InfoAlertButton*, std::string, std::string
                )>(
                    base + 0x14ef50
                )(
                    this, title, text
                );
            }

            virtual void activate() override {
                reinterpret_cast<InfoAlertButton*(__thiscall*)(InfoAlertButton*)>(
                    base + 0x14f050
                )(this);
            }

            InfoAlertButton() {
                reinterpret_cast<InfoAlertButton*(__thiscall*)(InfoAlertButton*)>(
                    base + 0x14ef50
                )(this);
            }

            // can't get the gd function to work,
            // so just gonna recreate it instead
            static InfoAlertButton* create(std::string const& title, std::string const& text, float scale) {
                __asm movss xmm0, scale

                auto ret = reinterpret_cast<InfoAlertButton*(__cdecl*)(std::string const&, std::string const&)>(
                    base + 0x14ed20
                )(title, text);

                __asm add esp, 0x30

                return ret;
            }
    };
}

#endif
