#ifndef GAME_ANIM_INVENTORY_H
#define GAME_ANIM_INVENTORY_H

#include "Game/AnimProps/AnimProperties.h"

class cAnimInventory
{
public:
    bool GetMirrored(int i)
    {
        return m_pAnimProperties[i].mirror;
    }

private:
    /* 0x00 */ int m_nNumProperties;
    /* 0x04 */ void* m_pSAnimInventory;
    /* 0x08 */ void* m_pSAnims;
    /* 0x0C */ const AnimProperties* m_pAnimProperties;
}; // total size: 0x10

#endif // GAME_ANIM_INVENTORY_H
