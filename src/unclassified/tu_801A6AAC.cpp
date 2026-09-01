#include "unclassified/tu_801A6AAC.h"

#include "Game/AI/AiUtil.h"
#include "Game/NetworkSession.h"
#include "NL/gl/glDraw2.h"
#include "NL/gl/glState.h"
#include "NL/gl/glTexture.h"
#include "NL/nlMath.h"
#include "NL/nlString.h"
#include "NL/nlTicker.h"

struct UnidentifiedControllerInfo_801A7C48
{
    u8 mUnidentified000[0x1C];
    void* mUnidentified01C;
};

struct UnidentifiedMegaBallMessage
{
    void* mVTable;
    u32 mUnidentified004;
    s16 mX;
    s16 mY;
    u8 mAngle;
    u8 mTextureIndex;
    u8 mStatus;
};

extern "C"
{
    int fn_802C2C84(const char* pPath, int nDefault);
    GLView* fn_8027267C(int nIndex);
    bool fn_80273B00();
    int fn_80364630(void* pData, nlVector2* pPosition, u16* pAngle);
    u32 fn_8032C830(void* pCodec, void* pMessage, void* pBuffer, int nSize);
    void fn_801A8F1C(u16 nAngle, u32 nTextureIndex, u32 nStatus,
        float fX, float fY);

    extern void* lbl_806E2100;
    extern int lbl_806E227C;
    extern int lbl_806E228C;
    extern u8 lbl_8050ADCC[];
}

static char lbl_80514210[] = "/Rendering/Engine/Reduce Textures";
static char lbl_80514238[] = "global/the_ball";
static char lbl_80514248[] = "global/the_ball_exploded";
static char lbl_80514264[] = "global/timer_empty";
static char lbl_80514278[] = "global/timer_endcap_right";

float lbl_806DCFB8 = 0.08f;
float lbl_806DCFBC = 25.0f;
float lbl_806DCFC0 = 10.0f;
float lbl_806DCFC4 = 400.0f;
float lbl_806DCFC8 = 300.0f;
float lbl_806DCFCC = 25.0f;
float lbl_806DCFD0 = -10.05f;
float lbl_806DCFD4 = 30.0f;
float lbl_806DCFD8 = 0.666f;
float lbl_806DCFDC = 0.666f;
float lbl_806DCFE0 = 320.0f;
float lbl_806DCFE4 = 54.0f;
float lbl_806DCFE8 = 54.0f;
float lbl_806DCFEC = 54.0f;
float lbl_806DCFF0 = 54.0f;
float lbl_806DCFF4 = 430.0f;
float lbl_806DCFF8 = 1.0f;
int lbl_806DCFFC = 1;

void* lbl_806E15C0;
bool lbl_806E15C4;
bool lbl_806E15C5;
bool lbl_806E15C6;
float lbl_806E15C8;
u32 lbl_806E15CC = nlStringLowerHash(lbl_80514238);
u32 lbl_806E15D0;
u32 lbl_806E15D4;
u32 lbl_806E15D8 = nlStringLowerHash(lbl_80514238);
u32 lbl_806E15DC = nlStringLowerHash(lbl_80514248);
u32 lbl_806E15E0 = nlStringLowerHash(lbl_80514264);
u32 lbl_806E15E4 = nlStringLowerHash(lbl_80514278);
u32 lbl_806E15E8;
bool lbl_806E15EC;
bool lbl_806E15ED;
u32 lbl_806E15F0;
bool lbl_806E15F4;
float lbl_806E15F8;
float lbl_806E15FC;
u16 lbl_806E1600;
u16 lbl_806E1602;

u32 lbl_80572410[8] = { lbl_806E15CC, lbl_806E15D0, lbl_806E15D4, lbl_806E15D8, lbl_806E15DC, lbl_806E15E0, lbl_806E15E4, 0 };
UnidentifiedMegaBallState lbl_80572430[10];
UnidentifiedMegaBallState lbl_805729A8[10];
UnidentifiedMegaBallState lbl_80572F20[10];
UnidentifiedMegaBallState lbl_80573498;
UnidentifiedMegaBallState lbl_80573528[10];
UnidentifiedMegaBallState lbl_80573AA0[2];
int lbl_80573BB8[10];

