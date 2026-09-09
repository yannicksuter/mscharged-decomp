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
class AvoidableObject;

enum eAvoidableThings
{
    AVOID_NOTHING = 0,
    AVOID_FIELDERS = 1,
    AVOID_POWERUPS = 2,
    AVOID_GOALIES = 4,
    AVOID_UNIDENTIFIED_08 = 8,
    AVOID_BOWSER = 16,
    AVOID_UNIDENTIFIED_20 = 32,
    AVOID_SIDELINES = 64,
    AVOID_EVERYTHING = 127,
    NUM_AVOIDABLES = 8,
};

class UnidentifiedAvoidanceHistoryBase
{
public:
    UnidentifiedAvoidanceHistoryBase(float duration = 0.3f)
    {
        mUnidentified004 = duration;
        int count = (int)(float)ceil(duration * 60.0f) + 2;
        mUnidentified014 = (nlVector3*)nlMalloc(
            count * sizeof(nlVector3), 8, false);
        mUnidentified018 = (float*)nlMalloc(
            count * sizeof(float), 8, false);
        mUnidentified008 = count;
        memset(&mUnidentified02C, 0, sizeof(mUnidentified02C));
        mUnidentified00C = 0;
        mUnidentified010 = 0;
        mUnidentified01C = mUnidentified02C;
        mUnidentified028 = 0.0f;
    }
    virtual ~UnidentifiedAvoidanceHistoryBase()
    {
        delete[] mUnidentified014;
        delete[] mUnidentified018;
    }
    virtual void UnidentifiedGetValue(
        nlVector3&, float, const nlVector3&) const = 0;

    void UnidentifiedReset()
    {
        mUnidentified00C = 0;
        mUnidentified010 = 0;
        mUnidentified01C = mUnidentified02C;
        mUnidentified028 = 0.0f;
    }

    const nlVector3& UnidentifiedLast() const
    {
        return mUnidentified00C != mUnidentified010
            ? mUnidentified014[mUnidentified00C - 1 >= 0
                  ? mUnidentified00C - 1 : mUnidentified008 - 1]
            : mUnidentified02C;
    }

    void UnidentifiedRemoveOldest()
    {
        nlVector3 value;
        nlVec3Scale(value, mUnidentified014[mUnidentified010],
            mUnidentified018[mUnidentified010]);
        nlVec3Sub(mUnidentified01C, mUnidentified01C, value);
        mUnidentified028 -= mUnidentified018[mUnidentified010];
        mUnidentified010 = (mUnidentified010 + 1) % mUnidentified008;
    }

    bool UnidentifiedCanTrim()
    {
        return mUnidentified028 - mUnidentified018[mUnidentified010] > mUnidentified004;
    }

    void UnidentifiedAdd(const nlVector3& value, float dt)
    {
        mUnidentified014[mUnidentified00C] = value;
        mUnidentified018[mUnidentified00C] = dt;
        nlVec3ScaleAdd(mUnidentified01C, dt, value, mUnidentified01C);
        mUnidentified028 += dt;
        mUnidentified00C = (mUnidentified00C + 1) % mUnidentified008;
        if (mUnidentified00C == mUnidentified010)
            UnidentifiedRemoveOldest();
        while (UnidentifiedCanTrim() && mUnidentified00C != mUnidentified010)
            UnidentifiedRemoveOldest();
    }

    void Update(nlVector3& value, const nlVector3& sample, float dt)
    {
        UnidentifiedAdd(sample, dt);
        if (mUnidentified00C != mUnidentified010)
        {
            nlVector3 input = mUnidentified01C;
            float magnitude = mUnidentified028;
            float excess = mUnidentified028 - mUnidentified004;
            float weight = mUnidentified018[mUnidentified010];
            if ((mUnidentified010 + 1) % mUnidentified008 != mUnidentified00C
                && excess > 0.0f)
            {
                float fraction = excess / weight;
                nlVec3ScaleAdd(input, fraction * -weight,
                    mUnidentified014[mUnidentified010], input);
                magnitude -= fraction * mUnidentified018[mUnidentified010];
            }
            UnidentifiedGetValue(value, magnitude, input);
        }
        else
            value = mUnidentified02C;
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
    UnidentifiedAvoidanceHistory(float duration = 0.3f)
        : UnidentifiedAvoidanceHistoryBase(duration)
    {
    }
    virtual ~UnidentifiedAvoidanceHistory()
    {
    }
    virtual void UnidentifiedGetValue(
        nlVector3& value, float magnitude, const nlVector3& input) const
    {
        if (magnitude > 0.0001f)
        {
            nlVec3Scale(value, input, 1.0f / magnitude);
        }
        else
        {
            value = mUnidentified00C != mUnidentified010
                ? mUnidentified014[mUnidentified00C - 1 >= 0
                      ? mUnidentified00C - 1 : mUnidentified008 - 1]
                : mUnidentified02C;
        }
    }
};

struct UnidentifiedAvoidanceContext
{
    void UnidentifiedNormalize();
    nlVector3 mUnidentified000;
    float mUnidentified00C;
    float mUnidentified010;
    int mUnidentified014;
    float mUnidentified018;
    float mUnidentified01C;
    nlVector3 mUnidentified020;
    nlVector3 mUnidentified02C;
    nlVector3 mUnidentified038;
    nlVector3 mUnidentified044;
    float mUnidentified050;
    nlVector3 mUnidentified054;
};

struct UnidentifiedAvoidanceValue
{
    UnidentifiedAvoidanceValue();
    void UnidentifiedTurn(nlVector3&, const nlVector3&, const nlVector3&, bool);
    bool UnidentifiedResponse_800127E0(int, UnidentifiedAvoidanceContext&, float);
    bool UnidentifiedResponse_800123D8(UnidentifiedAvoidanceContext&, float);
    bool UnidentifiedResponse_800121D0(UnidentifiedAvoidanceContext&, float);
    bool UnidentifiedMovingResponse(UnidentifiedAvoidanceContext&, float);
    void UnidentifiedPrepareContext(UnidentifiedAvoidanceContext&, float);
    void Update(float fDeltaT);
    void UnidentifiedInitialize(AvoidableObject*, AvoidableObject*);
    float UnidentifiedGetWeight() const;

    UnidentifiedAvoidanceValue* next;
    AvoidableObject* mUnidentified004;
    AvoidableObject* mUnidentified008;
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

typedef nlAVLTreeSlotPool<u32,
    UnidentifiedAvoidanceValue,
    DefaultKeyCompare<u32> >
    UnidentifiedAvoidanceTree;

struct sCornerSegment;

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
    bool AvoidSidelines(nlVector3&);
    bool CalcDesiredVelocityToAvoidCorner(nlVector2&, const sCornerSegment&, const nlVector2&, const nlVector2&);
    bool CalcDesiredVelocityToAvoidSideline(nlVector2&, const nlVector2&, const nlVector2&, const nlVector2&, const nlVector2&);
    void ApplyRepulsionVector(nlVector3 v3Repulsion);
    bool UnidentifiedCanAvoid(int);
    void UnidentifiedSetLast(eAvoidableThings, const nlVector3&, float);

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

class DebugWriteCache;
extern "C" void fn_8000F324(AvoidController* controller,
    void* context, DebugWriteCache* cache);

#endif // GAME_AI_AVOID_CONTROLLER_H
