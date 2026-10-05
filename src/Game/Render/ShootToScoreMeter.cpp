#include "NL/gl/glPlat.h"
#include "Game/Render/ShootToScoreMeter.h"

#include "Game/UnidentifiedStaticStorage.h"
#include "Game/AI/AiUtil.h"
#include "Game/Camera/CameraMan.h"
#include "Game/Game.h"
#include "Game/OverlayManager.h"
#include "Game/Render/RLView.h"
#include "NL/gl/glDraw3.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlString.h"
#include "NL/platvmath.h"

static u32 LightTexture = glGetTexture("global/lightramp");
static u32 BlackTexture = glGetTexture("global/black");
static u32 WhiteTexture = glGetTexture("global/white");
static u32 MeterTexture = glGetTexture("fe/megastrike_metre_track");
static float sfMeterStart = 0.0f;

u32 NumberTextures[4] = {
    nlStringLowerHash("fe/controller_1_indicator"),
    nlStringLowerHash("fe/controller_2_indicator"),
    nlStringLowerHash("fe/controller_3_indicator"),
    nlStringLowerHash("fe/controller_4_indicator"),
};

ShootToScoreMeter ShootToScoreMeter::instance;

static nlColour sWhiteBarColour = { 255, 255, 255, 255 };
static nlColour sGreenRegionColour = { 5, 150, 5, 255 };
static nlColour sYellowRegionColour = { 255, 255, 0, 255 };
static nlColour sOrangeRegionColour = { 255, 175, 0, 255 };
static nlColour sRedRegionColour = { 255, 100, 0, 255 };
float ShootToScoreMeter::MeterWidth = 0.315f;
static s32 sSavedSegmentAlpha = 150;
static float sfIndicatorBarHeight = 0.008f;
static float sfRegionBandWidth = 0.05f;
static float sfIndicatorBarOverhang = 0.04f;
static float sfMeterRadiusScale = 0.388f;
static float sfMeterEnd = 180.0f;
static float sfTrailIntensity = 0.33f;
static float sfTrailLengthScale = 0.33f;
static s32 sfNumBarsInTrail = 12;
static float sfRumbleDuration = 0.005f;
static float sfRumbleOffset = 0.165f;
static float sfRumbleMaxRotation = 8.0f;

static inline void InterpolateColours(const nlColour& colour0,
    const nlColour& colour1, float alpha, nlColour& result)
{
    float oneMinusAlpha = 1.0f - alpha;
    result.c[0] = (u8)(s32)(oneMinusAlpha * (float)colour0.c[0]
        + alpha * (float)colour1.c[0]);
    result.c[1] = (u8)(s32)(oneMinusAlpha * (float)colour0.c[1]
        + alpha * (float)colour1.c[1]);
    result.c[2] = (u8)(s32)(oneMinusAlpha * (float)colour0.c[2]
        + alpha * (float)colour1.c[2]);
    result.c[3] = (u8)(s32)(oneMinusAlpha * (float)colour0.c[3]
        + alpha * (float)colour1.c[3]);
}

static inline float clamp_ge(float x, float limit)
{
    if (x >= limit)
    {
        return x;
    }
    return limit;
}

static inline float clamp_le(float x, float limit)
{
    if (x <= limit)
    {
        return x;
    }
    return limit;
}

static inline float MeterPosition(float position)
{
    float oneMinusPosition = 1.0f - position;
    position *= sfMeterEnd;
    return oneMinusPosition * sfMeterStart + position;
}

void ShootToScoreMeter::SetSegment4Width(float width)
{
    m_fSegment4Width = MeterPosition(width);
}

void ShootToScoreMeter::SetSegment4Position(float position)
{
    m_fSegment4Angle = MeterPosition(position);
}

void ShootToScoreMeter::SetSegment3Width(float width)
{
    m_fSegment3Width = MeterPosition(width);
}

void ShootToScoreMeter::SetSegment3Position(float position)
{
    m_fSegment3Angle = MeterPosition(position);
}

void ShootToScoreMeter::SetSegment2Width(float width)
{
    m_fSegment2Width = MeterPosition(width);
}

void ShootToScoreMeter::SetSegment2Position(float position)
{
    m_fSegment2Angle = MeterPosition(position);
}

