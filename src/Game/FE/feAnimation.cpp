#include "Game/FE/feAnimation.h"

#include "Game/FE/tlInstance.h"
#include "NL/nlDLRing.h"
#include "NL/nlMath.h"

extern "C" void fn_803023B4(TLInstance*, float, float, float);
extern "C" void fn_803023D0(TLInstance*, float);
extern "C" void fn_803023E4(TLInstance*, float);
extern "C" void fn_803023F8(TLInstance*, float);
extern "C" void fn_8030240C(TLInstance*, float);

void FEAnimation::AnimateTargetAtTimeWithFloat(float fCurrentTime)
{
    fAnimationKeyframe* currentFrame;
    float fAnimatedResult;

    currentFrame = nlDLRingGetStart<fAnimationKeyframe>((fAnimationKeyframe*)m_DLRingHead);
    if (currentFrame != currentFrame->m_next)
    {
        if (fCurrentTime >= currentFrame->pKeyFrameData.m_fTime)
        {
            while (fCurrentTime > currentFrame->pKeyFrameData.m_fTime)
            {
                currentFrame = currentFrame->m_next;
                if (nlDLRingIsEnd<fAnimationKeyframe>((fAnimationKeyframe*)m_DLRingHead, currentFrame))
                {
                    break;
                }
            }

            float fTime = currentFrame->pKeyFrameData.m_fTime;
            if (fCurrentTime == fTime)
            {
                fAnimatedResult = currentFrame->pKeyFrameData.m_fPoint;
            }
            else if (!(fCurrentTime > fTime) || currentFrame->pKeyFrameData.m_fControl1 != -1.0f)
            {
                float fPrevTime = currentFrame->m_prev->pKeyFrameData.m_fTime;
                float controlPoints[4];
                controlPoints[0] = currentFrame->m_prev->pKeyFrameData.m_fPoint;
                controlPoints[1] = currentFrame->m_prev->pKeyFrameData.m_fControl1;
                controlPoints[2] = currentFrame->m_prev->pKeyFrameData.m_fControl2;
                controlPoints[3] = currentFrame->pKeyFrameData.m_fPoint;
                fAnimatedResult = nlBezier(controlPoints, 3, (fCurrentTime - fPrevTime) / (fTime - fPrevTime));
            }
            else
            {
                fAnimatedResult = currentFrame->pKeyFrameData.m_fPoint;
            }

            if (fAnimatedResult != -1.0f && m_type == eAnimOpacity)
            {
                nlColour newColour = m_pTLInstanceTarget->GetAssetColour();
                newColour.c[3] = (u8)fAnimatedResult;
                m_pTLInstanceTarget->SetAssetColour(newColour);
            }
            else
            {
                switch (m_type)
                {
                case (AnimType)7:
                    fn_803023D0(m_pTLInstanceTarget, fAnimatedResult);
                    break;
                case (AnimType)8:
                    fn_803023E4(m_pTLInstanceTarget, fAnimatedResult);
                    break;
                case (AnimType)9:
                    fn_803023F8(m_pTLInstanceTarget, fAnimatedResult);
                    break;
                case (AnimType)10:
                    fn_8030240C(m_pTLInstanceTarget, fAnimatedResult);
                    break;
                }
            }
        }
    }
}

