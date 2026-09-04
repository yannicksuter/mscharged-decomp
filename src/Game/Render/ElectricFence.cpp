#include "Game/Render/ElectricFence.h"

#include "Game/Effects/EmissionManager.h"
#include "Game/Field.h"
#include "Game/GL/MeshWriter.h"
#include "Game/Net.h"
#include "NL/gl/glDraw3.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/nlTask.h"

#include <math.h>

class EffectsGroup;

extern "C" float AIsgn(float);
extern "C" EffectsGroup* fn_802E7CDC(EmissionManager*, const char*);
extern "C" EmissionController* fn_802E7FE4(
    EmissionManager*, EffectsGroup*, int, bool, bool);
extern "C" void* memcpy(void*, const void*, unsigned long);

static float sfGridTextureSize = 7.0f;
static float sfNumGridSquares = 9.48f;
static float lbl_806DCDF0 = 16.0f;
static float sfFadeOutTime = 0.2f;
static float sfAlignmentOffset1 = 0.25f;
static float sfAlignmentOffset2 = 0.13f;
static float sfAngleAnimationRate = 100.0f;
static float sfTimeBetweenEffects = 0.02f;
static int sNumRevolutionsToDisplay = 2;
static float sfStartAngle = 180.0f;
static float lbl_806DCE10 = 5.0f;

const unsigned long GridTexture = glGetTexture("global/grid");
static float sfAngleRandomOffset;
static bool sbUseSparksDuringElectricFenceFlyBy;
GLView* lbl_806E1514;

int ElectricFenceData::numAllocated;
nlList<ElectricFenceData> ElectricFenceData::sActiveElectricFences(
    (ElectricFenceData*)0, (ElectricFenceData*)0);
SlotPool<ElectricFenceData> ElectricFenceData::sElectricFenceDataPool(16, 16);
SlotPool<ElectricFenceGeometry>
    ElectricFenceGeometry::sElectricFenceGeometryPool(4, 4);

static bool sbIsElectricFenceBeingDisplayed;
static float sfElectricFenceDisplayAngle;

static void GetWallPoint(const nlVector3& impactPosition, float xOffset,
    float zOffset, nlVector3& outPosition)
{
    float radius = cField::GetCornerRadius();
    float goalLineX = cField::GetGoalLineX(1U);
    float sideLineY = cField::GetSidelineY(1U);
    bool xIsPositive = impactPosition.x > 0.0f;
    bool yIsPositive = impactPosition.y > 0.0f;

    nlVector3 impactPositionPositive = { {
        (float)__fabs(impactPosition.x),
        (float)__fabs(impactPosition.y),
        impactPosition.z,
    } };

    float inCoordinate;
    float cornerCircumference = radius;
    cornerCircumference *= 1.5707964f;

    if (impactPositionPositive.x >= goalLineX - radius
        && impactPositionPositive.y >= sideLineY - radius)
    {
        inCoordinate = nlATan2f(
            impactPositionPositive.y - (sideLineY - radius),
            impactPositionPositive.x - (goalLineX - radius));
        inCoordinate = inCoordinate * radius;
    }
    else if (impactPositionPositive.x < goalLineX - radius)
    {
        inCoordinate = cornerCircumference
            + ((goalLineX - radius) - impactPositionPositive.x);
    }
    else
    {
        inCoordinate = -((sideLineY - radius) - impactPositionPositive.y);
    }

    float increment = sfGridTextureSize / sfNumGridSquares;
    inCoordinate = sfAlignmentOffset2
        + increment * (float)floor(inCoordinate / increment);
    float outCoordinate = inCoordinate + xOffset;

    if (outCoordinate <= 0.0f)
    {
        nlVec3Set(outPosition, goalLineX,
            outCoordinate + (sideLineY - radius),
            impactPositionPositive.z + zOffset);
    }
    else if (outCoordinate >= cornerCircumference)
    {
        nlVec3Set(outPosition,
            (goalLineX - radius) - (outCoordinate - cornerCircumference),
            sideLineY, impactPositionPositive.z + zOffset);
    }
    else
    {
        float ratio_8019DEA0 = outCoordinate / radius;
        inCoordinate =
            nlSin((u16)(s32)(10430.378f * ratio_8019DEA0));
        nlVec3Set(outPosition,
            (goalLineX - radius)
                + (radius
                    * nlSin((u16)((u16)(s32)(10430.378f
                                          * ratio_8019DEA0)
                        + 0x4000))),
            (sideLineY - radius) + (radius * inCoordinate),
            impactPositionPositive.z + zOffset);
    }

    if (xIsPositive == 0)
    {
        outPosition.x = -outPosition.x;
    }
    if (yIsPositive == 0)
    {
        outPosition.y = -outPosition.y;
    }
}

