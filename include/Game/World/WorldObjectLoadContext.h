#ifndef GAME_WORLD_WORLD_OBJECT_LOAD_CONTEXT_H
#define GAME_WORLD_WORLD_OBJECT_LOAD_CONTEXT_H

#include "types.h"

class World;

struct WorldObjectLoadContext
{
    WorldObjectLoadContext() { }

    WorldObjectLoadContext(World* pWorld)
    {
        m_pObject = 0;
        m_pWorld = pWorld;
        m_uNumObjectsLoaded = 0;
        m_pParent = 0;
    }

    u8* GetParentData();

    /* 0x00 */ u8* m_pObject;
    /* 0x04 */ World* m_pWorld;
    /* 0x08 */ unsigned long m_uNumObjectsLoaded;
    /* 0x0C */ u8* m_pParent;
};

#endif // GAME_WORLD_WORLD_OBJECT_LOAD_CONTEXT_H
