#ifndef GAME_WORLD_WORLD_HELPER_OBJECT_H
#define GAME_WORLD_WORLD_HELPER_OBJECT_H

#include "Game/World/WorldObject.h"
#include "NL/nlMath.h"

// Stream object type 0x104: a world-space transform node without a model,
// also the base of the stadium markers, lights and WorldEffect. Its vtable
// shares the matrix accessor at 0x80129EE0 (returning the matrix at +0x20)
// and the empty SetWorldMatrix at 0x80341EE8. The predecessor named this kind
// of node HelperObject.
class WorldAnimController;

class WorldHelperObject : public WorldObject
{
public:
    virtual ~WorldHelperObject() { }
    virtual void Initialize(WorldObjectLoadContext* context);
    virtual void ReleaseResources();
    virtual nlMatrix4* GetWorldMatrix() { return &mWorldMatrix; }
    virtual void SetWorldMatrix(const nlMatrix4& transform);

    /* 0x04 */ unsigned long m_uHashID;
    /* 0x08 */ unsigned long m_uObjectType;
    /* 0x0C */ unsigned long m_uObjectCreationFlags;
    /* 0x10 */ World* m_pWorld;
    /* 0x14 */ int m_nAnimNode;
    /* 0x18 */ WorldAnimController* m_pAnimController;
    /* 0x1C */ unsigned char m_pad1C[0x04];
    /* 0x20 */ nlMatrix4 mWorldMatrix;
}; // size: 0x60

#endif // GAME_WORLD_WORLD_HELPER_OBJECT_H
