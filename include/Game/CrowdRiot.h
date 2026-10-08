#ifndef GAME_CROWD_RIOT_H
#define GAME_CROWD_RIOT_H

#include "NL/nlMath.h"
#include "types.h"

class DebugWriteCache;
class EmissionController;
class PhysicsTriggerVolume;

class CrowdRiot
{
public:
    enum State
    {
        STATE_DISABLED = 0,
        STATE_READY = 1,
        STATE_ACTIVE = 2,
        STATE_EXITING = 4
    };

    CrowdRiot(bool enableRiot);
    ~CrowdRiot();

    void RegisterDebugFields(u16* type, DebugWriteCache* cache);
    void SyncLog(void* context, DebugWriteCache* cache);
    void ResetGenerators();
    void InitializeRiotMotion();
    void Reset(bool preserveActiveRiot);
    void UpdateEmissionPosition(EmissionController& controller);

    /* 0x00 */ float mfStateTime;
    /* 0x04 */ float mfRiotTime;
    /* 0x08 */ nlVector3 mv3Target;
    /* 0x14 */ nlVector3 mv3Position;
    /* 0x20 */ nlVector3 mv3Velocity;
    /* 0x2C */ u16 maDesiredFacingDirection;
    /* 0x30 */ PhysicsTriggerVolume* mpTriggerVolume;
    /* 0x34 */ State meState;
}; // total size: 0x38

#endif // GAME_CROWD_RIOT_H
