#include "Game/Render/MegaBallIndicators.h"
#include "Game/NetworkMessageRegistry.h"
#include "Game/MathHelpers.h"
#include "Game/Render/RLView.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/TweakRegistry.h"
#include "Game/AI/AiUtil.h"
#include "Game/NetworkSession.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/gl/glDraw2.h"
#include "NL/gl/glState.h"
#include "NL/gl/glTexture.h"
#include "NL/globalpad.h"
#include "NL/nlMath.h"
#include "NL/nlString.h"
#include "NL/nlTicker.h"
#include "NL/plat/DPDData.h"
#include "NL/plat/WiiPad.h"
#include <math.h>

void SendMegaBallPointerUpdate(u16 angle, u32 textureIndex, u32 status,
    float x, float y);

static char sReduceTexturesTweakPath[] = "/Rendering/Engine/Reduce Textures";
static char sMegaBallTextureName[] = "global/the_ball";
static char sMegaBallExplodedTextureName[] = "global/the_ball_exploded";
static char sMegaBallTimerEmptyTextureName[] = "global/timer_empty";
static char sMegaBallTimerEndCapTextureName[] = "global/timer_endcap_right";

float gMegaBallCollisionRadiusScale = 0.08f;
float gMegaBallCollisionHalfWidth = 25.0f;
float gMegaBallCollisionYOffset = 10.0f;
float gMegaBallPointerXScale = 400.0f;
float gMegaBallPointerYScale = 300.0f;
float gMegaBallPointerMagnetRadius = 25.0f;
float gMegaBallPointerMagnetCurve = -10.05f;
float gMegaBallPointerPacketInterval = 30.0f;
float gMegaBallPointerOpacity = 0.666f;
float gRemoteMegaBallPointerOpacity = 0.666f;
float gMegaBallScreenCenterX = 320.0f;
float gMegaBallTimerBallWidth = 54.0f;
float gMegaBallTimerBallHeight = 54.0f;
float gMegaBallTimerExplodedWidth = 54.0f;
float gMegaBallTimerExplodedHeight = 54.0f;
float gMegaBallTimerY = 430.0f;
float gMegaBallTimerScale = 1.0f;
int gRemoteMegaBallPointerTextureIndex = 1;

cGlobalPad* gMegaBallController;
bool lbl_806E15C4;
bool gMegaBallPointerMagnetism;
bool gSuppressMegaBallPointerPackets;
float gMegaBallTimerYOffset;
u32 gMegaBallDefaultTextureId = nlStringLowerHash(sMegaBallTextureName);
u32 gMegaBallPointerTextureId;
u32 gMegaBallCatchTextureId;
u32 gMegaBallTargetTextureId = nlStringLowerHash(sMegaBallTextureName);
u32 gMegaBallExplodedTextureId = nlStringLowerHash(sMegaBallExplodedTextureName);
u32 gMegaBallTimerEmptyTextureId = nlStringLowerHash(sMegaBallTimerEmptyTextureName);
u32 gMegaBallTimerEndCapTextureId = nlStringLowerHash(sMegaBallTimerEndCapTextureName);
u32 gMegaBallTimerCount;
bool gMegaBallTimerVisible;
bool lbl_806E15ED;
u32 gLastMegaBallPointerPacketTicker;
bool gRemoteMegaBallPointerReceived;
float gRemoteMegaBallPointerX;
float gRemoteMegaBallPointerY;
u16 gRemoteMegaBallPointerAngle;
u16 gRemoteMegaBallPointerStatus;

u32 gMegaBallIndicatorTextures[8] = { gMegaBallDefaultTextureId, gMegaBallPointerTextureId, gMegaBallCatchTextureId, gMegaBallTargetTextureId, gMegaBallExplodedTextureId, gMegaBallTimerEmptyTextureId, gMegaBallTimerEndCapTextureId, 0 };
MegaBallIndicator gMegaBallIndicators[10];
MegaBallIndicator gMegaBallTargetIndicators[10];
MegaBallIndicator gMegaBallCatchIndicators[10];
MegaBallIndicator gMegaBallPointer;
MegaBallIndicator gMegaBallTimerSegments[10];
MegaBallIndicator gMegaBallTimerEndCaps[2];
int gMegaBallTimerStatuses[10];

static inline void ResetTween(MegaBallIndicatorTween& tween)
{
    tween.mElapsedTime = 0.0f;
    tween.mValue = 1.0f;
    tween.mEndTime = 0.0f;
    tween.mStartValue = 0.0f;
    tween.mTransitionTime = 0.0f;
    tween.mEndValue = 0.0f;
    tween.mRepeatCount = 0;
    tween.mActive = false;
}

