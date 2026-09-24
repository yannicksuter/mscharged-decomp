#include "Game/Render/Indicators.h"
#include "Game/AI/Fielder.h"
#include "Game/Goalie.h"

#include "Game/Render/RLView.h"

#include "Game/AI/AiUtil.h"
#include "Game/CharacterTweaks.h"
#include "Game/CharacterTemplate.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/Game.h"
#include "Game/MathHelpers.h"
#include "Game/NetworkSession.h"
#include "Game/Player.h"
#include "Game/ReplayManager.h"
#include "Game/Team.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/gl/glDraw2.h"
#include "NL/gl/glState.h"
#include "NL/gl/glTexture.h"
#include "NL/gl/glView.h"
#include "NL/globalpad.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"
#include "NL/nlString.h"
#include "types.h"
#include "Game/NetworkInput.h"
#include <math.h>

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
    IndicatorInfo();

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
    const CharacterInfo* pInfo = pCaptain->mUnidentified11C;
    const CharacterInfo* pOtherInfo = pOtherCaptain->mUnidentified11C;
    return GetTeamColour(*pInfo, *pOtherInfo, true);
}

extern "C" int fn_801A323C(cPlayer* pCharacter, bool* pSameMachine)
{
    if (g_pNetworkSessionBase->GetNumMachines() > 1)
    {
        DetInput* pGlobalPad = pCharacter->GetGlobalPad();
        if (pGlobalPad == 0)
        {
            *pSameMachine = false;
            return -1;
        }

        NetworkPeerChannel* pOwner;
        NetworkPeer* pPeer;
        int index;
        pPeer = g_pNetworkSessionBase->GetLocalPeer();
        index = -1;
        pOwner = (NetworkPeerChannel*)pGlobalPad->m_pMyUser;

        if (pOwner->mPeer == pPeer)
        {
            *pSameMachine = true;
            index = pOwner->GetLocalChannelPad()->m_padIndex;
        }
        else
        {
            *pSameMachine = false;
            bool used[4] = { false, false, false, false };
            for (int i = 0; i < (int)pPeer->mPlayerCount; ++i)
            {
                used[pPeer->GetNetworkPeerChannel(i)->GetLocalChannelPad()->m_padIndex]
                    = true;
            }

            int available[4] = { -1, -1, -1, -1 };
            int next = 0;
            int remoteChannelIndex = pOwner->mChannelIndex;
            for (int pad = 0; pad < 4; ++pad)
            {
                if (!used[pad])
                {
                    available[pad] = next++;
                }
            }

            if (remoteChannelIndex == available[0])
            {
                index = 0;
            }
            else if (remoteChannelIndex == available[1])
            {
                index = 1;
            }
            else if (remoteChannelIndex == available[2])
            {
                index = 2;
            }
            else if (remoteChannelIndex == available[3])
            {
                index = 3;
            }
        }
        return index;
    }

    int index = -1;
    if (pCharacter->fn_800972CC() != 0)
    {
        index = pCharacter->fn_800972CC()->m_padIndex;
    }
    *pSameMachine = true;
    return index;
}

static void DrawIndicator(int xCentre, int yCentre, float fPixelWidth,
    float fPixelHeight, float fOpacity, unsigned long uTexID,
    nlColour colour, float rotationAngle,
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
        poly.Attach(GetLayerView(eCLV_UnsortedSquareOrtho), 0, 0);
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
    float screenPosX;
    float screenPosY;
    GLView* pView = GetLayerView(eCLV_UnsortedSquareOrtho);
    float screenLimitX = glViewGetOrthographicWidth(pView);
    float screenLimitY = glViewGetOrthographicWidth(pView);
    screenPosX = v3NormalizedScreenPos.x;
    screenLimitX -= 32.0f;
    screenLimitY -= 32.0f;

    screenPosX
        = nlMinEquals(nlMaxEquals(screenPosX, 32.0f), screenLimitX);
    screenPosY = v3NormalizedScreenPos.y;
    screenPosY
        = nlMinEquals(nlMaxEquals(screenPosY, 32.0f), screenLimitY);

    float absY = fabsf(screenPosY);
    float absX = fabsf(screenPosX);
    float scale = fabsf(1.0f - max_float(absY, absX));
    scale = InterpolateRangeClamped(1.0f, 0.5f, 0.0f, 2.0f, scale);

    bool sameMachine = false;
    unsigned long indicatorTexID = GetCharacterTexID(pCharacter, &sameMachine);
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
        if (g_pCharacters[i] == 0)
        {
            continue;
        }

        ReplayManager* pReplay = ReplayManager::Instance();
        if (pReplay->mRender != 0)
        {
            pReplay = ReplayManager::Instance();
            worldPos = pReplay->mRender->mCharacters[i].position;
        }

        {
            cPlayer* pCharacter = (cPlayer*)g_pCharacters[i];
            if (pCharacter->m_eClassType == FIELDER)
            {
                if (((cFielder*)pCharacter)->IsShattered())
                {
                    continue;
                }
                float height = ((cFielder*)pCharacter)->GetTweaks()->mUnidentified004.GetValue();
                worldPos.z += height / 2.0f;
            }
            else
            {
                float height
                    = ((Goalie*)pCharacter)->m_pTweaks->fPhysCapsuleHeight.GetValue();
                worldPos.z += height / 2.0f;
            }
        }

        nlVector3 projectedPos;
        glViewProjectPoint(GetLayerView(eCLV_Unshadowed), worldPos, projectedPos);
        ((cPlayer*)g_pCharacters[i])->m_v3ScreenPosition = projectedPos;

        bool sameMachine = false;
        if (fn_801A323C((cPlayer*)g_pCharacters[i], &sameMachine) == -1)
        {
            continue;
        }

        if (((cPlayer*)g_pCharacters[i])->fn_8001E184() || !g_pGame->IsGameplayOrOvertime())
        {
            indicatorInfo[i].IncrementOnscreenTimer(dt);
        }
        else
        {
            indicatorInfo[i].IncrementOffscreenTimer(dt);
            projectedPos.y = -projectedPos.y;

            projectedPos.x
                = nlMinEquals(nlMaxEquals(projectedPos.x, -0.95f), 0.95f);
            projectedPos.y
                = nlMinEquals(nlMaxEquals(projectedPos.y, -0.925f), 0.95f);

            glViewUnprojectOrthographicPoint(
                GetLayerView(eCLV_UnsortedSquareOrtho), &projectedPos, &projectedPos);
            DrawOffscreenIndicator(
                projectedPos, &indicatorInfo[i], (cPlayer*)g_pCharacters[i]);
        }
    }
}

