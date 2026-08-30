#include "Game/SAnim/pnSAnimController.h"
#include "Game/SHierarchy.h"

SlotPool<cPN_SAnimController>
    cPN_SAnimController::m_SAnimControllerSlotPool(16, 16);

int cPN_SAnimController::RemapNode(int nodeIndex) const
{
    if (m_pAnimRetarget != 0)
    {
        return m_pAnimRetarget->m_pMap[nodeIndex];
    }
    return nodeIndex;
}
bool cPN_SAnimController::TestFrameTrigger(float frame)
{
    return TestTrigger(frame / (float)m_pSAnim->m_nNumKeys);
}

bool cPN_SAnimController::TestTrigger(float time) const
{
    if (time == 0.0f)
    {
        time = 1.17549435e-38f;
    }
    else if (time < 0.0f)
    {
        if (m_bNegativeTriggerProcessed)
        {
            return false;
        }
        m_bNegativeTriggerProcessed = true;
        return true;
    }

    if (m_ePlayMode != PM_PING_PONG)
    {
        if (m_fPlaybackSpeedScale >= 0.0f)
        {
            if (m_bLooped)
            {
                return time <= m_fTime || time > m_fPrevTime;
            }
            return time > m_fPrevTime && time <= m_fTime;
        }

        if (m_bLooped)
        {
            return time < m_fPrevTime || time >= m_fTime;
        }
        return time >= m_fTime && time < m_fPrevTime;
    }

    return time > m_fPrevTime && time <= m_fTime;
}

void cPN_SAnimController::ProcessCallbacks()
{
    if (m_bIgnoreTriggers)
    {
        return;
    }

    cSAnimCallback* callback = m_pSAnim->m_pCallbackList;
    while (callback != 0)
    {
        if (TestTrigger(callback->m_fTime))
        {
            callback->m_funcCallback(m_pSAnim, callback->m_nParam1);
        }
        callback = callback->next;
    }
}

void cPN_SAnimController::BlendRootRot(
    u16* rootRotation, float weight, float* accumulatedWeight)
{
    u16 deltaRotation;

    if (m_bLooped)
    {
        u16 previousRotation;
        u16 endRotation;
        u16 startRotation;
        u16 currentRotation;
        m_pSAnim->GetRootRot(m_fPrevTime, &previousRotation);
        m_pSAnim->GetRootRot(1.0f, &endRotation);
        m_pSAnim->GetRootRot(0.0f, &startRotation);
        m_pSAnim->GetRootRot(m_fTime, &currentRotation);

        if (m_fPlaybackSpeedScale >= 0.0f)
        {
            deltaRotation = (endRotation - previousRotation)
                          + (currentRotation - startRotation);
        }
        else
        {
            deltaRotation = (startRotation - previousRotation)
                          + (currentRotation - endRotation);
        }
    }
    else
    {
        u16 previousRotation;
        u16 currentRotation;
        m_pSAnim->GetRootRot(m_fTime, &currentRotation);
        m_pSAnim->GetRootRot(m_fPrevTime, &previousRotation);
        deltaRotation = currentRotation - previousRotation;
    }

    if (m_bMirror)
    {
        deltaRotation = -deltaRotation;
    }

    *accumulatedWeight += weight;
    if (*accumulatedWeight != 0.0f)
    {
        float blend = weight / *accumulatedWeight;
        s16 difference = deltaRotation - *rootRotation;
        *rootRotation += (int)(blend * difference);
    }
}

static inline void GetRootTransDelta(cPN_SAnimController* controller,
    nlVector3* rootTranslation, float startTime, float endTime)
{
    nlVector3 previousTranslation;

    controller->m_pSAnim->GetRootTrans(endTime, rootTranslation);
    controller->m_pSAnim->GetRootTrans(startTime, &previousTranslation);

    rootTranslation->x -= previousTranslation.x;
    rootTranslation->y -= previousTranslation.y;
    rootTranslation->z -= previousTranslation.z;
}

