#ifndef GAME_WORLD_WORLD_DRAWABLE_H
#define GAME_WORLD_WORLD_DRAWABLE_H

#include "Game/World/WorldObject.h"
#include "NL/nlMath.h"
#include "types.h"

class GLView;
class glModel;
class WorldAnimController;

// Common world drawable prefix; model-specific storage starts at 0x70.
class WorldDrawable : public WorldObject
{
public:
    virtual ~WorldDrawable() { }
    virtual nlMatrix4* GetWorldMatrix();
    virtual void ReleaseResources() { }
    virtual void SetWorldMatrix(const nlMatrix4& transform)
    {
        mWorldMatrix = transform;
    }
    virtual void Draw();
    virtual bool IsVisibleInFrustum(const nlVector4* planes) const;
    virtual void UpdateModelMaterials(glModel* model);
    virtual void DrawToView(GLView* view);

    void Initialize(WorldObjectLoadContext* context);

    unsigned long GetHashID() const { return m_uHashID; }
    glModel* GetModel() const { return m_pModel; }

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
}; // size: 0x70

#endif // GAME_WORLD_WORLD_DRAWABLE_H
