#include "Game/Render/ShootToScoreMeter.h"

#include "Game/AI/AiUtil.h"
#include "Game/Camera/CameraMan.h"
#include "Game/Game.h"
#include "Game/Render/RLView.h"
#include "NL/gl/glDraw3.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/nlString.h"
#include "NL/platvmath.h"

class BaseGameSceneManager;

extern "C"
{
    GLView* fn_8027267C(int index);
    void fn_802CE528(
        GLView* view, const nlVector3* world, nlVector3* projected);
    void fn_802CE6DC(
        GLView* view, const nlVector3* normalized, nlVector3* screen);
    float fn_802CE76C(GLView* view);
    float fn_802CE7B0(GLView* view);
    void fn_802CEA40(GLView* source, GLView* destination,
        const nlVector3* world, nlVector3* projected);
    extern BaseGameSceneManager* lbl_806E1860;
    void fn_801E29C0(BaseGameSceneManager* manager, nlVector3 position);
    u32 fn_80369D4C();
    u32 fn_80369D54();
}

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
static nlColour lbl_806DD0AC = { 255, 175, 0, 255 };
static nlColour lbl_806DD0B0 = { 255, 100, 0, 255 };
float ShootToScoreMeter::MeterWidth = 0.315f;
static s32 lbl_806DD0B8 = 150;
static float lbl_806DD0BC = 0.008f;
static float lbl_806DD0C0 = 0.05f;
static float lbl_806DD0C4 = 0.04f;
static float lbl_806DD0C8 = 0.388f;
static float lbl_806DD0CC = 180.0f;
static float sfTrailIntensity = 0.33f;
static float sfTrailLengthScale = 0.33f;
static s32 sfNumBarsInTrail = 12;
static float lbl_806DD0DC = 0.005f;
static float lbl_806DD0E0 = 0.165f;
static float lbl_806DD0E4 = 8.0f;

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
    position *= 180.0f;
    return oneMinusPosition * sfMeterStart + position;
}

void ShootToScoreMeter::fn_801B10E4(float value)
{
    mUnidentified58 = MeterPosition(value);
}

void ShootToScoreMeter::fn_801B10C4(float value)
{
    mUnidentified54 = MeterPosition(value);
}

void ShootToScoreMeter::fn_801B10A4(float value)
{
    mUnidentified50 = MeterPosition(value);
}

void ShootToScoreMeter::fn_801B1084(float value)
{
    mUnidentified4C = MeterPosition(value);
}

void ShootToScoreMeter::fn_801B1064(float value)
{
    mUnidentified48 = MeterPosition(value);
}

void ShootToScoreMeter::fn_801B1044(float value)
{
    mUnidentified44 = MeterPosition(value);
}

void ShootToScoreMeter::fn_801B1024(float value)
{
    mUnidentified40 = MeterPosition(value);
}

void ShootToScoreMeter::fn_801B1004(float value)
{
    mUnidentified3C = MeterPosition(value);
}

