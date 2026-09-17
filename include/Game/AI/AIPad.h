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
    int GetMaxRemoteAccelDelta(unsigned int count, nlVector3* deltaOut);
    int GetMaxFreestyleAccelDelta(unsigned int count, nlVector3* deltaOut);
    void ResetAccelerationHistory();

private:
    /* 0x000 */ u32 mUnidentified000;
    /* 0x004 */ nlVector3 mUnidentified004[30];
    /* 0x16C */ nlVector3 mUnidentified16C[30];
    /* 0x2D4 */ u32 mUnidentified2D4;
    /* 0x2D8 */ int mUnidentified2D8;

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

#endif // GAME_AI_AIPAD_H
