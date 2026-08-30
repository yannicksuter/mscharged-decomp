#ifndef GAME_SANIM_PN_SANIM_CONTROLLER_H
#define GAME_SANIM_PN_SANIM_CONTROLLER_H

#include "Game/PoseNode.h"
#include "Game/SAnim.h"
#include "Game/SAnim/AnimRetargeter.h"
#include "NL/nlSlotPool.h"

class cPN_SAnimController : public cPoseNode
{
public:
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
    void SetTime(float time)
    {
        m_fPrevTime = m_fTime;
        m_fTime = time;
        m_bLooped = false;
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
