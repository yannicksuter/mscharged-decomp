#ifndef GAME_EFFECTS_EMISSION_CONTROLLER_H
#define GAME_EFFECTS_EMISSION_CONTROLLER_H

#include "types.h"

class cPN_SAnimController;
class cPoseAccumulator;

class EmissionController
{
public:
    void SetPoseAccumulator(const cPoseAccumulator& poseAccumulator);
    void SetAnimController(const cPN_SAnimController& animController);

    /* 0x00 */ u8 unknown_0x00[0x40];
    /* 0x40 */ u16 m_aFacing;
    /* 0x42 */ u8 unknown_0x42[0x32];
    /* 0x74 */ u32 m_uUserData;
};

#endif // GAME_EFFECTS_EMISSION_CONTROLLER_H