void FEAnimation::AnimateTargetAtTimeWithVector3(float fCurrentTime)
{
    v3AnimationKeyframe* currentFrame;
    float result[3];
    float fMu;

    currentFrame = nlDLRingGetStart<v3AnimationKeyframe>((v3AnimationKeyframe*)m_DLRingHead);
    if (fCurrentTime < currentFrame->pKeyFrameDataX.m_fTime)
    {
        fCurrentTime = currentFrame->pKeyFrameDataX.m_fTime;
    }

    while (fCurrentTime > currentFrame->pKeyFrameDataX.m_fTime
           && (m_type != eAnimRotation || -1.0f != currentFrame->pKeyFrameDataX.m_fControl1
               || -1.0f != currentFrame->pKeyFrameDataX.m_fControl2))
    {
        currentFrame = currentFrame->m_next;
        if (nlDLRingIsEnd<v3AnimationKeyframe>((v3AnimationKeyframe*)m_DLRingHead, currentFrame))
        {
            break;
        }
    }

    float currentTime = currentFrame->pKeyFrameDataX.m_fTime;
    if (fCurrentTime == currentTime)
    {
        result[0] = currentFrame->pKeyFrameDataX.m_fPoint;
        result[1] = currentFrame->pKeyFrameDataY.m_fPoint;
        result[2] = currentFrame->pKeyFrameDataZ.m_fPoint;
    }
    else if (!(fCurrentTime > currentTime) || currentFrame->pKeyFrameDataX.m_fControl1 != -1.0f)
    {
        float prevTime = currentFrame->m_prev->pKeyFrameDataX.m_fTime;

        float controlPointsX[4];
        controlPointsX[0] = currentFrame->m_prev->pKeyFrameDataX.m_fPoint;
        controlPointsX[1] = currentFrame->m_prev->pKeyFrameDataX.m_fControl1;
        controlPointsX[2] = currentFrame->m_prev->pKeyFrameDataX.m_fControl2;
        controlPointsX[3] = currentFrame->pKeyFrameDataX.m_fPoint;

        float controlPointsY[4];
        controlPointsY[0] = currentFrame->m_prev->pKeyFrameDataY.m_fPoint;
        controlPointsY[1] = currentFrame->m_prev->pKeyFrameDataY.m_fControl1;
        controlPointsY[2] = currentFrame->m_prev->pKeyFrameDataY.m_fControl2;
        controlPointsY[3] = currentFrame->pKeyFrameDataY.m_fPoint;

        float controlPointsZ[4];
        controlPointsZ[0] = currentFrame->m_prev->pKeyFrameDataZ.m_fPoint;
        controlPointsZ[1] = currentFrame->m_prev->pKeyFrameDataZ.m_fControl1;
        controlPointsZ[2] = currentFrame->m_prev->pKeyFrameDataZ.m_fControl2;
        controlPointsZ[3] = currentFrame->pKeyFrameDataZ.m_fPoint;

        fMu = (fCurrentTime - prevTime) / (currentTime - prevTime);
        result[0] = nlBezier(controlPointsX, 3, fMu);
        result[1] = nlBezier(controlPointsY, 3, fMu);
        result[2] = nlBezier(controlPointsZ, 3, fMu);
    }
    else
    {
        result[0] = currentFrame->pKeyFrameDataX.m_fPoint;
        result[1] = currentFrame->pKeyFrameDataY.m_fPoint;
        result[2] = currentFrame->pKeyFrameDataZ.m_fPoint;
    }

    switch (m_type)
    {
    case eAnimPosition:
        m_pTLInstanceTarget->SetAssetPosition(result[0], result[1], result[2]);
        break;
    case eAnimRotation:
        m_pTLInstanceTarget->SetAssetRotation(result[0], result[1], result[2]);
        break;
    case eAnimScale:
        m_pTLInstanceTarget->SetAssetScale(result[0], result[1], result[2]);
        break;
    case eAnimPivot:
        fn_803023B4(m_pTLInstanceTarget, result[0], result[1], result[2]);
        break;
    case eAnimColor:
    {
        nlColour newColour;
        newColour.c[2] = (u8)result[2];
        newColour.c[1] = (u8)result[1];
        newColour.c[0] = (u8)result[0];
        newColour.c[3] = 255;
        m_pTLInstanceTarget->SetAssetColour(newColour);
        break;
    }
    }
}

void FEAnimation::Update(float fCurrentTime)
{
    switch (m_cast_type)
    {
    case 1:
        AnimateTargetAtTimeWithVector3(fCurrentTime);
        return;
    case 0:
        AnimateTargetAtTimeWithFloat(fCurrentTime);
        return;
    }
}
