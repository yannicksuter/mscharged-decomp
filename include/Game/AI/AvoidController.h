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
    AVOID_POLYGONS = 8,
    AVOID_BOWSER = 16,
    AVOID_PATCHES = 32,
    AVOID_SIDELINES = 64,
    AVOID_EVERYTHING = 127,
    NUM_AVOIDABLES = 8,
};

class VectorHistoryBase
{
public:
    VectorHistoryBase(float duration, float sampleRate)
    {
        mWindowDuration = duration;
        int count = (int)(float)ceil(duration * sampleRate) + 2;
        mpSamples = (nlVector3*)nlMalloc(
            count * sizeof(nlVector3), 8, false);
        mpSampleDurations = (float*)nlMalloc(
            count * sizeof(float), 8, false);
        mCapacity = count;
        memset(&mZero, 0, sizeof(mZero));
        mWriteIndex = 0;
        mOldestIndex = 0;
        mWeightedSum = mZero;
        mTotalDuration = 0.0f;
    }
    virtual ~VectorHistoryBase()
    {
        delete[] mpSamples;
        delete[] mpSampleDurations;
    }
    virtual void Evaluate(
        nlVector3&, float, const nlVector3&) const = 0;

    void Reset()
    {
        mWriteIndex = 0;
        mOldestIndex = 0;
        mWeightedSum = mZero;
        mTotalDuration = 0.0f;
    }

    const nlVector3& GetLastSample() const
    {
        return mWriteIndex != mOldestIndex
            ? mpSamples[mWriteIndex - 1 >= 0
                  ? mWriteIndex - 1 : mCapacity - 1]
            : mZero;
    }

    void RemoveOldest()
    {
        nlVector3 value;
        nlVec3Scale(value, mpSamples[mOldestIndex],
            mpSampleDurations[mOldestIndex]);
        nlVec3Sub(mWeightedSum, mWeightedSum, value);
        mTotalDuration -= mpSampleDurations[mOldestIndex];
        mOldestIndex = (mOldestIndex + 1) % mCapacity;
    }

    bool CanRemoveOldest()
    {
        return mTotalDuration - mpSampleDurations[mOldestIndex] > mWindowDuration;
    }

    void AdvanceWriteIndex()
    {
        mWriteIndex = (mWriteIndex + 1) % mCapacity;
        if (mWriteIndex == mOldestIndex)
            RemoveOldest();
    }

    void TrimToDuration()
    {
        while (CanRemoveOldest() && mWriteIndex != mOldestIndex)
            RemoveOldest();
    }

    void Update(nlVector3& value, const nlVector3& sample, float dt,
        float decayDuration = 0.0f, const Timer* timer = 0)
    {
        nlVector3 faded;
        const nlVector3* pSample = &sample;
        if (decayDuration > 0.0f)
        {
            faded = sample;
            float scale = timer->GetSeconds() / decayDuration;
            nlVec3Set(faded, scale * faded.x, scale * faded.y, scale * faded.z);
            pSample = &faded;
        }

        mpSamples[mWriteIndex] = *pSample;
        mpSampleDurations[mWriteIndex] = dt;
        nlVec3ScaleAdd(mWeightedSum, dt, *pSample, mWeightedSum);
        mTotalDuration += dt;
        AdvanceWriteIndex();
        TrimToDuration();
        if (mWriteIndex != mOldestIndex)
        {
            nlVector3 input = mWeightedSum;
            int next = (mOldestIndex + 1) % mCapacity;
            float magnitude = mTotalDuration;
            float excess = mTotalDuration - mWindowDuration;
            float weight = mpSampleDurations[mOldestIndex];
            if (next != mWriteIndex && excess > 0.0f)
            {
                float fraction = excess / weight;
                nlVec3ScaleAdd(input, fraction * -weight,
                    mpSamples[mOldestIndex], input);
                magnitude -= fraction * mpSampleDurations[mOldestIndex];
            }
            Evaluate(value, magnitude, input);
        }
        else
            value = mZero;
    }

protected:
    friend class DesireSteering;

    float mWindowDuration;
    int mCapacity;
    int mWriteIndex;
    int mOldestIndex;
    nlVector3* mpSamples;
    float* mpSampleDurations;
    nlVector3 mWeightedSum;
    float mTotalDuration;
    nlVector3 mZero;
};

class VectorAverageHistory : public VectorHistoryBase
{
public:
    VectorAverageHistory(float duration = 0.3f)
        : VectorHistoryBase(duration, 60.0f)
    {
    }
    virtual ~VectorAverageHistory()
    {
    }
    virtual void Evaluate(
        nlVector3& value, float magnitude, const nlVector3& input) const;
};

