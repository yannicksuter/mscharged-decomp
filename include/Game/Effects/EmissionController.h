#ifndef GAME_EFFECTS_EMISSION_CONTROLLER_H
#define GAME_EFFECTS_EMISSION_CONTROLLER_H

#include "NL/nlDLListContainer.h"
#include "NL/nlFunction.h"
#include "NL/nlMath.h"
#include "types.h"

class cPN_SAnimController;
class cPoseAccumulator;
class EffectsGroup;
class EmissionManager;
class ParticleSystem;
class UserEffectSpec;
struct EffectsSpec;

class EmissionController
{
public:
    EmissionController(EffectsGroup* pEffectsGroup, EmissionManager* arg5, unsigned short id, void* arg7, int view);
    ~EmissionController();
    void InitializeSystemsFromGroup();
    void SetPosition(const nlVector3& pos);
    void SetDirection(const nlVector3& dir);
    void SetVelocity(const nlVector3& velocity);
    void SetPoseAccumulator(const cPoseAccumulator& pose);
    void SetAnimController(const cPN_SAnimController& animController);
    void ClearParticles();
    void Die();
    const nlVector3& GetPosition() const
    {
        return m_vPosition;
    }
    float GetRemainingTime() const;
    bool IsLingering() const;
    void fn_802E4DF8(EffectsSpec& spec, nlVector3& pos, nlVector3& vel);
    void fn_802E5164(EffectsSpec* pSpec, ParticleSystem* pSys);
    bool Update(float dt);
    int Render();
    float GetBoundingRadius() const;
    void SetUpdateCallback(const Function1<void, EmissionController&>& callback);
    void SetFinishedCallback(const Function2<void, EmissionController&, int>& callback);

    /* 0x00 */ EffectsGroup* m_pGroup;
    /* 0x04 */ nlDLListContainer<ParticleSystem*> m_Systems;
    /* 0x0C */ Function<void(EmissionController&)> mUpdateCallback;
    /* 0x14 */ Function<void(EmissionController&, int)> mFinishedCallback;
    /* 0x1C */ void* m_pContext;
    /* 0x20 */ Function<nlVector3(EmissionController&, EffectsSpec&)> mUnidentified020;
    /* 0x28 */ bool m_Replaying;
    /* 0x29 */ u8 unknown_0x29[0x03];
    /* 0x2C */ float m_Age;
    /* 0x30 */ float m_TimeScale;
    /* 0x34 */ float m_ReplayDeltaTime;
    // R4QE01 keeps the m_Systems head load below the callee saves in both const
    // reductions (GetRemainingTime, GetBoundingRadius), which GC/3.0a5 emits only
    // when the class carries a mutable non-pointer member; the constructor's
    // store of m_pGroup at offset 0 rules out the vptr that produces the same
    // gate. Which member held the qualifier is not recoverable from the stripped
    // DOL - the gate is class-wide - and m_bLingering is the plain flag this
    // class's const reader (IsLingering) consumes directly.
    /* 0x38 */ mutable bool m_bLingering;
    /* 0x39 */ bool m_bDying;
    /* 0x3A */ u8 unknown_0x3A[0x02];
    /* 0x3C */ float m_fGround;
    /* 0x40 */ u16 m_aFacing;
    /* 0x42 */ bool m_bVisible;
    /* 0x43 */ bool m_bDisabled;
    /* 0x44 */ nlVector3 m_vPosition;
    /* 0x50 */ nlVector3 m_vDirection;
    /* 0x5C */ nlVector3 m_vVelocity;
    /* 0x68 */ const cPoseAccumulator* m_pPose;
    /* 0x6C */ const cPN_SAnimController* m_pAnimController;
    /* 0x70 */ u16 m_Id;
    /* 0x72 */ u8 unknown_0x72[0x02];
    /* 0x74 */ u32 m_uUserData;
    /* 0x78 */ u32 m_uJointIDOverride;
    /* 0x7C */ int m_nUserEffects;
    /* 0x80 */ UserEffectSpec** m_pUserEffects;
    /* 0x84 */ bool m_bPoseErrorDisplayed;
    /* 0x85 */ u8 unknown_0x85[0x03];
    /* 0x88 */ EmissionManager* m_pManager;
    /* 0x8C */ u32 m_View;
};

void* fxLoadEntireFileHigh(const char* filename, unsigned long* fileSize);

#endif // GAME_EFFECTS_EMISSION_CONTROLLER_H