void ResetMegaBallIndicator(
    MegaBallIndicator* pState, unsigned int nTextureIndex)
{
    pState->mX = gMegaBallScreenCenterX;
    pState->mY = 240.0f;
    pState->mScale = 1.0f;
    pState->mOpacity = 1.0f;
    pState->mAngle = 0.0f;
    pState->mWidth = 100.0f;
    pState->mHeight = 100.0f;
    pState->mTextureId = gMegaBallIndicatorTextures[nTextureIndex];
    pState->mTextureIndex = nTextureIndex;
    pState->mVisible = false;
    pState->mActive = false;
    ResetTween(pState->mScaleTween);
    ResetTween(pState->mOpacityTween);
    ResetTween(pState->mVisibilityTween);
}

void SetMegaBallIndicatorTexture(
    MegaBallIndicator* pState, unsigned int nTextureIndex)
{
    pState->mTextureId = gMegaBallIndicatorTextures[nTextureIndex];
    pState->mTextureIndex = nTextureIndex;
    if (glTextureLoad(pState->mTextureId))
    {
        pState->mWidth = (float)(int)glTextureGetWidth();
        pState->mHeight = (float)(int)glTextureGetHeight();
        if (GetTweakBool(sReduceTexturesTweakPath, false))
        {
            pState->mWidth *= 2.0f;
            pState->mHeight *= 2.0f;
        }
    }
}

bool UpdateMegaBallIndicatorTween(
    MegaBallIndicatorTween* pTween, float fDeltaT)
{
    if (pTween->mActive)
    {
        pTween->mElapsedTime += fDeltaT;
        if (pTween->mElapsedTime < pTween->mTransitionTime)
        {
            pTween->mValue = InterpolateRange(
                pTween->mStartValue, pTween->mEndValue, 0.0f, pTween->mTransitionTime, pTween->mElapsedTime);
        }
        else if (pTween->mEndTime > pTween->mTransitionTime)
        {
            if (pTween->mElapsedTime < pTween->mEndTime)
            {
                pTween->mValue = InterpolateRange(
                    pTween->mEndValue, pTween->mStartValue, pTween->mTransitionTime, pTween->mEndTime, pTween->mElapsedTime);
            }
            else
            {
                pTween->mValue = pTween->mStartValue;
                if (pTween->mRepeatCount != 0)
                {
                    if (pTween->mRepeatCount > 0)
                    {
                        pTween->mRepeatCount--;
                    }
                    pTween->mElapsedTime = 0.0f;
                }
                else
                {
                    pTween->mActive = false;
                }
                return true;
            }
        }
        else
        {
            pTween->mValue = pTween->mEndValue;
            if (pTween->mRepeatCount != 0)
            {
                if (pTween->mRepeatCount > 0)
                {
                    pTween->mRepeatCount--;
                }
                pTween->mElapsedTime = 0.0f;
            }
            else
            {
                pTween->mActive = false;
            }
            return true;
        }
    }
    return pTween->mActive;
}

static inline void ConfigureTween(MegaBallIndicatorTween& tween,
    int nRepeat,
    float fTransitionTime, float fEnd, float fEndTime, float fStart)
{
    tween.mElapsedTime = 0.0f;
    if (fTransitionTime > 0.0f)
    {
        tween.mValue = fStart;
    }
    else
    {
        tween.mValue = fEnd;
    }
    tween.mRepeatCount = nRepeat;
    tween.mActive = true;
    tween.mTransitionTime = fTransitionTime;
    tween.mEndValue = fEnd;
    tween.mEndTime = fEndTime;
    tween.mStartValue = fStart;
}

void SetMegaBallIndicatorScaleTween(MegaBallIndicator* pState,
    int nRepeat, float fTransitionTime, float fEnd, float fEndTime,
    float fStart)
{
    ConfigureTween(pState->mScaleTween, nRepeat, fTransitionTime, fEnd, fEndTime, fStart);
}

void SetMegaBallIndicatorOpacityTween(MegaBallIndicator* pState,
    int nRepeat, float fTransitionTime, float fEnd, float fEndTime,
    float fStart)
{
    MegaBallIndicatorTween& tween = pState->mOpacityTween;
    tween.mElapsedTime = 0.0f;
    if (fTransitionTime > 0.0f)
    {
        tween.mValue = fStart;
    }
    else
    {
        tween.mValue = fEnd;
    }
    tween.mActive = true;
    tween.mRepeatCount = nRepeat;
    tween.mTransitionTime = fTransitionTime;
    tween.mEndValue = fEnd;
    tween.mEndTime = fEndTime;
    tween.mStartValue = fStart;
    tween.mValue = 1.0f;
}

void StopMegaBallIndicatorOpacityTween(MegaBallIndicator* pState)
{
    pState->mOpacityTween.mActive = false;
    pState->mOpacityTween.mValue = 1.0f;
}

static inline void ResetAndLoad(MegaBallIndicator& state,
    unsigned int nTextureIndex, unsigned int nIndex)
{
    ResetMegaBallIndicator(&state, nTextureIndex);
    SetMegaBallIndicatorTexture(&state, nTextureIndex);
    state.mIndex = nIndex;
}

