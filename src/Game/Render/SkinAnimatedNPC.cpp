#include "Game/Render/SkinAnimatedNPC.h"

#include "Game/BasicStadium.h"
#include "Game/Field.h"
#include "Game/GL/GLInventory.h"
#include "Game/GL/ShaderSkinMesh.h"
#include "Game/GameInfo.h"
#include "Game/GameObjectLighting.h"
#include "Game/PoseAccumulator.h"
#include "Game/Render/NPCManager.h"
#include "Game/Render/RLView.h"
#include "Game/Render/RenderShadow.h"
#include "Game/SAnim/pnBlender.h"
#include "Game/SAnim/pnSAnimController.h"
#include "NL/MemAlloc.h"
#include "NL/gl/gl.h"
#include "NL/gl/glModel.h"
#include "NL/nlColour.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

extern "C"
{
    void fn_802CC628(glModelPacket* packet, u32 hash, float value);
    void fn_802CC6C0(glModelPacket* packet, u32 hash, u32 value);
}

static char ShadowLevelName[] = "shadowLevel";
static char AlphaValueName[] = "alphaValue";

static float ShadowRadius = 7.0f;
static float ShadowHeight = 7.0f;
static float SidelineShadowMargin = 2.5f;

void SkinAnimatedNPC::DrawShadow(
    const glModel* pModel, const nlMatrix4& matrix)
{
    RLView* pView = GetLayerView(eCLV_Characters);
    float shadowLevel = 0.75f;
    UnidentifiedStadiumShadowData* pStadium =
        reinterpret_cast<UnidentifiedStadiumShadowData*>(
            BasicStadium::GetCurrentStadium());
    float alphaValue = 0.75f;
    static u32 alphaValueHash = nlStringLowerHash(AlphaValueName);

    ProjectedShadowParams params;
    params.fScalar = 1.0f;
    nlVec4Set(params.vLight, pStadium->unknown8C.x,
        pStadium->unknown8C.y, pStadium->unknown8C.z, 1.0f);
    params.vPosition = matrix.GetTranslation();
    params.fRadius = ShadowRadius;
    params.fHeight = ShadowHeight;
    params.pModel = 0;
    params.fScalar = shadowLevel;
    params.nPartitionIndex = 10;
    params.nVisibleInterval = 2;
    params.nInvisibleInterval = 3;

    if (ShouldShadowBeUpdated(params))
    {
        params.pModel = glModelDupNoStreams(pModel, false, 0);
        if (1.0f != alphaValue)
        {
            for (glModelPacket* pPacket = params.pModel->packets;
                 pPacket < params.pModel->packets + params.pModel->numPackets;
                 ++pPacket)
            {
                fn_802CC628(pPacket, alphaValueHash, 1.0f);
            }
        }
        RenderCharacterIntoTexture(params);
    }

    RLView* pOldView = SetCharacterShadowView(pView);
    RenderProjectedShadow(params);
    SetCharacterShadowView(pOldView);
}

void SkinAnimatedNPC::SetAnimState(
    cSAnim& pAnim, float fBlendTime, ePlayMode playMode)
{
    cPN_SAnimController* controller = new cPN_SAnimController(
        &pAnim, 0, playMode, 0, 0, false);

    if (mpPoseTree != 0 && fBlendTime > 0.0f)
    {
        cPN_Blender* blender =
            new cPN_Blender(mpPoseTree, controller, fBlendTime);
        mpPoseTree = blender;
    }
    else
    {
        mpPoseTree = controller;
        delete mpAnimController;
    }

    mpAnimController = controller;
}

void SkinAnimatedNPC::DrawShadow(
    const cPoseAccumulator& poseAccumulator,
    const nlMatrix4& worldMatrix)
{
    RLView* pView = GetLayerView(eCLV_Characters);
    glModel* pModel = glModelDupNoStreams(
        mpSkinMesh->GetModel(), false, 0);
    float shadowLevel = 0.5f;
    UnidentifiedStadiumShadowData* pStadium =
        reinterpret_cast<UnidentifiedStadiumShadowData*>(
            BasicStadium::GetCurrentStadium());
    float alphaValue = 0.5f;
    static u32 alphaValueHash = nlStringLowerHash(AlphaValueName);

    ProjectedShadowParams params;
    params.fScalar = 1.0f;
    nlVec4Set(params.vLight, pStadium->unknown8C.x,
        pStadium->unknown8C.y, pStadium->unknown8C.z, 1.0f);
    params.vPosition = mWorldMatrix.GetTranslation();
    params.fRadius = ShadowRadius;
    params.fHeight = ShadowHeight;
    params.pModel = 0;
    params.fScalar = shadowLevel;
    params.nPartitionIndex = 10;
    params.nVisibleInterval = 2;
    params.nInvisibleInterval = 3;

    if (ShouldShadowBeUpdated(params))
    {
        params.pModel = glModelDupNoStreams(pModel, false, 0);
        if (1.0f != alphaValue)
        {
            for (glModelPacket* pPacket = params.pModel->packets;
                 pPacket < params.pModel->packets + params.pModel->numPackets;
                 ++pPacket)
            {
                fn_802CC628(pPacket, alphaValueHash, 1.0f);
            }
        }
        RenderCharacterIntoTexture(params);
    }

    RLView* pOldView = SetCharacterShadowView(pView);
    RenderProjectedShadow(params);
    SetCharacterShadowView(pOldView);
}