static void CreateElectricFenceGeometry(
    ElectricFenceGeometry& prim, const nlVector3& impactPosition)
{
    float radius = cField::GetCornerRadius();
    nlVector3 cornerPosition = {
        AIsgn(impactPosition.x) * (cField::GetGoalLineX(1U) - radius),
        AIsgn(impactPosition.y) * (cField::GetSidelineY(1U) - radius),
        0.0f,
    };

    prim.vertCount = 32;
    nlVector3* pdst = prim.position;
    nlVector2* tdst = prim.texcoord;

    float startOffset = 0.5f * -sfGridTextureSize;
    float endOffset = 0.5f * sfGridTextureSize;
    float fDeltaSegmentOffset = (endOffset - startOffset) / 15.0f;
    float z0 = impactPosition.z + startOffset;
    float z1 = impactPosition.z + endOffset;

    for (int nSegment = 0; nSegment < 16;)
    {
        nlVector3 wallPoint;
        GetWallPoint(impactPosition,
            ((float)nSegment * fDeltaSegmentOffset) + startOffset,
            0.0f, wallPoint);

        int segment = nSegment;
        float wallY = wallPoint.y;
        float wallX = wallPoint.x;
        pdst[0].x = wallX;
        pdst[0].y = wallY;
        pdst[0].z = z0;
        tdst[0].x = (float)segment / 15.0f;
        tdst[0].y = 0.0f;

        float wallY2 = wallPoint.y;
        float wallX2 = wallPoint.x;
        pdst[1].x = wallX2;
        pdst[1].y = wallY2;
        pdst[1].z = z1;
        tdst[1].x = (float)segment / 15.0f;
        tdst[1].y = 1.0f;
        ++nSegment;

        pdst += 2;
        tdst += 2;
    }

    (void)cornerPosition;
}

static void DrawPrimitive(const ElectricFenceGeometry& prim,
    const nlMatrix4& objMatrix, int nPrimType,
    unsigned long textureHandle, float intensity)
{
    MeshWriter mesh;
    nlVector3* pPosition = (nlVector3*)prim.position;
    nlVector2* pTexcoord = (nlVector2*)prim.texcoord;

    glSetDefaultState(true);
    glSetRasterState(GLS_Culling, 0);
    glSetRasterState(GLS_DepthWrite, 0);
    glSetRasterState(GLS_AlphaBlend, 2);
    glSetCurrentRasterState(glHandleizeRasterState());

    unsigned long matrixHandle = glAllocMatrix();
    if (matrixHandle != (unsigned long)-1)
    {
        glSetMatrix(matrixHandle, objMatrix);
    }
    glSetCurrentMatrix(matrixHandle);

    float colour[4];
    colour[0] = intensity;
    colour[1] = intensity;
    colour[2] = intensity;
    colour[3] = 1.0f;
    if (mesh.Begin(prim.vertCount, nPrimType, 0))
    {
        UnidentifiedTextureState* textureState =
            (UnidentifiedTextureState*)mesh.model->packets->unknown20;
        textureState->texture = textureHandle;
        textureState->textureIndex = 0xFFFF;
        textureState->SetWrapS(false);
        textureState->SetWrapT(false);
        textureState->unknown07 = 0;
        memcpy((u8*)mesh.GetModel()->packets->unknown20
                + sizeof(UnidentifiedTextureState),
            colour, sizeof(colour));

        int index = 0;
        while (index < prim.vertCount)
        {
            mesh.Texcoord(pTexcoord[index]);
            mesh.Vertex(pPosition[index]);
            ++index;
        }

        if (!mesh.End())
        {
            return;
        }
        lbl_806E1514->AttachModel(mesh.GetModel(), 0);
    }
}