void ShootToScoreMeter::SetSegment1Width(float width)
{
    m_fSegment1Width = MeterPosition(width);
}

void ShootToScoreMeter::SetSegment1Position(float position)
{
    m_fSegment1Angle = MeterPosition(position);
}

void ShootToScoreMeter::SetYellowRegionWidth(float width)
{
    m_fYellowRegionWidth = MeterPosition(width);
}

void ShootToScoreMeter::SetGreenRegionWidth(float width)
{
    m_fGreenRegionWidth = MeterPosition(width);
}

void ShootToScoreMeter::SetGreenBarPosition(float position)
{
    m_fGreenBarAngle = MeterPosition(position);
}

void ShootToScoreMeter::SetSavedWhiteBarPosition(float position)
{
    m_fSavedWhiteBarAngle = MeterPosition(position);
}

void ShootToScoreMeter::SetWhiteBarPosition(float position)
{
    float angle = MeterPosition(position);
    m_fWhiteBarPreviousAngle = m_fWhiteBarAngle;
    m_fWhiteBarAngle = angle;
}

void ShootToScoreMeter::UpdateAndRender(float fDeltaT)
{
    if (!m_bMeterVisible)
    {
        return;
    }

    DrawMeter();

    if (mfRumbleAmount > 0.0f)
    {
        mfRumbleAmount -= fDeltaT;
        if (mfRumbleAmount < 0.0f)
        {
            mfRumbleAmount = 0.0f;
            m_v3MeterPosition = m_v3OriginalMeterPosition;
        }
        else
        {
            float amount = mfRumbleAmount / sfRumbleDuration;
            m_v3MeterPosition.x = (1.0f - amount) * m_v3MeterPosition.x
                + amount * m_v3OriginalMeterPosition.x;
            m_v3MeterPosition.y = (1.0f - amount) * m_v3MeterPosition.y
                + amount * m_v3OriginalMeterPosition.y;
            m_v3MeterPosition.z = (1.0f - amount) * m_v3MeterPosition.z
                + amount * m_v3OriginalMeterPosition.z;
        }
    }
}

void ShootToScoreMeter::DrawColouredRegion(float startAngle,
    float endAngle, const nlColour& startColour, const nlColour& endColour,
    nlMatrix4 meterMatrix, float scale)
{
    glSetCurrentTexture(WhiteTexture, GLTT_Diffuse);
    glSetTextureState(GLTS_DiffuseWrap, 0);
    glSetCurrentTextureState(glHandleizeTextureState());

    float scaledMeterWidth;
    float scaledWhiteBarWidth;
    float radius;
    nlVector3 vertexPosition;
    float widthAngle;
    int i;
    glQuad3 barQuad;
    float startFraction;
    float endFraction;
    float innerRadius;
    float outerRadius;

    scaledMeterWidth = MeterWidth * scale;
    scaledWhiteBarWidth = sfRegionBandWidth * scale;
    widthAngle = endAngle - startAngle;
    radius = scaledMeterWidth * sfMeterRadiusScale;

    for (i = 0; i < 8; i++)
    {
        startFraction = (float)i / 8.0f;
        endFraction = (float)(i + 1) / 8.0f;
        innerRadius = radius - scaledWhiteBarWidth / 2.0f;
        outerRadius = radius + scaledWhiteBarWidth / 2.0f;

        float segmentStartAngleRadians
            = DegreesToRadians(startFraction * widthAngle + startAngle);
        float segmentEndAngleRadians
            = DegreesToRadians(endFraction * widthAngle + startAngle);

        float segmentStartCosine = nlSin((u16)((u16)(s32)(10430.378f
            * segmentStartAngleRadians)
            + 0x4000));
        float segmentStartSine = nlSin(
            (u16)(s32)(10430.378f * segmentStartAngleRadians));
        float segmentEndCosine = nlSin((u16)((u16)(s32)(10430.378f
            * segmentEndAngleRadians)
            + 0x4000));
        float segmentEndSine = nlSin(
            (u16)(s32)(10430.378f * segmentEndAngleRadians));

        vertexPosition.x = innerRadius * segmentStartCosine;
        vertexPosition.y = innerRadius * segmentStartSine;
        vertexPosition.z = -100.0f;
        nlMultPosVectorMatrix(vertexPosition, meterMatrix);
        barQuad.m_pos[0] = vertexPosition;

        vertexPosition.x = outerRadius * segmentStartCosine;
        vertexPosition.y = outerRadius * segmentStartSine;
        vertexPosition.z = -100.0f;
        nlMultPosVectorMatrix(vertexPosition, meterMatrix);
        barQuad.m_pos[1] = vertexPosition;

        vertexPosition.x = outerRadius * segmentEndCosine;
        vertexPosition.y = outerRadius * segmentEndSine;
        vertexPosition.z = -100.0f;
        nlMultPosVectorMatrix(vertexPosition, meterMatrix);
        barQuad.m_pos[2] = vertexPosition;

        vertexPosition.x = innerRadius * segmentEndCosine;
        vertexPosition.y = innerRadius * segmentEndSine;
        vertexPosition.z = -100.0f;
        nlMultPosVectorMatrix(vertexPosition, meterMatrix);
        barQuad.m_pos[3] = vertexPosition;

        InterpolateColours(startColour, endColour, startFraction,
            barQuad.m_colour[0]);
        barQuad.m_colour[1] = barQuad.m_colour[0];
        InterpolateColours(startColour, endColour, endFraction,
            barQuad.m_colour[2]);
        barQuad.m_colour[3] = barQuad.m_colour[2];

        glAttachQuad3(
            (eGLView)(u32)GetLayerView(eCLV_UnsortedSquareOrtho), 1,
            &barQuad);
    }
}

