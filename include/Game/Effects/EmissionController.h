#ifndef GAME_EFFECTS_EMISSION_CONTROLLER_H
#define GAME_EFFECTS_EMISSION_CONTROLLER_H

#include "NL/nlFunction.h"
#include "NL/nlMath.h"
#include "types.h"

class cPN_SAnimController;
class cPoseAccumulator;

class EmissionController
{
public:
    void SetPosition(const nlVector3& pos);
    void SetDirection(const nlVector3& dir);
    void SetVelocity(const nlVector3& velocity);
    void SetPoseAccumulator(const cPoseAccumulator& pose);
    void SetAnimController(const cPN_SAnimController& animController);
    const nlVector3& GetPosition() const
    {
        return m_vPosition;
    }
    float GetRemainingTime() const;
    bool IsLingering() const;
    void SetUpdateCallback(const Function1<void, EmissionController&>& callback);
    void SetFinishedCallback(const Function1<void, EmissionController&>& callback);

    /* 0x00 */ u8 unknown_0x00[0x0C];
    /* 0x0C */ Function1<void, EmissionController&> mUpdateCallback;
    /* 0x14 */ Function1<void, EmissionController&> mFinishedCallback;
    /* 0x1C */ u8 unknown_0x1C[0x0C];
    /* 0x28 */ u8 m_GlView;
    /* 0x29 */ u8 unknown_0x29[0x0F];
    /* 0x38 */ bool m_bLingering;
    /* 0x39 */ u8 unknown_0x39[0x03];
    /* 0x3C */ float m_fGround;
    /* 0x40 */ u16 m_aFacing;
    /* 0x42 */ u8 unknown_0x42[0x02];
    /* 0x44 */ nlVector3 m_vPosition;
    /* 0x50 */ nlVector3 m_vDirection;
    /* 0x5C */ nlVector3 m_vVelocity;
    /* 0x68 */ const cPoseAccumulator* m_pPose;
    /* 0x6C */ const cPN_SAnimController* m_pAnimController;
    /* 0x70 */ u8 unknown_0x70[0x04];
    /* 0x74 */ u32 m_uUserData;
};

void* fxLoadEntireFileHigh(const char* filename, unsigned long* fileSize);

#endif // GAME_EFFECTS_EMISSION_CONTROLLER_H