static void RenderElectricFenceFlat(const nlVector3& position,
    nlVector3 normal, float intensity)
{
    glSetDefaultState(true);
    glSetRasterState(GLS_AlphaBlend, 2);
    glSetRasterState(GLS_Culling, 0);
    glSetRasterState(GLS_DepthWrite, 0);
    glSetRasterState(GLS_DepthTest, 1);
    glSetCurrentRasterState(glHandleizeRasterState());
    glSetCurrentTexture(GridTexture, GLTT_Diffuse);
    glSetTextureState(GLTS_DiffuseWrap, 0);
    glSetCurrentTextureState(glHandleizeTextureState());

    nlMatrix4 matrix;
    nlMakeRotationMatrixX(matrix, 1.5707964f);

    float angle = nlATan2f(normal.y, normal.x);
    nlMatrix4 matrix2;
    nlMakeRotationMatrixZ(matrix2,
        0.0000958738f * (float)(u16)(s32)(10430.378f * angle));
    nlMatrix4 matrix_8019E544;
    nlMultMatrices(matrix_8019E544, matrix, matrix2);
    matrix = matrix_8019E544;

    matrix.m41 = position.x;
    matrix.m42 = position.y;
    matrix.m43 = position.z;
    matrix.m44 = 1.0f;

    glQuad3 quad;
    quad.SetupRotatedRectangle(
        sfGridTextureSize, sfGridTextureSize, matrix, false, false);
    u8 lightenAmount = (u8)(255.0f * intensity);
    quad.SetColour(
        lightenAmount, lightenAmount, lightenAmount, 0xFF);
    glAttachQuad3((eGLView)lbl_806E1514, 1, &quad);
    glSetDefaultState(false);
}

static void ElectricFenceFinished(EmissionController& controller)
{
    ElectricFenceData* node = ElectricFenceData::sActiveElectricFences.m_pStart;
    while (node != 0)
    {
        if (node->mpEmissionController == &controller)
        {
            if (node != 0)
            {
                nlListRemoveElement<ElectricFenceData>(
                    &ElectricFenceData::sActiveElectricFences.m_pStart,
                    node,
                    &ElectricFenceData::sActiveElectricFences.m_pEnd);
                --ElectricFenceData::numAllocated;
                if (node->mpGeometry != 0)
                {
                    ElectricFenceGeometry::sElectricFenceGeometryPool.Free(
                        node->mpGeometry);
                }
                ElectricFenceData::sElectricFenceDataPool.Free(node);
            }
            return;
        }
        node = node->next;
    }
}

static inline ElectricFenceData* FindElectricFenceData(
    EmissionController* pEmissionController)
{
    ElectricFenceData* data =
        ElectricFenceData::sActiveElectricFences.m_pStart;
    while (data != 0)
    {
        if (data->mpEmissionController == pEmissionController)
        {
            return data;
        }
        data = data->next;
    }
    return 0;
}

