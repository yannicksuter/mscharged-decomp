#include "Game/Render/ImpostorCharacter.h"

#include "Game/CharacterEffects.h"
#include "Game/GL/GLInventory.h"
#include "Game/Inventory.h"
#include "Game/GL/ShaderSkinMesh.h"
#include "Game/PoseAccumulator.h"
#include "Game/SHierarchy.h"
#include "Game/SAnim/pnBlender.h"
#include "Game/SAnim/pnSAnimController.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

extern "C"
{
    extern unsigned long lbl_806E1F0C;

    void* fn_802CDF0C();
    unsigned long fn_802CE1B8(void*, unsigned long);
    bool fn_802CC8FC(const glModelPacket*, unsigned long);
    unsigned long fn_802CC7E4(
        const glModelPacket*, unsigned long);
    void fn_802CC458(
        glModelPacket*, unsigned long, unsigned long);
    void fn_802CC4FC(
        glModelPacket*, unsigned long, const unsigned long*);
}

static unsigned long sTextureParameterHashes[10] = {
    nlStringLowerHash("NLG_DIFFUSE"),
    nlStringLowerHash("NLG_DETAIL"),
};

ImpostorModel_802DAEE0::ImpostorModel_802DAEE0(cSHierarchy& hierarchy,
    unsigned long modelID, cInventory<cSAnim>* animations,
    ResourceInterface_802CC094* resource)
{
    mModelID = modelID;
    mAnimController = 0;
    mPoseAccumulator = 0;
    mSkinMesh = 0;
    mPoseTree = 0;
    mHierarchy = &hierarchy;
    mVisible = false;
    mTexturesResolved = false;
    mLastModel = 0;
    mAnimations = animations;
    mOriginalTexture = 0;
    mReplacementTexture = 0;
    mModelCallback = 0;

    if (modelID == (unsigned long)-1)
    {
        mSkinMesh = 0;
    }
    else
    {
        mSkinMesh = resource->m_inventory->MakeSkinMesh(
            modelID, &hierarchy);
        if (mSkinMesh != 0)
        {
            mSkinMesh->m_Unknown0C = (void*)1;
        }
    }

    mPoseAccumulator = new (nlMalloc(sizeof(cPoseAccumulator), 8, false))
        cPoseAccumulator(&hierarchy, true);
    mWorldMatrix.SetIdentity();
    mVisible = true;
}

ImpostorModel_802DAEE0::ImpostorModel_802DAEE0(cSHierarchy& hierarchy,
    unsigned long modelID, ResourceInterface_802CC094* resource)
{
    mModelID = modelID;
    mAnimController = 0;
    mPoseAccumulator = 0;
    mSkinMesh = 0;
    mPoseTree = 0;
    mHierarchy = &hierarchy;
    mVisible = false;
    mLastModel = 0;
    mAnimations = 0;
    mModelCallback = 0;

    if (modelID == (unsigned long)-1)
    {
        mSkinMesh = 0;
    }
    else
    {
        mSkinMesh = resource->m_inventory->MakeSkinMesh(
            modelID, &hierarchy);
        if (mSkinMesh != 0)
        {
            mSkinMesh->m_Unknown0C = (void*)1;
        }
    }

    mPoseAccumulator = new (nlMalloc(sizeof(cPoseAccumulator), 8, false))
        cPoseAccumulator(&hierarchy, true);
    mWorldMatrix.SetIdentity();
    mVisible = true;
}

extern "C" ImpostorModel_802DAEE0* fn_802DB0AC(
    const ImpostorModel_802DAEE0* model,
    ResourceInterface_802CC094* resource)
{
    return new (8, false) ImpostorModel_802DAEE0(
        *model->mHierarchy, model->mModelID, model->mAnimations, resource);
}

ImpostorModel_802DAEE0::~ImpostorModel_802DAEE0()
{
    if (mPoseTree != 0)
    {
        delete mPoseTree;
    }
    if (mPoseAccumulator != 0)
    {
        delete mPoseAccumulator;
    }
    if (mSkinMesh != 0)
    {
        delete mSkinMesh;
    }
}

void ImpostorModel_802DAEE0::UnidentifiedVirtual10(float dt)
{
    mPoseTree = mPoseTree->Update(dt);
    mPoseAccumulator->InitAccumulators();
    mPoseTree->Evaluate(1.0f, mPoseAccumulator);
}

extern "C" void fn_802DB22C(
    ImpostorModel_802DAEE0* model, float dt)
{
    model->mPoseTree = model->mPoseTree->Update(dt);
}

extern "C" void fn_802DB26C(ImpostorModel_802DAEE0* model)
{
    model->mPoseAccumulator->InitAccumulators();
    model->mPoseTree->Evaluate(1.0f, model->mPoseAccumulator);
}

extern "C" void fn_802DB2B8(
    ImpostorModel_802DAEE0* model, float time)
{
    model->mAnimController->SetTime(time);
}

