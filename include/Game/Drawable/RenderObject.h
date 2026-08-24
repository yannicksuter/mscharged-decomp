#ifndef GAME_DRAWABLE_RENDER_OBJECT_H
#define GAME_DRAWABLE_RENDER_OBJECT_H

#include "NL/nlMath.h"

// The shared renderable that the drawable snapshots drive. Only the fields and
// vtable slots the retail code touches are known, so the remaining virtuals
// keep index names.
class RenderObject
{
public:
    virtual void V0();
    virtual void V1();
    virtual void V2();
    virtual void V3(const nlMatrix4*);
    virtual void V4();
    virtual void Draw();
    virtual void V6();
    virtual void V7();
    virtual void V8(void*);

    char _004[0x6C];
    u32 objectFlags;
    char _074[4];
    u32 renderFlags;
    nlQuaternion orientation;
    nlVector3 translation;
    float modelScale;
    float snapshotScale;
    char _0A0[0x40];
    bool worldMatrixUpToDate;
};

#endif // GAME_DRAWABLE_RENDER_OBJECT_H