static inline void ResetTween(UnidentifiedMegaBallTween& tween)
{
    tween.mUnidentified000 = 0.0f;
    tween.mUnidentified004 = 1.0f;
    tween.mUnidentified008 = 0.0f;
    tween.mUnidentified00C = 0.0f;
    tween.mUnidentified010 = 0.0f;
    tween.mUnidentified014 = 0.0f;
    tween.mUnidentified018 = 0;
    tween.mUnidentified01C = false;
}

extern "C" void fn_801A6AAC(
    UnidentifiedMegaBallState* pState, unsigned int nTextureIndex)
{
    pState->mUnidentified000 = lbl_806DCFE0;
    pState->mUnidentified004 = 240.0f;
    pState->mUnidentified010 = 1.0f;
    pState->mUnidentified014 = 1.0f;
    pState->mUnidentified018 = 0.0f;
    pState->mUnidentified008 = 100.0f;
    pState->mUnidentified00C = 100.0f;
    pState->mUnidentified020 = lbl_80572410[nTextureIndex];
    pState->mUnidentified024 = nTextureIndex;
    pState->mUnidentified028 = false;
    pState->mUnidentified029 = false;
    ResetTween(pState->mUnidentified02C);
    ResetTween(pState->mUnidentified04C);
    ResetTween(pState->mUnidentified06C);
}

extern "C" void fn_801A6B64(
    UnidentifiedMegaBallState* pState, unsigned int nTextureIndex)
{
    pState->mUnidentified020 = lbl_80572410[nTextureIndex];
    pState->mUnidentified024 = nTextureIndex;
    if (glTextureLoad(pState->mUnidentified020))
    {
        pState->mUnidentified008 = (float)(int)glTextureGetWidth();
        pState->mUnidentified00C = (float)(int)glTextureGetHeight();
        if (fn_802C2C84(lbl_80514210, 0))
        {
            pState->mUnidentified008 *= 0.5f;
            pState->mUnidentified00C *= 0.5f;
        }
    }
}

extern "C" bool fn_801A6C34(
    UnidentifiedMegaBallTween* pTween, float fDeltaT)
{
    if (pTween->mUnidentified01C)
    {
        pTween->mUnidentified000 += fDeltaT;
        if (pTween->mUnidentified000 < pTween->mUnidentified010)
        {
            pTween->mUnidentified004 = InterpolateRange(
                pTween->mUnidentified00C, pTween->mUnidentified014, 0.0f, pTween->mUnidentified010, pTween->mUnidentified000);
        }
        else if (pTween->mUnidentified008 > pTween->mUnidentified010)
        {
            if (pTween->mUnidentified000 < pTween->mUnidentified008)
            {
                pTween->mUnidentified004 = InterpolateRange(
                    pTween->mUnidentified014, pTween->mUnidentified00C, pTween->mUnidentified010, pTween->mUnidentified008, pTween->mUnidentified000);
            }
            else
            {
                pTween->mUnidentified004 = pTween->mUnidentified00C;
                if (pTween->mUnidentified018 != 0)
                {
                    if (pTween->mUnidentified018 > 0)
                    {
                        pTween->mUnidentified018--;
                    }
                    pTween->mUnidentified000 = 0.0f;
                }
                else
                {
                    pTween->mUnidentified01C = false;
                }
                return true;
            }
        }
        else
        {
            pTween->mUnidentified004 = pTween->mUnidentified014;
            if (pTween->mUnidentified018 != 0)
            {
                if (pTween->mUnidentified018 > 0)
                {
                    pTween->mUnidentified018--;
                }
                pTween->mUnidentified000 = 0.0f;
            }
            else
            {
                pTween->mUnidentified01C = false;
            }
            return true;
        }
    }
    return pTween->mUnidentified01C;
}

static inline void ConfigureTween(UnidentifiedMegaBallTween& tween,
    int nRepeat,
    float fTransitionTime, float fEnd, float fEndTime, float fStart)
{
    tween.mUnidentified000 = 0.0f;
    if (fTransitionTime > 0.0f)
    {
        tween.mUnidentified004 = fStart;
    }
    else
    {
        tween.mUnidentified004 = fEnd;
    }
    tween.mUnidentified018 = nRepeat;
    tween.mUnidentified01C = true;
    tween.mUnidentified010 = fTransitionTime;
    tween.mUnidentified014 = fEnd;
    tween.mUnidentified008 = fEndTime;
    tween.mUnidentified00C = fStart;
}

