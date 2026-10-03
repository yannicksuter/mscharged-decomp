#ifndef GAME_AI_AIPAD_H
#define GAME_AI_AIPAD_H

#include "types.h"
#include "Game/DetInput.h"

class cAIPad
{
    friend struct AIPadManager;

public:
    cAIPad();

    u16 GetMovementStickDirection();
    float GetMovementStickMagnitude();
    u16 GetCStickMovementStickDirection();
    float GetCStickMovementStickMagnitude();
    bool IsWiiController() const;
    bool DetectLeftShake(u16* direction);
    bool DetectRightShake(u16* direction);
    // Returns the age of the sample with the largest delta, or zero if none.
    int GetMaxRemoteAccelDelta(unsigned int requestedSamples, nlVector3* deltaOut);
    int GetMaxFreestyleAccelDelta(unsigned int requestedSamples, nlVector3* deltaOut);
    void ResetAccelerationHistory();

private:
    /* 0x000 */ u32 mUnidentified000;
    /* 0x004 */ nlVector3 mRemoteAccelerationHistory[30];
    /* 0x16C */ nlVector3 mFreestyleAccelerationHistory[30];
    /* 0x2D4 */ u32 mAccelerationHistoryIndex;
    /* 0x2D8 */ int mLocalControllerIndex;

public:
    /* 0x2DC */ DetInput* m_pGlobalPad;
}; // total size: 0x2E0

struct AIPadManager
{
    static void Startup();
    static void UpdateAccelerationHistory();
    static cAIPad mAIPads[16];
};

cAIPad* GetAIPad(int index);

void StartupAIPads();

extern float g_fAccelerationHistoryBlend;

#endif // GAME_AI_AIPAD_H