void cPN_SAnimController::BlendRootTrans(
    nlVector3* rootTranslation, float weight, float* accumulatedWeight)
{
    u16 previousFacing;
    nlVector3 delta;
    nlVector3 localDelta;
    nlVector3* localDeltaPointer;
    u16 mirrorAdjustment;
    nlVector3 wrappedDelta;
    float inverseBlend;
    float blend;

    localDeltaPointer = &localDelta;
    mirrorAdjustment = 0;

    if (m_bLooped)
    {
        if (m_fPlaybackSpeedScale >= 0.0f)
        {
            GetRootTransDelta(this, &delta, m_fPrevTime, 1.0f);
            GetRootTransDelta(this, &wrappedDelta, 0.0f, m_fTime);
        }
        else
        {
            GetRootTransDelta(this, &delta, m_fPrevTime, 0.0f);
            GetRootTransDelta(this, &wrappedDelta, 1.0f, m_fTime);
        }

        delta.x += wrappedDelta.x;
        delta.y += wrappedDelta.y;
        delta.z += wrappedDelta.z;
    }
    else
    {
        GetRootTransDelta(this, &delta, m_fPrevTime, m_fTime);
    }

    m_pSAnim->GetRootRot(m_fPrevTime, &previousFacing);

    if (m_bMirror)
    {
        float rootX = delta.x;
        float angle = nlATan2f(delta.y, rootX);
        mirrorAdjustment = (u16)((previousFacing
                                     - (u16)(int)(10430.378f * angle))
                                 << 1);
    }

    {
        float cosine;
        float sine;

        nlSinCos(&sine, &cosine, mirrorAdjustment - previousFacing);

        localDeltaPointer->x = delta.x * cosine - delta.y * sine;
        localDeltaPointer->y = delta.y * cosine + delta.x * sine;
        localDeltaPointer->z = delta.z;
        *accumulatedWeight += weight;
    }

    if (*accumulatedWeight != 0.0f)
    {
        blend = weight / *accumulatedWeight;
        inverseBlend = 1.0f - blend;
        rootTranslation->x
            = inverseBlend * rootTranslation->x
            + blend * localDeltaPointer->x;
        rootTranslation->y
            = inverseBlend * rootTranslation->y
            + blend * localDeltaPointer->y;
        rootTranslation->z
            = inverseBlend * rootTranslation->z
            + blend * localDeltaPointer->z;
    }
}

void cPN_SAnimController::EvaluateScale(
    int nodeIndex, float weight, cPoseAccumulator* accumulator) const
{
    int remappedNodeIndex;
    int actualNodeIndex = nodeIndex;
    if (m_bMirror)
    {
        actualNodeIndex
            = accumulator->m_pHierarchy->GetMirroredNode(nodeIndex);
    }

    if (m_pAnimRetarget != 0)
    {
        remappedNodeIndex = m_pAnimRetarget->m_pMap[actualNodeIndex];
    }
    else
    {
        remappedNodeIndex = actualNodeIndex;
    }

    if (remappedNodeIndex != -1)
    {
        m_pSAnim->BlendScaleMultiply(
            nodeIndex, remappedNodeIndex, m_fTime, weight, accumulator);
    }
}

