#ifndef GAME_CHARACTER_TEMPLATE_H
#define GAME_CHARACTER_TEMPLATE_H

#include "Game/Character.h"
#include "Game/Inventory.h"

class AnimRetargetList;
class CharacterPhysicsData;
class cAnimInventory;
class cSHierarchy;

struct tCharacterTemplate
{
    /* 0x00 */ int nCharacterModelID[4];
    /* 0x10 */ unsigned long uAnimInventoryHashID;
    /* 0x14 */ unsigned char bAnimInventoryCopy;
    /* 0x15 */ unsigned char pad_0x15[3];
    /* 0x18 */ cInventory<cSHierarchy>* pHierarchyInventory;
    /* 0x1C */ cAnimInventory* pAnimInventory;
    /* 0x20 */ const CharacterPhysicsData* pPhysicsData;
    /* 0x24 */ cInventory<AnimRetargetList>* pAnimRetargetListInventory;
    /* 0x28 */ void* pUnidentified28;
    /* 0x2C */ void* pUnidentified2C;
    /* 0x30 */ void* pUnidentified30;
}; // total size: 0x34

int GetCharacterIndex(const cCharacter* character);

extern cCharacter* g_pCharacters[10];

#endif // GAME_CHARACTER_TEMPLATE_H