void ResetMegaBallIndicators()
{
    for (unsigned int i = 0; i < 10; i++)
    {
        ResetAndLoad(gMegaBallIndicators[i], 0, i);
        ResetMegaBallIndicator(&gMegaBallTargetIndicators[i], 0);
        SetMegaBallIndicatorTexture(&gMegaBallTargetIndicators[i], 3);
        gMegaBallTargetIndicators[i].mIndex = i;
    }
    for (unsigned int i = 0; i < 10; i++)
    {
        ResetMegaBallIndicator(&gMegaBallCatchIndicators[i], 0);
        SetMegaBallIndicatorTexture(&gMegaBallCatchIndicators[i], 2);
        gMegaBallCatchIndicators[i].mOpacity = 0.5f;
        gMegaBallCatchIndicators[i].mIndex = i;
    }

    gLastMegaBallPointerPacketTicker = nlGetTicker();
    gRemoteMegaBallPointerReceived = false;
    gRemoteMegaBallPointerX = 0.0f;
    gRemoteMegaBallPointerY = 0.0f;
    gRemoteMegaBallPointerAngle = 0;
    gRemoteMegaBallPointerTextureIndex = 1;
    gRemoteMegaBallPointerStatus = 0;
    ResetMegaBallTimer();
}

void ResetMegaBallTimer()
{
    gMegaBallTimerCount = 0;
    gMegaBallTimerVisible = false;

    MegaBallIndicator* pTimerState = gMegaBallTimerSegments;
    int* pTimerStatus = gMegaBallTimerStatuses;
    unsigned int i;
    for (i = 0; i < 10; i++)
    {
        ResetMegaBallIndicator(pTimerState, 0);
        SetMegaBallIndicatorTexture(pTimerState, 5);
        pTimerState->mIndex = i;
        *pTimerStatus = -1;
        pTimerState++;
        pTimerStatus++;
    }

    unsigned int j;
    pTimerState = gMegaBallTimerEndCaps;
    for (j = 0; j < 2; j++)
    {
        ResetMegaBallIndicator(pTimerState, 0);
        SetMegaBallIndicatorTexture(pTimerState, 6);
        pTimerState->mIndex = j;
        pTimerState++;
    }
}

static inline MegaBallIndicator* AllocateState(
    MegaBallIndicator* pStates, float fX, float fY,
    float fScale)
{
    MegaBallIndicator* pState = 0;
    for (unsigned int i = 0; i < 10; i++)
    {
        if (!pStates[i].mActive)
        {
            pState = &pStates[i];
            break;
        }
    }
    if (pState != 0)
    {
        pState->mActive = true;
        pState->mVisible = true;
        pState->mX = fX;
        pState->mY = fY;
        pState->mScale = fScale;
    }
    return pState;
}

MegaBallIndicator* CreateMegaBallIndicator(
    float fX, float fY, float fScale)
{
    return AllocateState(gMegaBallIndicators, fX, fY, fScale);
}

void ReleaseMegaBallIndicator(MegaBallIndicator* pState)
{
    pState->mActive = false;
    pState->mVisible = false;
}

MegaBallIndicator* GetMegaBallIndicator(unsigned int nIndex)
{
    return &gMegaBallIndicators[nIndex];
}

MegaBallIndicator* GetMegaBallTargetIndicator(unsigned int nIndex)
{
    return &gMegaBallTargetIndicators[nIndex];
}

MegaBallIndicator* CreateMegaBallCatchIndicator(
    float fX, float fY, float fScale, float fAngle)
{
    MegaBallIndicator* pState
        = AllocateState(gMegaBallCatchIndicators, fX, fY, fScale);
    if (pState != 0)
    {
        pState->mAngle = fAngle;
    }
    return pState;
}

MegaBallIndicator* GetMegaBallCatchIndicator(unsigned int nIndex)
{
    return &gMegaBallCatchIndicators[nIndex];
}

void SetMegaBallIndicatorTextures(
    unsigned int nTexture1, unsigned int nTexture2)
{
    gMegaBallIndicatorTextures[1] = nTexture1;
    gMegaBallIndicatorTextures[2] = nTexture2;
}

void ActivateMegaBallPointer(
    bool bParam, float fX, float fY, float fScale)
{
    gMegaBallPointer.mX = fX;
    gMegaBallPointer.mY = fY;
    gMegaBallPointer.mScale = fScale;
    gMegaBallPointer.mAngle = 0.0f;
    SetMegaBallIndicatorTexture(&gMegaBallPointer, 1);
    gMegaBallPointer.mActive = true;
    gMegaBallPointer.mVisible = true;
    gMegaBallPointer.mOpacity = gMegaBallPointerOpacity;
    lbl_806E15ED = false;
    gMegaBallPointerMagnetism = bParam;
}

