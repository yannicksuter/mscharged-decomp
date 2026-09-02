#include "Game/Render/Indicators.h"

#include "Game/AI/AiUtil.h"
#include "Game/CharacterTweaks.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/Game.h"
#include "Game/MathHelpers.h"
#include "Game/NetworkSession.h"
#include "Game/Player.h"
#include "Game/ReplayManager.h"
#include "Game/Team.h"
#include "NL/gl/glDraw2.h"
#include "NL/gl/glState.h"
#include "NL/gl/glTexture.h"
#include "NL/gl/glView.h"
#include "NL/globalpad.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"
#include "NL/nlString.h"
#include "types.h"

struct IndicatorPadOwner
{
    /* 0x00 */ UnidentifiedNetworkPeer* mPeer;
    /* 0x04 */ s8 mPadIndex;
};

struct IndicatorGlobalPadState
{
    /* 0x00 */ u8 mUnidentified00[0x40];
    /* 0x40 */ IndicatorPadOwner* mOwner;
};

struct IndicatorControllerInfo
{
    /* 0x00 */ u32 mUnidentified00;
    /* 0x04 */ int mPadIndex;
};

struct IndicatorPlayerState
{
    /* 0x000 */ u8 mUnidentified000[0x1B4];
    /* 0x1B4 */ nlVector3 mScreenPosition;
    /* 0x1C0 */ u8 mUnidentified1C0[0xAC];
    /* 0x26C */ float mSwitchScale;
};

struct IndicatorPlayerTweaks
{
    /* 0x000 */ u8 mUnidentified000[0x10];
    /* 0x010 */ float* fPhysCapsuleHeight;
    /* 0x014 */ u8 mUnidentified014[0x2E0];
    /* 0x2F4 */ float* fGoalieCapsuleHeight;
};

struct IndicatorCharacterInfoState
{
    /* 0x000 */ u8 mUnidentified000[0x11C];
    /* 0x11C */ CharacterInfo* mCharacterInfo;
};

extern "C"
{
    extern cPlayer* lbl_8056B800[10];

    bool fn_8001E184(cPlayer* pCharacter);
    bool fn_800387CC(cPlayer* pCharacter);
    IndicatorPlayerTweaks* fn_8003E6E4(cPlayer* pCharacter);
    GLView* fn_8027267C(int index);
    void fn_802CE528(GLView* view, const nlVector3* world, nlVector3* projected);
    void fn_802CE6DC(GLView* view, const nlVector3* normalized, nlVector3* screen);
    float fn_802CE76C(GLView* view);
    void fn_802CEA40(GLView* source, GLView* destination, const nlVector3* world, nlVector3* projected);
    IndicatorPadOwner* fn_80336B6C(UnidentifiedNetworkPeer* peer, int index);
    IndicatorControllerInfo* fn_80336D90(IndicatorPadOwner* owner);
    UnidentifiedNetworkPeer* fn_80338C0C(UnidentifiedNetworkSession* session);
}

static float s_fOverheadSize = 35.0f;
static float lbl_806DCED4 = 15.0f;
static float lbl_806DCED8 = 20.0f;
static float lbl_806DCEDC = -20.0f;
static float s_fAdditiveBlendingIntensity = 0.55f;
static float s_fAdditiveTextureScale = 1.25f;
static unsigned char s_bPulseGlowTexture = 1;
static float s_fPulseRate = 2.0f;
static float lbl_806DCEF0 = 30.0f;
static float lbl_806DCEF4 = 0.3f;
float fMaxAlpha = 0.9f;
float fOpacityFadePerSecond = 1.2f;

static float s_fGlowIntensityScale;
static unsigned char s_bGlowIsRising;
static float lbl_806E1578;
static float lbl_806E157C;

class IndicatorInfo
{
public:
    IndicatorInfo()
        : m_fOpacity(1.0f)
    {
    }

    inline void IncrementOnscreenTimer(float fTimeDelta)
    {
        m_fOpacity -= fTimeDelta * fOpacityFadePerSecond;
        if (m_fOpacity < 0.0f)
        {
            m_fOpacity = 0.0f;
        }
    }