extern "C" void fn_801A6D44(UnidentifiedMegaBallState* pState,
    int nRepeat, float fTransitionTime, float fEnd, float fEndTime,
    float fStart)
{
    ConfigureTween(pState->mUnidentified02C, nRepeat, fTransitionTime, fEnd, fEndTime, fStart);
}

extern "C" void fn_801A6D80(UnidentifiedMegaBallState* pState,
    int nRepeat, float fTransitionTime, float fEnd, float fEndTime,
    float fStart)
{
    UnidentifiedMegaBallTween& tween = pState->mUnidentified04C;
    tween.mUnidentified000 = 0.0f;
    if (fTransitionTime > 0.0f)
    {
        tween.mUnidentified004 = fStart;
    }
    else
    {
        tween.mUnidentified004 = fEnd;
    }
    tween.mUnidentified01C = true;
    tween.mUnidentified018 = nRepeat;
    tween.mUnidentified010 = fTransitionTime;
    tween.mUnidentified014 = fEnd;
    tween.mUnidentified008 = fEndTime;
    tween.mUnidentified00C = fStart;
    tween.mUnidentified004 = 1.0f;
}

extern "C" void fn_801A6DC4(UnidentifiedMegaBallState* pState)
{
    pState->mUnidentified04C.mUnidentified01C = false;
    pState->mUnidentified04C.mUnidentified004 = 1.0f;
}

static inline void ResetAndLoad(UnidentifiedMegaBallState& state,
    unsigned int nTextureIndex, unsigned int nIndex)
{
    fn_801A6AAC(&state, nTextureIndex);
    fn_801A6B64(&state, nTextureIndex);
    state.mUnidentified01C = nIndex;
}

extern "C" void fn_801A6DD8()
{
    for (unsigned int i = 0; i < 10; i++)
    {
        ResetAndLoad(lbl_80572430[i], 0, i);
        ResetAndLoad(lbl_805729A8[i], 3, i);
    }
    for (unsigned int i = 0; i < 10; i++)
    {
        ResetAndLoad(lbl_80572F20[i], 2, i);
        lbl_80572F20[i].mUnidentified014 = 0.5f;
    }

    lbl_806E15F0 = nlGetTicker();
    lbl_806E15F4 = false;
    lbl_806E15F8 = 0.0f;
    lbl_806E15FC = 0.0f;
    lbl_806E1600 = 0;
    lbl_806DCFFC = 1;
    lbl_806E1602 = 0;
    fn_801A7258();
}

extern "C" void fn_801A7258()
{
    lbl_806E15E8 = 0;
    lbl_806E15EC = false;

    UnidentifiedMegaBallState* pTimerState = lbl_80573528;
    int* pTimerStatus = lbl_80573BB8;
    for (unsigned int i = 0; i < 10; i++)
    {
        ResetAndLoad(*pTimerState, 5, i);
        *pTimerStatus = -1;
        pTimerState++;
        pTimerStatus++;
    }

    UnidentifiedMegaBallState* pEndCap = lbl_80573AA0;
    for (unsigned int i = 0; i < 2; i++)
    {
        ResetAndLoad(*pEndCap, 6, i);
        pEndCap++;
    }
}

static inline UnidentifiedMegaBallState* AllocateState(
    UnidentifiedMegaBallState* pStates, float fX, float fY,
    float fScale)
{
    UnidentifiedMegaBallState* pState = 0;
    for (unsigned int i = 0; i < 10; i++)
    {
        if (!pStates[i].mUnidentified029)
        {
            pState = &pStates[i];
            break;
        }
    }
    if (pState != 0)
    {
        pState->mUnidentified029 = true;
        pState->mUnidentified028 = true;
        pState->mUnidentified000 = fX;
        pState->mUnidentified004 = fY;
        pState->mUnidentified010 = fScale;
    }
    return pState;
}