static void UpdateAndRenderPlayerIndicators(float)
{
    static int whoHadBall = -1;
    int whoHasBall = -1;

    for (int i = 0; i < 10; ++i)
    {
        if (g_pCharacters[i] == 0)
        {
            continue;
        }

        bool sameMachine = false;
        if (fn_801A323C((cPlayer*)g_pCharacters[i], &sameMachine) == -1)
        {
            continue;
        }

        float fOpacity = 1.0f - indicatorInfo[i].m_fOpacity;
        if (fOpacity <= 0.011764706f)
        {
            continue;
        }

        unsigned long indicatorTexID
            = GetCharacterTexID((cPlayer*)g_pCharacters[i], &sameMachine);
        unsigned long glowTexID
            = GetCharacterGlowTexID((cPlayer*)g_pCharacters[i], &sameMachine);
        unsigned long directionArrowTexID
            = nlStringLowerHash("fe/direction_arrow");
        float opacityScale = 1.0f;
        if (!sameMachine)
        {
            opacityScale = lbl_806DCEF4;
        }

        ReplayManager* pReplay = ReplayManager::Instance();
        nlVector3 v3Position
            = pReplay->mRender->mCharacters[i].bip01Position;

        float fVerticalOffset = 0.0f;
        {
            cPlayer* pCharacter = (cPlayer*)g_pCharacters[i];
            if (pCharacter->m_eClassType == FIELDER)
            {
                if (((cFielder*)pCharacter)->IsShattered())
                {
                    continue;
                }
                fVerticalOffset
                    = ((cFielder*)pCharacter)->GetTweaks()->mUnidentified004.GetValue() * 0.5f
                    * pCharacter->mUnidentified024.m_fPlayerScale;
            }
            else
            {
                fVerticalOffset
                    += ((Goalie*)pCharacter)->m_pTweaks->fPhysCapsuleHeight.GetValue();
            }
        }

        float screenOffset = lbl_806DCEF0;
        nlColour colour = GetIndicatorColour((cPlayer*)g_pCharacters[i]);
        v3Position.z += fVerticalOffset;
        nlVector3 v3ScreenPosition;
        glViewProjectPointBetweenViews(GetLayerView(eCLV_Unshadowed), GetLayerView(eCLV_UnsortedSquareOrtho), &v3Position,
            &v3ScreenPosition);
        v3ScreenPosition.y -= screenOffset;

        float switchScale
            = ((cPlayer*)g_pCharacters[i])->mUnidentified1E4.m_UserControlledTime;
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
        else if (((cPlayer*)g_pCharacters[i])->m_pBall != 0)
        {
            whoHasBall = i;
            if (whoHadBall == -1)
            {
                s_fGlowIntensityScale = 0.0f;
            }

            DrawIndicator((int)v3ScreenPosition.x,
                (int)v3ScreenPosition.y,
                s_fOverheadSize * s_fAdditiveTextureScale,
                s_fOverheadSize * s_fAdditiveTextureScale,
                opacityScale
                    * (s_fAdditiveBlendingIntensity
                        * (s_bPulseGlowTexture ? s_fGlowIntensityScale : 1.0f)),
                glowTexID, colour, 0.0f, true);

            DrawIndicator((int)v3ScreenPosition.x,
                (int)v3ScreenPosition.y, s_fOverheadSize,
                s_fOverheadSize, opacityScale * fOpacity, indicatorTexID,
                colour, 0.0f, false);

            float rotationDegrees;
            float xOffset;
            float yOffset;
            if (((cPlayer*)g_pCharacters[i])->m_pTeam->m_nSide == HOME)
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

IndicatorInfo::IndicatorInfo()
    : m_fOpacity(1.0f)
{
}