void ResetMegaBallPointer()
{
    ResetMegaBallIndicator(&gMegaBallPointer, 0);
}

float TestMegaBallIndicatorCollision(MegaBallIndicator* pState1,
    MegaBallIndicator* pState2)
{
    if (pState2 == 0)
    {
        pState2 = &gMegaBallPointer;
    }
    if (!pState1->mActive)
    {
        return 0.0f;
    }

    nlVector2 v2Centre;
    nlVec2Set(v2Centre, pState2->mX, pState2->mY + gMegaBallCollisionYOffset);
    nlVector2 v2Left;
    nlVec2Set(v2Left, -gMegaBallCollisionHalfWidth, gMegaBallCollisionYOffset);
    nlVector2 v2Right;
    nlVec2Set(v2Right, gMegaBallCollisionHalfWidth, gMegaBallCollisionYOffset);
    nlVector2 v2Position;
    nlVec2Set(v2Position, pState1->mX, pState1->mY);

    float fSin;
    float fCos;
    float fAngle = pState2->mAngle;
    u16 nAngle = (u16)(int)(fAngle * 10430.378f);
    nlSinCos(&fSin, &fCos, nAngle);

    float fDeltaX = v2Position.x - v2Centre.x;
    float fDeltaY = v2Position.y - v2Centre.y;
    nlVector2 v2Rotated;
    v2Rotated.x = fCos * fDeltaX + fSin * fDeltaY;
    v2Rotated.y = fCos * fDeltaY - fSin * fDeltaX;
    float fTargetWidth = pState2->mWidth * pState2->mScale;
    float fIndicatorWidth = pState1->mWidth * pState1->mScale;
    nlVector2 v2LeftDelta;
    nlVec2Sub(v2LeftDelta, v2Left, v2Rotated);
    nlVector2 v2RightDelta;
    nlVec2Sub(v2RightDelta, v2Right, v2Rotated);

    float fIndicatorRadius;
    float fTargetRadius;
    fTargetRadius = gMegaBallCollisionRadiusScale * fTargetWidth;
    fIndicatorRadius = 0.5f * fIndicatorWidth;
    float fRadius = fTargetRadius + fIndicatorRadius;
    float fRadiusSquared = fRadius * fRadius;
    float fLeftDistanceSquared = nlVec2LengthSquared(v2LeftDelta);
    float fRightDistanceSquared = nlVec2LengthSquared(v2RightDelta);
    if (fLeftDistanceSquared < fRadiusSquared
        || fRightDistanceSquared < fRadiusSquared)
    {
        return 1.0f;
    }
    return 0.0f;
}

static inline void DrawVisibleState(const MegaBallIndicator& state)
{
    if (state.mVisible && state.mOpacity > 0.0f)
    {
        float fX;
        float fY;
        fY = state.mY;
        fX = state.mX;
        int nX = (int)fX;
        int nY = (int)fY;
        float fHeight = state.mHeight;
        float fScale = state.mScale;
        float fWidth = state.mWidth;
        fHeight *= fScale;
        fWidth *= fScale;
        float fAngle = state.mAngle;
        DrawMegaBallIndicator(nX,
            nY,
            state.mTextureId,
            fWidth,
            fHeight,
            state.mOpacity,
            fAngle);
    }
}

static inline void DrawState(const MegaBallIndicator& state)
{
    if (state.mActive)
    {
        DrawVisibleState(state);
    }
}

void DrawMegaBallIndicator(int nX, int nY, unsigned int nTexture,
    float fWidth, float fHeight, float fOpacity, float fAngle)
{
    bool bWideScreen = IsWidescreen();
    glPoly2 poly;
    glSetDefaultState(false);
    glSetRasterState(GLS_AlphaBlend, 1);
    glSetRasterState(GLS_AlphaTest, 1);
    glSetRasterState(GLS_AlphaTestRef, 0);
    glSetCurrentRasterState(glHandleizeRasterState());
    glSetCurrentTexture(nTexture, GLTT_Diffuse);

    if (bWideScreen)
    {
        if (fAngle != 0.0f)
        {
            float fSin;
            float fCos;
            nlSinCos(&fSin, &fCos, (u16)(int)(10430.378f * fAngle));
            float fAbsCos = fabsf(fCos);
            float fAbsSin = fabsf(fSin);
            fWidth = (float)((double)fWidth
                             * (1.0 - (double)(0.2f * fAbsCos)));
            fHeight = (float)((double)fHeight
                              * (1.0 - (double)(0.2f * fAbsSin)));
        }
        else
        {
            fWidth *= 0.8f;
        }
    }

    poly.SetupRotatedRectangle((float)nX, (float)nY, fWidth, fHeight, fAngle, 10000000000.0f);
    nlColour colour;
    nlColourSet(colour, 0xFF, 0xFF, 0xFF, (u8)(int)(255.0f * fOpacity));
    poly.SetColour(colour);
    poly.depth = -0.5f;
    poly.Attach(GetLayerView(eCLV_UnsortedOrtho640), 0, 0);
}

