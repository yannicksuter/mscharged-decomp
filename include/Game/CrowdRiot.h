#ifndef GAME_CROWD_RIOT_H
#define GAME_CROWD_RIOT_H

#include "NL/nlMath.h"
#include "types.h"

class DebugWriteCache;
class EmissionController;
class PhysicsObject;

struct Generators
{
    /* 0x00 */ nlVector2 v2Location;
    /* 0x08 */ bool bIsOn;
    /* 0x0C */ float fTimeToExplode;
}; // total size: 0x10

class CrowdRiot
{
public:
    CrowdRiot(bool param1);
    ~CrowdRiot();

    void SyncLog(void* context, DebugWriteCache* cache);
    void fn_8002921C();
    void fn_80029320();
    void fn_80029460(bool param1);
    void fn_80029D78(EmissionController& controller);

    /* 0x00 */ float mfStateTime;
    /* 0x04 */ float mfRiotTime;
    /* 0x08 */ nlVector3 mv3Target;
    /* 0x14 */ nlVector3 mv3Position;
    /* 0x20 */ nlVector3 mv3Velocity;
    /* 0x2C */ u16 maDesiredFacingDirection;
    /* 0x30 */ PhysicsObject* mUnidentified30;
    /* 0x34 */ int meState;
}; // total size: 0x38

#endif // GAME_CROWD_RIOT_H
