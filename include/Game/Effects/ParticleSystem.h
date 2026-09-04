#ifndef GAME_EFFECTS_PARTICLE_SYSTEM_H
#define GAME_EFFECTS_PARTICLE_SYSTEM_H

#include "Game/Effects/EffectsGroup.h"
#include "Game/Effects/EffectsTemplate.h"
#include "NL/nlDLListContainer.h"
#include "NL/nlMath.h"
#include "types.h"

class GLView;
class EffectsLight;
struct glModel;

struct ParticleReturn
{
    /* 0x00 */ nlVector2 texcoord[4];
    /* 0x20 */ nlVector3 position[4];
    /* 0x50 */ nlColour c;
}; // size: 0x54

struct Particle
{
    /* 0x00 */ EffectsTemplate* mUnidentified000;
    /* 0x04 */ float timeElapsed;
    /* 0x08 */ float mUnidentified008;
    /* 0x0C */ float lifeSpan;
    /* 0x10 */ nlVector3 mUnidentified010;
    /* 0x1C */ nlVector3 position;
    /* 0x28 */ nlVector3 velDir;
    /* 0x34 */ float mass;
    /* 0x38 */ float size;
    /* 0x3C */ float m_fUnidentified03C;
    /* 0x40 */ float mUnidentified040;
    /* 0x44 */ float rot;
    /* 0x48 */ float dRot;
    /* 0x4C */ float velocity;
    /* 0x50 */ float acceleration;
    /* 0x54 */ float frame;
    /* 0x58 */ float FPS;
    /* 0x5C */ float mUnidentified05C;
    /* 0x60 */ bool mUnidentified060;
    /* 0x61 */ unsigned char mPadding061[7];
}; // size: 0x68

class ParticleSystem
{
public:
    ParticleSystem(EffectsTemplate* pTemplate,
        nlDLListSlotPool<Particle*>* pFreeParticles, EffectsSpec* spec,
        unsigned long resourceID);
    ~ParticleSystem();

    void UpdateCoordSys();
    void UpdateCoordSys(nlMatrix4& mCoordSys);
    void CreateNewParticles(int numParticles);
    void UpdateAllParticles(float dt, const nlMatrix4* pCoordSys);
    void UpdateLight(EffectsLight* pLight, Particle* pPart,
        EffectsTemplate* pTemplate, const nlVector3& viewRight,
        const nlVector3& viewUp, const nlMatrix4* pCoordSys);
    void fn_802E1EC0(Particle* pPart, const nlMatrix4* pCoordSys);
    void UpdateParticle(ParticleReturn* pReturn, Particle* pPart,
        EffectsTemplate* pTemplate, const nlVector3& viewRight,
        const nlVector3& viewUp, const nlMatrix4* pCoordSys);
    void fn_802E2848();
    int RenderAllParticles(GLView* view);
    void Die();
    bool Update(float dt);
    float GetRemainingTime() const;

    /* 0x00 */ unsigned long mUnidentified000;
    /* 0x04 */ EffectsSpec* m_pSpec;
    /* 0x08 */ EffectsTemplate* m_pTemplate;
    /* 0x0C */ float m_fElapsedTime;
    /* 0x10 */ float mUnidentified010;
    /* 0x14 */ float mUnidentified014;
    /* 0x18 */ float m_fNumParticlesToCreate;
    /* 0x1C */ float m_fDelay;
    /* 0x20 */ float mUnidentified020;
    /* 0x24 */ unsigned long m_uLayer;
    /* 0x28 */ nlVector3 m_vVelocity;
    /* 0x34 */ nlVector3 m_vPosition;
    /* 0x40 */ nlVector3 m_vForward;
    /* 0x4C */ nlVector3 m_vSourcePosition;
    /* 0x58 */ nlMatrix4 mUnidentified058;
    /* 0x98 */ bool m_bAmDying;
    /* 0x99 */ unsigned char mPadding099[3];
    /* 0x9C */ unsigned long mUnidentified09C;
    /* 0xA0 */ nlDLListSlotPool<Particle*> m_Particles;
    /* 0xBC */ int mUnidentified0BC;
    /* 0xC0 */ nlDLListSlotPool<Particle*>* mUnidentified0C0;
    /* 0xC4 */ unsigned short m_aFacing;
    /* 0xC6 */ bool m_bVisible;
    /* 0xC7 */ unsigned char mPadding0C7;

    static int m_NumInstances;
    static bool (*m_Callback)(ParticleSystem*, GLView*,
        nlDLListSlotPool<Particle*>*, const nlVector3&, const nlVector3&,
        const nlMatrix4*);
    static glModel* (*m_LightingCallback)(glModel*);
    static bool m_AllowInFront;
    static float m_fAspect;
}; // size: 0xC8

bool fxParticleStartup(int maxNumParticles);
bool fxParticleShutdown();
void fn_802E3AC0(int maxNumParticles);

#endif // GAME_EFFECTS_PARTICLE_SYSTEM_H