void SetMegaBallController(cGlobalPad* pController)
{
    gMegaBallController = pController;
}

static inline void UpdateStateTweens(
    MegaBallIndicator& state, float fDeltaT)
{
    if (state.mActive)
    {
        if (UpdateMegaBallIndicatorTween(&state.mScaleTween, fDeltaT))
        {
            state.mScale
                = state.mScaleTween.mValue;
        }
        if (UpdateMegaBallIndicatorTween(&state.mOpacityTween, fDeltaT))
        {
            state.mOpacity
                = state.mOpacityTween.mValue;
        }
        if (UpdateMegaBallIndicatorTween(&state.mVisibilityTween, fDeltaT))
        {
            state.mVisible
                = state.mVisibilityTween.mValue >= 1.0f;
        }
    }
}

static inline void UpdatePointerTween(
    MegaBallIndicator& state, int nStatus)
{
    if (nStatus > 0 && state.mVisibilityTween.mActive)
    {
        state.mVisibilityTween.mActive = false;
        state.mVisibilityTween.mValue = 1.0f;
    }
    else if (!state.mVisibilityTween.mActive)
    {
        MegaBallIndicatorTween& tween = state.mVisibilityTween;
        float fTwo = 2.0f;
        float fOne = 1.0f;
        float fStartValue = fOne;
        fStartValue += fTwo;
        tween.mElapsedTime = 0.0f;
        tween.mValue = fStartValue;
        tween.mActive = true;
        tween.mRepeatCount = -1;
        tween.mTransitionTime = 0.2f;
        tween.mEndValue = 0.0f;
        tween.mEndTime = 0.4f;
        tween.mStartValue = fStartValue;
    }
}