void SkinAnimatedNPC::SendToGL() const
{
    static unsigned long prevFrame = 0;

    unsigned long frame = glGetCurrentFrame();
    if (prevFrame != frame)
    {
        prevFrame = frame;
    }

    GetGameObjectLightRamp();

    GLSkinMesh* skinMesh = mpSkinMesh;
    skinMesh->PrepareToRender();
    glModel* pModel = glModelDupNoStreams(
        skinMesh->GetModel(), false, 0);

    static u32 shadowLevelHash = nlStringLowerHash(ShadowLevelName);
    nlColour shadowColour = fn_80183C9C(
        reinterpret_cast<const nlVector2*>(
            &mWorldMatrix.GetTranslation()),
        true);
    glModelPacket* pPacket = pModel->packets;
    u32 shadowColourValue = *reinterpret_cast<u32*>(&shadowColour);

    while (pPacket < pModel->packets + pModel->numPackets)
    {
        fn_802CC6C0(pPacket, shadowLevelHash, shadowColourValue);
        ++pPacket;
    }

    eCLV view = eCLV_WorldShadowed;
    bool isChainChomp =
        GetSkinAnimatedNPC_Type() == SkinAnimatedNPC_CHAIN_CHOMP;
    if (isChainChomp)
    {
        if (lbl_806E1608->mpChainChomp == 0)
        {
            return;
        }
        view = eCLV_MoreCharacters;
    }
    else
    {
        bool isBowser =
            GetSkinAnimatedNPC_Type() == SkinAnimatedNPC_BOWSER;
        if (isBowser)
        {
            view = eCLV_MoreCharacters;
        }
    }

    GetLayerView(view)->AttachModel(pModel, 0);
    const_cast<SkinAnimatedNPC*>(this)->mpLastModel = pModel;
}

void SkinAnimatedNPC::RenderFromReplay(
    const cPoseAccumulator& poseAccumulator,
    const nlMatrix4* pWorldMatrix)
{
    if (!mbIsVisible)
    {
        return;
    }
    if (mpSkinMesh == 0)
    {
        return;
    }

    mpSkinMesh->Pose(
        const_cast<cPoseAccumulator*>(&poseAccumulator));
    SendToGL();

    if (GameInfoManager::Instance()->GetStadium() == 0x0B)
    {
        float positionY = mv3Position.y;
        if (positionY
            > cField::GetSidelineY(1) - SidelineShadowMargin)
        {
            return;
        }
        if (positionY
            < -(cField::GetSidelineY(1) + SidelineShadowMargin))
        {
            return;
        }
    }

    DrawShadow(poseAccumulator, *pWorldMatrix);
}

void SkinAnimatedNPC::Render()
{
    mpPoseAccumulator->BuildNodeMatrices(mWorldMatrix);
    RenderFromReplay(*mpPoseAccumulator, &mWorldMatrix);
}

void SkinAnimatedNPC::Update(float dt)
{
    mpPoseTree = mpPoseTree->Update(dt);
    mpPoseAccumulator->InitAccumulators();
    mpPoseTree->Evaluate(1.0f, mpPoseAccumulator);
}

SkinAnimatedNPC::~SkinAnimatedNPC()
{
    delete mpPoseTree;
    delete mpPoseAccumulator;
    if (mpSkinMesh != 0)
    {
        delete mpSkinMesh;
    }
}

SkinAnimatedNPC::SkinAnimatedNPC(
    cSHierarchy& pHierarchy, int nModelID, void* resource)
{
    maFacingDirection = 0;
    mpAnimController = 0;
    mpPoseAccumulator = 0;
    mpSkinMesh = 0;
    mpPoseTree = 0;
    mbIsVisible = 0;

    if ((u32)nModelID == (u32)-1)
    {
        mpSkinMesh = 0;
    }
    else
    {
        GLInventory* pInventory =
            static_cast<GLInventory*>(
                static_cast<MemoryAllocator*>(resource)->m_0C);
        mpSkinMesh = pInventory->MakeSkinMesh(
            (unsigned long)nModelID, &pHierarchy);
    }

    cPoseAccumulator* pAccum = new (
        nlMalloc(sizeof(cPoseAccumulator), 8, false))
        cPoseAccumulator(&pHierarchy, true);
    mpPoseAccumulator = pAccum;

    mWorldMatrix.SetIdentity();
    mbIsVisible = true;
    mv3Position.x = 0.0f;
    mv3Position.y = 0.0f;
    mv3Position.z = 0.0f;
}
