#ifndef GAME_AI_DESIRE_RECEIVE_PASS_H
#define GAME_AI_DESIRE_RECEIVE_PASS_H

#include "Game/AI/Desire.h"
#include "Game/AI/UnidentifiedStringHash.h"

class DesireReceivePass;
class SpaceSearch;
struct LooseBallContactAnimInfo;

extern "C" void fn_800C22CC(DesireReceivePass*, cPlayer*, bool,
    bool, bool, const nlVector3*, float, float);
extern "C" void fn_800C0704(DesireReceivePass*);
extern "C" void fn_800C089C(DesireReceivePass*, bool);
extern "C" void fn_800C0AE8(
    DesireReceivePass*, bool, cPlayer*);

class DesireReceivePass : public Desire
{
public:
    DesireReceivePass();
    virtual ~DesireReceivePass();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

    bool CalcRoughEstimates(int);
    bool CalcExactEstimates(bool);
    bool StartPickupAnimation();

    bool UnidentifiedIsActive() const
    {
        return mUnidentifiedActive;
    }

    const nlVector3& GetAnimStartPosition() const
    {
        return mEstimated.v3AnimStartPos;
    }

private:
    struct Estimated
    {
        Estimated()
        {
            Reset();
        }

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
            mUnidentifiedAnimInfo = 0;
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
        const LooseBallContactAnimInfo* mUnidentifiedAnimInfo;
        float fAnimStartTime;
        int nReceivePassAnim;
        float fReceivePassAnimTime;
    };

    bool fn_800C0E54();
    bool fn_800C0E74();
    const LooseBallContactAnimInfo* fn_800C1FA4(
        int, int&);
    const LooseBallContactAnimInfo* fn_800C2048(
        const nlVector3&, const nlVector3&, nlVector3&,
        unsigned short, int);
    void fn_800C1A08();

    bool mbValidPassIntercept;
    nlVector3 mv3PassIntercept;
    float mUnidentifiedB4;
    int meReceiveAnimType;
    int meDesireSubState;
    SpaceSearch* m_pSpaceSearch;
    bool mbOneTouchVolley;
    bool mbOneTouchShot;
    bool mbOneTouchShotLate;
    bool mbOneTouchPass;
    cPlayer* mpOneTouchPassTarget;
    Estimated mEstimated;

    friend void fn_800C22CC(DesireReceivePass*, cPlayer*, bool,
        bool, bool, const nlVector3*, float, float);
    friend void fn_800C0704(DesireReceivePass*);
    friend void fn_800C089C(DesireReceivePass*, bool);
    friend void fn_800C0AE8(DesireReceivePass*, bool, cPlayer*);
};

#endif // GAME_AI_DESIRE_RECEIVE_PASS_H
