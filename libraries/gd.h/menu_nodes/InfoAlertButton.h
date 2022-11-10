#ifndef __INFOALERTBUTTON_H__
#define __INFOALERTBUTTON_H__

#include <gd.h>

namespace gd {
    class InfoAlertButton : public CCMenuItemSpriteExtra {
        public:
            static InfoAlertButton* create(const char* title, const char* text, float scale) {
                __asm movss xmm0, scale
                
                using skip_t = char;

                if (strlen(title) > 15) {
                    auto ret = reinterpret_cast<InfoAlertButton*(__cdecl*)(
                        char*, skip_t[0xa], int, int, const char*, int, int
                    )>( base + 0x1450b0 )(
                        const_cast<char*>(title), 0, strlen(title), strlen(title), text, strlen(text), strlen(text)
                    );

                    __asm add esp, 0x30

                    return ret;
                }

                char buf[16];
                strcpy_s(buf, title);

                auto ret = reinterpret_cast<InfoAlertButton*(__cdecl*)(char[16], int, int, const char*, int, int)>(
                    base + 0x14ed20
                )(buf, strlen(title), 15, text, strlen(text), strlen(text));

                __asm add esp, 0x30

                return ret;
            }
    };
}

#endif