extern "C" UnidentifiedMegaBallState* fn_801A75A8(
    float fX, float fY, float fScale)
{
    return AllocateState(lbl_80572430, fX, fY, fScale);
}

extern "C" void fn_801A7610(UnidentifiedMegaBallState* pState)
{
    pState->mUnidentified029 = false;
    pState->mUnidentified028 = false;
}

extern "C" UnidentifiedMegaBallState* fn_801A7620(unsigned int nIndex)
{
    return &lbl_80572430[nIndex];
}

extern "C" UnidentifiedMegaBallState* fn_801A7634(unsigned int nIndex)
{
    return &lbl_805729A8[nIndex];
}

extern "C" UnidentifiedMegaBallState* fn_801A7648(
    float fX, float fY, float fScale, float fAngle)
{
    UnidentifiedMegaBallState* pState
        = AllocateState(lbl_80572F20, fX, fY, fScale);
    if (pState != 0)
    {
        pState->mUnidentified018 = fAngle;
    }
    return pState;
}

extern "C" UnidentifiedMegaBallState* fn_801A76BC(unsigned int nIndex)
{
    return &lbl_80572F20[nIndex];
}

extern "C" void fn_801A76D0(
    unsigned int nTexture1, unsigned int nTexture2)
{
    lbl_80572410[1] = nTexture1;
    lbl_80572410[2] = nTexture2;
}

extern "C" void fn_801A76E4(
    bool bParam, float fX, float fY, float fScale)
{
    lbl_80573498.mUnidentified000 = fX;
    lbl_80573498.mUnidentified004 = fY;
    lbl_80573498.mUnidentified010 = fScale;
    lbl_80573498.mUnidentified018 = 0.0f;
    fn_801A6B64(&lbl_80573498, 1);
    lbl_80573498.mUnidentified029 = true;
    lbl_80573498.mUnidentified028 = true;
    lbl_80573498.mUnidentified014 = lbl_806DCFD8;
    lbl_806E15ED = false;
    lbl_806E15C5 = bParam;
}

extern "C" void fn_801A7800()
{
    fn_801A6AAC(&lbl_80573498, 0);
}

extern "C" float fn_801A78B8(UnidentifiedMegaBallState* pState1,
    UnidentifiedMegaBallState* pState2)
{
    if (pState2 == 0)
    {
        pState2 = &lbl_80573498;
    }
    if (!pState1->mUnidentified029)
    {
        return 0.0f;
    }

    nlVector2 v2Centre;
    nlVec2Set(v2Centre, pState2->mUnidentified000, pState2->mUnidentified004 + lbl_806DCFC0);
    nlVector2 v2Left;
    nlVec2Set(v2Left, -lbl_806DCFBC, lbl_806DCFC0);
    nlVector2 v2Right;
    nlVec2Set(v2Right, lbl_806DCFBC, lbl_806DCFC0);
    nlVector2 v2Position;
    nlVec2Set(v2Position, pState1->mUnidentified000, pState1->mUnidentified004);

    float fSin;
    float fCos;
    u16 nAngle = (u16)(int)(pState2->mUnidentified018 * 10430.378f);
    nlSinCos(&fSin, &fCos, nAngle);

    float fDeltaX = v2Position.x - v2Centre.x;
    float fDeltaY = v2Position.y - v2Centre.y;
    nlVector2 v2Rotated;
    v2Rotated.x = fCos * fDeltaX + fSin * fDeltaY;
    v2Rotated.y = fCos * fDeltaY - fSin * fDeltaX;
    nlVector2 v2LeftDelta;
    nlVec2Sub(v2LeftDelta, v2Left, v2Rotated);
    nlVector2 v2RightDelta;
    nlVec2Sub(v2RightDelta, v2Right, v2Rotated);

    float fRadius = lbl_806DCFB8
                      * (pState2->mUnidentified008 * pState2->mUnidentified010)
                  + 0.5f
                        * (pState1->mUnidentified008 * pState1->mUnidentified010);
    float fRadiusSquared = fRadius * fRadius;
    if (nlVec2LengthSquared(v2LeftDelta) < fRadiusSquared
        || nlVec2LengthSquared(v2RightDelta) < fRadiusSquared)
    {
        return 1.0f;
    }
    return 0.0f;
}