    inline void IncrementOffscreenTimer(float fTimeDelta)
    {
        m_fOpacity += fTimeDelta * fOpacityFadePerSecond;
        if (m_fOpacity > fMaxAlpha)
        {
            m_fOpacity = fMaxAlpha;
        }
    }

    float m_fOpacity;
};

unsigned long uIndicatorTexID[4] = {
    nlStringLowerHash("fe/controller_1_indicator"),
    nlStringLowerHash("fe/controller_2_indicator"),
    nlStringLowerHash("fe/controller_3_indicator"),
    nlStringLowerHash("fe/controller_4_indicator")
};

unsigned long uGlowTexID[4] = {
    nlStringLowerHash("fe/controller_1_indicator_E"),
    nlStringLowerHash("fe/controller_2_indicator_E"),
    nlStringLowerHash("fe/controller_3_indicator_E"),
    nlStringLowerHash("fe/controller_4_indicator_E")
};

IndicatorInfo indicatorInfo[10];

static inline float max_float(float a, float b)
{
    return (b >= a) ? b : a;
}

static inline void SetIndicatorPolyColour(glPoly2& poly, unsigned char r,
    unsigned char g, unsigned char b, unsigned char a)
{
    nlColour c;
    nlColourSet(c, r, g, b, a);
    poly.SetColour(c);
}

static inline nlColour GetIndicatorColour(cPlayer* pCharacter)
{
    cFielder* pCaptain = pCharacter->m_pTeam->GetCaptain();
    cFielder* pOtherCaptain
        = pCharacter->m_pTeam->GetOtherTeam()->GetCaptain();
    CharacterInfo* pInfo
        = ((IndicatorCharacterInfoState*)pCaptain)->mCharacterInfo;
    CharacterInfo* pOtherInfo
        = ((IndicatorCharacterInfoState*)pOtherCaptain)->mCharacterInfo;
    unsigned long packed = GetTeamColour(*pInfo, *pOtherInfo, true);

    nlColour colour;
    colour.c[0] = packed >> 24;
    colour.c[1] = packed >> 16;
    colour.c[2] = packed >> 8;
    colour.c[3] = packed;
    return colour;
}

extern "C" int fn_801A323C(cPlayer* pCharacter, bool* pSameMachine)
{
    if (fn_80338BF0(lbl_806E20D8) > 1)
    {
        cGlobalPad* pGlobalPad = pCharacter->GetGlobalPad();
        if (pGlobalPad == 0)
        {
            *pSameMachine = false;
            return -1;
        }

        UnidentifiedNetworkPeer* pPeer = fn_80338C0C(lbl_806E20D8);
        IndicatorPadOwner* pOwner
            = ((IndicatorGlobalPadState*)pGlobalPad)->mOwner;
        int index = -1;

        if (pOwner->mPeer == pPeer)
        {
            *pSameMachine = true;
            index = fn_80336D90(pOwner)->mPadIndex;
        }
        else
        {
            *pSameMachine = false;
            bool used[4] = { false, false, false, false };
            for (int i = 0; i < (int)pPeer->mUnidentified004; ++i)
            {
                used[fn_80336D90(fn_80336B6C(pPeer, i))->mPadIndex] = true;
            }

            int available[4] = { -1, -1, -1, -1 };
            int next = 0;
            if (!used[0])
            {
                available[0] = next++;
            }
            if (!used[1])
            {
                available[1] = next++;
            }
            if (!used[2])
            {
                available[2] = next++;
            }
            if (!used[3])
            {
                available[3] = next;
            }

            if (pOwner->mPadIndex == available[0])
            {
                index = 0;
            }
            else if (pOwner->mPadIndex == available[1])
            {
                index = 1;
            }
            else if (pOwner->mPadIndex == available[2])
            {
                index = 2;
            }
            else if (pOwner->mPadIndex == available[3])
            {
                index = 3;
            }
        }
        return index;
    }

    IndicatorControllerInfo* pInfo
        = (IndicatorControllerInfo*)pCharacter->fn_800972CC();
    int index = -1;
    if (pInfo != 0)
    {
        index = pInfo->mPadIndex;
    }
    *pSameMachine = true;
    return index;
}

