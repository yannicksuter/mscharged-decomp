#ifndef GAME_RENDER_STADIUM_WORLD_OBJECTS_H
#define GAME_RENDER_STADIUM_WORLD_OBJECTS_H

#include "Game/MathHelpers.h"
#include "NL/nlColour.h"
#include "Game/World/WorldDrawable.h"
#include "Game/World/WorldObject_80129EE0.h"

struct WorldObjectLoadContext;

class StadiumWorldDrawable : public WorldDrawable
{
public:
    virtual ~StadiumWorldDrawable() { }
    virtual void ReleaseResources();
    virtual void Draw();
    virtual bool IsVisibleInFrustum(const nlVector4* planes) const;
    virtual void UpdateModelMaterials(glModel* model);
    virtual void Initialize(WorldObjectLoadContext* context);

    float GetBlend() const { return m_fBlend; }
    void UpdateBlend();

    /* 0x70 */ nlVector3 m_boundsMin;
    /* 0x7C */ nlVector3 m_boundsMax;
    /* 0x88 */ unsigned long m_uFlags;
    /* 0x8C */ float m_fBlend;
}; // size: 0x90

class StadiumLight : public WorldObject_80129EE0
{
public:
    virtual ~StadiumLight();
    virtual void ReleaseResources();
    virtual void Initialize(WorldObjectLoadContext* context);

    /* 0x60 */ u8 mUnidentified60[0x04];
    /* 0x64 */ float m_fIntensity;
    /* 0x68 */ u8 mUnidentified68[0x08];
    /* 0x70 */ nlFloatColour m_colour;
    /* 0x80 */ u8 mUnidentified80[0x10];
}; // size: 0x90

class StadiumAttackSideIndicator : public WorldDrawable
{
public:
    virtual ~StadiumAttackSideIndicator();
    virtual void ReleaseResources();
    virtual void Draw();
    virtual void Initialize(WorldObjectLoadContext* context);

    /* 0x70 */ int m_nIndex;
    /* 0x74 */ int m_nVisible;
    /* 0x78 */ u8 mUnidentified78[0x08];
}; // size: 0x80

class StadiumToggleDrawable : public WorldDrawable
{
public:
    virtual ~StadiumToggleDrawable();
    virtual void ReleaseResources();
    virtual void Draw();
    virtual void Initialize(WorldObjectLoadContext* context);

    /* 0x70 */ int m_nVisible;
    /* 0x74 */ u8 mUnidentified74[0x0C];
}; // size: 0x80

class StadiumShadowVolumeDrawable : public WorldDrawable
{
public:
    virtual ~StadiumShadowVolumeDrawable();
    virtual void ReleaseResources();
    virtual void Draw();
    virtual void Initialize(WorldObjectLoadContext* context);

    /* 0x70 */ glModel* m_pShadowModels[2];
    /* 0x78 */ u8 mUnidentified78[0x08];
}; // size: 0x80

class StadiumHighRangeDrawable : public StadiumWorldDrawable
{
public:
    virtual ~StadiumHighRangeDrawable();
    virtual void ReleaseResources();
    virtual void Draw();
    virtual void Initialize(WorldObjectLoadContext* context);
}; // size: 0x90

class StadiumCupTrophyDrawable : public WorldDrawable
{
public:
    virtual ~StadiumCupTrophyDrawable();
    virtual void ReleaseResources();
    virtual void Draw();
    virtual void Initialize(WorldObjectLoadContext* context);

    void SetOpacity(float opacity)
    {
        float clamped = nlMaxEquals(opacity, 0.0f);
        clamped = nlMinEquals(clamped, 1.0f);
        m_fCupTrophyOpacity = clamped;
    }
    float GetOpacity() const { return m_fCupTrophyOpacity; }

    /* 0x70 */ unsigned long m_uCupTrophyKey;
    /* 0x74 */ float m_fCupTrophyOpacity;
    /* 0x78 */ u8 mUnidentified78[0x08];
}; // size: 0x80

#endif // GAME_RENDER_STADIUM_WORLD_OBJECTS_H