static inline void DrawState(const UnidentifiedMegaBallState& state)
{
    if (state.mUnidentified029 && state.mUnidentified028
        && state.mUnidentified014 > 0.0f)
    {
        fn_801A7A3C((int)state.mUnidentified000,
            (int)state.mUnidentified004,
            state.mUnidentified020,
            state.mUnidentified008 * state.mUnidentified010,
            state.mUnidentified00C * state.mUnidentified010,
            state.mUnidentified014,
            state.mUnidentified018);
    }
}

extern "C" void fn_801A7A3C(int nX, int nY, unsigned int nTexture,
    float fWidth, float fHeight, float fOpacity, float fAngle)
{
    bool bWideScreen = fn_80273B00();
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
            float fAbsCos = nlAbs(fCos);
            float fAbsSin = nlAbs(fSin);
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
    poly.Attach(fn_8027267C(0x26), 0, 0);
}

extern "C" void fn_801A7C40(void* pControllerInfo)
{
    lbl_806E15C0 = pControllerInfo;
}

static inline void* GetMegaBallControllerData()
{
    UnidentifiedControllerInfo_801A7C48* pInfo
        = (UnidentifiedControllerInfo_801A7C48*)lbl_806E15C0;
    if (pInfo == 0 || pInfo->mUnidentified01C == 0)
    {
        return 0;
    }

    void* pPlatform = pInfo->mUnidentified01C;
    void** pVTable = *(void***)pPlatform;
    typedef int (*ControllerMethod)(void*);
    if (!((ControllerMethod)pVTable[3])(pPlatform))
    {
        return 0;
    }
    int nType = ((ControllerMethod)pVTable[20])(pPlatform);
    if (nType == lbl_806E227C)
    {
        return (u8*)pPlatform + 0x1B0;
    }
    if (nType == lbl_806E228C)
    {
        return (u8*)pPlatform + 0x1D0;
    }
    return 0;
}

static inline void UpdateStateTweens(
    UnidentifiedMegaBallState& state, float fDeltaT)
{
    if (state.mUnidentified029)
    {
        if (fn_801A6C34(&state.mUnidentified02C, fDeltaT))
        {
            state.mUnidentified010
                = state.mUnidentified02C.mUnidentified004;
        }
        if (fn_801A6C34(&state.mUnidentified04C, fDeltaT))
        {
            state.mUnidentified014
                = state.mUnidentified04C.mUnidentified004;
        }
        if (fn_801A6C34(&state.mUnidentified06C, fDeltaT))
        {
            state.mUnidentified028
                = state.mUnidentified06C.mUnidentified004 >= 1.0f;
        }
    }
}

static inline void UpdatePointerTween(
    UnidentifiedMegaBallState& state, int nStatus)
{
    if (nStatus > 0 && state.mUnidentified06C.mUnidentified01C)
    {
        state.mUnidentified06C.mUnidentified01C = false;
        state.mUnidentified06C.mUnidentified004 = 1.0f;
    }
    else if (!state.mUnidentified06C.mUnidentified01C)
    {
        ConfigureTween(state.mUnidentified06C, -1, 0.2f, 0.0f, 3.0f, 1.5f);
    }
}