void ImpostorModel_802DAEE0::UnidentifiedVirtual0C(
    GLView* opaqueView, GLView* translucentView)
{
    mPoseAccumulator->BuildNodeMatrices(mWorldMatrix);
    UnidentifiedVirtual14(
        opaqueView, translucentView, *mPoseAccumulator, &mWorldMatrix);
}

void ImpostorModel_802DAEE0::UnidentifiedVirtual14(GLView* opaqueView,
    GLView* translucentView, const cPoseAccumulator& poseAccumulator,
    const nlMatrix4*)
{
    if (!mVisible)
    {
        return;
    }
    if (!mTexturesResolved)
    {
        fn_802DB79C(this);
    }
    if (mSkinMesh == 0)
    {
        return;
    }

    mSkinMesh->Pose(
        const_cast<cPoseAccumulator*>(&poseAccumulator));
    mSkinMesh->PrepareToRender();
    glModel* model = glModelDupNoStreams(
        mSkinMesh->GetModel(), false, 0);

    if (mOriginalTexture != 0 && mReplacementTexture != 0)
    {
        for (glModelPacket* packet = model->packets;
            packet < model->packets + model->numPackets;
            ++packet)
        {
            unsigned long texture = fn_802CC7E4(packet, lbl_806E1F0C);
            if (texture == mOriginalTexture)
            {
                fn_802CC458(packet, lbl_806E1F0C, mReplacementTexture);
                unsigned long resolvedTexture = mResolvedTexture;
                fn_802CC4FC(packet, lbl_806E1F0C, &resolvedTexture);
            }
        }
    }

    if (mModelCallback != 0)
    {
        mModelCallback(this, model);
    }

    for (unsigned long i = 0; i < model->numPackets; ++i)
    {
        glModelPacket* packet = &model->packets[i];
        if (glGetRasterState(packet->rasterState, GLS_AlphaBlend) == 0)
        {
            opaqueView->AttachPacket(packet, 0);
        }
        else
        {
            translucentView->AttachPacket(packet, 1);
        }
    }
    mLastModel = model;
}

extern "C" void fn_802DB4EC(
    ImpostorModel_802DAEE0* model, unsigned long texture)
{
    model->mReplacementTexture = texture;
    model->mResolvedTexture = fn_802CE1B8(
        fn_802CDF0C(), model->mReplacementTexture);
}

extern "C" void fn_802DB528(ImpostorModel_802DAEE0* model,
    const char* name, float blendTime, ePlayMode playMode)
{
    cSAnim* anim = model->mAnimations->Find(const_cast<char*>(name));

    cPN_SAnimController* controller = new cPN_SAnimController(
        anim, 0, playMode, 0, 0, false);
    if (model->mPoseTree != 0 && blendTime > 0.0f)
    {
        model->mPoseTree = new cPN_Blender(
            model->mPoseTree, controller, blendTime);
    }
    else
    {
        if (model->mPoseTree != 0)
        {
            delete model->mPoseTree;
        }
        model->mPoseTree = controller;
    }
    model->mAnimController = controller;
}

extern "C" void fn_802DB6CC(ImpostorModel_802DAEE0* model,
    cSAnim& anim, ePlayMode playMode, const AnimRetarget* retarget)
{
    cPN_SAnimController* controller = new cPN_SAnimController(
        &anim, retarget, playMode, 0, 0, false);
    if (model->mPoseTree != 0)
    {
        delete model->mPoseTree;
    }
    model->mPoseTree = controller;
    model->mAnimController = controller;
}

extern "C" void fn_802DB79C(ImpostorModel_802DAEE0* model)
{
    if (model->mTexturesResolved)
    {
        return;
    }
    if (model->mSkinMesh != 0)
    {
        void* modelIndex = model->mSkinMesh->m_Unknown0C;
        for (unsigned long i = 0; i < 2; ++i)
        {
            model->mSkinMesh->m_Unknown0C = (void*)i;
            glModel* skinModel = model->mSkinMesh->GetModel();
            if (skinModel != 0)
            {
                for (glModelPacket* packet = skinModel->packets;
                    packet < model->mSkinMesh->GetModel()->packets
                                 + model->mSkinMesh->GetModel()->numPackets;
                    ++packet)
                {
                    for (int j = 0; j < 10; ++j)
                    {
                        unsigned long parameter = sTextureParameterHashes[j];
                        if (fn_802CC8FC(packet, parameter))
                        {
                            unsigned long texture = fn_802CC7E4(packet, parameter);
                            unsigned long resolvedTexture = fn_802CE1B8(fn_802CDF0C(), texture);
                            fn_802CC4FC(packet, parameter, &resolvedTexture);
                        }
                    }
                }
            }
        }
        model->mSkinMesh->m_Unknown0C = modelIndex;
    }
    model->mTexturesResolved = true;
}
