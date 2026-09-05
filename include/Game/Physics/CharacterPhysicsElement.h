#ifndef GAME_PHYSICS_CHARACTER_PHYSICS_ELEMENT_H
#define GAME_PHYSICS_CHARACTER_PHYSICS_ELEMENT_H

#include "NL/nlMath.h"
#include "types.h"

struct CharacterPhysicsElement
{
    /* 0x00 */ nlMatrix4 matLocalToParent;
    /* 0x40 */ char szName[32];
    /* 0x60 */ u32 uHashID;
    /* 0x64 */ char szParentName[32];
    /* 0x84 */ u32 uParentHashID;
    /* 0x88 */ u32 uPrimitiveType;
    /* 0x8C */ float fWidth;
    /* 0x90 */ float fLength;
    /* 0x94 */ float fHeight;
    /* 0x98 */ float fRadius;
    /* 0x9C */ u32 uReserved;
}; // size: 0xA0

class CharacterPhysicsData
{
public:
    virtual ~CharacterPhysicsData() { delete[] pPhysicsElements; }

    /* 0x04 */ u32 physicsElementCount;
    /* 0x08 */ CharacterPhysicsElement* pPhysicsElements;
}; // size: 0xC

bool LoadCharacterPhysicsElements(void* pFileData, unsigned long nFileSize,
    CharacterPhysicsData* pPhysicsData, bool arg3);

#endif