static inline void MagnetizePointer(float& fX, float& fY)
{
    if (!lbl_806E15C5 && !lbl_806E15C4)
    {
        return;
    }

    float fBestDistance = 1000000.0f;
    int nClosest = -1;
    if (lbl_805729A8[0].mUnidentified029
        && lbl_805729A8[0].mUnidentified028)
    {
        float fDeltaX = fX - lbl_805729A8[0].mUnidentified000;
        float fDeltaY = fY - lbl_805729A8[0].mUnidentified004;
        float fDistance = fDeltaX * fDeltaX + fDeltaY * fDeltaY;
        if (fDistance < fBestDistance)
        {
            fBestDistance = fDistance;
        }
    }

    float fDistances[10];
    for (int i = 0; i < 10; i++)
    {
        if (lbl_80572430[i].mUnidentified029)
        {
            float fDeltaX = fX - lbl_80572430[i].mUnidentified000;
            float fDeltaY = fY - lbl_80572430[i].mUnidentified004;
            fDistances[i] = fDeltaX * fDeltaX + fDeltaY * fDeltaY;
            if (fDistances[i] < fBestDistance)
            {
                fBestDistance = fDistances[i];
                nClosest = i;
            }
        }
        else
        {
            fDistances[i] = 1000000.0f;
        }
    }

    float fRadiusSquared = 25.0f * 25.0f;
    if (fBestDistance >= fRadiusSquared)
    {
        return;
    }

    float fSecondDistance = fRadiusSquared;
    int nSecond = nClosest;
    for (int i = 0; i < 10; i++)
    {
        if (i != nClosest && fDistances[i] < fSecondDistance)
        {
            fSecondDistance = fDistances[i];
            nSecond = i;
        }
    }

    float fSecondRadius = 25.0f;
    if (nSecond != nClosest)
    {
        fSecondRadius = nlSqrt(fSecondDistance, true);
    }
    UnidentifiedMegaBallState* pTarget;
    if ((unsigned int)nClosest < 10)
    {
        pTarget = &lbl_80572430[nClosest];
    }
    else
    {
        pTarget = &lbl_805729A8[0];
    }

    float fDistance = nlSqrt(fBestDistance, true);
    float fRatio = fDistance / fSecondRadius;
    float fParam = (1.0f + lbl_806DCFD0
                       + (-lbl_806DCFD0 * (1.0f + lbl_806DCFD0))
                             / (fRatio + lbl_806DCFD0))
                 / fRatio;
    fX = pTarget->mUnidentified000
       + fParam * (fX - pTarget->mUnidentified000);
    fY = pTarget->mUnidentified004
       + fParam * (fY - pTarget->mUnidentified004);
}

extern "C" void fn_801A7C48(float fDeltaT)
{
    void* pData = GetMegaBallControllerData();
    if (pData != 0)
    {
        UpdateStateTweens(lbl_80573498, fDeltaT);

        nlVector2 v2Position;
        u16 nAngle;
        int nStatus = fn_80364630(pData, &v2Position, &nAngle);
        float fX = 320.0f - 400.0f * v2Position.x;
        if (fX < 30.0f)
        {
            fX = 30.0f;
        }
        if (fX > 610.0f)
        {
            fX = 610.0f;
        }
        float fY = 240.0f - 300.0f * v2Position.y;
        if (fY < 30.0f)
        {
            fY = 30.0f;
        }
        if (fY > 450.0f)
        {
            fY = 450.0f;
        }
        MagnetizePointer(fX, fY);

        lbl_80573498.mUnidentified000 = fX;
        lbl_80573498.mUnidentified004 = fY;
        fn_801A6B64(&lbl_80573498, lbl_80573498.mUnidentified024);
        UpdatePointerTween(lbl_80573498, nStatus);
        lbl_80573498.mUnidentified018
            = 0.0000958738f * (float)nAngle;

        if (lbl_806E10EC->fn_80123314())
        {
            fn_801A8F1C(nAngle, lbl_80573498.mUnidentified024, nStatus, fX, fY);
        }
        DrawState(lbl_80573498);
    }
    else if (lbl_806E10EC->fn_80123314() && lbl_806E15F4)
    {
        UpdateStateTweens(lbl_80573498, fDeltaT);
        lbl_80573498.mUnidentified000 = lbl_806E15F8;
        lbl_80573498.mUnidentified004 = lbl_806E15FC;
        lbl_80573498.mUnidentified014 = 0.666f;
        fn_801A6B64(&lbl_80573498, lbl_806DCFFC);
        UpdatePointerTween(lbl_80573498, lbl_806E1602);
        lbl_80573498.mUnidentified018
            = 0.0000958738f * (float)lbl_806E1600;
        DrawState(lbl_80573498);
    }
}

extern "C" void fn_801A8504()
{
    UnidentifiedMegaBallState* pFirstState = lbl_80572430;
    UnidentifiedMegaBallState* pSecondState = lbl_805729A8;
    for (unsigned int i = 0; i < 10; i++)
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

    UnidentifiedMegaBallState* pThirdState = lbl_80572F20;
    for (unsigned int i = 0; i < 10; i++)
    {
        if (pThirdState != 0)
        {
            DrawState(*pThirdState);
        }
        pThirdState++;
    }
}

