#ifndef GAME_WORLD_WORLD_OBJECT_80129EE0_H
#define GAME_WORLD_WORLD_OBJECT_80129EE0_H

#include "Game/World/WorldObject.h"
#include "NL/nlMath.h"

// Common base of the stream objects whose vtables share the matrix accessor at
// 0x80129EE0 (it returns the matrix at +0x20) and the empty SetWorldMatrix at
// 0x80341EE8: the two stadium markers, WorldObject_805223B0 and WorldEffect.
// No drawable vtable uses either body. WorldEffect also uses the world and
// animation context in this prefix. The original class name is unknown.
class WorldAnimController;

class WorldObject_80129EE0 : public WorldObject
{
public:
    virtual nlMatrix4* GetWorldMatrix() { return &mWorldMatrix; }
    virtual void SetWorldMatrix(const nlMatrix4& transform);

    /* 0x04 */ unsigned char mUnidentified004[0x0C];
    /* 0x10 */ World* m_pWorld;
    /* 0x14 */ int m_nAnimNode;
    /* 0x18 */ WorldAnimController* m_pAnimController;
    /* 0x1C */ unsigned char mUnidentified01C[0x04];
    /* 0x20 */ nlMatrix4 mWorldMatrix;
}; // size: 0x60

#endif // GAME_WORLD_WORLD_OBJECT_80129EE0_H