struct AvoidanceContext
{
    void NormalizeRepulsion();
    float GetNormalDotDesiredDir() const
    {
        return (mObstacleNormal.x * mDesiredDir.x)
            + (mObstacleNormal.y * mDesiredDir.y)
            + (mObstacleNormal.z * mDesiredDir.z);
    }
    nlVector3 mRepulsionDir;
    float mRepulsionMag;
    float mProximity;
    int mContactState;
    float mGap;
    float mClosingSpeed;
    nlVector3 mAvoiderPoint;
    nlVector3 mObstaclePoint;
    nlVector3 mAvoiderNormal;
    nlVector3 mObstacleNormal;
    float mDesiredSpeed;
    nlVector3 mDesiredDir;
};

struct ObstacleAvoidance
{
    ObstacleAvoidance();
    void CalcPerpendicularToward(nlVector3&, const nlVector3&, const nlVector3&, bool);
    bool OverlapResponse(int, AvoidanceContext&, float);
    bool StaticObstacleResponse(AvoidanceContext&, float);
    bool MobileObstacleResponse(AvoidanceContext&, float);
    bool AgentResponse(AvoidanceContext&, float);
    void CalcContext(AvoidanceContext&, float);
    void Update(float fDeltaT);
    void Initialize(AvoidableObject*, AvoidableObject*);
    float GetWeight() const;

    ObstacleAvoidance* next;
    AvoidableObject* mpAvoider;
    AvoidableObject* mpObstacle;
    nlVector3 mRepulsion;
    float mWeight;
    Timer mActiveTimer;
    Timer mFadeOutTimer;
    VectorAverageHistory mRepulsionHistory;
};

struct AvoidanceMemory
{
    AvoidanceMemory();

    Timer mTimer;
    nlVector3 mRepulsion;
    float m_pad014;
};

typedef nlAVLTreeSlotPool<u32,
    ObstacleAvoidance,
    DefaultKeyCompare<u32> >
    ObstacleAvoidanceTree;

struct sCornerSegment;
struct sSideLinePlane;

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
    bool CalcDesiredVelocityToAvoidSideline(nlVector2&, const sSideLinePlane&, const nlVector2&, const nlVector2&);
    bool CalcDesiredVelocityToAvoidSideline(nlVector2&, const nlVector2&, const nlVector2&, const nlVector2&, const nlVector2&);
    void ApplyRepulsionVector(nlVector3 v3Repulsion);
    void RegisterDebugFields(u16*, DebugWriteCache*);
    bool IsAvoiding(int);
    void SetLastRepulsionVector(eAvoidableThings, const nlVector3&, float);

    /* 0x000 */ cFielder* m_pFielder;
    /* 0x004 */ int m_ThingsToAvoid;
    /* 0x008 */ int m_CurrentlyAvoiding;
    /* 0x00C */ int m_pad00C;
    /* 0x010 */ float m_fRepulsionMult;
    /* 0x014 */ bool m_VeryCloseToSideline;
    /* 0x015 */ bool m_SidelineUnavoidable;
    /* 0x018 */ nlVector2 m_SidelineNormal;
    /* 0x020 */ nlVector2 m_SidelineDirection;
    /* 0x028 */ nlVector3 m_SidelineRepulsion;
    /* 0x034 */ nlVector3 m_LastRepulVec[NUM_AVOIDABLES];
    /* 0x094 */ float m_LastRepulWeight[NUM_AVOIDABLES];
    /* 0x0B4 */ AvoidanceMemory m_AvoidanceMemory[NUM_AVOIDABLES];
    /* 0x174 */ ObstacleAvoidanceTree m_Avoidances;
    /* 0x198 */ int m_NumAvoidances;
};

class DebugWriteCache;
extern "C" void fn_8000F324(AvoidController* controller,
    void* context, DebugWriteCache* cache);

inline void VectorAverageHistory::Evaluate(
    nlVector3& value, float magnitude, const nlVector3& input) const
{
    if (magnitude > 0.0001f)
    {
        nlVec3Scale(value, input, 1.0f / magnitude);
    }
    else
    {
        value = mWriteIndex != mOldestIndex
            ? mpSamples[mWriteIndex - 1 >= 0
                  ? mWriteIndex - 1 : mCapacity - 1]
            : mZero;
    }
}


extern "C" void fn_8000F178(AvoidController* controller);
extern "C" float fn_8000F558( AvoidController* controller, eAvoidableThings things);
extern "C" void RemoveFromAvoidControllers(AvoidableObject* pObject);

#endif // GAME_AI_AVOID_CONTROLLER_H
