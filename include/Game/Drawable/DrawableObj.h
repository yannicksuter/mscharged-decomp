#ifndef GAME_DRAWABLE_DRAWABLE_OBJ_H
#define GAME_DRAWABLE_DRAWABLE_OBJ_H

#include "Game/World/WorldObject.h"
#include "NL/nlMath.h"
#include "NL/gl/glModel.h"

class GLView;
class World;
class WorldAnimController;

// The drawable half of a world object: the cached world matrix the world
// object stream fills in, the model it renders and the flags the render
// passes test. Everything past 0x78 belongs to the derived classes; the
// posed variant lives in RenderObject.
class DrawableObject : public WorldObject
{
public:
    virtual ~DrawableObject() { }
    virtual void ReleaseResources();
    virtual nlMatrix4* GetWorldMatrix();
    virtual void SetWorldMatrix(const nlMatrix4& transform);
    virtual void Draw();
    virtual bool V6(const nlVector4* planes);
    virtual void V7(glModel* model);
    virtual void V8(GLView* view);
    virtual DrawableObject* Clone(unsigned long hash);

    unsigned long GetHashID() const { return m_uHashID; }
    float GetTranslucency() const { return m_fTranslucency; }

    /* 0x04 */ unsigned long m_uHashID;
    /* 0x08 */ unsigned long m_uRenderLayer;
    /* 0x0C */ unsigned long m_uObjectCreationFlags;
    /* 0x10 */ World* m_pWorldContext;
    /* 0x14 */ int m_nAnimNode;
    /* 0x18 */ WorldAnimController* m_pAnimController;
    /* 0x1C */ u8 mUnidentified1C[0x04];
    /* 0x20 */ nlMatrix4 mWorldMatrix;
    /* 0x60 */ float m_fBoundingRadius;
    /* 0x64 */ glModel* m_pModel;
    /* 0x68 */ u8 mUnidentified68[0x08];
    /* 0x70 */ unsigned long m_uObjectFlags;
    // 0x74 is per-drawable storage: the stadium models keep their planar
    // shadow translucency there, the indicator objects an integer state.
    union
    {
        /* 0x74 */ float m_fTranslucency;
        /* 0x74 */ int mUnidentified074;
    };
}; // size: 0x78

#endif // GAME_DRAWABLE_DRAWABLE_OBJ_H
