#include "smartStartpos.hpp"
#include <vector>

namespace smartStartpos
{
    bool enabled = false;
    std::vector<uint32_t *> gamemodePortals;
    //std::vector<uint32_t *> mirrorPortals;
    std::vector<uint32_t *> sizePortals;
    std::vector<uint32_t *> speedPortals;
    std::vector<uint32_t *> startPositions;

    void(__thiscall *addObject)(void *, void *) = nullptr;

    void __fastcall addObjectHook(void *self, void *_, uint32_t *object) {
        uint32_t objectID = object[0xE1];
        switch (objectID) {
        case 12:
        case 13:
        case 47:
        case 111:
        case 660:
        case 745:
        case 1331:
        case 1933:
            gamemodePortals.push_back(object);
            updateStartPos(self);
            break;

        //case 45:
        //case 46:
        //    mirrorPortals.push_back(object);
        //    updateStartPos();
        //    break;

        case 99:
        case 101:
            sizePortals.push_back(object);
            updateStartPos(self);
            break;

        case 200:
        case 201:
        case 202:
        case 203:
        case 1334:
            speedPortals.push_back(object);
            updateStartPos(self);
            break;

        case 31:
            startPositions.push_back(object);
            updateStartPos(self);
            break;
        }

        addObject(self, object);
    }

    void init() {
        uintptr_t base = (uintptr_t)GetModuleHandleA(0);

        MH_CreateHook((LPVOID)(base + 0x2DBD30), addObjectHook, (LPVOID *)&addObject);
    }

    void resetObjects() {
        gamemodePortals = {};
        //mirrorPortals = {};
        sizePortals = {};
        speedPortals = {};
        startPositions = {};
    }

    void setEnabled(bool enable) {
        enabled = enable;
    }

    void updateStartPos(void *playLayer) {
        if (!enabled) {
            return;
        }

        for (uint32_t *startpos : startPositions) {
            // +10C -> gamemode
            // +114 -> mini
            // +110 -> speed
            uint32_t *startposSettings = (uint32_t *)startpos[0x19E];
            uint32_t *levelSettings = (uint32_t *)(((uint32_t *)playLayer)[0x21E]);
            float startposX = ((float *)startpos)[0x12];
            float maxPortalX = -1;
            
            startposSettings[0x43] = levelSettings[0x43];
            for (uint32_t *gamemodePortal : gamemodePortals) {
                float portalX = ((float *)gamemodePortal)[0x12];
                uint32_t id = gamemodePortal[0xE1];
                if (portalX - 10 > startposX) break;
                if (portalX - 10 < startposX && maxPortalX < portalX) {
                    maxPortalX = portalX;
                    switch (id) {
                    case 12:
                        startposSettings[0x43] = 0;
                        break;
                    case 13:
                        startposSettings[0x43] = 1;
                        break;
                    case 47:
                        startposSettings[0x43] = 2;
                        break;
                    case 111:
                        startposSettings[0x43] = 3;
                        break;
                    case 660:
                        startposSettings[0x43] = 4;
                        break;
                    case 745:
                        startposSettings[0x43] = 5;
                        break;
                    case 1331:
                        startposSettings[0x43] = 6;
                        break;
                    case 1933:
                        startposSettings[0x43] = 7;
                        break;
                    }
                }
            }

            maxPortalX = -1;
            startposSettings[0x45] = levelSettings[0x45];
            for (uint32_t *sizePortal : sizePortals) {
                float portalX = ((float *)sizePortal)[0x12];
                uint32_t id = sizePortal[0xE1];
                if (portalX - 10 > startposX) break;
                if (portalX - 10 < startposX && maxPortalX < portalX) {
                    maxPortalX = portalX;
                    startposSettings[0x45] = (int)(id == 101);
                }
            }

            maxPortalX = -1;
            startposSettings[0x44] = levelSettings[0x44];
            for (uint32_t *speedPortal : speedPortals) {
                float portalX = ((float *)speedPortal)[0x12];
                uint32_t id = speedPortal[0xE1];
                if (portalX - 10 > startposX) break;
                if (portalX - 10 < startposX && maxPortalX < portalX) {
                    maxPortalX = portalX;
                    switch (id) {
                    case 200:
                    case 201:
                    case 202:
                    case 203:
                        startposSettings[0x44] = id - 200;
                        break;

                    case 1334:
                        startposSettings[0x44] = 4;
                        break;
                    }
                }
            }
        }
    }
}