void ShootToScoreMeter::DrawIndicatorBar(float angle,
    const nlColour& colour, const nlMatrix4& meterMatrix, float scale)
{
    glSetCurrentTexture(WhiteTexture, GLTT_Diffuse);
    glSetTextureState(GLTS_DiffuseWrap, 0);
    glSetCurrentTextureState(glHandleizeTextureState());
    cCameraManager::GetDistanceFromCameraToObject(m_v3MeterPosition);

    glQuad3 barQuad;
    float zDepth;
    float scaledMeterWidth = MeterWidth * scale;
    float angleRadians;
    float scaledWhiteBarWidth
        = (sfRegionBandWidth + sfIndicatorBarOverhang) * scale;
    float scaledWhiteBarHeight = sfIndicatorBarHeight * scale;
    angleRadians = (3.1415927f * angle) / 180.0f;

    nlMatrix4 barMatrix;
    nlMakeRotationMatrixZ(barMatrix, angleRadians);

    float sine;
    float radius = scaledMeterWidth * sfMeterRadiusScale;
    sine = radius * nlSin((u16)(s32)(10430.378f * angleRadians));
    float cosine = radius
        * nlSin(
            (u16)((u16)(s32)(10430.378f * angleRadians) + 0x4000));
    zDepth = -100.0f;

    barMatrix.e2[3][0] = cosine;
    barMatrix.e2[3][1] = sine;
    barMatrix.e2[3][2] = zDepth;
    barMatrix.e2[3][3] = 1.0f;
    nlMatrix4 result;
    nlMultMatrices(result, barMatrix, meterMatrix);
    barMatrix = result;

    barQuad.SetupRotatedRectangle(scaledWhiteBarWidth,
        scaledWhiteBarHeight, barMatrix, false, false);
    barQuad.SetColour(colour);
    glAttachQuad3((eGLView)(u32)GetLayerView(eCLV_UnsortedSquareOrtho), 1,
        &barQuad);
}

