#ifndef GAME_PHYSICS_CHARACTER_PHYSICS_ELEMENT_H
#define GAME_PHYSICS_CHARACTER_PHYSICS_ELEMENT_H

#include "NL/nlMath.h"
#include "types.h"

struct CharacterPhysicsElement
{
    /* 0x00 */ nlMatrix4 m_LocalToParent;
    /* 0x40 */ char m_Name[32];
    /* 0x60 */ u32 m_HashID;
    /* 0x64 */ char m_ParentName[32];
    /* 0x84 */ u32 m_ParentHashID;
    /* 0x88 */ u32 m_PrimitiveType;
    /* 0x8C */ float m_Width;
    /* 0x90 */ float m_Length;
    /* 0x94 */ float m_Height;
    /* 0x98 */ float m_Radius;
    /* 0x9C */ u32 m_Reserved;
}; // size: 0xA0

class CharacterPhysicsData
{
public:
    virtual ~CharacterPhysicsData() { delete[] m_Elements; }

    /* 0x04 */ u32 m_ElementCount;
    /* 0x08 */ CharacterPhysicsElement* m_Elements;
}; // size: 0xC

#endif
