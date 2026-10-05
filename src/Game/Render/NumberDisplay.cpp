#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"

#include "Game/Render/NumberDisplay.h"

#include "Game/Drawable/DrawableObj.h"
#include "Game/EventRegistry.h"
#include "Game/Game.h"
#include "Game/Render/RLView.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/Render/StadiumLoading.h"
#include "Game/Team.h"
#include "NL/gl/glMaterialParameters.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/nlLocalization.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"
#include "NL/nlFunction.inl"

extern "C"
{
    const float gNumberDigitWidths[10] = {
        0.1662f,
        0.1164f,
        0.1682f,
        0.1658f,
        0.1749f,
        0.1675f,
        0.1667f,
        0.1644f,
        0.1632f,
        0.1671f,
    };

    float gNumberDigitSpacing = -0.0f;
    float gScoreCollapseTime = 0.6f;
    float gGoalCountExpandTime = 1.0f;
    float gScoreIncrementInterval = 0.4f;
    float gAccumulatedScoreIncrementInterval = 0.4f;
    float gNumberWidescreenWidthScale = 0.8f;
    float gNumberWidescreenDepthScale = 1.05f;
    float gScoreCompactScale = 140.0f;
    float gScoreCompactY = 50.0f;
    float gScoreCompactSeparation = 60.0f;
    float gScoreCompactWideSeparation = 82.0f;
    float gScoreCompactOpacity = 1.0f;
    float gScoreExpandedScale = 650.0f;
    float gScoreExpandedThreeDigitScale = 400.0f;
    float gScoreExpandedThreeDigitWideScale = 500.0f;
    float gScoreExpandedY = 220.0f;
    float gScoreExpandedSeparation = 80.0f;
    float gScoreExpandedWideSeparation = 80.0f;
    float gScoreExpandedOpacity = 0.45f;
    float gGoalCountStartScale = 50.0f;
    float gGoalCountEndScale = 800.0f;
    float gGoalCountOpacity = 0.6f;
    float gNumberRotationX[2] = { 90.0f, 0.0f };

    NumberDisplay* gpNumberDisplay;
    bool gScoreForceExpanded;
    bool gScoreUseAlternateSeparator;
    float gNumberPositionZ;
    float gScoreExpandTime;
    float gScoreExpandedHoldTime;
    float gNumberRotationY;
    float gNumberRotationZ;
}

NumberDisplay::NumberDisplay()
{
    mVisible = false;
    mHoldUntilKickoff = false;
    mExpanded = false;
    mExpansion = 0.0f;
    mScoreUpdateTimer = 0.0f;
    mExpandedHoldTimer = 0.0f;
    mGoalCount = 0;
    mShowGoalCount = false;
    mShowAccumulatedScore = false;

    mModels = GetNumberRenderObjects();
    mLeftScore = 0;
    mRightScore = 0;

    typedef Detail::MemFunImpl<void, void (NumberDisplay::*)()> MemFunImpl_NumberDisplay_v;
    typedef BindExp1<void, MemFunImpl_NumberDisplay_v, NumberDisplay*> BindExp1_NumberDisplay_v;
    UnidentifiedFindEvent<void>("GetReadyForKickoff", -1)->Add(Function<void*>(BindExp1_NumberDisplay_v(MemFun(&NumberDisplay::OnGetReadyForKickoff), this)), 0, -1);
}

NumberDisplay::~NumberDisplay()
{
}

void NumberDisplay::OnGetReadyForKickoff()
{
    mHoldUntilKickoff = false;
}

void NumberDisplay::Update(float deltaTime)
{
    if (g_pGame->mbCaptainShotToScoreOn)
    {
        return;
    }

    if (nlTaskManager::m_pInstance->mCurrentState == 2)
    {
        mShowAccumulatedScore = false;
    }

    if (mShowGoalCount)
    {
        float amount = 1.0f;
        if (gGoalCountExpandTime != 0.0f)
        {
            amount = deltaTime / gGoalCountExpandTime;
        }
        mExpansion += amount;
        mExpansion
            = 1.0f <= mExpansion ? 1.0f : mExpansion;
        return;
    }

    if (nlTaskManager::m_pInstance->mCurrentState == 0x10
        && !mShowAccumulatedScore)
    {
        return;
    }

    if (mExpanded || gScoreForceExpanded)
    {
        float amount = 1.0f;
        if (gScoreExpandTime != 0.0f)
        {
            amount = deltaTime / gScoreExpandTime;
        }
        mExpansion += amount;
        mExpansion
            = 1.0f <= mExpansion ? 1.0f : mExpansion;
    }
    else
    {
        float amount = 1.0f;
        if (gScoreCollapseTime != 0.0f)
        {
            amount = deltaTime / gScoreCollapseTime;
        }
        mExpansion -= amount;
        mExpansion
            = 0.0f >= mExpansion ? 0.0f : mExpansion;
    }

    if (mScoreUpdateTimer > 0.0f)
    {
        mScoreUpdateTimer -= deltaTime;
        if (mScoreUpdateTimer <= 0.0f)
        {
            mScoreUpdateTimer = 0.0f;
            bool changed = false;
            if (g_pTeams[0]->m_nScore > mLeftScore)
            {
                ++mLeftScore;
                changed = true;
            }
            if (g_pTeams[1]->m_nScore > mRightScore)
            {
                ++mRightScore;
                changed = true;
            }
            if (changed == true)
            {
                if (mShowAccumulatedScore)
                {
                    mScoreUpdateTimer = gAccumulatedScoreIncrementInterval;
                }
                else
                {
                    mScoreUpdateTimer = gScoreIncrementInterval;
                }
            }
        }
    }

    if (mHoldUntilKickoff)
    {
        return;
    }

    if (mExpanded && mScoreUpdateTimer == 0.0f)
    {
        mExpandedHoldTimer -= deltaTime;
        if (mExpandedHoldTimer <= 0.0f)
        {
            EndScoreUpdate();
        }
    }
    else if (!mExpanded)
    {
        mExpandedHoldTimer = 0.0f;
    }
}

