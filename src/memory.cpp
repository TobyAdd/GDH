#include "memory.h"

namespace Memory {
    bool Memory::MemoryWriteBytes(uintptr_t const address, std::vector<uint8_t> const& bytes) {
	    return WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<LPVOID>(address), bytes.data(), bytes.size(), nullptr);
    }

}