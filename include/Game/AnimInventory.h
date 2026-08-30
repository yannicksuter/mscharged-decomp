#ifndef GAME_ANIM_INVENTORY_H
#define GAME_ANIM_INVENTORY_H

#include "Game/AnimProps/AnimProperties.h"
#include "Game/Inventory.h"
#include "Game/SAnim.h"

class cAnimInventory
{
public:
    cAnimInventory(const AnimProperties* props, int count);
    ~cAnimInventory();

    void AddAnimBundle(char* pMem, int len, const char* szFilename);

    cSAnim* GetAnim(int i)
    {
        return m_pSAnims[i];
    }

    bool GetMirrored(int i)
    {
        return m_pAnimProperties[i].mirror;
    }
    int GetBallRotationMode(int i)
    {
        return m_pAnimProperties[i].ballRotationMode;
    }

    /* 0x00 */ int m_nNumProperties;
    /* 0x04 */ cInventory<cSAnim>* m_pSAnimInventory;
    /* 0x08 */ cSAnim** m_pSAnims;
    /* 0x0C */ const AnimProperties* m_pAnimProperties;
}; // total size: 0x10

extern cInventory<cSAnim>* g_pDefaultSAnimInventory;

#endif // GAME_ANIM_INVENTORY_H
