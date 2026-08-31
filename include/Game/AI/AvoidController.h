#ifndef GAME_AI_AVOID_CONTROLLER_H
#define GAME_AI_AVOID_CONTROLLER_H

#include "NL/nlAVLTree.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlTimer.h"
#include "types.h"
#include <math.h>
#include <string.h>

class DebugWriteCache;
class cFielder;
class cPlayer;
class DesireSteering;
class UnidentifiedAvoidanceObject;

enum eAvoidableThings
{
    AVOID_NOTHING = 0,
    AVOID_FIELDERS = 1,
    AVOID_GOALIES = 2,
    AVOID_POWERUPS = 4,
    AVOID_SIDELINES = 8,
    AVOID_BOWSER = 16,
    AVOID_UNIDENTIFIED_20 = 32,
    AVOID_UNIDENTIFIED_40 = 64,
    AVOID_EVERYTHING = 127,
    NUM_AVOIDABLES = 8,
};

class UnidentifiedAvoidanceHistoryBase
{
public:
    UnidentifiedAvoidanceHistoryBase()
    {
        mUnidentified004 = 0.3f;
        mUnidentified008 = (int)ceil(60.0f * mUnidentified004) + 2;
        mUnidentified014 = (nlVector3*)nlMalloc(
            mUnidentified008 * sizeof(nlVector3), 8, false);
        mUnidentified018 = (float*)nlMalloc(
            mUnidentified008 * sizeof(float), 8, false);
        memset(&mUnidentified02C, 0, sizeof(mUnidentified02C));
        mUnidentified00C = 0;
        mUnidentified010 = 0;
        mUnidentified01C = mUnidentified02C;
        mUnidentified028 = 0.0f;
    }
    virtual ~UnidentifiedAvoidanceHistoryBase();

    void UnidentifiedReset()
    {
        mUnidentified00C = 0;
        mUnidentified010 = 0;
        mUnidentified01C = mUnidentified02C;
        mUnidentified028 = 0.0f;
    }

protected:
    friend class DesireSteering;

    float mUnidentified004;
    int mUnidentified008;
    int mUnidentified00C;
    int mUnidentified010;
    nlVector3* mUnidentified014;
    float* mUnidentified018;
    nlVector3 mUnidentified01C;
    float mUnidentified028;
    nlVector3 mUnidentified02C;
};

class UnidentifiedAvoidanceHistory : public UnidentifiedAvoidanceHistoryBase
{
public:
    UnidentifiedAvoidanceHistory()
    {
    }
    virtual ~UnidentifiedAvoidanceHistory();
    virtual void UnidentifiedGetValue(
        nlVector3&, float, const nlVector3&) const;
};

struct UnidentifiedAvoidanceValue
{
    void* mUnidentified000;
    int mUnidentified004;
    int mUnidentified008;
    nlVector3 mUnidentified00C;
    float mUnidentified018;
    Timer mUnidentified01C;
    Timer mUnidentified024;
    UnidentifiedAvoidanceHistory mUnidentified02C;
};

struct UnidentifiedAvoidanceMemory
{
    UnidentifiedAvoidanceMemory();

    Timer mTimer;
    nlVector3 mRepulsion;
    float mUnidentified014;
};

typedef nlAVLTreeSlotPool<UnidentifiedAvoidanceObject*,
    UnidentifiedAvoidanceValue,
    DefaultKeyCompare<UnidentifiedAvoidanceObject*> >
    UnidentifiedAvoidanceTree;

class AvoidController
{
public:
    AvoidController(cFielder* fielder);
    ~AvoidController();

    void SetThingsToAvoid(int thingsToAvoid);
    void UseMinimumAvoidance(cPlayer*)
    {
        m_fRepulsionMult = 0.5f;
    }
    nlVector3& GetLastRepulsionVector(eAvoidableThings things);
    void Update(float fDeltaT);

    /* 0x000 */ cFielder* m_pFielder;
    /* 0x004 */ int m_ThingsToAvoid;
    /* 0x008 */ int m_CurrentlyAvoiding;
    /* 0x00C */ int mUnidentified00C;
    /* 0x010 */ float m_fRepulsionMult;
    /* 0x014 */ bool m_VeryCloseToSideline;
    /* 0x015 */ bool m_SidelineUnavoidable;
    /* 0x018 */ nlVector2 m_SidelineNormal;
    /* 0x020 */ nlVector2 m_SidelineDirection;
    /* 0x028 */ nlVector3 mUnidentified028;
    /* 0x034 */ nlVector3 m_LastRepulVec[NUM_AVOIDABLES];
    /* 0x094 */ float mUnidentified094[NUM_AVOIDABLES];
    /* 0x0B4 */ UnidentifiedAvoidanceMemory mUnidentified0B4[NUM_AVOIDABLES];
    /* 0x174 */ UnidentifiedAvoidanceTree mUnidentified174;
    /* 0x198 */ int mUnidentified198;
};

#endif // GAME_AI_AVOID_CONTROLLER_H