static void DrawIndicator(int xCentre, int yCentre, float fPixelWidth,
    float fPixelHeight, float fOpacity, unsigned long uTexID,
    const nlColour& colour, float rotationAngle,
    unsigned char additiveBlending)
{
    if (glTextureLoad(uTexID))
    {
        glPoly2 poly;

        glSetDefaultState(false);
        unsigned long blendMode = 1;
        if (additiveBlending != 0)
        {
            blendMode = 2;
        }
        glSetRasterState(GLS_AlphaBlend, blendMode);
        glSetRasterState(GLS_AlphaTest, 1);
        glSetRasterState(GLS_AlphaTestRef, 0);
        glSetCurrentRasterState(glHandleizeRasterState());
        glSetCurrentTexture(uTexID, GLTT_Diffuse);

        poly.SetupRotatedRectangle((float)xCentre, (float)yCentre,
            fPixelWidth, fPixelHeight, rotationAngle, 10000000000.0f);

        if (additiveBlending == 0)
        {
            SetIndicatorPolyColour(poly, colour.c[0], colour.c[1],
                colour.c[2], (unsigned char)(255.0f * fOpacity));
        }
        else
        {
            unsigned char intensity
                = (unsigned char)(255.0f * fOpacity);
            SetIndicatorPolyColour(
                poly, intensity, intensity, intensity, 0xFF);
        }

        poly.depth = -0.5f;
        poly.Attach(fn_8027267C(39), 0, 0);
    }
}

static inline unsigned long GetCharacterTexID(
    cPlayer* pCharacter, bool* pSameMachine)
{
    return uIndicatorTexID[fn_801A323C(pCharacter, pSameMachine)];
}

static inline unsigned long GetCharacterGlowTexID(
    cPlayer* pCharacter, bool* pSameMachine)
{
    return uGlowTexID[fn_801A323C(pCharacter, pSameMachine)];
}

static void DrawOffscreenIndicator(const nlVector3& v3NormalizedScreenPos,
    IndicatorInfo* pInfo, cPlayer* pCharacter)
{
    GLView* pView = fn_8027267C(39);
    float screenLimitX = fn_802CE76C(pView);
    float screenLimitY = fn_802CE76C(pView);
    float screenPosX = v3NormalizedScreenPos.x;
    float screenPosY = v3NormalizedScreenPos.y;
    screenLimitX -= 32.0f;
    screenLimitY -= 32.0f;

    screenPosX
        = nlMinEquals(nlMaxEquals(screenPosX, 32.0f), screenLimitX);
    screenPosY
        = nlMinEquals(nlMaxEquals(screenPosY, 32.0f), screenLimitY);

    float scale = (float)__fabs(1.0f
        - max_float((float)__fabs(screenPosY),
            (float)__fabs(screenPosX)));
    scale = InterpolateRangeClamped(1.0f, 0.5f, 0.0f, 2.0f, scale);

    bool sameMachine = false;
    int padIndex = fn_801A323C(pCharacter, &sameMachine);
    unsigned long indicatorTexID = uIndicatorTexID[padIndex];
    float opacityScale = 1.0f;
    if (!sameMachine)
    {
        opacityScale = lbl_806DCEF4;
    }
    nlColour colour = GetIndicatorColour(pCharacter);

    DrawIndicator((int)screenPosX, (int)screenPosY, 64.0f * scale,
        64.0f * scale, opacityScale * pInfo->m_fOpacity,
        indicatorTexID, colour, 0.0f, false);
}

