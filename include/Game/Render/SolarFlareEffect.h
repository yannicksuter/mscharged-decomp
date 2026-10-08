#ifndef GAME_RENDER_SOLAR_FLARE_EFFECT_H
#define GAME_RENDER_SOLAR_FLARE_EFFECT_H

#include "NL/nlMath.h"
#include "Game/World/WorldDrawable.h"
#include "Game/Render/TimedObject.h"

class SolarFlareEffect : public TimedObject
{
public:
    SolarFlareEffect(const nlVector3& targetPosition);
    virtual ~SolarFlareEffect();
    virtual void Update(float deltaTime);

    /* 0x10 */ nlVector3 mTargetPosition;
    /* 0x1C */ float mUnidentified01C;
    /* 0x20 */ bool mUnidentified020;
}; // size: 0x24

class SolarFlareDrawable : public WorldDrawable
{
public:
    virtual ~SolarFlareDrawable();
    virtual void ReleaseResources();
    virtual void Draw();
    virtual void Initialize(WorldObjectLoadContext* context);

    /* 0x70 */ unsigned long m_uDrawEnabled;
};

extern float gSolarFlareEffectLifetime;
extern float gSolarFlareEffectLifetimeVariation;
extern float lbl_806DEE90;
extern float lbl_806DEE94;
extern bool gForceDrawSolarFlareDrawable;
extern SolarFlareDrawable* g_pSolarFlareDrawable;

#endif // GAME_RENDER_SOLAR_FLARE_EFFECT_H