static void RenderElectricFence(EmissionController& ec)
{
    ElectricFenceData* pElectricFenceData = FindElectricFenceData(&ec);

    float intensity = 1.0f;
    float remainingTime = ec.GetRemainingTime();
    if (remainingTime < sfFadeOutTime)
    {
        intensity = remainingTime / sfFadeOutTime;
    }

    if (pElectricFenceData == 0)
    {
        pElectricFenceData = 0;
        ElectricFenceData::sElectricFenceDataPool.Allocate(
            pElectricFenceData);
        pElectricFenceData =
            new (pElectricFenceData) ElectricFenceData(&ec);
    }
    if (pElectricFenceData == 0)
    {
        return;
    }

    if (pElectricFenceData->mbIsFlat)
    {
        RenderElectricFenceFlat(pElectricFenceData->mPosition,
            pElectricFenceData->mNormal, intensity);
        return;
    }

    nlMatrix4 matrix;
    matrix.SetIdentity();
    DrawPrimitive(*pElectricFenceData->mpGeometry, matrix,
        GLP_TriStrip, GridTexture, intensity);
}

bool EmitElectricFenceBallEffect(const nlVector3& pos,
    const nlVector3& dir, unsigned long emitterID, bool bNoSpark)
{
    ElectricFenceData* data;
    const char* groupName;
    EmissionController* controller;

    nlVector3 clampedPos;
    clampedPos.as_u32[0] = pos.as_u32[0];
    clampedPos.as_u32[1] = pos.as_u32[1];
    clampedPos.as_u32[2] = pos.as_u32[2];

    float goalLineX = cField::GetGoalLineX(1U);
    float absPosX = (float)__fabs(clampedPos.x);
    if ((float)__fabs(absPosX - goalLineX) < 0.2f)
    {
        if (clampedPos.x > 0.0f)
        {
            clampedPos.x = goalLineX;
        }
        else
        {
            clampedPos.x = -goalLineX;
        }
    }

    groupName = bNoSpark ? "electric_fence_nospark" : "electric_fence";
    if (!EmissionManager::Instance()->IsPlaying(emitterID,
            fn_802E7CDC(EmissionManager::Instance(), groupName)))
    {
        controller = fn_802E7FE4(
            EmissionManager::Instance(),
            fn_802E7CDC(EmissionManager::Instance(), groupName),
            3, true, false);
        controller->m_uUserData = emitterID;
        controller->SetPosition(clampedPos);

        float angle = nlATan2f(dir.y, dir.x);
        controller->m_aFacing = (u16)(10430.378f * angle);

        data = 0;
        ElectricFenceData::sElectricFenceDataPool.Allocate(data);
        new (data) ElectricFenceData(controller);

        controller->SetUpdateCallback(
            Function1<void, EmissionController&>(RenderElectricFence));
        controller->SetFinishedCallback(
            Function1<void, EmissionController&>(ElectricFenceFinished));
        return true;
    }
    return false;
}

void EmitElectricFenceCharacterEffect(const nlVector3& pos,
    const nlVector3& dir, unsigned long emitterID)
{
    ElectricFenceData* data;
    EmissionController* controller;

    if (!EmissionManager::Instance()->IsPlaying(emitterID,
            fn_802E7CDC(EmissionManager::Instance(),
                "electric_fence_character")))
    {
        controller = fn_802E7FE4(
            EmissionManager::Instance(),
            fn_802E7CDC(EmissionManager::Instance(),
                "electric_fence_character"),
            3, true, false);
        controller->m_uUserData = emitterID;
        controller->SetPosition(pos);

        float angle = nlATan2f(dir.y, dir.x);
        controller->m_aFacing = (u16)(10430.378f * angle);

        data = 0;
        ElectricFenceData::sElectricFenceDataPool.Allocate(data);
        new (data) ElectricFenceData(controller);

        controller->SetUpdateCallback(
            Function1<void, EmissionController&>(RenderElectricFence));
        controller->SetFinishedCallback(
            Function1<void, EmissionController&>(ElectricFenceFinished));
    }
}

void InitializeElectricFence(GLView* view)
{
    lbl_806E1514 = view;
}

