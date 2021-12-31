#ifndef __TELEPORTPORTALOBJECT_H__
#define __TELEPORTPORTALOBJECT_H__

#include <gd.h>

namespace gd {
    class GameObject;
    class GDH_DLL TeleportPortalObject : public GameObject {
    public:
        PAD(4); // i have no idea what this is, its not even set to 0 in the ctor
        TeleportPortalObject* m_pOrangePortal; // 0x46c
        bool unk470;
        float unk474;
        bool unk478;
    };
}

#endif