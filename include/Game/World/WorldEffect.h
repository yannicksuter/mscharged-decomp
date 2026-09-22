#ifndef GAME_WORLD_WORLD_EFFECT_H
#define GAME_WORLD_WORLD_EFFECT_H

#include "types.h"
#include "Game/World/WorldObject.h"

class World;
class WorldAnimController;
class EmissionController;
struct WorldObjectLoadContext;

class WorldEffect : public WorldObject
{
public:
    WorldEffect() : m_bActive(1) { }
    virtual ~WorldEffect();
    virtual void ReleaseResources();
    virtual nlMatrix4* GetWorldMatrix();
    virtual void SetWorldMatrix(const nlMatrix4& transform);
    virtual void UnidentifiedVirtual1C(WorldObjectLoadContext* context);

    void Update(float fDeltaT);
    void Emit();
    void UpdateVisibility(EmissionController* pController);

    /* 0x04 */ u8 m_pad04[0x0C];
    /* 0x10 */ World* m_pWorld;
    /* 0x14 */ int m_nAnimNode;
    /* 0x18 */ WorldAnimController* m_pAnimController;
    /* 0x1C */ u8 m_pad1C[0x44];
    /* 0x60 */ float m_fEmissionInterval;
    /* 0x64 */ float mUnidentified064;
    /* 0x68 */ float m_fEmissionRadius;
    /* 0x6C */ u8 m_pad6C[0x04];
    /* 0x70 */ unsigned long m_uEffectHash;
    /* 0x74 */ unsigned long m_uProbability;
    /* 0x78 */ int m_nEmissionCount;
    /* 0x7C */ int m_nTimingMode;
    /* 0x80 */ float m_fEmissionTime;
    /* 0x84 */ float m_fPreviousEmissionTime;
    /* 0x88 */ u8 m_pad88[0x08];
    /* 0x90 */ int m_nRemainingEmissions;
    /* 0x94 */ int m_nEmissionID;
    /* 0x98 */ int m_bActive;
    /* 0x9C */ bool m_bAlwaysVisible;
};

typedef char WorldEffect_size_check[sizeof(WorldEffect) == 0xA0 ? 1 : -1];

#endif // GAME_WORLD_WORLD_EFFECT_H