void cPN_SAnimController::Evaluate(
    int nodeIndex, float weight, cPoseAccumulator* accumulator) const
{
    bool mirror;
    int remappedNodeIndex;

    if (nodeIndex == 0)
    {
        for (int i = 0; i < m_pSAnim->m_nNumMorphChannels; ++i)
        {
            ((cPoseAccumulator*)((u8*)accumulator + i * sizeof(float)))
                ->m_MorphWeights.mData[0]
                += weight * m_pSAnim->GetMorphWeight(i, m_fTime);
        }
    }

    int actualNodeIndex = nodeIndex;
    mirror = m_bMirror;
    if (mirror)
    {
        actualNodeIndex
            = accumulator->m_pHierarchy->GetMirroredNode(nodeIndex);
    }

    if (m_pAnimRetarget != 0)
    {
        remappedNodeIndex = m_pAnimRetarget->m_pMap[actualNodeIndex];
    }
    else
    {
        remappedNodeIndex = actualNodeIndex;
    }

    if (remappedNodeIndex != -1)
    {
        m_pSAnim->BlendRot(nodeIndex, remappedNodeIndex, m_fTime, weight, accumulator, mirror);
        m_pSAnim->BlendScale(nodeIndex, remappedNodeIndex, m_fTime, weight, accumulator, mirror);
        m_pSAnim->BlendTrans(nodeIndex, remappedNodeIndex, m_fTime, weight, accumulator, mirror);
        return;
    }

    nlVector3& translationOffset
        = accumulator->m_pHierarchy->GetTranslationOffset(nodeIndex);
    accumulator->BlendRotIdentity(nodeIndex, weight);
    accumulator->BlendScaleIdentity(nodeIndex, weight);
    accumulator->m_pTranslations[nodeIndex].t = translationOffset;
}

void cPN_SAnimController::Evaluate(
    float weight, cPoseAccumulator* accumulator) const
{
    m_fWeight = weight;

    for (int i = 0; i < accumulator->GetNumNodes(); ++i)
    {
        Evaluate(i, weight, accumulator);
    }
}

#pragma dont_inline on
void cPN_SAnimController::UpdateSynchronized(float time, bool looped)
{
    cSAnimCallback* callback;
    cPN_SAnimController* controller = this;
    cPN_SAnimController* nextController;
    cPN_SAnimController* finalController;

    controller->m_fPrevTime = controller->m_fTime;
    controller->m_fTime = time;
    controller->m_bLooped = false;
    controller->m_bLooped = looped;

    if (!controller->m_bIgnoreTriggers)
    {
        callback = controller->m_pSAnim->m_pCallbackList;
        while (callback != 0)
        {
            if (controller->TestTrigger(callback->m_fTime))
            {
                callback->m_funcCallback(
                    controller->m_pSAnim, callback->m_nParam1);
            }
            callback = callback->next;
        }
    }

    if ((controller = controller->m_pSynchronizedController) == 0)
    {
        return;
    }

    controller->m_fPrevTime = controller->m_fTime;
    controller->m_fTime = time;
    controller->m_bLooped = false;
    controller->m_bLooped = looped;

    if (!controller->m_bIgnoreTriggers)
    {
        callback = controller->m_pSAnim->m_pCallbackList;
        while (callback != 0)
        {
            if (controller->TestTrigger(callback->m_fTime))
            {
                callback->m_funcCallback(
                    controller->m_pSAnim, callback->m_nParam1);
            }
            callback = callback->next;
        }
    }

    if ((nextController = controller->m_pSynchronizedController) == 0)
    {
        return;
    }

    nextController->m_fPrevTime = nextController->m_fTime;
    nextController->m_fTime = time;
    nextController->m_bLooped = false;
    nextController->m_bLooped = looped;
    nextController->ProcessCallbacks();

    finalController = nextController->m_pSynchronizedController;
    if (finalController == 0)
    {
        return;
    }

    finalController->SetTime(time);
    finalController->m_bLooped = looped;
    finalController->ProcessCallbacks();

    if (finalController->m_pSynchronizedController != 0)
    {
        finalController->m_pSynchronizedController->UpdateSynchronized(
            time, looped);
    }
}
#pragma dont_inline reset