static void UpdateAndRenderOffScreenIndicators(float dt)
{
    nlVector3 worldPos = { 0.0f, 0.0f, 0.0f };

    for (int i = 0; i < 10; ++i)
    {
        cPlayer* pCharacter = lbl_8056B800[i];
        if (pCharacter == 0)
        {
            continue;
        }

        ReplayManager* pReplay = ReplayManager::Instance();
        if (pReplay->mRender != 0)
        {
            pReplay = ReplayManager::Instance();
            worldPos = pReplay->mRender->mCharacters[i].position;
        }

        if (pCharacter->m_eClassType == FIELDER)
        {
            if (fn_800387CC(pCharacter))
            {
                continue;
            }
            worldPos.z += *fn_8003E6E4(pCharacter)->fPhysCapsuleHeight
                * 0.5f;
        }
        else
        {
            IndicatorPlayerTweaks* pTweaks
                = (IndicatorPlayerTweaks*)pCharacter->m_pTweaks;
            worldPos.z += *pTweaks->fGoalieCapsuleHeight * 0.5f;
        }

        nlVector3 projectedPos;
        fn_802CE528(fn_8027267C(8), &worldPos, &projectedPos);
        ((IndicatorPlayerState*)pCharacter)->mScreenPosition = projectedPos;

        bool sameMachine = false;
        if (fn_801A323C(pCharacter, &sameMachine) == -1)
        {
            continue;
        }

        if (fn_8001E184(pCharacter) || !g_pGame->IsGameplayOrOvertime())
        {
            indicatorInfo[i].IncrementOnscreenTimer(dt);
        }
        else
        {
            indicatorInfo[i].IncrementOffscreenTimer(dt);
            projectedPos.y = -projectedPos.y;

            if (projectedPos.x < -0.95f)
            {
                projectedPos.x = -0.95f;
            }
            else if (projectedPos.x > 0.95f)
            {
                projectedPos.x = 0.95f;
            }

            if (projectedPos.y < -0.925f)
            {
                projectedPos.y = -0.925f;
            }
            else if (projectedPos.y > 0.95f)
            {
                projectedPos.y = 0.95f;
            }

            fn_802CE6DC(
                fn_8027267C(39), &projectedPos, &projectedPos);
            DrawOffscreenIndicator(
                projectedPos, &indicatorInfo[i], pCharacter);
        }
    }
}