extern "C" void fn_801A86D8(float fDeltaT)
{
    UnidentifiedMegaBallState* pFirstState = lbl_80572430;
    UnidentifiedMegaBallState* pSecondState = lbl_805729A8;
    for (unsigned int i = 0; i < 10; i++)
    {
        if (pFirstState != 0 && pFirstState->mUnidentified029)
        {
            UpdateStateTweens(*pFirstState, fDeltaT);
        }
        if (pSecondState != 0 && pSecondState->mUnidentified029)
        {
            UpdateStateTweens(*pSecondState, fDeltaT);
        }
        pFirstState++;
        pSecondState++;
    }

    UnidentifiedMegaBallState* pThirdState = lbl_80572F20;
    for (unsigned int i = 0; i < 10; i++)
    {
        if (pThirdState != 0 && pThirdState->mUnidentified029)
        {
            UpdateStateTweens(*pThirdState, fDeltaT);
        }
        pThirdState++;
    }
}

extern "C" void fn_801A8908(unsigned int nCount)
{
    lbl_806E15E8 = nCount;
    if (nCount == 0)
    {
        fn_801A7258();
        return;
    }

    float fAspectScale = fn_80273B00() ? 0.8f : 1.0f;
    float fSpacing = fAspectScale
                       * (lbl_80573528[0].mUnidentified008 * lbl_806DCFF8)
                   - 1.0f;
    float fHalfWidth = 0.5f * fSpacing;
    float fRowHalfWidth
        = fHalfWidth * (float)(lbl_806E15E8 - 1);
    float fOuterHalfWidth = fRowHalfWidth + fHalfWidth;
    float fY = lbl_806DCFF4 + lbl_806E15C8;

    lbl_80573AA0[0].mUnidentified029 = true;
    lbl_80573AA0[0].mUnidentified028 = true;
    lbl_80573AA0[0].mUnidentified010 = lbl_806DCFF8;
    lbl_80573AA0[0].mUnidentified000 = lbl_806DCFE0
                                     - fOuterHalfWidth
                                     - 0.5f
                                           * (fAspectScale
                                               * (lbl_80573AA0[0].mUnidentified008
                                                   * lbl_806DCFF8))
                                     + 1.0f;
    lbl_80573AA0[0].mUnidentified004 = fY;

    lbl_80573AA0[1].mUnidentified029 = true;
    lbl_80573AA0[1].mUnidentified028 = true;
    lbl_80573AA0[1].mUnidentified010 = lbl_806DCFF8;
    lbl_80573AA0[1].mUnidentified000 = lbl_806DCFE0
                                     + fOuterHalfWidth
                                     + 0.5f
                                           * (fAspectScale
                                               * (lbl_80573AA0[1].mUnidentified008
                                                   * lbl_806DCFF8))
                                     - 1.0f;
    lbl_80573AA0[1].mUnidentified004 = lbl_806DCFF4;

    float fX = lbl_806DCFE0 - fRowHalfWidth;
    unsigned int i = 0;
    for (; i < lbl_806E15E8; i++)
    {
        lbl_80573528[i].mUnidentified029 = true;
        lbl_80573528[i].mUnidentified028 = true;
        lbl_80573528[i].mUnidentified010 = lbl_806DCFF8;
        lbl_80573528[i].mUnidentified000 = fX;
        lbl_80573528[i].mUnidentified004 = lbl_806DCFF4;
        lbl_80573BB8[i] = -1;
        fX += fSpacing;
    }
    for (; i < 10; i++)
    {
        lbl_80573528[i].mUnidentified029 = false;
        lbl_80573528[i].mUnidentified028 = false;
    }
}

extern "C" void fn_801A8C8C(unsigned int nIndex, int nValue)
{
    lbl_80573BB8[nIndex] = nValue;
}