cPoseNode* cPN_SAnimController::Update(float dt)
{
    if (!m_bIsSynchronized)
    {
        m_bLooped = false;

        if (m_pPlaybackSpeedCallback != 0)
        {
            m_pPlaybackSpeedCallback(
                m_nPlaybackSpeedCallbackParam, this);
        }

        float playbackSpeed = m_fPlaybackSpeedScale;

        if (m_pSynchronizedController != 0)
        {
            if (m_pSynchronizedWeightCallback != 0)
            {
                m_pSynchronizedWeightCallback(
                    m_nSynchronizedWeightCallbackParam, this);
            }

            float synchronizedDuration;
            float controllerDuration;
            float synchronizedScale;
            controllerDuration
                = m_pSAnim->GetDuration() / m_fPlaybackSpeedScale;
            synchronizedScale
                = m_pSynchronizedController->m_fPlaybackSpeedScale;
            synchronizedDuration
                = m_pSynchronizedController->m_pSAnim->GetDuration()
                / synchronizedScale;
            playbackSpeed *= m_fSynchronizedWeight
                               * (controllerDuration / synchronizedDuration - 1.0f)
                           + 1.0f;
        }

        m_fPrevTime = m_fTime;
        float duration = m_pSAnim->GetDuration();

        if (duration == 0.0f)
        {
            m_fTime = 1.0f;
        }
        else
        {
            m_fTime += (dt * playbackSpeed) / duration;
        }

        if (m_fTime > 1.0f)
        {
            switch (m_ePlayMode)
            {
            case PM_CYCLIC:
                do
                {
                    m_fTime -= 1.0f;
                } while (m_fTime > 1.0f);
                m_bLooped = true;
                break;
            case PM_HOLD:
                m_fTime = 1.0f;
                break;
            case PM_PING_PONG:
                m_fTime = 1.0f;
                m_fPlaybackSpeedScale *= -1.0f;
                break;
            }
        }
        else if (m_fTime < 0.0f)
        {
            switch (m_ePlayMode)
            {
            case PM_CYCLIC:
                do
                {
                    m_fTime += 1.0f;
                } while (m_fTime < 0.0f);
                m_bLooped = true;
                break;
            case PM_HOLD:
                m_fTime = 0.0f;
                break;
            case PM_PING_PONG:
                m_fTime = 0.0f;
                m_fPlaybackSpeedScale *= -1.0f;
                break;
            }
        }

        union
        {
            cSAnimCallback* callback;
            unsigned int looped;
        } rootState;
        cSAnimCallback* synchronizedCallback;
        cPN_SAnimController* synchronizedController;
        cPN_SAnimController* secondController;
        cPN_SAnimController* thirdController;
        cPN_SAnimController* fourthController;
        float synchronizedTime;

        if (dt >= 0.0f && !m_bIgnoreTriggers)
        {
            rootState.callback = m_pSAnim->m_pCallbackList;
            while (rootState.callback != 0)
            {
                if (TestTrigger(rootState.callback->m_fTime))
                {
                    rootState.callback->m_funcCallback(
                        m_pSAnim, rootState.callback->m_nParam1);
                }
                rootState.callback = rootState.callback->next;
            }
        }

        synchronizedController = m_pSynchronizedController;
        if (synchronizedController != 0)
        {
            synchronizedTime = m_fTime;
            bool looped = m_bLooped;
            rootState.looped = looped;

            synchronizedController->m_fPrevTime
                = synchronizedController->m_fTime;
            synchronizedController->m_fTime = synchronizedTime;
            synchronizedController->m_bLooped = false;
            synchronizedController->m_bLooped = rootState.looped;

            if (!synchronizedController->m_bIgnoreTriggers)
            {
                synchronizedCallback
                    = synchronizedController->m_pSAnim->m_pCallbackList;
                while (synchronizedCallback != 0)
                {
                    if (synchronizedController->TestTrigger(
                            synchronizedCallback->m_fTime))
                    {
                        synchronizedCallback->m_funcCallback(
                            synchronizedController->m_pSAnim,
                            synchronizedCallback->m_nParam1);
                    }
                    synchronizedCallback = synchronizedCallback->next;
                }
            }

            secondController
                = synchronizedController->m_pSynchronizedController;
            if (secondController != 0)
            {
                secondController->m_fPrevTime = secondController->m_fTime;
                secondController->m_fTime = synchronizedTime;
                secondController->m_bLooped = false;
                secondController->m_bLooped = rootState.looped;

                if (!secondController->m_bIgnoreTriggers)
                {
                    synchronizedCallback
                        = secondController->m_pSAnim->m_pCallbackList;
                    while (synchronizedCallback != 0)
                    {
                        if (secondController->TestTrigger(
                                synchronizedCallback->m_fTime))
                        {
                            synchronizedCallback->m_funcCallback(
                                secondController->m_pSAnim,
                                synchronizedCallback->m_nParam1);
                        }
                        synchronizedCallback
                            = synchronizedCallback->next;
                    }
                }

                thirdController
                    = secondController->m_pSynchronizedController;
                if (thirdController != 0)
                {
                    thirdController->m_fPrevTime
                        = thirdController->m_fTime;
                    thirdController->m_fTime = synchronizedTime;
                    thirdController->m_bLooped = false;
                    thirdController->m_bLooped = rootState.looped;

                    if (!thirdController->m_bIgnoreTriggers)
                    {
                        synchronizedCallback
                            = thirdController->m_pSAnim->m_pCallbackList;
                        while (synchronizedCallback != 0)
                        {
                            if (thirdController->TestTrigger(
                                    synchronizedCallback->m_fTime))
                            {
                                synchronizedCallback->m_funcCallback(
                                    thirdController->m_pSAnim,
                                    synchronizedCallback->m_nParam1);
                            }
                            synchronizedCallback
                                = synchronizedCallback->next;
                        }
                    }

                    fourthController
                        = thirdController->m_pSynchronizedController;
                    if (fourthController != 0)
                    {
                        fourthController->m_fPrevTime
                            = fourthController->m_fTime;
                        fourthController->m_fTime = synchronizedTime;
                        fourthController->m_bLooped = false;
                        fourthController->m_bLooped = rootState.looped;

                        if (!fourthController->m_bIgnoreTriggers)
                        {
                            synchronizedCallback
                                = fourthController->m_pSAnim->m_pCallbackList;
                            while (synchronizedCallback != 0)
                            {
                                if (fourthController->TestTrigger(
                                        synchronizedCallback->m_fTime))
                                {
                                    synchronizedCallback->m_funcCallback(
                                        fourthController->m_pSAnim,
                                        synchronizedCallback->m_nParam1);
                                }
                                synchronizedCallback
                                    = synchronizedCallback->next;
                            }
                        }

                        if (fourthController->m_pSynchronizedController
                            != 0)
                        {
                            fourthController->m_pSynchronizedController
                                ->UpdateSynchronized(
                                    synchronizedTime, looped);
                        }
                    }
                }
            }
        }
    }

    return this;
}

cPN_SAnimController::cPN_SAnimController(cSAnim* anim,
    const AnimRetarget* retarget, ePlayMode playMode,
    void (*playbackSpeedCallback)(unsigned int, cPN_SAnimController*),
    unsigned int playbackSpeedCallbackParam, bool mirror)
    : cPoseNode(0)
{
    m_pSAnim = anim;
    m_fTime = 0.0f;
    m_bMirror = mirror;
    m_pAnimRetarget = retarget;
    m_fPrevTime = 0.0f;
    m_ePlayMode = playMode;
    m_bIsSynchronized = false;
    m_bIgnoreTriggers = false;
    m_bNegativeTriggerProcessed = false;
    m_bLooped = false;
    m_pPlaybackSpeedCallback = playbackSpeedCallback;
    m_nPlaybackSpeedCallbackParam = playbackSpeedCallbackParam;
    m_fPlaybackSpeedScale = 1.0f;
    m_pSynchronizedController = 0;
    m_pSynchronizedWeightCallback = 0;
    m_fSynchronizedWeight = 1.0f;
}
