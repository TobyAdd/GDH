#include "includes.hpp"

extern float left_over;

namespace hooks {
    inline bool(__thiscall* PlayLayer_init)(void*, void*, bool, bool);
    bool __fastcall PlayLayer_initH(void *self, void* m_level, bool a3, bool a4);

    inline void(__thiscall* PlayLayer_addObject)(void*, void*);
    void __fastcall PlayLayer_addObjectH(void* self, void* obj);

    inline void(__thiscall* PlayLayer_destructor)(void*);
    void __fastcall PlayLayer_destructor_H(void* self);

    inline int (__thiscall* GJBaseGameLayer_HandleButton)(void*, int hold, int player_button, bool is_player1);
    int __fastcall GJBaseGameLayer_HandleButton_H(void *self, int hold, int player_button, bool is_player1);

    void init();
    extern std::string log;
}