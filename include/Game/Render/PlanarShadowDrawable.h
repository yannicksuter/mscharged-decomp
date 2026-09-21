#ifndef GAME_RENDER_PLANAR_SHADOW_DRAWABLE_H
#define GAME_RENDER_PLANAR_SHADOW_DRAWABLE_H

#include "Game/World/WorldDrawable.h"
#include "Game/World/WorldObjectLoadContext.h"
#include "NL/nlMath.h"
#include "types.h"

class glModel;
class GLView;

// The stadium model drawable. It owns one model, renders it through the
// world's views and casts the planar ground shadow that gives it its name.
// It adds no storage of its own to the world drawable.
class PlanarShadowDrawable : public WorldDrawable
{
public:
    PlanarShadowDrawable(
        WorldObjectLoadContext* context, glModel* model, unsigned long type);

    virtual void Draw();
    virtual void V8(GLView* view);
    virtual PlanarShadowDrawable* Clone(unsigned long hash);
    virtual void Initialize(glModel* model, unsigned long hash);
    virtual void DrawPlanarShadow();
}; // size: 0x78

// The charge shadow keeps its own transform and one duplicated model per
// charge level.
class ChargeShadowDrawable : public PlanarShadowDrawable
{
public:
    ChargeShadowDrawable(
        WorldObjectLoadContext* context, glModel* model, unsigned long type);
    virtual ~ChargeShadowDrawable();

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

#endif // GAME_RENDER_PLANAR_SHADOW_DRAWABLE_H