void UpdateAndRenderMegaBallPointer(float fDeltaT)
{
    DPDData* pData;
    unsigned int nClosest;
    PadBackend* pPlatform;
    cGlobalPad* pController = gMegaBallController;
    if (pController == 0)
    {
        goto no_controller_data;
    }
    pPlatform = pController->mBackend;
    if (pPlatform == 0 || !pPlatform->IsConnected())
    {
        goto no_controller_data;
    }
    int nClassID = pPlatform->GetClassID();
    if (nClassID == gWiiRemotePadClassID)
    {
        pData = reinterpret_cast<DPDData*>(
            (u8*)pController->mBackend + 0x1B0);
        goto have_controller_data;
    }
    nClassID = pPlatform->GetClassID();
    if (nClassID == gWiiFreestylePadClassID)
    {
        pData = reinterpret_cast<DPDData*>(
            (u8*)pController->mBackend + 0x1D0);
        goto have_controller_data;
    }

no_controller_data:
    pData = 0;

have_controller_data:
    if (pData != 0)
    {
        MegaBallIndicator* pPointer = &gMegaBallPointer;
        UpdateStateTweens(*pPointer, fDeltaT);

        nlVector2 v2Position;
        u16 nAngle;
        int nStatus = pData->GetPosition(&v2Position, &nAngle);
        float fX = -gMegaBallPointerXScale * v2Position.x + 320.0f;
        fX = nlMaxEquals(fX, 30.0f);
        fX = nlMinEquals(fX, 610.0f);
        float fY = -gMegaBallPointerYScale * v2Position.y + 240.0f;
        fY = nlMaxEquals(fY, 30.0f);
        fY = nlMinEquals(fY, 450.0f);
        if (gMegaBallPointerMagnetism || lbl_806E15C4)
        {
            float fRadiusSquared
                = gMegaBallPointerMagnetRadius
                * gMegaBallPointerMagnetRadius;
            MegaBallIndicator* pTargetState
                = GetMegaBallTargetIndicator(0);
            float fBestDistance = 1000000.0f;
            nClosest = (unsigned int)-1;
            if (pTargetState != 0 && pTargetState->mActive
                && pTargetState->mVisible)
            {
                float fDeltaX;
                float fDeltaY;
                fDeltaY = fY - pTargetState->mY;
                fDeltaX = fX - pTargetState->mX;
                float fDistance
                    = fDeltaX * fDeltaX + fDeltaY * fDeltaY;
                if (fDistance < fBestDistance)
                {
                    fBestDistance = fDistance;
                }
            }

            MegaBallIndicator* pState;
            float* pDistance;
            float fDistances[10];
            pState = GetMegaBallIndicator(0);
            pDistance = fDistances;
            for (unsigned int i = 0; i < 10; i++)
            {
                if (pState != 0 && pState->mActive)
                {
                    float fDeltaX;
                    float fDeltaY;
                    fDeltaY = fY - pState->mY;
                    fDeltaX = fX - pState->mX;
                    float fDistance
                        = fDeltaX * fDeltaX + fDeltaY * fDeltaY;
                    if (fDistance < fBestDistance)
                    {
                        fBestDistance = fDistance;
                        nClosest = i;
                    }
                    *pDistance = fDistance;
                }
                else
                {
                    *pDistance = 1000000.0f;
                }
                pState++;
                pDistance++;
            }

            if (fBestDistance < fRadiusSquared)
            {
                float fSecondRadius = gMegaBallPointerMagnetRadius;
                float fSecondDistance = fSecondRadius * fSecondRadius;
                unsigned int i;
                unsigned int nSecond = nClosest;
                for (i = 0; i < 10; i++)
                {
                    if (i != nClosest
                        && fDistances[i] < fSecondDistance)
                    {
                        fSecondDistance = fDistances[i];
                        nSecond = i;
                    }
                }

                if (nSecond != nClosest)
                {
                    fSecondRadius = nlSqrt(fSecondDistance, true);
                }
                MegaBallIndicator* pTarget;
                if (nClosest < 10)
                {
                    pTarget = GetMegaBallIndicator(nClosest);
                }
                else
                {
                    pTarget = GetMegaBallTargetIndicator(0);
                }

                float fCurve = gMegaBallPointerMagnetCurve;
                float fCurveOffset = 1.0f + fCurve;
                float fNumerator = -fCurve * fCurveOffset;
                float fDistance = nlSqrt(fBestDistance, true);
                float fTargetX = pTarget->mX;
                float fTargetY = pTarget->mY;
                float fRatio = fDistance / fSecondRadius;
                float fParam
                    = (fCurveOffset
                          + fNumerator / (fRatio + fCurve))
                    / fRatio;
                fX = fTargetX + fParam * (fX - fTargetX);
                fY = fTargetY + fParam * (fY - fTargetY);
            }
        }

        pPointer->mX = fX;
        pPointer->mY = fY;
        u16 nPointerAngle = nAngle;
        SetMegaBallIndicatorTexture(pPointer, pPointer->mTextureIndex);
        UpdatePointerTween(*pPointer, nStatus);
        pPointer->mAngle
            = 0.0000958738f * (float)nPointerAngle;

        if (g_pNetworkSession->IsLiveNetworkGame())
        {
            SendMegaBallPointerUpdate(nAngle, pPointer->mTextureIndex, nStatus, fX, fY);
        }
        DrawVisibleState(*pPointer);
    }
    else if (g_pNetworkSession->IsLiveNetworkGame() && gRemoteMegaBallPointerReceived)
    {
        u16 nAngle;
        int nStatus;
        float fOpacity;
        float fX;
        float fY;
        MegaBallIndicator* pPointer = &gMegaBallPointer;
        UpdateStateTweens(*pPointer, fDeltaT);
        nStatus = gRemoteMegaBallPointerStatus;
        nAngle = gRemoteMegaBallPointerAngle;
        fOpacity = gRemoteMegaBallPointerOpacity;
        fY = gRemoteMegaBallPointerY;
        fX = gRemoteMegaBallPointerX;
        pPointer->mOpacity = fOpacity;
        pPointer->mX = fX;
        pPointer->mY = fY;
        SetMegaBallIndicatorTexture(pPointer, gRemoteMegaBallPointerTextureIndex);
        UpdatePointerTween(*pPointer, nStatus);
        pPointer->mAngle
            = 0.0000958738f * (float)nAngle;
        DrawVisibleState(*pPointer);
    }
}

void RenderMegaBallIndicators()
{
    MegaBallIndicator* pFirstState = gMegaBallIndicators;
    MegaBallIndicator* pSecondState = gMegaBallTargetIndicators;
    unsigned int i;
    for (i = 0; i < 10; i++)
    {
        if (pFirstState != 0)
        {
            DrawState(*pFirstState);
        }
        if (pSecondState != 0)
        {
            DrawState(*pSecondState);
        }
        pFirstState++;
        pSecondState++;
    }

    unsigned int j;
    pFirstState = gMegaBallCatchIndicators;
    for (j = 0; j < 10; j++)
    {
        if (pFirstState != 0)
        {
            DrawState(*pFirstState);
        }
        pFirstState++;
    }
}

void UpdateMegaBallIndicators(float fDeltaT)
{
    MegaBallIndicator* pFirstState = gMegaBallIndicators;
    MegaBallIndicator* pSecondState = gMegaBallTargetIndicators;
    unsigned int i;
    for (i = 0; i < 10; i++)
    {
        if (pFirstState != 0 && pFirstState->mActive)
        {
            UpdateStateTweens(*pFirstState, fDeltaT);
        }
        if (pSecondState != 0 && pSecondState->mActive)
        {
            UpdateStateTweens(*pSecondState, fDeltaT);
        }
        pFirstState++;
        pSecondState++;
    }

    unsigned int j;
    pFirstState = gMegaBallCatchIndicators;
    for (j = 0; j < 10; j++)
    {
        if (pFirstState != 0 && pFirstState->mActive)
        {
            UpdateStateTweens(*pFirstState, fDeltaT);
        }
        pFirstState++;
    }
}

