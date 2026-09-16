#include "Game/FE/feAnimation.h"

#include "Game/FE/tlInstance.h"
#include "NL/nlDLRing.h"
#include "NL/nlMath.h"

void FEAnimation::AnimateTargetAtTimeWithFloat(float fCurrentTime)
{
    fAnimationKeyframe* currentFrame;
    float fAnimatedResult;
    float fMu;

    currentFrame = nlDLRingGetStart<fAnimationKeyframe>((fAnimationKeyframe*)GetKeyframeHead());
    if (currentFrame != currentFrame->m_next)
    {
        if (fCurrentTime >= currentFrame->pKeyFrameData.GetTime())
        {
            while (fCurrentTime > currentFrame->pKeyFrameData.GetTime())
            {
                currentFrame = currentFrame->m_next;
                if (nlDLRingIsEnd<fAnimationKeyframe>((fAnimationKeyframe*)GetKeyframeHead(), currentFrame))
                {
                    break;
                }
            }

            if (fCurrentTime == currentFrame->pKeyFrameData.GetTime())
            {
                fAnimatedResult = currentFrame->pKeyFrameData.GetPoint();
            }
            else if (!(fCurrentTime > currentFrame->pKeyFrameData.GetTime())
                     || currentFrame->pKeyFrameData.GetControl1() != -1.0f)
            {
                float fPrevTime = currentFrame->m_prev->pKeyFrameData.GetTime();
                fMu = (fCurrentTime - fPrevTime) / (currentFrame->pKeyFrameData.GetTime() - fPrevTime);
                float controlPoints[4];
                controlPoints[0] = currentFrame->m_prev->pKeyFrameData.GetPoint();
                controlPoints[1] = currentFrame->m_prev->pKeyFrameData.GetControl1();
                controlPoints[2] = currentFrame->m_prev->pKeyFrameData.GetControl2();
                controlPoints[3] = currentFrame->pKeyFrameData.GetPoint();
                fAnimatedResult = nlBezier(controlPoints, 3, fMu);
            }
            else
            {
                fAnimatedResult = currentFrame->pKeyFrameData.GetPoint();
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
                case eAnimUVX:
                    m_pTLInstanceTarget->SetAssetUVX(fAnimatedResult);
                    break;
                case eAnimUVY:
                    m_pTLInstanceTarget->SetAssetUVY(fAnimatedResult);
                    break;
                case eAnimUVWidth:
                    m_pTLInstanceTarget->SetAssetUVWidth(fAnimatedResult);
                    break;
                case eAnimUVHeight:
                    m_pTLInstanceTarget->SetAssetUVHeight(fAnimatedResult);
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

    v3AnimationKeyframe* head = (v3AnimationKeyframe*)GetKeyframeHead();
    currentFrame = nlDLRingGetStart<v3AnimationKeyframe>(head);
    if (fCurrentTime < currentFrame->pKeyFrameDataX.GetTime())
    {
        fCurrentTime = currentFrame->pKeyFrameDataX.GetTime();
    }

    while (fCurrentTime > currentFrame->pKeyFrameDataX.GetTime()
           && (m_type != eAnimRotation || -1.0f != currentFrame->pKeyFrameDataX.GetControl1()
               || -1.0f != currentFrame->pKeyFrameDataX.GetControl2()))
    {
        currentFrame = currentFrame->m_next;
        if (nlDLRingIsEnd<v3AnimationKeyframe>(head, currentFrame))
        {
            break;
        }
    }

    if (fCurrentTime == currentFrame->pKeyFrameDataX.GetTime())
    {
        result[0] = currentFrame->pKeyFrameDataX.GetPoint();
        result[1] = currentFrame->pKeyFrameDataY.GetPoint();
        result[2] = currentFrame->pKeyFrameDataZ.GetPoint();
    }
    else if (!(fCurrentTime > currentFrame->pKeyFrameDataX.GetTime())
             || currentFrame->pKeyFrameDataX.GetControl1() != -1.0f)
    {
        float prevTime = currentFrame->m_prev->pKeyFrameDataX.GetTime();
        fMu = (fCurrentTime - prevTime) / (currentFrame->pKeyFrameDataX.GetTime() - prevTime);

        float controlPointsX[4];
        controlPointsX[0] = currentFrame->m_prev->pKeyFrameDataX.GetPoint();
        controlPointsX[1] = currentFrame->m_prev->pKeyFrameDataX.GetControl1();
        controlPointsX[2] = currentFrame->m_prev->pKeyFrameDataX.GetControl2();
        controlPointsX[3] = currentFrame->pKeyFrameDataX.GetPoint();

        float controlPointsY[4];
        controlPointsY[0] = currentFrame->m_prev->pKeyFrameDataY.GetPoint();
        controlPointsY[1] = currentFrame->m_prev->pKeyFrameDataY.GetControl1();
        controlPointsY[2] = currentFrame->m_prev->pKeyFrameDataY.GetControl2();
        controlPointsY[3] = currentFrame->pKeyFrameDataY.GetPoint();

        float controlPointsZ[4];
        controlPointsZ[0] = currentFrame->m_prev->pKeyFrameDataZ.GetPoint();
        controlPointsZ[1] = currentFrame->m_prev->pKeyFrameDataZ.GetControl1();
        controlPointsZ[2] = currentFrame->m_prev->pKeyFrameDataZ.GetControl2();
        controlPointsZ[3] = currentFrame->pKeyFrameDataZ.GetPoint();

        result[0] = nlBezier(controlPointsX, 3, fMu);
        result[1] = nlBezier(controlPointsY, 3, fMu);
        result[2] = nlBezier(controlPointsZ, 3, fMu);
    }
    else
    {
        result[0] = currentFrame->pKeyFrameDataX.GetPoint();
        result[1] = currentFrame->pKeyFrameDataY.GetPoint();
        result[2] = currentFrame->pKeyFrameDataZ.GetPoint();
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
        m_pTLInstanceTarget->SetAssetPivot(result[0], result[1], result[2]);
        break;
    case eAnimColor:
    {
        u8 b = (u8)result[2];
        u8 g = (u8)result[1];
        u8 r = (u8)result[0];
        nlColour newColour;
        nlColourSet(newColour, r, g, b, 255);
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
