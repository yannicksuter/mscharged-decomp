#ifndef GAME_AI_DESIRE_RECEIVE_PASS_H
#define GAME_AI_DESIRE_RECEIVE_PASS_H

#include "Game/AI/Desire.h"
#include "Game/AI/TransitionFunc.h"
#include "Game/Player.h"

class DesireReceivePass;
class SpaceSearch;
struct LooseBallContactAnimInfo;

class DesireReceivePass : public Desire
{
public:
    DesireReceivePass();

    virtual bool Initialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

    void ProcessUserInput();
    void RequestOneTouchShot(bool);
    void RequestOneTouchPass(bool, cPlayer*);
    void ExecutePass(cPlayer*, bool, bool, bool, const nlVector3*, float, float);
    void SetPassTransitionTimer();
    bool IsGroundReceive() { return !IsVolleyReceive(); }
    bool IsVolleyReceive();
    bool CalcRoughEstimates(int);
    bool CalcExactEstimates(bool);
    bool StartPickupAnimation();

    bool IsActive() const
    {
        return mActive;
    }

    bool IsOneTouchShot() const
    {
        return mbOneTouchShot;
    }

    const nlVector3& GetAnimStartPosition() const
    {
        return mEstimated.v3AnimStartPos;
    }

private:
    struct Estimated
    {
        void Reset()
        {
            bLocked = false;
            v3BallContactPos.x = 0.0f;
            v3BallContactPos.y = 0.0f;
            v3BallContactPos.z = 0.0f;
            v3AnimStartPos.x = 0.0f;
            v3AnimStartPos.y = 0.0f;
            v3AnimStartPos.z = 0.0f;
            aFacingDirection = 0;
            aFacingTargetDirection = 0;
            fBallContactTime = -1.0f;
            pAnimInfo = 0;
            fAnimStartTime = 0.0f;
            nReceivePassAnim = 0;
            fReceivePassAnimTime = 0.0f;
        }

        bool bLocked;
        nlVector3 v3BallContactPos;
        nlVector3 v3AnimStartPos;
        unsigned short aFacingDirection;
        unsigned short aFacingTargetDirection;
        float fBallContactTime;
        float fAnimStartOffset;
        const LooseBallContactAnimInfo* pAnimInfo;
        float fAnimStartTime;
        int nReceivePassAnim;
        float fReceivePassAnimTime;
    };

    static unsigned short sDesireReceivePassType;

    static int AddVolleyReceiveFlag(int, bool);
    bool CanRequestOneTouch();
    float GetBallContactHeight(int);
    bool fn_800C0E74();
    const LooseBallContactAnimInfo* GetContactAnimInfoList(
        int, int&);
    const LooseBallContactAnimInfo* FindBestContactAnimInfo(
        const nlVector3&, const nlVector3&, nlVector3&,
        unsigned short, int);
    void SelectContactAnimation();
    void FindPassPosition(cPlayer*, bool, bool, float, nlVector3&, float*);

    bool mbValidPassIntercept;
    nlVector3 mv3PassIntercept;
    float mfInitialBallSpeed;
    int meReceiveAnimType;

public:
    int meDesireSubState;

private:
    SpaceSearch* m_pSpaceSearch;
    bool mbOneTouchVolley;
    bool mbOneTouchShot;
    bool mbOneTouchShotLate;
    bool mbOneTouchPass;
    cPlayer* mpOneTouchPassTarget;
    Estimated mEstimated;
};

inline void DesireReceivePass::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field =
        cache->BeginType("DesireReceivePass");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->AddField(16, gDebugFieldTypes[16].size,
        (u8*)&mbValidPassIntercept - (u8*)&mvDesiredPosition,
        "mbValidPassIntercept");
    cache->AddField(22, gDebugFieldTypes[22].size,
        (u8*)&mv3PassIntercept - (u8*)&mvDesiredPosition,
        "mv3PassIntercept");
    cache->AddField(14, gDebugFieldTypes[14].size,
        (u8*)&meReceiveAnimType - (u8*)&mvDesiredPosition,
        "meReceiveAnimType");
    cache->AddField(14, gDebugFieldTypes[14].size,
        (u8*)&meDesireSubState - (u8*)&mvDesiredPosition,
        "meDesireSubState");
    cache->AddField(16, gDebugFieldTypes[16].size,
        (u8*)&mbOneTouchVolley - (u8*)&mvDesiredPosition,
        "mbOneTouchVolley");
    cache->AddField(16, gDebugFieldTypes[16].size,
        (u8*)&mbOneTouchShot - (u8*)&mvDesiredPosition,
        "mbOneTouchShot");
    cache->AddField(16, gDebugFieldTypes[16].size,
        (u8*)&mbOneTouchShotLate - (u8*)&mvDesiredPosition,
        "mbOneTouchShotLate");
    cache->AddField(16, gDebugFieldTypes[16].size,
        (u8*)&mbOneTouchPass - (u8*)&mvDesiredPosition,
        "mbOneTouchPass");
    cache->AddField(15, gDebugFieldTypes[15].size,
        (u8*)&mpOneTouchPassTarget - (u8*)&mvDesiredPosition,
        "mpOneTouchPassTarget");
    cache->AddField(16, gDebugFieldTypes[16].size,
        (u8*)&mEstimated.bLocked - (u8*)&mvDesiredPosition,
        "mEstimated.bLocked");
    cache->AddField(22, gDebugFieldTypes[22].size,
        (u8*)&mEstimated.v3BallContactPos - (u8*)&mvDesiredPosition,
        "mEstimated.v3BallContactPos");
    cache->AddField(22, gDebugFieldTypes[22].size,
        (u8*)&mEstimated.v3AnimStartPos - (u8*)&mvDesiredPosition,
        "mEstimated.v3AnimStartPos");
    cache->AddField(19, gDebugFieldTypes[19].size,
        (u8*)&mEstimated.aFacingDirection - (u8*)&mvDesiredPosition,
        "mEstimated.aFacingDirection");
    cache->AddField(19, gDebugFieldTypes[19].size,
        (u8*)&mEstimated.aFacingTargetDirection - (u8*)&mvDesiredPosition,
        "mEstimated.aFacingTargetDirection");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&mEstimated.fBallContactTime - (u8*)&mvDesiredPosition,
        "mEstimated.fBallContactTime");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&mEstimated.fAnimStartOffset - (u8*)&mvDesiredPosition,
        "mEstimated.fAnimStartOffset");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&mEstimated.fAnimStartTime - (u8*)&mvDesiredPosition,
        "mEstimated.fAnimStartTime");
    cache->AddField(8, gDebugFieldTypes[8].size,
        (u8*)&mEstimated.nReceivePassAnim - (u8*)&mvDesiredPosition,
        "mEstimated.nReceivePassAnim");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&mEstimated.fReceivePassAnimTime - (u8*)&mvDesiredPosition,
        "mEstimated.fReceivePassAnimTime");
    cache->EndType();
}

inline void DesireReceivePass::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireReceivePassType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireReceivePassType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = cache->WriteData(sDesireReceivePassType,
        (u8*)this + offset, sizeof(DesireReceivePass) - offset);
    if (data != 0)
    {
        DesireReceivePass* desire =
            (DesireReceivePass*)((u8*)data - offset);
        desire->mpOneTouchPassTarget =
            (cPlayer*)(mpOneTouchPassTarget == 0
                    ? -1
                    : mpOneTouchPassTarget->mUnidentified120);
        cache->ChecksumData(sDesireReceivePassType, data, context);
    }
}

#endif // GAME_AI_DESIRE_RECEIVE_PASS_H