void SetMegaBallTimerCount(unsigned int nCount)
{
    float fY;
    float fX;
    float fLeftCapWidth;
    float fRightCapWidth;
    gMegaBallTimerCount = nCount;
    if (nCount == 0)
    {
        ResetMegaBallTimer();
        return;
    }

    bool bWideScreen = IsWidescreen();
    float fAspectScale = 1.0f;
    if (bWideScreen)
    {
        fAspectScale = 0.8f;
    }
    gMegaBallTimerSegments[0].mScale = gMegaBallTimerScale;
    float fSpacing = fAspectScale
                       * (gMegaBallTimerSegments[0].mWidth
                           * gMegaBallTimerSegments[0].mScale)
                   - 1.0f;
    float fHalfWidth = 0.5f * fSpacing;
    fLeftCapWidth
        = gMegaBallTimerEndCaps[0].mWidth * gMegaBallTimerScale;
    fRightCapWidth
        = gMegaBallTimerEndCaps[1].mWidth * gMegaBallTimerScale;
    float fRowHalfWidth
        = fHalfWidth * (float)(gMegaBallTimerCount - 1);
    fHalfWidth += fRowHalfWidth;
    float fLeftCapX = gMegaBallScreenCenterX - fHalfWidth
                    - 0.5f * (fAspectScale * fLeftCapWidth) + 1.0f;
    float fRightCapX = gMegaBallScreenCenterX + fHalfWidth
                     + 0.5f * (fAspectScale * fRightCapWidth) - 1.0f;
    fY = gMegaBallTimerY + gMegaBallTimerYOffset;
    fX = gMegaBallScreenCenterX - fRowHalfWidth;

    gMegaBallTimerEndCaps[0].mActive = true;
    gMegaBallTimerEndCaps[0].mVisible = true;
    gMegaBallTimerEndCaps[0].mScale = gMegaBallTimerScale;
    gMegaBallTimerEndCaps[0].mX = fLeftCapX;
    gMegaBallTimerEndCaps[0].mY = fY;

    gMegaBallTimerEndCaps[1].mActive = true;
    gMegaBallTimerEndCaps[1].mVisible = true;
    gMegaBallTimerEndCaps[1].mScale = gMegaBallTimerScale;
    gMegaBallTimerEndCaps[1].mX = fRightCapX;
    gMegaBallTimerEndCaps[1].mY = gMegaBallTimerY;

    unsigned int i = 0;
    for (; i < gMegaBallTimerCount; i++)
    {
        gMegaBallTimerSegments[i].mActive = true;
        gMegaBallTimerSegments[i].mVisible = true;
        gMegaBallTimerSegments[i].mScale = gMegaBallTimerScale;
        gMegaBallTimerSegments[i].mX = fX;
        gMegaBallTimerSegments[i].mY = gMegaBallTimerY;
        gMegaBallTimerStatuses[i] = -1;
        fX += fSpacing;
    }
    for (; i < 10; i++)
    {
        gMegaBallTimerSegments[i].mActive = false;
        gMegaBallTimerSegments[i].mVisible = false;
    }
}

void SetMegaBallTimerStatus(unsigned int nIndex, int nValue)
{
    gMegaBallTimerStatuses[nIndex] = nValue;
}