void ShootToScoreMeter::fn_801B0FE4(float value)
{
    m_fYellowRegionWidth = MeterPosition(value);
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
            float amount = mfRumbleAmount / lbl_806DD0DC;
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

    scaledWhiteBarWidth = lbl_806DD0C0 * scale;
    widthAngle = endAngle - startAngle;
    radius = lbl_806DD0C8 * (MeterWidth * scale);

    float step = 0.125f;
    float sinScale = 10430.378f;
    float pi = 3.1415927f;
    float deg = 180.0f;

    for (i = 0; i < 8; i++)
    {
        startFraction = (float)i * step;
        endFraction = (float)(i + 1) * step;
        innerRadius = radius - scaledWhiteBarWidth / 2.0f;
        outerRadius = radius + scaledWhiteBarWidth / 2.0f;

        float segmentStartAngle = startFraction * widthAngle + startAngle;
        float segmentStartAngleRadians
            = pi * segmentStartAngle / deg;
        float segmentStartCosine = nlSin((u16)((u16)(s32)(sinScale
            * segmentStartAngleRadians)
            + 0x4000));
        float segmentStartSine = nlSin(
            (u16)(s32)(sinScale * segmentStartAngleRadians));

        float segmentEndAngle = endFraction * widthAngle + startAngle;
        float segmentEndAngleRadians = pi * segmentEndAngle / deg;
        float segmentEndCosine = nlSin((u16)((u16)(s32)(sinScale
            * segmentEndAngleRadians)
            + 0x4000));
        float segmentEndSine = nlSin(
            (u16)(s32)(sinScale * segmentEndAngleRadians));

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
            (eGLView)(u32)fn_8027267C(eCLV_UnsortedSquareOrtho), 1,
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
    float angleRadians;
    float scaledMeterWidth = MeterWidth * scale;
    float scaledWhiteBarWidth
        = (lbl_806DD0C0 + lbl_806DD0C4) * scale;
    float scaledWhiteBarHeight = lbl_806DD0BC * scale;
    angleRadians = (3.1415927f * angle) / 180.0f;

    nlMatrix4 barMatrix;
    nlMakeRotationMatrixZ(barMatrix, angleRadians);

    float radius = scaledMeterWidth * lbl_806DD0C8;
    float sine
        = radius * nlSin((u16)(s32)(10430.378f * angleRadians));
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
    glAttachQuad3((eGLView)(u32)fn_8027267C(eCLV_UnsortedSquareOrtho), 1,
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

    float rumbleScale = mfRumbleAmount / lbl_806DD0DC;
    float rotation;
    if (nlRandomf(1.0f, &nlDefaultSeed) < 0.5f)
    {
        rotation = lbl_806DD0E4;
    }
    else
    {
        rotation = -lbl_806DD0E4;
    }
    rotation = InterpolateRangeClamped(
        rotation, 0.0f, 0.0f, 1.0f, rumbleScale);
    nlMakeRotationMatrixZ(matrix, (3.1415927f * rotation) / 180.0f);

    static nlVector3 screenPosition;
    fn_802CEA40(fn_8027267C(eCLV_Unshadowed),
        fn_8027267C(eCLV_UnsortedSquareOrtho), &m_v3MeterPosition,
        &screenPosition);
    screenPosition.z = -0.1f;
    screenPosition.y += -20.0f;

    float screenWidth = (float)fn_80369D4C();
    float screenHeight = (float)fn_80369D54();
    float scaledMeterWidth = MeterWidth * screenWidth;
    float screenMargin = 60.0f;
    float lowerY = 0.05f
        * fn_802CE7B0(fn_8027267C(eCLV_UnsortedSquareOrtho));
    GLView* view = fn_8027267C(eCLV_UnsortedSquareOrtho);
    float upperY = 0.05f * fn_802CE7B0(view);
    upperY = fn_802CE7B0(view) - upperY;
    view = fn_8027267C(eCLV_UnsortedSquareOrtho);
    float upperX = 0.05f * fn_802CE76C(view);
    upperX = fn_802CE76C(view) - upperX - screenMargin;
    float lowerX = 0.05f
        * fn_802CE76C(fn_8027267C(eCLV_UnsortedSquareOrtho));
    lowerX += screenMargin;
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
    fn_802CE528(fn_8027267C(eCLV_UnsortedSquareOrtho), &screenPosition,
        &projectedPosition);
    fn_802CE6DC(fn_8027267C(eCLV_Anark), &projectedPosition,
        &projectedPosition);
    fn_801E29C0(lbl_806E1860, projectedPosition);

    glQuad3 quad;
    quad.SetupRotatedRectangle(
        scaledMeterWidth, scaledMeterWidth, matrix, true, false);
    glAttachQuad3((eGLView)(u32)fn_8027267C(eCLV_UnsortedSquareOrtho), 1,
        &quad);

    nlColour green = sGreenRegionColour;
    nlColour white = sWhiteBarColour;
    nlColour yellow = sYellowRegionColour;
    nlColour orange = lbl_806DD0AC;
    nlColour red = lbl_806DD0B0;

    if (mUnidentified2E)
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
                >= mUnidentified3C - 0.5f * mUnidentified40
            && m_fSavedWhiteBarAngle
                <= mUnidentified3C + 0.5f * mUnidentified40)
        {
            yellow.c[3] = (u8)lbl_806DD0B8;
            DrawColouredRegion(mUnidentified3C - 0.5f * mUnidentified40,
                mUnidentified3C + 0.5f * mUnidentified40, yellow, yellow,
                matrix, screenWidth);
            savedColour = yellow;
        }
        else if (m_fSavedWhiteBarAngle
                >= mUnidentified44 - 0.5f * mUnidentified48
            && m_fSavedWhiteBarAngle
                <= mUnidentified44 + 0.5f * mUnidentified48)
        {
            orange.c[3] = (u8)lbl_806DD0B8;
            DrawColouredRegion(mUnidentified44 - 0.5f * mUnidentified48,
                mUnidentified44 + 0.5f * mUnidentified48, orange, orange,
                matrix, screenWidth);
            savedColour = orange;
        }
        else if (m_fSavedWhiteBarAngle
                >= mUnidentified4C - 0.5f * mUnidentified50
            && m_fSavedWhiteBarAngle
                <= mUnidentified4C + 0.5f * mUnidentified50)
        {
            red.c[3] = (u8)lbl_806DD0B8;
            DrawColouredRegion(mUnidentified4C - 0.5f * mUnidentified50,
                mUnidentified4C + 0.5f * mUnidentified50, red, red, matrix,
                screenWidth);
            savedColour = red;
        }
        else if (m_fSavedWhiteBarAngle
                >= mUnidentified54 - 0.5f * mUnidentified58
            && m_fSavedWhiteBarAngle
                <= mUnidentified54 + 0.5f * mUnidentified58)
        {
            orange.c[3] = (u8)lbl_806DD0B8;
            DrawColouredRegion(mUnidentified54 - 0.5f * mUnidentified58,
                mUnidentified54 + 0.5f * mUnidentified58, orange, orange,
                matrix, screenWidth);
            savedColour = orange;
        }
        else if (m_fSavedWhiteBarAngle
                >= mUnidentified5C - 0.5f * mUnidentified60
            && m_fSavedWhiteBarAngle
                <= mUnidentified5C + 0.5f * mUnidentified60)
        {
            yellow.c[3] = (u8)lbl_806DD0B8;
            DrawColouredRegion(mUnidentified5C - 0.5f * mUnidentified60,
                mUnidentified5C + 0.5f * mUnidentified60, yellow, yellow,
                matrix, screenWidth);
            savedColour = yellow;
        }
        savedColour.c[3] = 255;
        DrawIndicatorBar(
            m_fSavedWhiteBarAngle, savedColour, matrix, screenWidth);
    }
    else
    {
        DrawColouredRegion(mUnidentified3C - 0.5f * mUnidentified40,
            mUnidentified3C + 0.5f * mUnidentified40, yellow, yellow,
            matrix, screenWidth);
        DrawColouredRegion(mUnidentified44 - 0.5f * mUnidentified48,
            mUnidentified44 + 0.5f * mUnidentified48, orange, orange,
            matrix, screenWidth);
        DrawColouredRegion(mUnidentified4C - 0.5f * mUnidentified50,
            mUnidentified4C + 0.5f * mUnidentified50, red, red, matrix,
            screenWidth);
        DrawColouredRegion(mUnidentified54 - 0.5f * mUnidentified58,
            mUnidentified54 + 0.5f * mUnidentified58, orange, orange,
            matrix, screenWidth);
        DrawColouredRegion(mUnidentified5C - 0.5f * mUnidentified60,
            mUnidentified5C + 0.5f * mUnidentified60, yellow, yellow,
            matrix, screenWidth);
        DrawIndicatorBar(
            m_fSavedWhiteBarAngle, white, matrix, screenWidth);
    }

    nlColour trailColour = sWhiteBarColour;
    if (mUnidentified2C)
    {
        if (m_fWhiteBarAngle
                >= m_fGreenBarAngle - 0.5f * m_fYellowRegionWidth
            && m_fWhiteBarAngle
                <= m_fGreenBarAngle + 0.5f * m_fYellowRegionWidth)
        {
            DrawIndicatorBar(
                m_fWhiteBarAngle, red, matrix, screenWidth);
            trailColour = red;
        }
        else if (m_fWhiteBarAngle
                >= m_fGreenBarAngle - 0.5f * m_fGreenRegionWidth
            && m_fWhiteBarAngle
                <= m_fGreenBarAngle + 0.5f * m_fGreenRegionWidth)
        {
            DrawIndicatorBar(
                m_fWhiteBarAngle, green, matrix, screenWidth);
            trailColour = green;
        }
        else
        {
            DrawIndicatorBar(
                m_fWhiteBarAngle, sWhiteBarColour, matrix, screenWidth);
            trailColour = sWhiteBarColour;
        }
    }
    else
    {
        DrawIndicatorBar(
            m_fWhiteBarAngle, sWhiteBarColour, matrix, screenWidth);
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
            amount = lbl_806DD0E0;
        }
        else
        {
            amount = -lbl_806DD0E0;
        }

        nlVector3 offset;
        nlPolarToCartesian(offset.x, offset.y, angle, amount);
        offset.z = 0.0f;
        m_v3MeterPosition.x = m_v3OriginalMeterPosition.x + offset.x;
        m_v3MeterPosition.y = m_v3OriginalMeterPosition.y;
        m_v3MeterPosition.z = m_v3OriginalMeterPosition.z + offset.y;
        mfRumbleAmount = lbl_806DD0DC;
    }
}

void ShootToScoreMeter::fn_801AF97C()
{
    m_bMeterVisible = false;
}

void ShootToScoreMeter::TurnOnMeter()
{
    m_bMeterVisible = true;
    mbShowSavedWhiteBar = false;
    mUnidentified2C = false;
    mUnidentified2E = false;
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
    mUnidentified2C = false;
    mbShowSavedWhiteBar = false;
    mUnidentified2E = false;
    m_fGreenBarAngle = 0.0f;
    m_fGreenRegionWidth = 0.0f;
    m_fYellowRegionWidth = 0.0f;
    mUnidentified3C = 0.0f;
    mUnidentified40 = 0.0f;
    mUnidentified44 = 0.0f;
    mUnidentified48 = 0.0f;
    mUnidentified4C = 0.0f;
    mUnidentified50 = 0.0f;
    mUnidentified54 = 0.0f;
    mUnidentified58 = 0.0f;
    mUnidentified5C = 0.0f;
    mUnidentified60 = 0.0f;
}
