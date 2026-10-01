#ifndef GAME_SANIM_PN_SANIM_CONTROLLER_H
#define GAME_SANIM_PN_SANIM_CONTROLLER_H

#include "Game/PoseNode.h"
#include "Game/SAnim.h"
#include "Game/SAnim/AnimRetargeter.h"
#include "NL/nlSlotPool.h"

class cPN_SAnimController : public cPoseNode
{
public:
    cPN_SAnimController()
        : cPoseNode(0)
        , m_pSAnim(NULL)
        , m_fTime(0.0f)
    {
    }
    cPN_SAnimController(cSAnim* anim, const AnimRetarget* retarget,
        ePlayMode playMode,
        void (*playbackSpeedCallback)(unsigned int, cPN_SAnimController*),
        unsigned int playbackSpeedCallbackParam, bool mirror);
    virtual ~cPN_SAnimController()
    {
    }
    virtual void Evaluate(
        int nodeIndex, float weight, cPoseAccumulator* accumulator) const;
    virtual void Evaluate(float weight, cPoseAccumulator* accumulator) const;
    virtual void EvaluateScale(
        int nodeIndex, float weight, cPoseAccumulator* accumulator) const;
    virtual cPoseNode* Update(float dt);
    virtual int GetType()
    {
        return 2;
    }
    virtual void BlendRootTrans(
        nlVector3* rootTranslation, float weight, float* accumulatedWeight);
    virtual void BlendRootRot(
        u16* rootRotation, float weight, float* accumulatedWeight);

    template <typename T>
    void Replay(T& frame)
    {
        Replayable<0>(frame, (cPoseNode&)*this);
        Replayable<0>(frame, FloatCompressor<0, 1, 15>(m_fTime));

        unsigned int animPtr = 0;
        if (!ReplayFrameTraits<T>::IsLoadFrame)
        {
            animPtr = (unsigned int)m_pSAnim;
            if (m_bMirror)
                animPtr |= 1;
        }
        Replayable<0>(frame, animPtr);
        if (ReplayFrameTraits<T>::IsLoadFrame)
        {
            m_bMirror = animPtr & 1;
            m_pSAnim = (cSAnim*)(animPtr & ~1);
        }
        Replayable<0>(frame, (unsigned int&)m_pAnimRetarget);
    }

    static void* operator new(unsigned long)
    {
        cPN_SAnimController* controller = 0;
        m_SAnimControllerSlotPool.Allocate(controller);
        return controller;
    }

    static void operator delete(void* pointer)
    {
        m_SAnimControllerSlotPool.Free((cPN_SAnimController*)pointer);
    }

    void UpdateSynchronized(float time, bool looped);
    // Bounded fit, not recovered source: at both mirror stores of R4QE01's
    // FEModelHandle::PlayAnimation the controller pointer is allocated ahead
    // of the handle's flag byte. GC/3.0a5 evaluates the value of a bit-field
    // assignment before its address, so the pointer was bound first - as an
    // inline function's receiver, since the impostor branch evaluates it
    // again for SetTime - and a conditional on the flag kept that binding out
    // of the store's own flow node. The function's owner, name and exact
    // conditional are not recoverable: PlayAnimation is the only consumer and
    // the DOL keeps no out-of-line copy. It sits beside SetTime, with the
    // field it writes.
    void SetMirror(bool mirror)
    {
        m_bMirror = mirror ? true : false;
    }
    void SetTime(float time)
    {
        m_fPrevTime = m_fTime;
        m_fTime = time;
        m_bLooped = false;
    }
    bool UnidentifiedAtEnd() const
    {
        return m_ePlayMode == PM_HOLD && m_fTime == 1.0f;
    }
    void ProcessCallbacks();
    bool TestTrigger(float time) const;
    bool TestFrameTrigger(float frame);
    int RemapNode(int nodeIndex) const;

    inline const float get_fTime() const
    {
        return m_fTime;
    }

    cSAnim* m_pSAnim;
    float m_fTime;
    const AnimRetarget* m_pAnimRetarget;
    float m_fPrevTime;
    ePlayMode m_ePlayMode;
    mutable float m_fWeight;
    unsigned int m_bMirror : 1;
    unsigned int m_bIsSynchronized : 1;
    unsigned int m_bIgnoreTriggers : 1;
    mutable unsigned int m_bNegativeTriggerProcessed : 1;
    unsigned int m_bLooped : 1;
    void (*m_pPlaybackSpeedCallback)(unsigned int, cPN_SAnimController*);
    unsigned int m_nPlaybackSpeedCallbackParam;
    float m_fPlaybackSpeedScale;
    cPN_SAnimController* m_pSynchronizedController;
    void (*m_pSynchronizedWeightCallback)(
        unsigned int, cPN_SAnimController*);
    unsigned int m_nSynchronizedWeightCallbackParam;
    float m_fSynchronizedWeight;

    static SlotPool<cPN_SAnimController> m_SAnimControllerSlotPool;
};

#endif // GAME_SANIM_PN_SANIM_CONTROLLER_H