void NumberDisplay::Render()
{
    mModels = GetNumberRenderObjects();
    if (mModels == 0)
    {
        return;
    }
    if ((g_pGame->m_eGameState == 3
         && nlTaskManager::m_pInstance->mCurrentState == 8)
        || !mVisible)
    {
        return;
    }

    unsigned int state = nlTaskManager::m_pInstance->mCurrentState;
    if (state == 0x10 && !mShowGoalCount && !mShowAccumulatedScore)
    {
        return;
    }
    if (state == 1)
    {
        return;
    }

    if (mShowGoalCount)
    {
        float centerX = 320.0f;
        float scaleRange = gGoalCountEndScale - gGoalCountStartScale;
        float scale = scaleRange * mExpansion + gGoalCountStartScale;
        if (IsWidescreen())
        {
            centerX = 320.0f;
        }
        nlVector2 position;
        nlVec2Set(position, centerX, 240.0f);
        RenderGlyph(mGoalCount, scale, gGoalCountOpacity, position);
    }
    else if (mShowAccumulatedScore || state != 0x10)
    {
        RenderScores();
    }
}

void NumberDisplay::RenderScores()
{
    float positionYRange = gScoreExpandedY - gScoreCompactY;
    float separationRange = gScoreExpandedSeparation - gScoreCompactSeparation;
    float scaleRange = gScoreExpandedScale - gScoreCompactScale;
    float opacityRange = gScoreExpandedOpacity - gScoreCompactOpacity;

    float positionY = positionYRange * mExpansion + gScoreCompactY;
    float separation = separationRange * mExpansion + gScoreCompactSeparation;
    float scale = scaleRange * mExpansion + gScoreCompactScale;
    float opacity = opacityRange * mExpansion + gScoreCompactOpacity;

    if (IsWidescreen())
    {
        separationRange = gScoreExpandedWideSeparation - gScoreCompactWideSeparation;
        separation = separationRange * mExpansion + gScoreCompactWideSeparation;
    }

    if (mLeftScore >= 100 || mRightScore >= 100)
    {
        scaleRange = IsWidescreen()
            ? gScoreExpandedThreeDigitWideScale - gScoreCompactScale
            : gScoreExpandedThreeDigitScale - gScoreCompactScale;
        scale = scaleRange * mExpansion + gScoreCompactScale;
    }

    nlVector2 position;
    position.y = positionY;
    float centerX = 320.0f;
    if (IsWidescreen())
    {
        centerX = 320.0f;
    }
    float digitScale = scale;
    if (IsWidescreen())
    {
        digitScale *= gNumberWidescreenWidthScale;
    }
    position.y = positionY + 0.2f * scale / 2.0f;

    float width = 0.0f;
    int remaining = mLeftScore;
    do
    {
        int digit = remaining % 10;
        width += digitScale * gNumberDigitWidths[digit];
        if (remaining < 10)
        {
            break;
        }
        width += gNumberDigitSpacing * digitScale;
        remaining /= 10;
    } while (true);

    width = 0.0f;
    int value = mLeftScore;
    do
    {
        int digit = value % 10;
        float digitWidth = digitScale * gNumberDigitWidths[digit];
        position.x = centerX - separation - width - digitWidth / 2.0f;
        RenderGlyph(digit, scale, opacity, position);
        width += digitWidth + gNumberDigitSpacing * digitScale;
        if (value >= 10)
        {
            value /= 10;
        }
        else
        {
            break;
        }
    } while (true);

    width = 0.0f;
    remaining = mRightScore;
    do
    {
        int digit = remaining % 10;
        width += digitScale * gNumberDigitWidths[digit];
        if (remaining < 10)
        {
            break;
        }
        width += gNumberDigitSpacing * digitScale;
        remaining /= 10;
    } while (true);

    value = mRightScore;
    do
    {
        int digit = value % 10;
        float digitWidth = digitScale * gNumberDigitWidths[digit];
        position.x = width + (centerX + separation) - digitWidth / 2.0f;
        RenderGlyph(digit, scale, opacity, position);
        width -= digitWidth + gNumberDigitSpacing * digitScale;
        if (value >= 10)
        {
            value /= 10;
        }
        else
        {
            break;
        }
    } while (true);

    if (mExpansion == 1.0f)
    {
        position.x = centerX;
        if (g_pLocalization->m_CurrentLanguage == nlLocalization::LangGerman
            || gScoreUseAlternateSeparator == true)
        {
            RenderGlyph(11, scale, opacity, position);
        }
        else
        {
            RenderGlyph(10, scale, opacity, position);
        }
    }
}

