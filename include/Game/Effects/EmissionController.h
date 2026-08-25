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
    void SetPoseAccumulator(const cPoseAccumulator& poseAccumulator);
    void SetAnimController(const cPN_SAnimController& animController);
    void SetUpdateCallback(const Function1<void, EmissionController&>& callback);
    void SetFinishedCallback(const Function1<void, EmissionController&>& callback);

    /* 0x00 */ u8 unknown_0x00[0x40];
    /* 0x40 */ u16 m_aFacing;
    /* 0x42 */ u8 unknown_0x42[0x32];
    /* 0x74 */ u32 m_uUserData;
};

#endif // GAME_EFFECTS_EMISSION_CONTROLLER_H
