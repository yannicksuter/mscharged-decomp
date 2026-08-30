#ifndef GAME_AI_AVOID_CONTROLLER_H
#define GAME_AI_AVOID_CONTROLLER_H

#include "NL/nlAVLTree.h"
#include "NL/nlMath.h"
#include "NL/nlTimer.h"
#include "types.h"

class DebugWriteCache;
class cFielder;
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
    UnidentifiedAvoidanceHistoryBase();
    virtual ~UnidentifiedAvoidanceHistoryBase();

protected:
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
    UnidentifiedAvoidanceHistory();
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