void ShootToScoreMeter::DrawMeter()
{
    glSetRasterState((eGLState)6, 0);
    glSetDefaultState(true);
    glSetRasterState((eGLState)1, 0);
    glSetRasterState((eGLState)0, 0);
    glSetRasterState((eGLState)5, 1);
    glSetRasterState((eGLState)6, 0);
    glSetCurrentRasterState(glHandleizeRasterState());

    glSetCurrentTexture(MeterTexture, GLTT_Diffuse);
    glSetTextureState(GLTS_DiffuseWrap, 0);
    glSetCurrentTextureState(glHandleizeTextureState());

    nlMatrix4 matrix;
    matrix.SetIdentity();

    float rumbleScale = mfRumbleAmount / sfRumbleDuration;
    float rotation;
    if (nlRandomf(1.0f, &nlDefaultSeed) < 0.5f)
    {
        rotation = sfRumbleMaxRotation;
    }
    else
    {
        rotation = -sfRumbleMaxRotation;
    }
    rotation = InterpolateRangeClamped(
        0.0f, rotation, 0.0f, 1.0f, rumbleScale);
    nlMakeRotationMatrixZ(matrix, (3.1415927f * rotation) / 180.0f);

    static nlVector3 screenPosition;
    glViewProjectPointBetweenViews(GetLayerView(eCLV_Unshadowed),
        GetLayerView(eCLV_UnsortedSquareOrtho), &m_v3MeterPosition,
        &screenPosition);
    screenPosition.z = -0.1f;
    screenPosition.y += -20.0f;

    float screenWidth = (float)glplatGetDefaultTargetWidth();
    float screenHeight = (float)glplatGetDefaultTargetHeight();
    float scaledMeterWidth = MeterWidth * screenWidth;
    float lowerY;
    float upperY;
    float upperX;
    float screenMargin;

    screenMargin = 60.0f;
    lowerY = 0.05f
        * glViewGetOrthographicHeight(GetLayerView(eCLV_UnsortedSquareOrtho));
    GLView* view = GetLayerView(eCLV_UnsortedSquareOrtho);
    float upperYMargin = 0.05f * glViewGetOrthographicHeight(view);
    upperY = glViewGetOrthographicHeight(view) - upperYMargin;
    view = GetLayerView(eCLV_UnsortedSquareOrtho);
    upperX = 0.05f * glViewGetOrthographicWidth(view);
    upperX = glViewGetOrthographicWidth(view) - upperX - screenMargin;
    float lowerX = 0.05f
        * glViewGetOrthographicWidth(GetLayerView(eCLV_UnsortedSquareOrtho));
    lowerX = screenMargin + lowerX;
    screenPosition.x
        = clamp_le(clamp_ge(screenPosition.x, lowerX), upperX);
    screenPosition.y = clamp_le(
        clamp_ge(screenPosition.y, lowerY + screenMargin),
        upperY - screenMargin - 25.0f);

    matrix.e2[3][0] = screenPosition.x;
    matrix.e2[3][1] = screenPosition.y;
    matrix.e2[3][2] = screenPosition.z;
    matrix.e2[3][3] = 1.0f;

    nlVector3 projectedPosition = { 0.0f, 0.0f, 0.0f };
    glViewProjectPoint(GetLayerView(eCLV_UnsortedSquareOrtho), screenPosition, projectedPosition);
    glViewUnprojectOrthographicPoint(GetLayerView(eCLV_Anark), &projectedPosition,
        &projectedPosition);
    static_cast<OverlayManager*>(g_pOverlayManager)->SetMegaStrikeMeterPosition(projectedPosition);

    glQuad3 quad;
    quad.SetupRotatedRectangle(
        scaledMeterWidth, scaledMeterWidth, matrix, true, false);
    glAttachQuad3((eGLView)(u32)GetLayerView(eCLV_UnsortedSquareOrtho), 1,
        &quad);

    nlColour green = sGreenRegionColour;
    nlColour white = sWhiteBarColour;
    nlColour yellow = sYellowRegionColour;
    nlColour orange = sOrangeRegionColour;
    nlColour red = sRedRegionColour;

    if (mbShowGreenRegion)
    {
        DrawColouredRegion(m_fGreenBarAngle - 0.5f * m_fGreenRegionWidth,
            m_fGreenBarAngle + 0.5f * m_fGreenRegionWidth, green, green,
            matrix, screenWidth);
        DrawColouredRegion(m_fGreenBarAngle - 0.5f * m_fYellowRegionWidth,
            m_fGreenBarAngle + 0.5f * m_fYellowRegionWidth, red, red,
            matrix, screenWidth);
    }

    if (mbShowSavedWhiteBar)
    {
        nlColour savedColour = white;
        if (m_fSavedWhiteBarAngle
                >= m_fSegment1Angle - m_fSegment1Width / 2.0f
            && m_fSavedWhiteBarAngle
                <= m_fSegment1Angle + m_fSegment1Width / 2.0f)
        {
            yellow.c[3] = (u8)sSavedSegmentAlpha;
            DrawColouredRegion(m_fSegment1Angle - 0.5f * m_fSegment1Width,
                m_fSegment1Angle + 0.5f * m_fSegment1Width, yellow, yellow,
                matrix, screenWidth);
            savedColour = yellow;
        }
        else if (m_fSavedWhiteBarAngle
                >= m_fSegment2Angle - m_fSegment2Width / 2.0f
            && m_fSavedWhiteBarAngle
                <= m_fSegment2Angle + m_fSegment2Width / 2.0f)
        {
            orange.c[3] = (u8)sSavedSegmentAlpha;
            DrawColouredRegion(m_fSegment2Angle - 0.5f * m_fSegment2Width,
                m_fSegment2Angle + 0.5f * m_fSegment2Width, orange, orange,
                matrix, screenWidth);
            savedColour = orange;
        }
        else if (m_fSavedWhiteBarAngle
                >= m_fSegment3Angle - m_fSegment3Width / 2.0f
            && m_fSavedWhiteBarAngle
                <= m_fSegment3Angle + m_fSegment3Width / 2.0f)
        {
            red.c[3] = (u8)sSavedSegmentAlpha;
            DrawColouredRegion(m_fSegment3Angle - 0.5f * m_fSegment3Width,
                m_fSegment3Angle + 0.5f * m_fSegment3Width, red, red, matrix,
                screenWidth);
            savedColour = red;
        }
        else if (m_fSavedWhiteBarAngle
                >= m_fSegment4Angle - m_fSegment4Width / 2.0f
            && m_fSavedWhiteBarAngle
                <= m_fSegment4Angle + m_fSegment4Width / 2.0f)
        {
            orange.c[3] = (u8)sSavedSegmentAlpha;
            DrawColouredRegion(m_fSegment4Angle - 0.5f * m_fSegment4Width,
                m_fSegment4Angle + 0.5f * m_fSegment4Width, orange, orange,
                matrix, screenWidth);
            savedColour = orange;
        }
        else if (m_fSavedWhiteBarAngle
                >= m_fSegment5Angle - m_fSegment5Width / 2.0f
            && m_fSavedWhiteBarAngle
                <= m_fSegment5Angle + m_fSegment5Width / 2.0f)
        {
            yellow.c[3] = (u8)sSavedSegmentAlpha;
            DrawColouredRegion(m_fSegment5Angle - 0.5f * m_fSegment5Width,
                m_fSegment5Angle + 0.5f * m_fSegment5Width, yellow, yellow,
                matrix, screenWidth);
            savedColour = yellow;
        }
        savedColour.c[3] = 255;
        DrawIndicatorBar(
            m_fSavedWhiteBarAngle, savedColour, matrix, screenWidth);
    }
    else
    {
        DrawColouredRegion(m_fSegment1Angle - 0.5f * m_fSegment1Width,
            m_fSegment1Angle + 0.5f * m_fSegment1Width, yellow, yellow,
            matrix, screenWidth);
        DrawColouredRegion(m_fSegment2Angle - 0.5f * m_fSegment2Width,
            m_fSegment2Angle + 0.5f * m_fSegment2Width, orange, orange,
            matrix, screenWidth);
        DrawColouredRegion(m_fSegment3Angle - 0.5f * m_fSegment3Width,
            m_fSegment3Angle + 0.5f * m_fSegment3Width, red, red, matrix,
            screenWidth);
        DrawColouredRegion(m_fSegment4Angle - 0.5f * m_fSegment4Width,
            m_fSegment4Angle + 0.5f * m_fSegment4Width, orange, orange,
            matrix, screenWidth);
        DrawColouredRegion(m_fSegment5Angle - 0.5f * m_fSegment5Width,
            m_fSegment5Angle + 0.5f * m_fSegment5Width, yellow, yellow,
            matrix, screenWidth);
        DrawIndicatorBar(
            GetWhiteBarAngle(), white, matrix, screenWidth);
    }

    nlColour trailColour = sWhiteBarColour;
    if (mbSecondButtonPressed)
    {
        if (m_fWhiteBarAngle
                >= m_fGreenBarAngle - m_fYellowRegionWidth / 2.0f
            && m_fWhiteBarAngle
                <= m_fGreenBarAngle + m_fYellowRegionWidth / 2.0f)
        {
            DrawIndicatorBar(
                GetWhiteBarAngle(), red, matrix, screenWidth);
            trailColour = red;
        }
        else if (m_fWhiteBarAngle
                >= m_fGreenBarAngle - m_fGreenRegionWidth / 2.0f
            && m_fWhiteBarAngle
                <= m_fGreenBarAngle + m_fGreenRegionWidth / 2.0f)
        {
            DrawIndicatorBar(
                GetWhiteBarAngle(), green, matrix, screenWidth);
            trailColour = green;
        }
        else
        {
            DrawIndicatorBar(
                GetWhiteBarAngle(), sWhiteBarColour, matrix, screenWidth);
            trailColour = sWhiteBarColour;
        }
    }
    else
    {
        DrawIndicatorBar(
            GetWhiteBarAngle(), sWhiteBarColour, matrix, screenWidth);
        trailColour = sWhiteBarColour;
    }

    float diffCurrentPrev = m_fWhiteBarPreviousAngle - m_fWhiteBarAngle;
    for (int i = 0; i < sfNumBarsInTrail; ++i)
    {
        trailColour.c[3] = (u8)(s32)(255.0f
            * (sfTrailIntensity
                * (1.0f - ((float)i / (float)sfNumBarsInTrail))));
        float angle = sfTrailLengthScale * ((float)i * diffCurrentPrev)
            + m_fWhiteBarAngle;
        DrawIndicatorBar(angle, trailColour, matrix, screenWidth);
    }

    glSetDefaultState(false);
}

