#ifndef GAME_WORLD_WORLD_OBJECT_80129EE0_H
#define GAME_WORLD_WORLD_OBJECT_80129EE0_H

#include "Game/World/WorldObject.h"
#include "NL/nlMath.h"

// Common base of the stream objects whose vtables share the matrix accessor at
// 0x80129EE0 (it returns the matrix at +0x20) and the empty SetWorldMatrix at
// 0x80341EE8: the two stadium markers, WorldObject_805223B0 and WorldEffect.
// No drawable vtable uses either body. The real name is not recoverable; only
// the member those two bodies touch is known.
class WorldObject_80129EE0 : public WorldObject
{
public:
    virtual nlMatrix4* GetWorldMatrix();
    virtual void SetWorldMatrix(const nlMatrix4& transform);

    /* 0x04 */ unsigned char mUnidentified004[0x1C];
    /* 0x20 */ nlMatrix4 mWorldMatrix;
}; // size: 0x60

#endif // GAME_WORLD_WORLD_OBJECT_80129EE0_H
