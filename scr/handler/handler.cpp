#include "handler.h"
#include "../bools.cpp"
#include <iostream>
#include <regex>


Handler::Handler()
{
    this->procId = 0;
    this->hProcess = NULL;
}

bool Handler::AttemptAttach(const char *window, const char *process)
{
    if ((this->hWnd = FindWindowA(0, window)))
    {
        if (this->hProcess)
            this->Free(this->alloc_base, this->alloc_size);
        this->procId = 0;
        GetWindowThreadProcessId(this->hWnd, &this->procId);
        if ((this->hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION, FALSE, procId)))
        {
            this->base = this->GetModuleBase(process);
            this->MemAlloc(0x1000);
            return true;
        }
    }
    return false;
}

bool Handler::SetWindowSize(int width, int height){
    if (this->hWnd != NULL) { SetWindowPos(this->hWnd, 0, 0, 0, width, height, SWP_SHOWWINDOW | SWP_NOMOVE); }
    return 0;
}

std::pair<int, int> Handler::GetWindowSize()
{
    GetWindowRect(this->hWnd, &rect);
    return std::make_pair(rect.right - rect.left, rect.bottom - rect.top);
}

bool Handler::Detatch()
{
    return CloseHandle(this->hProcess);
}

uint32_t Handler::GetModuleBase(const char *module) const
{
    static const int size = 0x1000;
    DWORD out;
    HMODULE hmods[size];
    if (EnumProcessModulesEx(this->hProcess, hmods, 0x1000, &out, LIST_MODULES_ALL))
    {
        for (uint32_t i = 0; i < out / 4; ++i)
        {
            char path[MAX_PATH];
            if (GetModuleBaseNameA(this->hProcess, hmods[i], path, MAX_PATH))
            {
                if (!strcmp(path, module))
                    return reinterpret_cast<uint32_t>(hmods[i]);
            }
        }
    }
    return 0;
}

bool Handler::Write(uint32_t vaddress, const void *bytes, size_t size) const
{
    return WriteProcessMemory(this->hProcess, reinterpret_cast<void*>(vaddress), bytes, size, NULL);
}

std::string Handler::Read(uint32_t vaddress, size_t len) const
{
    std::string str;
    str.reserve(len);
    return ReadProcessMemory(this->hProcess, reinterpret_cast<void*>(vaddress), &str[0], len, NULL) ? str : std::string();
}

uint32_t Handler::GetPointerAddress(std::vector<uint32_t> offsets, const char *lib)
{
    if (offsets.size() > 1)
    {
        uint32_t buf = this->Read<uint32_t>(offsets[0] + this->GetModuleBase(lib));

        for (size_t i = 1; i < offsets.size() - 1 /*ignore last offset*/; ++i)
            buf = this->Read<uint32_t>(buf + offsets[i]);
        return buf + offsets.back();
    }
    return offsets.size() ? offsets[0] + this->GetModuleBase(lib) : 0;
}

uint32_t Handler::Protect(uint32_t vaddress, size_t size, uint32_t new_protect) const
{
    DWORD old;
    return VirtualProtectEx(this->hProcess, reinterpret_cast<void*>(vaddress), size, new_protect, &old) ? static_cast<uint32_t>(old) : 0;
}

uint32_t Handler::Allocate(size_t size, uint32_t vaddress) const
{
    return reinterpret_cast<uint32_t>(VirtualAllocEx(this->hProcess, reinterpret_cast<void*>(vaddress), size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE));
}

bool Handler::Free(uint32_t vaddress, size_t size) const
{
    return VirtualFreeEx(this->hProcess, reinterpret_cast<void*>(vaddress), size, MEM_RELEASE);
}

bool Handler::NewThread(uint32_t vaddress, void *param) const
{
    return CreateRemoteThread(this->hProcess, 0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(vaddress), param, 0, 0);
}

bool Handler::Inject(const char *dll_path) const
{
    uint32_t addr = this->Allocate(strlen(dll_path) + 1);
    if (addr && this->Write(addr, dll_path, strlen(dll_path)))
        return this->NewThread(reinterpret_cast<uint32_t>(LoadLibraryA), reinterpret_cast<void*>(addr));
    return false;
}

char *Handler::FilePath() const
{
    char *buf = new char[MAX_PATH]();
    GetModuleFileNameExA(this->hProcess, NULL, buf, MAX_PATH);
    return buf;
}

char *Handler::FilePath(const char *module_name) const
{
    char *buf = new char[MAX_PATH]();
    GetModuleFileNameExA(this->hProcess, reinterpret_cast<HMODULE>(this->GetModuleBase(module_name)), buf, MAX_PATH);
    return buf;
}

std::vector<std::string> Handler::GetModules() const
{
    static const int size = 0x1000;
    DWORD out;
    HMODULE hmods[size];
    if (EnumProcessModulesEx(this->hProcess, hmods, 0x1000, &out, LIST_MODULES_ALL))
    {
        std::vector<std::string> v;
        for (uint32_t i = 0; i < out / 4; ++i)
        {
            char path[MAX_PATH];
            if (GetModuleBaseNameA(this->hProcess, hmods[i], path, MAX_PATH))
            {
                v.push_back(path);
            }
        }
        return v;
    }
    return std::vector<std::string>();
}




bool Handler::MemAlloc(size_t size)
{
    this->alloc_offset = 0;
    this->alloc_size = size;
    this->alloc_map.clear();
    return (this->alloc_base = this->Allocate(size)) != 0;
}

bool Handler::MemNew(std::string key, size_t size)
{
    if (this->alloc_map.find(key) == this->alloc_map.end() && this->alloc_size >= this->alloc_offset + size)
    {
        this->alloc_map[key] = std::pair<uint32_t, size_t>(this->alloc_offset, size);
        this->alloc_offset += size;
        return true;
    }
    return false;
}
