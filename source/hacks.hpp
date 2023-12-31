#include "include.hpp"
#include <fstream>
#include <streambuf>
#include <vector>
#include <sstream>
#include <string>

namespace hacks {
    extern json content;
    bool writemem(uintptr_t address, string bytes);
    bool writemem2(uintptr_t address, DWORD destination_address);    
    bool push_write(const uintptr_t address, const DWORD destination_address);
    
    void load();
    void inject_extensions();
}