static void UpdateAndRenderPlayerIndicators(float)
{
    static int whoHadBall = -1;
    int whoHasBall = -1;

    for (int i = 0; i < 10; ++i)
    {
        cPlayer* pCharacter = lbl_8056B800[i];
        if (pCharacter == 0)
        {
            continue;
        }

        bool sameMachine = false;
        if (fn_801A323C(pCharacter, &sameMachine) == -1)
        {
            continue;
        }

        float fOpacity = 1.0f - indicatorInfo[i].m_fOpacity;
        if (fOpacity <= 0.011764706f)
        {
            continue;
        }

        unsigned long indicatorTexID
            = GetCharacterTexID(pCharacter, &sameMachine);
        unsigned long glowTexID
            = GetCharacterGlowTexID(pCharacter, &sameMachine);
        unsigned long directionArrowTexID
            = nlStringLowerHash("fe/direction_arrow");
        float opacityScale = sameMachine ? 1.0f : lbl_806DCEF4;

        ReplayManager* pReplay = ReplayManager::Instance();
        nlVector3 v3Position
            = pReplay->mRender->mCharacters[i].bip01Position;

        float fVerticalOffset = 0.0f;
        if (pCharacter->m_eClassType == FIELDER)
        {
            if (fn_800387CC(pCharacter))
            {
                continue;
            }
            fVerticalOffset
                = *fn_8003E6E4(pCharacter)->fPhysCapsuleHeight * 0.5f
                * pCharacter->mUnidentified0A0;
        }
        else
        {
            IndicatorPlayerTweaks* pTweaks
                = (IndicatorPlayerTweaks*)pCharacter->m_pTweaks;
            fVerticalOffset += *pTweaks->fGoalieCapsuleHeight;
        }
        v3Position.z += fVerticalOffset;

        nlColour colour = GetIndicatorColour(pCharacter);
        nlVector3 v3ScreenPosition;
        fn_802CEA40(fn_8027267C(8), fn_8027267C(39), &v3Position,
            &v3ScreenPosition);
        v3ScreenPosition.y -= lbl_806DCEF0;

        float switchScale
            = ((IndicatorPlayerState*)pCharacter)->mSwitchScale;
        if (switchScale < 0.5f)
        {
            switchScale = (0.5f - switchScale) / 0.5f;
            float sizeScale = Interpolate(1.0f, 2.0f, switchScale);

            DrawIndicator((int)v3ScreenPosition.x,
                (int)v3ScreenPosition.y,
                s_fAdditiveTextureScale * (s_fOverheadSize * sizeScale),
                s_fAdditiveTextureScale * (s_fOverheadSize * sizeScale),
                opacityScale
                    * (s_fAdditiveBlendingIntensity * switchScale),
                glowTexID, colour, 0.0f, true);

            float fDistInPixels = s_fOverheadSize * sizeScale;
            DrawIndicator((int)v3ScreenPosition.x,
                (int)v3ScreenPosition.y, fDistInPixels, fDistInPixels,
                opacityScale * fOpacity, indicatorTexID, colour, 0.0f,
                false);
        }
        else if (pCharacter->m_pBall != 0)
        {
            whoHasBall = i;
            if (whoHadBall == -1)
            {
                s_fGlowIntensityScale = 0.0f;
            }

            float pulseScale
                = s_bPulseGlowTexture ? s_fGlowIntensityScale : 1.0f;
            DrawIndicator((int)v3ScreenPosition.x,
                (int)v3ScreenPosition.y,
                s_fOverheadSize * s_fAdditiveTextureScale,
                s_fOverheadSize * s_fAdditiveTextureScale,
                opacityScale
                    * (s_fAdditiveBlendingIntensity * pulseScale),
                glowTexID, colour, 0.0f, true);

            DrawIndicator((int)v3ScreenPosition.x,
                (int)v3ScreenPosition.y, s_fOverheadSize,
                s_fOverheadSize, opacityScale * fOpacity, indicatorTexID,
                colour, 0.0f, false);

            float rotationDegrees;
            float xOffset;
            float yOffset;
            if (pCharacter->m_pTeam->m_nSide == HOME)
            {
                rotationDegrees = 180.0f;
                xOffset = lbl_806DCED8;
                yOffset = lbl_806E1578;
            }
            else
            {
                rotationDegrees = 0.0f;
                xOffset = lbl_806DCEDC;
                yOffset = lbl_806E157C;
            }

            float rotationAngle
                = 3.1415927f * rotationDegrees / 180.0f;
            DrawIndicator((int)(v3ScreenPosition.x + xOffset),
                (int)(v3ScreenPosition.y - yOffset), lbl_806DCED4,
                lbl_806DCED4, opacityScale * fOpacity,
                directionArrowTexID, colour, rotationAngle, false);
        }
        else
        {
            DrawIndicator((int)v3ScreenPosition.x,
                (int)v3ScreenPosition.y, s_fOverheadSize,
                s_fOverheadSize, opacityScale * fOpacity, indicatorTexID,
                colour, 0.0f, false);
        }
    }

    whoHadBall = whoHasBall;
}

void UpdateAndRenderIndicators(float dt)
{
    UpdateAndRenderOffScreenIndicators(dt);
    UpdateAndRenderPlayerIndicators(dt);

    if (s_bPulseGlowTexture)
    {
        if (s_bGlowIsRising)
        {
            s_fGlowIntensityScale += s_fPulseRate * dt;
            if (s_fGlowIntensityScale > 1.0f)
            {
                s_fGlowIntensityScale = 1.0f;
                s_bGlowIsRising = 0;
            }
        }
        else
        {
            s_fGlowIntensityScale -= s_fPulseRate * dt;
            if (s_fGlowIntensityScale < 0.0f)
            {
                s_fGlowIntensityScale = 0.0f;
                s_bGlowIsRising = 1;
            }
        }
    }
}