void NumberDisplay::RenderGlyph(
    int modelIndex, float scale, float opacity, const nlVector2& position)
{
    DrawableObject* source = mModels[modelIndex];
    source->m_uObjectFlags |= 1;

    nlMatrix4 transform;
    transform.SetIdentity();
    if (IsWidescreen())
    {
        nlMakeScaleMatrix(transform,
            scale * gNumberWidescreenWidthScale, scale, scale * gNumberWidescreenDepthScale);
    }
    else
    {
        nlMakeScaleMatrix(transform, scale, scale, scale);
    }

    if (gNumberRotationX[0] != 0.0f)
    {
        nlMatrix4 rotation;
        rotation.SetIdentity();
        nlVector3 axis = { 1.0f, 0.0f, 0.0f };
        nlMakeRotationMatrixAxisAngle(rotation, axis,
            DegreesToRadians(gNumberRotationX[0]));
        nlMultMatrices(transform, rotation);
    }
    if (gNumberRotationY != 0.0f)
    {
        nlMatrix4 rotation;
        rotation.SetIdentity();
        nlVector3 axis = { 0.0f, 1.0f, 0.0f };
        nlMakeRotationMatrixAxisAngle(rotation, axis,
            DegreesToRadians(gNumberRotationY));
        nlMultMatrices(transform, rotation);
    }
    if (gNumberRotationZ != 0.0f)
    {
        nlMatrix4 rotation;
        rotation.SetIdentity();
        nlVector3 axis = { 0.0f, 0.0f, 1.0f };
        nlMakeRotationMatrixAxisAngle(rotation, axis,
            DegreesToRadians(gNumberRotationZ));
        nlMultMatrices(transform, rotation);
    }

    transform.SetRow4_(3, position.x, position.y, gNumberPositionZ, 1.0f);

    glModel* model = glModelDupNoStreams(source->m_pModel, false, 0);
    glModelSetMatrix(model, transform);

    static unsigned long constantColourHash
        = nlStringLowerHash("constantcolour");
    nlVector4 colour = { 1.0f, 1.0f, 1.0f, 0.0f };
    colour.w = opacity;
    glModelPacket* packet = model->packets;
    while (packet < model->packets + model->numPackets)
    {
        if (glHasMaterialParameter(packet, constantColourHash))
        {
            glSetMaterialParameterArray(packet, constantColourHash, &colour, 4);
        }
        glSetRasterState(packet->rasterState, GLS_Culling, 0);
        glSetRasterState(packet->rasterState, GLS_AlphaBlend, 1);
        glSetRasterState(packet->rasterState, GLS_AlphaTest, 1);
        glSetRasterState(packet->rasterState, GLS_AlphaTestRef, 3);
        ++packet;
    }
    GetLayerView(eCLV_UnsortedOrtho640)->AttachModel(model, 0);
}

void NumberDisplay::SetScores(
    int firstScore, int secondScore)
{
    mLeftScore = firstScore;
    mRightScore = secondScore;
}

void NumberDisplay::Reset()
{
    mVisible = false;
    mHoldUntilKickoff = false;
    mExpanded = false;
    mExpansion = 0.0f;
    mScoreUpdateTimer = 0.0f;
    mExpandedHoldTimer = 0.0f;
    mGoalCount = 0;
    mShowGoalCount = false;
    mShowAccumulatedScore = false;
    mLeftScore = 0;
    mRightScore = 0;
}

void NumberDisplay::BeginScoreUpdate()
{
    mExpanded = true;
    mScoreUpdateTimer = gScoreIncrementInterval;
    mHoldUntilKickoff = true;
    mExpandedHoldTimer = gScoreExpandedHoldTime;
}

void NumberDisplay::IncrementGoalCount()
{
    mVisible = true;
    mExpansion = 0.0f;
    ++mGoalCount;
    mShowGoalCount = true;
    mShowAccumulatedScore = false;
}

void NumberDisplay::ResetGoalCount()
{
    mGoalCount = 0;
}

void NumberDisplay::ShowScores()
{
    mVisible = true;
    if (mShowGoalCount == true)
    {
        mShowGoalCount = false;
    }
}

void NumberDisplay::ShowAccumulatedScore()
{
    mVisible = true;
    mShowAccumulatedScore = true;
    if (mGoalCount > 0)
    {
        mExpanded = true;
        mGoalCount = 0;
        mScoreUpdateTimer = gScoreIncrementInterval;
        mHoldUntilKickoff = true;
        mExpandedHoldTimer = gScoreExpandedHoldTime;
    }
}
