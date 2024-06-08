#pragma once
#include "includes.hpp"

template <class R, class T>
inline R& from(T base, intptr_t offset) {
    return *reinterpret_cast<R*>(reinterpret_cast<uintptr_t>(base) + offset);
}

class CCPoint {
public:
    float x;
    float y;
};

namespace gd {
    namespace gjbaselayer {
        inline auto &get_player1(void* gjbl) {
            return from<void*>(gjbl, 3480);
        }

        inline auto &get_player2(void* gjbl) {
            return from<void*>(gjbl, 3488);
        }

        inline auto &get_position(void* player) {
            return from<CCPoint>(player, 2704);
        }

        inline auto &get_yaccel(void* player) {
            return from<double>(player, 2464);
        }

        inline auto &get_time(void* gjbl) {
            return from<double>(gjbl, 968);
        }

        inline auto &get_isDualMode(void* gjbl) {
            return from<void*>(gjbl, 1050);
        }
    }

    namespace gameobject {
        inline auto &get_objid(void* obj) {
            return from<int>(obj, 1036);
        }
    }
}
