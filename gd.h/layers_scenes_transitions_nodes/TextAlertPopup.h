#ifndef __TEXTALERTPOPUP_H__
#define __TEXTALERTPOPUP_H__

#include <gd.h>

#define __cdecl
namespace gd {
    class GDH_DLL TextAlertPopup : public cocos2d::CCNode {
        public:
            static TextAlertPopup* create(const char* _text, float _time, float _scale) {
                __asm {
                    movss xmm0, _time
                    movss xmm1, _scale
                }

                using skip_t = char;

                // ok this has got to be some of the
                // wackiest bugfixing i've ever done

                // those of you with a keen eye may have
                // noticed and started to wonder:
                // "wait, isn't that just std::string with SSO?"

                // and the answer to that one is yes!

                // now, the follow-up question might be:
                // "why aren't you using std::string then?"

                // and the answer to that one is that
                // when i use std::string, it works except
                // xmm0 becomes a number so large it's only
                // overshadowed by your mom

                // however, if we manually set the length and
                // capacity of the string, it works for long
                // strings just fine

                // so that's this here

                if (strlen(_text) > 15)
                    return reinterpret_cast<TextAlertPopup*(__cdecl*)(
                        char*, skip_t[0xa], int, int
                    )>( base + 0x1450b0 )(
                        const_cast<char*>(_text), 0, strlen(_text), strlen(_text)
                    );

                // now, i know what you're thinking:
                // "what in the fuck is this unused
                // buffer variable that you copy the
                // text contents into?"

                // and the answer to that one is:
                // idk, but if i remove it, the alert
                // doesn't display for the proper amount
                // of time (xmm0 bullfuckey again it seems)
                
                char buf[16];
                strcpy_s(buf, _text);

                return reinterpret_cast<TextAlertPopup*(__cdecl*)(
                    char[16], int, int
                )>( base + 0x1450b0 )(buf, strlen(_text), 15);
            }
    };
    
}

#endif
