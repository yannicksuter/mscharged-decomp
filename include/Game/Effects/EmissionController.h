#ifndef GAME_EFFECTS_EMISSION_CONTROLLER_H
#define GAME_EFFECTS_EMISSION_CONTROLLER_H

#include "NL/nlFunction.h"
#include "NL/nlMath.h"
#include "types.h"

class cPN_SAnimController;
class cPoseAccumulator;
class EffectsGroup;
class EmissionManager;

class EmissionController
{
public:
    ~EmissionController();
    void SetPosition(const nlVector3& pos);
    void SetDirection(const nlVector3& dir);
    void SetVelocity(const nlVector3& velocity);
    void SetPoseAccumulator(const cPoseAccumulator& pose);
    void SetAnimController(const cPN_SAnimController& animController);
    void Die();
    const nlVector3& GetPosition() const
    {
        return m_vPosition;
    }
    float GetRemainingTime() const;
    bool IsLingering() const;
    void SetUpdateCallback(const Function1<void, EmissionController&>& callback);
    void SetFinishedCallback(const Function1<void, EmissionController&>& callback);

    /* 0x00 */ EffectsGroup* m_pGroup;
    /* 0x04 */ u8 unknown_0x04[0x08];
    /* 0x0C */ Function1<void, EmissionController&> mUpdateCallback;
    /* 0x14 */ Function1<void, EmissionController&> mFinishedCallback;
    /* 0x1C */ void* m_pContext;
    /* 0x20 */ u8 unknown_0x20[0x08];
    /* 0x28 */ u8 m_GlView;
    /* 0x29 */ u8 unknown_0x29[0x03];
    /* 0x2C */ float m_Age;
    /* 0x30 */ u8 unknown_0x30[0x08];
    /* 0x38 */ bool m_bLingering;
    /* 0x39 */ bool m_bPlaying;
    /* 0x3A */ u8 unknown_0x3A[0x02];
    /* 0x3C */ float m_fGround;
    /* 0x40 */ u16 m_aFacing;
    /* 0x42 */ bool m_bVisible;
    /* 0x43 */ u8 unknown_0x43;
    /* 0x44 */ nlVector3 m_vPosition;
    /* 0x50 */ nlVector3 m_vDirection;
    /* 0x5C */ nlVector3 m_vVelocity;
    /* 0x68 */ const cPoseAccumulator* m_pPose;
    /* 0x6C */ const cPN_SAnimController* m_pAnimController;
    /* 0x70 */ u16 m_Id;
    /* 0x72 */ u8 unknown_0x72[0x02];
    /* 0x74 */ u32 m_uUserData;
    /* 0x78 */ u8 unknown_0x78[0x10];
    /* 0x88 */ EmissionManager* m_pManager;
    /* 0x8C */ int m_View;
};

void* fxLoadEntireFileHigh(const char* filename, unsigned long* fileSize);

#endif // GAME_EFFECTS_EMISSION_CONTROLLER_H
