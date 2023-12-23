#include "startposSwitcher.hpp"

namespace startposSwitcher 
{
    void(__thiscall *resetLevel)(void *) = nullptr;
    void(__thiscall *setStartPosObject)(void *, void *) = nullptr;
    void(__thiscall *playLayerInit)(void *) = nullptr;
    void(__thiscall *startposObjectInit)(void *) = nullptr;
    void(__thiscall *playLayerDestructor)(void *) = nullptr;

    void *playLayer = nullptr;
    std::vector<float *> startposObjects;
    int startposIndex = -1;
    float tmp[] = { 0, 0 };
    bool enabled = false;

    void __fastcall startposObjectInitHook(void *self, void *_) 
    {
        startposObjects.push_back((float *)self);
        startposIndex = startposObjects.size() - 1;
        startposObjectInit(self);
    }

    void __fastcall playLayerInitHook(void *self, void *_) 
    {
        startposObjects = {};
        startposIndex = 0;
        playLayer = self;
        playLayerInit(self);
    }

    void __fastcall playLayerDestructorHook(void *self, void *_) 
    {
        playLayer = nullptr;
        playLayerDestructor(self);
    }

    void init()
    {
        uintptr_t base = (uintptr_t)GetModuleHandleA(0);

        MH_CreateHook((LPVOID)(base + 0x3A0D10), startposObjectInitHook, (LPVOID *)&startposObjectInit);
        MH_CreateHook((LPVOID)(base + 0x18CC80), playLayerInitHook, (LPVOID *)&playLayerInit);
        MH_CreateHook((LPVOID)(base + 0x2D6580), playLayerDestructorHook, (LPVOID *)&playLayerDestructor);
        resetLevel = (decltype(resetLevel))(base + 0x2E42B0);
        setStartPosObject = (decltype(setStartPosObject))(base + 0x195FC0);
    }

    void switchStartPos(bool direction)
    {
        if (playLayer == nullptr) 
        {
            return;
        }

        if (direction) 
        {
            startposIndex = startposIndex + 1;
            if (startposIndex >= startposObjects.size()) 
            {
                startposIndex = -1;
            }
        }
        else 
        {
            startposIndex = startposIndex - 1;
            if (startposIndex < -1) 
            {
                startposIndex = startposObjects.size() - 1;
            }
        }

        ((uint32_t *)playLayer)[0xB7D] = 0; // PlayLayer::startPosCheckpoint i believe?

        if (startposIndex >= 0) 
        {
            setStartPosObject(playLayer, startposObjects[startposIndex]);
        } 
        else 
        {
            setStartPosObject(playLayer, nullptr);
        }

        resetLevel(playLayer);
    }

    void setEnabled(bool enable) 
    {
        enabled = enable;
    }
}
