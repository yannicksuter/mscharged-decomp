#ifndef GAME_RENDER_TU_8027A4BC_H
#define GAME_RENDER_TU_8027A4BC_H

#include "NL/nlMath.h"
#include "types.h"

class BasicStadium;
class glModel;
class GLView;

// Planar shadow drawables. Both extend the world drawable the world-animation
// unit operates on, so everything below 0x60 is that base's storage and stays
// padding here, as in Game/World/worldanim.cpp.
class GLResourcePool;

struct PlanarShadowDrawable_8027A4BC
{
    virtual ~PlanarShadowDrawable_8027A4BC();
    virtual void V1();
    virtual nlMatrix4* GetWorldMatrix();
    virtual void SetWorldMatrix(const nlMatrix4* transform);
    virtual void V4(void* world);
    virtual void Draw();
    virtual bool V6(const nlVector4* planes);
    virtual void V7(glModel* model);
    virtual void V8(GLView* view);
    virtual PlanarShadowDrawable_8027A4BC* Clone(unsigned long hash);
    virtual void Initialize(glModel* model, unsigned long hash);
    virtual void DrawShadow();

    float GetOpacity() const { return m_fOpacity; }

    /* 0x04 */ unsigned long m_uHashID;
    /* 0x08 */ unsigned long m_uType;
    /* 0x0C */ unsigned long m_uObjectCreationFlags;
    /* 0x10 */ BasicStadium* m_pWorld;
    /* 0x14 */ void* m_pUnidentified14;
    /* 0x18 */ void* m_pUnidentified18;
    /* 0x1C */ u8 m_pad1C[0x44];
    /* 0x60 */ float m_fRadius;
    /* 0x64 */ glModel* m_pModel;
    /* 0x68 */ u8 m_pad68[0x08];
    /* 0x70 */ unsigned long m_uFlags;
    /* 0x74 */ float m_fOpacity;
}; // size: 0x78

// The charge shadow keeps its own transform and one duplicated model per
// charge level.
struct ChargeShadowDrawable_8027A7F0 : public PlanarShadowDrawable_8027A4BC
{
    virtual ~ChargeShadowDrawable_8027A7F0();

    /* 0x78 */ unsigned long m_uChargeFlags;
    /* 0x7C */ nlQuaternion m_orientation;
    /* 0x8C */ nlVector3 m_translation;
    /* 0x98 */ float m_fScale;
    /* 0x9C */ float m_fCharge;
    /* 0xA0 */ nlMatrix4 m_worldMatrix;
    /* 0xE0 */ bool m_bWorldMatrixUpToDate;
    /* 0xE1 */ u8 m_padE1[3];
    /* 0xE4 */ glModel* m_pChargeModels[6];
}; // size: 0xFC

// The load context the clone paths build for the copy they construct.
struct ShadowLoadContext_8027A4BC
{
    /* 0x00 */ void* m_pUnidentified00;
    /* 0x04 */ BasicStadium* m_pWorld;
    /* 0x08 */ void* m_pUnidentified08;
    /* 0x0C */ void* m_pUnidentified0C;
}; // size: 0x10

extern void* lbl_80523138[];
extern void* lbl_80523188[];

#endif // GAME_RENDER_TU_8027A4BC_H
