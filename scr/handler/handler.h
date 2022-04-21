#ifndef HANDLER_H
#define HANDLER_H

#include <windows.h>
#include <psapi.h>
#include <cstdint>

#include <vector>
#include <string>
#include <map>


class Handler
{
public:
    Handler();
    ~Handler() {}

    bool AttemptAttach(const char *window, const char *process);
    bool Detatch();

    //generic functions



    template<class T>
    bool Write(uint32_t vaddress, const T& value) const { return WriteProcessMemory(this->hProcess, reinterpret_cast<void*>(vaddress), &value, sizeof(T), NULL); }
    bool Write(uint32_t vaddress, const void *bytes, size_t size) const;
    bool WriteJump(uint32_t vaddress, uint32_t to) const { return this->Write(vaddress, '\xE9') && this->Write(vaddress + 1, to - vaddress - 5); }
    bool SetWindowSize(int width, int height);
    std::pair<int, int> GetWindowSize();
    template<class T>
    T Read(uint32_t vaddress) const { T buf; return ReadProcessMemory(this->hProcess, reinterpret_cast<void*>(vaddress), &buf, sizeof(T), NULL) ? buf : T(); }
    std::string Read(uint32_t vaddress, size_t len) const;

    uint32_t GetPointerAddress(std::vector<uint32_t> offsets, const char *lib);

    uint32_t Protect(uint32_t vaddress, size_t size, uint32_t new_protect) const;
    uint32_t Allocate(size_t size, uint32_t vaddress = 0) const;

    bool Free(uint32_t vaddress, size_t size = 0) const;

    bool NewThread(uint32_t vaddress, void *param = nullptr) const;

    bool Inject(const char *dll_path) const;

    //getting info

    uint32_t GetModuleBase(const char* module) const;

    uint32_t GetBase() const { return this->base; }

    char *FilePath() const;
    char *FilePath(const char *module_name) const;

    std::vector<std::string> GetModules() const;

    HANDLE GetHandle() const { return this->hProcess; }
    DWORD GetPID() const { return this->procId; }
    HWND GetWindow() const { return this->hWnd; }

    //our memory page

    bool MemAlloc(size_t size);

    bool MemNew(std::string key, size_t size);

    template<class T>
    uint32_t MemWriteKey(std::string key, T value) const { return this->Write(this->MemGetKeyAddress(key), value) ? this->MemGetKeyAddress(key) : 0; }

    template<class T>
    T MemReadKey(std::string key) const { return this->alloc_map.find(key) != this->alloc_map.end() ? this->Read<T>(this->alloc_base + this->alloc_map.at(key).first) : T(); }

    uint32_t MemGetKeyAddress(std::string key) const { return alloc_base + alloc_map.at(key).first; }

private:
    HWND hWnd;
    DWORD procId;
    HANDLE hProcess;
    RECT rect;

    uint32_t base, alloc_base, alloc_offset;
    size_t alloc_size;
    std::map<std::string /*key*/, std::pair<uint32_t /*offset*/, size_t /*size*/>> alloc_map;
};

#endif // HANDLER_H