void RenderMegaBallTimer(float)
{
    if (gMegaBallTimerCount != 0 && gMegaBallTimerVisible)
    {
        for (unsigned int i = 0; i < gMegaBallTimerCount; i++)
        {
            MegaBallIndicator& state = gMegaBallTimerSegments[i];
            if (state.mVisible && state.mOpacity > 0.0f)
            {
                float fX;
                float fY;
                fY = state.mY;
                fX = state.mX;
                int nX = (int)fX;
                int nY = (int)fY;
                float fHeight = state.mHeight;
                float fScale = state.mScale;
                float fWidth = state.mWidth;
                fHeight *= fScale;
                fWidth *= fScale;
                float fAngle = state.mAngle;
                DrawMegaBallIndicator(nX,
                    nY,
                    state.mTextureId,
                    fWidth,
                    fHeight,
                    state.mOpacity,
                    fAngle);
            }
            float fWidth;
            float fHeight;
            if (gMegaBallTimerStatuses[i] == 0)
            {
                float fX;
                float fY;
                fY = state.mY;
                fX = state.mX;
                int nX = (int)fX;
                int nY = (int)fY;
                float fScale = gMegaBallTimerScale;
                fWidth = gMegaBallTimerBallWidth;
                fHeight = gMegaBallTimerBallHeight;
                fWidth = fScale * fWidth;
                fHeight = fScale * fHeight;
                float fOpacity = state.mOpacity;
                DrawMegaBallIndicator(nX,
                    nY,
                    gMegaBallIndicatorTextures[3],
                    fWidth,
                    fHeight,
                    fOpacity,
                    0.0f);
            }
            else if (gMegaBallTimerStatuses[i] == 1)
            {
                float fX;
                float fY;
                fY = state.mY;
                fX = state.mX;
                int nX = (int)fX;
                int nY = (int)fY;
                float fScale = gMegaBallTimerScale;
                fWidth = gMegaBallTimerExplodedWidth;
                fHeight = gMegaBallTimerExplodedHeight;
                fWidth = fScale * fWidth;
                fHeight = fScale * fHeight;
                float fOpacity = state.mOpacity;
                DrawMegaBallIndicator(nX,
                    nY,
                    gMegaBallIndicatorTextures[4],
                    fWidth,
                    fHeight,
                    fOpacity,
                    0.0f);
            }
        }
        MegaBallIndicator& leftCap = gMegaBallTimerEndCaps[0];
        int nLeftX = (int)leftCap.mX;
        int nLeftY = (int)leftCap.mY;
        float fLeftHeight = leftCap.mHeight;
        float fLeftScale = leftCap.mScale;
        float fLeftWidth = leftCap.mWidth;
        fLeftHeight *= fLeftScale;
        fLeftWidth *= fLeftScale;
        float fLeftOpacity = leftCap.mOpacity;
        DrawMegaBallIndicator(nLeftX,
            nLeftY,
            leftCap.mTextureId,
            fLeftWidth,
            fLeftHeight,
            fLeftOpacity,
            3.1415927f);

        MegaBallIndicator& rightCap = gMegaBallTimerEndCaps[1];
        if (rightCap.mVisible
            && rightCap.mOpacity > 0.0f)
        {
            float fRightX;
            float fRightY;
            fRightY = rightCap.mY;
            fRightX = rightCap.mX;
            int nRightX = (int)fRightX;
            int nRightY = (int)fRightY;
            float fRightHeight = rightCap.mHeight;
            float fRightScale = rightCap.mScale;
            float fRightWidth = rightCap.mWidth;
            fRightHeight *= fRightScale;
            fRightWidth *= fRightScale;
            float fRightAngle = rightCap.mAngle;
            DrawMegaBallIndicator(nRightX,
                nRightY,
                rightCap.mTextureId,
                fRightWidth,
                fRightHeight,
                rightCap.mOpacity,
                fRightAngle);
        }
    }
}

void UpdateAndRenderMegaBallIndicators(float fDeltaT)
{
    RenderMegaBallIndicators();
    RenderMegaBallTimer(fDeltaT);
    UpdateAndRenderMegaBallPointer(fDeltaT);
}

void SendMegaBallPointerUpdate(u16 nAngle, u32 nTextureIndex,
    u32 nStatus, float fX, float fY)
{
    if (gSuppressMegaBallPointerPackets)
    {
        return;
    }
    u32 nTicker = nlGetTicker();
    if (nlGetTickerDifference(gLastMegaBallPointerPacketTicker, nTicker)
        < gMegaBallPointerPacketInterval)
    {
        return;
    }
    gLastMegaBallPointerPacketTicker = nTicker;

    NetMessageMegaBallPointer message;
    message.mPointerX = (s16)(int)fX;
    message.mPointerY = (s16)(int)fY;
    message.mAngleHighByte = (u8)(nAngle >> 8);
    message.mTextureIndex = (u8)nTextureIndex;
    message.mStatus = (u8)nStatus;

    u8 buffer[50];
    int nSize = gNetworkMessageRegistry->Serialize(&message, buffer, sizeof(buffer));
    int nPlayerCount = g_pNetworkSessionBase->GetNumMachines();
    for (s8 i = 0; i < nPlayerCount; i++)
    {
        if (i != g_pNetworkSessionBase->GetLocalMachineId())
        {
            g_pNetworkSessionBase->Send(i, buffer, nSize, false);
        }
    }
}

void ReceiveMegaBallPointerUpdate(void* pMessage)
{
    u8* pData = (u8*)pMessage;
    gRemoteMegaBallPointerReceived = true;
    gRemoteMegaBallPointerX = (float)*(s16*)(pData + 8);
    gRemoteMegaBallPointerY = (float)*(s16*)(pData + 10);
    gRemoteMegaBallPointerAngle = (u16)(pData[12] << 8);
    gRemoteMegaBallPointerTextureIndex = pData[13];
    gRemoteMegaBallPointerStatus = pData[14];
}

inline MegaBallIndicator::MegaBallIndicator()
{
    ResetMegaBallIndicator(this, 0);
}