void FreeElectricFence()
{
    ElectricFenceData* node;
    while (ElectricFenceData::sActiveElectricFences.m_pStart != 0)
    {
        node = ElectricFenceData::sActiveElectricFences.m_pStart;
        if (node != 0)
        {
            nlListRemoveElement<ElectricFenceData>(
                &ElectricFenceData::sActiveElectricFences.m_pStart,
                node, &ElectricFenceData::sActiveElectricFences.m_pEnd);
            --ElectricFenceData::numAllocated;
            if (node->mpGeometry != 0)
            {
                ElectricFenceGeometry::sElectricFenceGeometryPool.Free(
                    node->mpGeometry);
            }
            ElectricFenceData::sElectricFenceDataPool.Free(node);
        }
    }
    ElectricFenceData::sElectricFenceDataPool.FreeBlocks();
    ElectricFenceGeometry::sElectricFenceGeometryPool.FreeBlocks();
}

ElectricFenceData::ElectricFenceData(
    EmissionController* pEmissionController)
{
    mpEmissionController = pEmissionController;
    mfIntensity = 0.0f;
    mpGeometry = 0;

    nlListAddEnd<ElectricFenceData>(&sActiveElectricFences.m_pStart,
        &sActiveElectricFences.m_pEnd, this);
    ++numAllocated;

    mPosition = pEmissionController->GetPosition();

    f32 absY = (f32)__fabs(mPosition.y);
    f32 diffY = (f32)__fabs(absY - cField::GetSidelineY(1U));
    f32 distanceFromSideline = diffY;

    f32 absX = (f32)__fabs(mPosition.x);
    f32 diffX = (f32)__fabs(absX - cField::GetGoalLineX(1U));
    f32 distanceFromGoal = diffX;

    float increment = sfGridTextureSize / sfNumGridSquares;
    float verticalIncrement = sfGridTextureSize / lbl_806DCDF0;
    float cornerDiameter = 2.0f * cField::GetCornerRadius();
    if (distanceFromGoal > cornerDiameter
        || distanceFromSideline > cornerDiameter)
    {
        mbIsFlat = true;
        if (distanceFromGoal < distanceFromSideline)
        {
            bool isXPositive = mPosition.x > 0.0f;
            if (isXPositive)
            {
                float goalX;
                if (isXPositive)
                {
                    goalX = cField::GetGoalLineX(1U);
                }
                else
                {
                    goalX = -cField::GetGoalLineX(1U);
                }
                mPosition.x = goalX;
            }

            mNormal.x = 0.0f;
            mNormal.y = 1.0f;
            mNormal.z = 0.0f;

            bool negative = false;
            if (mPosition.y < 0.0f)
            {
                mPosition.y = -mPosition.y;
                negative = true;
            }
            mPosition.y = increment
                    * (float)floor(mPosition.y / increment)
                + sfAlignmentOffset1;
            if (negative)
            {
                mPosition.y = -mPosition.y;
            }
            mPosition.z = verticalIncrement
                * (float)floor(mPosition.z / verticalIncrement);
        }
        else
        {
            bool isYPositive = mPosition.y > 0.0f;
            if (isYPositive)
            {
                float sideY;
                if (isYPositive)
                {
                    sideY = cField::GetSidelineY(1U);
                }
                else
                {
                    sideY = -cField::GetSidelineY(1U);
                }
                mPosition.y = sideY;
            }

            mNormal.x = 1.0f;
            mNormal.y = 0.0f;
            mNormal.z = 0.0f;
            mPosition.x =
                increment * (float)floor(mPosition.x / increment);
            mPosition.z = verticalIncrement
                * (float)floor(mPosition.z / verticalIncrement);
        }
    }
    else
    {
        mbIsFlat = false;
        mPosition.z = verticalIncrement
            * (float)floor(mPosition.z / verticalIncrement);

        ElectricFenceGeometry* geom = 0;
        ElectricFenceGeometry::sElectricFenceGeometryPool.Allocate(geom);
        mpGeometry = geom;

        nlVector3 impactPosition = mPosition;
        CreateElectricFenceGeometry(*mpGeometry, impactPosition);
        return;
    }
}