extern "C" void fn_801A8CA0(float)
{
    if (lbl_806E15E8 == 0 || !lbl_806E15EC)
    {
        return;
    }

    for (unsigned int i = 0; i < lbl_806E15E8; i++)
    {
        UnidentifiedMegaBallState& state = lbl_80573528[i];
        if (state.mUnidentified028 && state.mUnidentified014 > 0.0f)
        {
            fn_801A7A3C((int)state.mUnidentified000,
                (int)state.mUnidentified004,
                state.mUnidentified020,
                state.mUnidentified008 * state.mUnidentified010,
                state.mUnidentified00C * state.mUnidentified010,
                state.mUnidentified014,
                state.mUnidentified018);
        }
        if (lbl_80573BB8[i] == 0)
        {
            fn_801A7A3C((int)lbl_80573528[i].mUnidentified000,
                (int)lbl_80573528[i].mUnidentified004,
                lbl_80572410[3],
                lbl_806DCFE4 * lbl_80573528[i].mUnidentified010,
                lbl_806DCFE8 * lbl_80573528[i].mUnidentified010,
                lbl_80573528[i].mUnidentified014,
                0.0f);
        }
        else if (lbl_80573BB8[i] == 1)
        {
            fn_801A7A3C((int)lbl_80573528[i].mUnidentified000,
                (int)lbl_80573528[i].mUnidentified004,
                lbl_80572410[4],
                lbl_806DCFEC * lbl_80573528[i].mUnidentified010,
                lbl_806DCFF0 * lbl_80573528[i].mUnidentified010,
                lbl_80573528[i].mUnidentified014,
                0.0f);
        }
    }
    UnidentifiedMegaBallState& leftCap = lbl_80573AA0[0];
    fn_801A7A3C((int)leftCap.mUnidentified000,
        (int)leftCap.mUnidentified004,
        leftCap.mUnidentified020,
        leftCap.mUnidentified008 * leftCap.mUnidentified010,
        leftCap.mUnidentified00C * leftCap.mUnidentified010,
        leftCap.mUnidentified014,
        3.1415927f);

    UnidentifiedMegaBallState& rightCap = lbl_80573AA0[1];
    if (rightCap.mUnidentified028
        && rightCap.mUnidentified014 > 0.0f)
    {
        fn_801A7A3C((int)rightCap.mUnidentified000,
            (int)rightCap.mUnidentified004,
            rightCap.mUnidentified020,
            rightCap.mUnidentified008 * rightCap.mUnidentified010,
            rightCap.mUnidentified00C * rightCap.mUnidentified010,
            rightCap.mUnidentified014,
            rightCap.mUnidentified018);
    }
}

extern "C" void fn_801A8EE0(float fDeltaT)
{
    fn_801A8504();
    fn_801A8CA0(fDeltaT);
    fn_801A7C48(fDeltaT);
}

extern "C" void fn_801A8F1C(u16 nAngle, u32 nTextureIndex,
    u32 nStatus, float fX, float fY)
{
    if (lbl_806E15C6)
    {
        return;
    }
    u32 nTicker = nlGetTicker();
    if (nlGetTickerDifference(lbl_806E15F0, nTicker) < 30.0f)
    {
        return;
    }
    lbl_806E15F0 = nTicker;

    UnidentifiedMegaBallMessage message;
    message.mUnidentified004 = 0;
    message.mVTable = lbl_8050ADCC;
    message.mX = (s16)(int)fX;
    message.mY = (s16)(int)fY;
    message.mAngle = (u8)(nAngle >> 8);
    message.mTextureIndex = (u8)nTextureIndex;
    message.mStatus = (u8)nStatus;

    u8 buffer[50];
    int nSize = fn_8032C830(
        lbl_806E2100, &message, buffer, sizeof(buffer));
    int nPlayerCount = fn_80338BF0(lbl_806E20D8);
    for (s8 i = 0; i < nPlayerCount; i++)
    {
        if (i != fn_80338C20(lbl_806E20D8))
        {
            lbl_806E20D8->Send(i, buffer, nSize, false);
        }
    }
}

extern "C" void fn_801A9074(void* pMessage)
{
    u8* pData = (u8*)pMessage;
    lbl_806E15F4 = true;
    lbl_806E15F8 = (float)*(s16*)(pData + 8);
    lbl_806E15FC = (float)*(s16*)(pData + 10);
    lbl_806E1600 = (u16)(pData[12] << 8);
    lbl_806DCFFC = pData[13];
    lbl_806E1602 = pData[14];
}

inline UnidentifiedMegaBallState::UnidentifiedMegaBallState()
{
    fn_801A6AAC(this, 0);
}
