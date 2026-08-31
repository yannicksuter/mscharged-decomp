#ifndef GAME_FIXED_UPDATE_TASK_H
#define GAME_FIXED_UPDATE_TASK_H

#include "Game/Task/DispatchEventsTask.h"
#include "NL/nlTask.h"

class UnidentifiedFixedUpdateTaskBase
{
public:
    virtual void UnidentifiedVirtual00();
    virtual void UnidentifiedVirtual04();
    virtual void UnidentifiedVirtual08();
    virtual void UnidentifiedVirtual0C();
    virtual void UnidentifiedVirtual10();
    virtual void UnidentifiedVirtual14();
    virtual void UnidentifiedVirtual18();
    virtual void UnidentifiedVirtual1C();
    virtual void UnidentifiedVirtual20();
    virtual void UnidentifiedVirtual24();
    virtual void UnidentifiedVirtual28();
    virtual void UnidentifiedVirtual2C();
    virtual void UnidentifiedVirtual30();
    virtual int UnidentifiedVirtual34();
    virtual void UnidentifiedVirtual38();
    virtual void UnidentifiedVirtual3C();
};

class FixedUpdateTask : public nlTask, public UnidentifiedFixedUpdateTaskBase
{
public:
    virtual void Run(float dt);
    virtual const char* GetName();

    static float GetPhysicsUpdateTick();

    /* 0x24 */ float mAccumulatedDeltaT;
    /* 0x28 */ float mUnidentified28;
    /* 0x2C */ float mSimulationTime;
    /* 0x30 */ float mfFrameLockTime;
    /* 0x34 */ unsigned int mUnidentified34;
    /* 0x38 */ bool mUnidentified38;
    /* 0x3C */ EventDispatcher mEventDispatcher;
};

#endif // GAME_FIXED_UPDATE_TASK_H