void DisplayElectricFence()
{
    sbIsElectricFenceBeingDisplayed = true;
    sfElectricFenceDisplayAngle = sfStartAngle;
}

void StopDisplayingElectricFence()
{
    sbIsElectricFenceBeingDisplayed = false;
}

void UpdateElectricFence(float fDeltaT)
{
    static unsigned long counter = 1;
    static float timeSinceLastEffect;

    if (nlTaskManager::m_pInstance->mCurrentState == 1
        || nlTaskManager::m_pInstance->mCurrentState == 0x20)
    {
        return;
    }
    if (!sbIsElectricFenceBeingDisplayed)
    {
        return;
    }

    while (timeSinceLastEffect > sfTimeBetweenEffects)
    {
        float goalLineX = cField::GetGoalLineX(1U);
        float sideLineY = cField::GetSidelineY(1U);
        float randomAngleOffset = nlRandomf(-sfAngleRandomOffset,
            sfAngleRandomOffset, &nlDefaultSeed);
        nlVector3 pos = { 0.0f, 0.0f, 0.0f };
        nlVector3 normal;
        u16 sinArg = (u16)(s32)(10430.378f
            * (3.1415927f
                * (sfElectricFenceDisplayAngle + randomAngleOffset)
                / 180.0f));
        pos.x = nlSin(sinArg);
        sinArg = (u16)(s32)(10430.378f
            * (3.1415927f
                * (sfElectricFenceDisplayAngle + randomAngleOffset)
                / 180.0f));
        pos.y = nlSin((u16)(sinArg + 0x4000));

        float scale;
        if (pos.x == 0.0f)
        {
            scale = sideLineY;
            normal.x = 0.0f;
            normal.y = 1.0f;
            normal.z = 0.0f;
        }
        else if (pos.y == 0.0f)
        {
            scale = goalLineX;
            normal.x = 1.0f;
            normal.y = 0.0f;
            normal.z = 0.0f;
        }
        else
        {
            float goalLineScale = goalLineX / pos.x;
            float sideLineScale = sideLineY / pos.y;
            if (goalLineScale < 0.0f)
            {
                goalLineScale = -goalLineScale;
            }
            if (sideLineScale < 0.0f)
            {
                sideLineScale = -sideLineScale;
            }
            if (goalLineScale < sideLineScale)
            {
                scale = goalLineScale;
                normal.x = 1.0f;
                normal.y = 0.0f;
                normal.z = 0.0f;
            }
            else
            {
                scale = sideLineScale;
                normal.x = 0.0f;
                normal.y = 1.0f;
                normal.z = 0.0f;
            }
        }

        nlVec3Scale(pos, pos, scale);
        pos.z = nlRandomf(0.0f, lbl_806DCE10, &nlDefaultSeed);
        if ((counter & 1) == 0)
        {
            pos.x = -pos.x;
        }

        float netWidth = cNet::m_fNetWidth;
        float netHeight = cNet::m_fNetHeight;
        if ((float)__fabs(pos.x - goalLineX) < 0.01)
        {
            if ((float)__fabs(pos.y) < netWidth)
            {
                pos.z = nlRandomf(netHeight, 5.0f, &nlDefaultSeed);
            }
        }

        EmitElectricFenceBallEffect(pos, normal, counter++,
            !sbUseSparksDuringElectricFenceFlyBy);
        timeSinceLastEffect -= sfTimeBetweenEffects;
    }

    timeSinceLastEffect += fDeltaT;
    sfElectricFenceDisplayAngle += sfAngleAnimationRate * fDeltaT;
    float endAngle = sfStartAngle
        + 180.0f * (float)(s32)sNumRevolutionsToDisplay;
    if (sfElectricFenceDisplayAngle > endAngle)
    {
        sbIsElectricFenceBeingDisplayed = false;
    }
}