void ShootToScoreMeter::RumbleMeter(u16 angle)
{
    if (mfRumbleAmount <= 0.0f)
    {
        float amount;
        if (nlRandomf(1.0f, &nlDefaultSeed) < 0.5f)
        {
            amount = sfRumbleOffset;
        }
        else
        {
            amount = -sfRumbleOffset;
        }

        nlVector3 offset;
        nlPolarToCartesian(offset.x, offset.y, angle, amount);
        offset.z = 0.0f;
        m_v3MeterPosition.x = m_v3OriginalMeterPosition.x + offset.x;
        m_v3MeterPosition.y = m_v3OriginalMeterPosition.y;
        m_v3MeterPosition.z = m_v3OriginalMeterPosition.z + offset.y;
        mfRumbleAmount = sfRumbleDuration;
    }
}

void ShootToScoreMeter::TurnOffMeter()
{
    m_bMeterVisible = false;
}

void ShootToScoreMeter::TurnOnMeter()
{
    m_bMeterVisible = true;
    mbShowSavedWhiteBar = false;
    mbSecondButtonPressed = false;
    mbShowGreenRegion = false;
    m_fWhiteBarAngle = 0.0f;
    m_fSavedWhiteBarAngle = 0.0f;
    mfRumbleAmount = 0.0f;
    m_fWhiteBarPreviousAngle = 0.0f;
}

ShootToScoreMeter::ShootToScoreMeter()
{
    m_bMeterVisible = false;
    mfRumbleAmount = 0.0f;
    m_fWhiteBarAngle = 0.0f;
    m_fWhiteBarPreviousAngle = 0.0f;
    m_fSavedWhiteBarAngle = 0.0f;
    mbSecondButtonPressed = false;
    mbShowSavedWhiteBar = false;
    mbShowGreenRegion = false;
    m_fGreenBarAngle = 0.0f;
    m_fGreenRegionWidth = 0.0f;
    m_fYellowRegionWidth = 0.0f;
    m_fSegment1Angle = 0.0f;
    m_fSegment1Width = 0.0f;
    m_fSegment2Angle = 0.0f;
    m_fSegment2Width = 0.0f;
    m_fSegment3Angle = 0.0f;
    m_fSegment3Width = 0.0f;
    m_fSegment4Angle = 0.0f;
    m_fSegment4Width = 0.0f;
    m_fSegment5Angle = 0.0f;
    m_fSegment5Width = 0.0f;
}
