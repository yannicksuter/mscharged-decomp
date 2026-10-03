#include "Game/Render/Nis.h"

#include "Game/AI/AIPad.h"
#include "Game/AnimInventory.h"
#include "Game/GL/GLSkinMesh.h"
#include "Game/Render/RLView.h"
#include "Game/BasicStadium.h"
#include "Game/Game.h"
#include "Game/Player.h"
#include "Game/Render/CrowdImpostors.h"
#include "Game/Render/ElectricFence.h"
#include "Game/Render/depthoffield.h"
#include "Game/RumbleActions.h"
#include "Game/Sys/audio.h"
#include "Game/CharacterTemplate.h"
#include "Game/Camera/animcam.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/Effects/EmitterCallbacks.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "NL/nlBind.h"
#include "Game/GameInfo.h"
#include "Game/NisPlayer.h"
#include "Game/Render/ImpostorModel.h"
#include "Game/Render/NPCManager.h"
#include "Game/Render/StadiumLoading.h"
#include "Game/GameObjectLighting.h"
#include "NL/gl/glTexture.h"
#include "Game/RenderSnapshot.h"
#include "Game/ReplayManager.h"
#include "NL/MemAlloc.h"
#include "NL/gl/glMaterialParameters.h"
#include "NL/gl/glModel.h"
#include "NL/nlDebug.h"
#include "NL/nlFile.h"
#include "NL/nlFileGC.h"
#include "NL/nlSlotPool.h"
#include "NL/nlTask.h"
#include "NL/gl/glState.h"
#include "NL/nlstring_tmpl.h"
#include "Game/Render/Presentation.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"

GLView* g_pNisRenderView;

bool g_bNisAnimatedCharacters[Nis::MAX_NUM_CHARACTERS];

struct PendingAnimationRequest
{
    Nis::PendingAnimation* animation;
    bool active;
};

SlotPool<PendingAnimationRequest> lbl_8057AB80(16, 16);

void OnCharacterAnimationLoaded(void* data, unsigned long size, void* userData);

#include "src/Game/Render/Nis_interp.cpp"

Nis::Nis(NisHeader& header, char* data, int size)
    : InterpreterCore(100)
    , mHeader(&header)
    , mTarget(header.target)
    , mWinnerType(header.winnerType)
    , unknown_0x034(header.unknown_0x180)
    , mData(data)
    , mSize(size)
    , mMirrored(header.mirrored)
    , mCamera(0)
    , mNumCameras(0)
    , mNumTriggers(0)
    , mMainCharacterIndex(-1)
    , mAudioCharacterIndex(-1)
    , mUnidentified850(0)
    , mDryBonesHead(0)
    , mDryBonesHeadCharacter(0)
    , mShyGuyMask(0)
    , mShyGuyMaskCharacter(0)
    , mScriptStarted(false)
{
    int i;
    for (int i = 0; i < MAX_NUM_CHARACTERS; ++i)
    {
        mPendingAnimations[i].name = 0;
        mPendingAnimations[i].loaded = false;
        mPendingAnimations[i].characterIndex = -1;
        mPendingAnimations[i].loadHandle = 0;
        mPendingAnimations[i].data = 0;
        mPendingAnimations[i].size = 0;
        mPendingAnimations[i].request = 0;
    }
    g_pNisRenderView = GetLayerView(eCLV_WorldShadowed);
    for (int i = 0; i < MAX_NUM_CHARACTERS; ++i)
    {
        mCharacterControllers[i] = 0;
        mBallId[i] = -1;
        mCharacterAnimProxy[i] = -1;
    }
    for (int i = 0; i < 8; ++i)
    {
        mImpostors[i] = 0;
        mImpostorNames[i] = 0;
    }
    for (int i = 0; i < 10; ++i)
    {
        mCameraData[i] = new (8, false) cCameraData();
    }
    nlChunk* chunk = (nlChunk*)data;
    nlChunk* end = (nlChunk*)(data + size);
    int numAnimations = 0;
    int numBalls = 0;
    while (chunk != end)
    {
        if (chunk->GetID() == 0x80017000)
        {
            cSAnim* anim = cSAnim::Initialize(chunk);
            NPCTemplate* npcTemplate = gNPCManager->FindNPCTemplate(anim->m_szName);
            if (npcTemplate != 0)
            {
                for (i = 0; i < 8; ++i)
                {
                    if (mImpostors[i] == 0)
                        break;
                }
                mImpostors[i] = new (8, false) ImpostorModel(*npcTemplate->hierarchy, npcTemplate->modelID, npcTemplate->mResourcePool);
                mImpostors[i]->PlayAnimation(*anim, PM_HOLD, 0);
                mImpostors[i]->mVisible = true;
                mImpostors[i]->mModelCallback = fn_80183F78;
                mImpostorNames[i] = npcTemplate->mName;
                int lastIndex = nlStrLen(anim->m_szName) - 1;
                mImpostorSuffixes[i] = anim->m_szName[lastIndex];
                char textureName[256];
                if (nlStrCmp(npcTemplate->mName, "vice_image_plane_top") == 0
                    || nlStrCmp(npcTemplate->mName, "vice_image_plane_bottom") == 0)
                {
                    nlSNPrintf(textureName, sizeof(textureName), "_%s/%s", fn_802772C4(), npcTemplate->mName + 5);
                    unsigned long texture = nlStringLowerHash(textureName);
                    if (glTextureLoad(texture))
                    {
                        mImpostors[i]->SetReplacementTexture(texture);
                        nlSNPrintf(textureName, sizeof(textureName), "%s/%s", npcTemplate->mName, npcTemplate->mName);
                        mImpostors[i]->mOriginalTexture = nlStringLowerHash(textureName);
                    }
                }
                else if (nlStrCmp(npcTemplate->mName, "mario_mega_orange_bg") == 0)
                {
                    int charIdx = TargetToIndex(mTarget, mWinnerType, true);
                    int captain = GameInfoManager::Instance()->GetTeam((short)((charIdx < 4 || charIdx == 8) == false));
                    nlSNPrintf(textureName, sizeof(textureName), "%s/mega_cone_colour", GetCharacterInfo(GetCharacterIndexFromCaptain(captain)).mName);
                    unsigned long texture = nlStringLowerHash(textureName);
                    if (glTextureLoad(texture))
                    {
                        mImpostors[i]->SetReplacementTexture(texture);
                        mImpostors[i]->mOriginalTexture = nlStringLowerHash("mario_mega_orange_bg/mega_cone_colour");
                    }
                }
                else if (nlStrCmp(npcTemplate->mName, "NIS_ball") == 0)
                {
                    ++numBalls;
                    if (numBalls >= NisPlayer::Instance()->mMaxNumBallsVisible)
                    {
                        mImpostors[i]->mVisible = false;
                    }
                }
            }
            else if (mTarget != NIS_TARGET_NONE && mTarget != NIS_TARGET_STADIUM)
            {
                i = TargetToIndex(mTarget, mWinnerType, true);
                if (mCharacterControllers[i] != 0)
                {
                    i = TargetToIndex(NIS_TARGET_HOME_CAPTAIN, mWinnerType, true);
                }
                if (mCharacterControllers[i] != 0)
                {
                    i = TargetToIndex(NIS_TARGET_AWAY_CAPTAIN, mWinnerType, true);
                }
                if (mCharacterControllers[i] != 0)
                {
                    for (i = 0; i < MAX_NUM_CHARACTERS; ++i)
                    {
                        if (mCharacterControllers[i] == 0)
                            break;
                    }
                }
                if (i < MAX_NUM_CHARACTERS)
                {
                    mBallId[i] = numAnimations;
                    cPN_SAnimController* controller = new cPN_SAnimController(anim, 0, PM_HOLD, 0, 0, false);
                    g_bNisAnimatedCharacters[i] = true;
                    mCharacterControllers[i] = controller;
                    if (mAudioCharacterIndex < 0)
                    {
                        mAudioCharacterIndex = i;
                    }
                }
                ++numAnimations;
            }
        }
        if (chunk->GetID() == 0x8002500B)
        {
            char name[32];
            nlSNPrintf(name, sizeof(name), "%s_%d", mHeader->name, mNumCameras);
            nlChunk* cameraBegin = (nlChunk*)chunk->GetData();
            nlChunk* cameraEnd = chunk->GetLastChunk();
            if (LoadAnimCameraData(cameraBegin, cameraEnd, mCameraData[mNumCameras], false)
                && mNumCameras < 10)
            {
                ++mNumCameras;
            }
        }
        chunk = chunk->GetNextChunk();
    }
    NisPlayer* player = NisPlayer::Instance();
    LoadByteCode(player->mUnidentified34334);
}
void Nis::StartScript()
{
    mScriptStarted = true;
    char name[64];
    nlStrNCpy(name, mHeader->name, sizeof(name));
    *nlStrChr(name, '.') = '\0';
    CallFunction(nlStringHash(name));
}

void Nis::ApplyLoadedAnimations()
{
    for (int i = 0; i < MAX_NUM_CHARACTERS; ++i)
    {
        if (mPendingAnimations[i].name != 0
            && mPendingAnimations[i].characterIndex != -1)
        {
            cInventory<cSAnim>& inventory = mAnimInventories[mPendingAnimations[i].characterIndex];
            inventory.AddFile((char*)mPendingAnimations[i].data,
                mPendingAnimations[i].size);
            mPendingAnimations[i].data = 0;
            cSAnim* anim = inventory.Find(nlStringHash(mPendingAnimations[i].name));
            mCharacterControllers[mPendingAnimations[i].characterIndex] =
                new cPN_SAnimController(anim, 0, PM_CYCLIC, 0, 0, false);
        }
    }
}

char* Nis::Name() const
{
    return mHeader->name;
}

Nis::~Nis()
{
    for (int i = 0; i < MAX_NUM_CHARACTERS; ++i)
    {
        if (mPendingAnimations[i].loadHandle != 0)
        {
            if (nlAsyncReadBusy((AsyncEntry*)mPendingAnimations[i].loadHandle))
            {
                ((PendingAnimationRequest*)mPendingAnimations[i].request)->active = false;
                ((PendingAnimationRequest*)mPendingAnimations[i].request)->animation = 0;
            }
            else
            {
                nlCancelEntireFileLoad(mPendingAnimations[i].loadHandle, 0);
                lbl_8057AB80.Free((PendingAnimationRequest*)mPendingAnimations[i].request);
            }
        }
        if (mPendingAnimations[i].data != 0)
        {
            ::operator delete(mPendingAnimations[i].data);
        }
        mPendingAnimations[i].loadHandle = 0;
        mPendingAnimations[i].name = 0;
        mPendingAnimations[i].loaded = false;
        mPendingAnimations[i].characterIndex = -1;
        mPendingAnimations[i].data = 0;
        mPendingAnimations[i].size = 0;
        mPendingAnimations[i].request = 0;
    }

    for (int i = 0; i < 8; ++i)
    {
        if (mImpostors[i] != 0)
        {
            delete mImpostors[i];
        }
    }
    for (int i = 0; i < MAX_NUM_CHARACTERS; ++i)
    {
        if (mCharacterControllers[i] != 0)
        {
            delete mCharacterControllers[i];
        }
    }
    for (int i = 0; i < 10; ++i)
    {
        delete mCameraData[i];
        mCameraData[i] = 0;
    }
    if (mCamera != 0)
    {
        mCamera->UnselectCameraAnimation();
    }
    NisPlayer::Instance()->ResetEffects();
    nlTaskManager::SetTimeDilation(1.0f);
}

void Nis::Update(float dt)
{
    for (int i = 0; i < MAX_NUM_CHARACTERS; ++i)
    {
        cPN_SAnimController* pController = mCharacterControllers[i];
        if (pController != 0)
        {
            pController->Update(dt);
        }
    }

    for (int i = 0; i < 8; ++i)
    {
        ImpostorModel* pModel = mImpostors[i];
        if (pModel != 0)
        {
            pModel->Update(dt);
        }
    }
}

void Nis::UpdateTriggers(float oldTime, float newTime, float duration)
{
    if (duration != 0.0f)
    {
        for (int i = 0; i < mNumTriggers; ++i)
        {
            float triggerFrame = (mTriggers[i].frameNumber / 30.0f) / duration;
            if ((oldTime <= triggerFrame) && (newTime > triggerFrame))
            {
                mTriggers[i].Fire(*this);
            }
        }
    }
}

void Nis::SelectCamera(cAnimCamera& camera, int cameraIndex)
{
    int index = cameraIndex % mNumCameras;
    camera.m_pActiveCameraData = mCameraData[index];
    if (mMirrored)
    {
        camera.m_Mirror = (nlVector3){ -1.0f, 1.0f, 1.0f };
    }
    else
    {
        camera.m_Mirror = (nlVector3){ 1.0f, 1.0f, 1.0f };
    }
    camera.SetAnimationTime(0.0f, false);
    camera.m_bCyclic = false;
    mCamera = &camera;
}

void Nis::SelectRandomCamera(cAnimCamera& camera)
{
    int randomIndex = nlRandom(mNumCameras, GetPresentationRandomSeed(GetPresentation()));
    SelectCamera(camera, randomIndex);
}

void Nis::Render(int param1)
{
    DrawableCharacter* pDC;
    RenderSnapshot& snapshot = ReplayManager::Instance()->GetMutableRenderSnapshot();
    int numBalls = 0;
    nlVector3 offset = { 0.0f, 0.0f, 0.0f };

    for (int i = 0; i < MAX_NUM_CHARACTERS; ++i)
    {
        pDC = &snapshot.GetCharacter(i);
        if (mCharacterControllers[i] == 0)
            continue;
        pDC->visible = true;

        nlVector3 rootTrans = { 0.0f, 0.0f, 0.0f };
        u16 angle = 0;
        int index = mCharacterAnimProxy[i];
        if (index >= 0)
        {
            mCharacterControllers[i]->GetRootTrans(&rootTrans, mAnimProxyDirections[index], 1.0f);
            nlVec2Add(mAnimProxyPositions[index], mAnimProxyPositions[index],
                *(const nlVector2*)&rootTrans);
            nlVec3Set(rootTrans, mAnimProxyPositions[index].x, mAnimProxyPositions[index].y, 0.0f);
            mCharacterControllers[i]->GetRootRot(&angle);
            mAnimProxyDirections[index] += angle;
            angle = mAnimProxyDirections[index];
        }
        else
        {
            float fTime = mCharacterControllers[i]->get_fTime();
            mCharacterControllers[i]->m_pSAnim->GetRootTrans(fTime, &rootTrans);
            fTime = mCharacterControllers[i]->get_fTime();
            mCharacterControllers[i]->m_pSAnim->GetRootRot(fTime, &angle);
        }
        if (mMirrored)
        {
            mCharacterControllers[i]->m_bMirror = true;
            rootTrans.x *= -1.0f;
            angle = angle + (0x4000 - angle) * 2;
        }
        nlVec3Add(rootTrans, rootTrans, mHeader->stadiumOffset);
        nlVec3Add(rootTrans, rootTrans, offset);
        pDC->EvaluateFrom(*mCharacterControllers[i], rootTrans, angle, 1.0f);
        if (mBallId[i] >= 0 && numBalls < mHeader->numBalls
            && numBalls < NisPlayer::Instance()->mMaxNumBallsVisible)
        {
            if (mBallId[i] == 0)
            {
                snapshot.mBall.mFlags.bits.visible = true;
                snapshot.mBall.EvaluateFrom(*pDC);
            }
            ++numBalls;
        }
    }

    for (int i = 0; i < 8; ++i)
    {
        if (mImpostors[i] == 0)
            continue;
        nlVector3 rootTrans = { 0.0f, 0.0f, 0.0f };
        u16 angle = 0;
        float fTime = mImpostors[i]->mAnimController->get_fTime();
        mImpostors[i]->mAnimController->m_pSAnim->GetRootTrans(fTime, &rootTrans);
        fTime = mImpostors[i]->mAnimController->get_fTime();
        mImpostors[i]->mAnimController->m_pSAnim->GetRootRot(fTime, &angle);
        if (mMirrored)
        {
            mImpostors[i]->mAnimController->m_bMirror = true;
            rootTrans.x *= -1.0f;
            angle = angle + (0x4000 - angle) * 2;
        }
        nlMatrix4 matrix;
        nlMakeRotationMatrixZ(matrix, AngUnitsToRad_fromUnsignedShort(angle));
        matrix.SetTranslation(rootTrans);
        mImpostors[i]->mWorldMatrix = matrix;

        GLView* view = GetLayerView(eCLV_MoreCharacters);
        if (param1 == 1 && (unknown_0x034 == 1 || unknown_0x034 == 2))
        {
            view = GetLayerView(eCLV_PictureInPicture);
        }
        GLSkinMesh* skinMesh = mImpostors[i]->GetSkinMesh();
        if (skinMesh != 0)
        {
            static const u32 hash1 = nlStringLowerHash("peachwingleft/peachwingleft");
            static const u32 hash2 = nlStringLowerHash("peachwingright/peachwingright");
            static const u32 hash3 = nlStringLowerHash("peachcrown/peachcrown");
            static const u32 hash4 = nlStringLowerHash("waluigiwhip/waluigiwhip");
            static const u32 hash5 = nlStringLowerHash("yoshiwingleft/yoshiwingleft");
            static const u32 hash6 = nlStringLowerHash("yoshiwingright/yoshiwingright");
            u32 modelID = skinMesh->GetModel()->id;
            if (modelID == hash1 || modelID == hash2 || modelID == hash3
                || modelID == hash4 || modelID == hash5 || modelID == hash6)
            {
                view = GetLayerView(eCLV_HighRange3D);
            }
        }
        if (view == GetLayerView(eCLV_MoreCharacters))
        {
            mImpostors[i]->Render(view, GetLayerView(eCLV_WorldAlphaBlended));
        }
        else
        {
            mImpostors[i]->Render(view, 0);
        }
        if (mImpostors[i] == mDryBonesHead)
        {
            ApplyDamageEffects(mImpostors[i]->mLastModel, mDryBonesHeadCharacter);
        }
        if (mImpostors[i] == mShyGuyMask)
        {
            ApplyDamageEffects(mImpostors[i]->mLastModel, mShyGuyMaskCharacter);
        }
    }
}

nlVector3 Nis::Offset() const
{
    return mHeader->stadiumOffset;
}

void Nis::AddTrigger(NisTriggerType triggerType, float frameNumber,
    const char* name, const char* target, Nis::TriggerParams* trigParams)
{
    mTriggers[mNumTriggers].type = triggerType;
    mTriggers[mNumTriggers].frameNumber = frameNumber;
    mTriggers[mNumTriggers].name = name;
    mTriggers[mNumTriggers].target = target;

    TriggerParams* pParams = &(mTriggers[mNumTriggers].params);
    pParams->float1 = -1.0f;
    pParams->param1 = -1;
    pParams->param2 = -1;
    pParams->param3 = -1;
    pParams->param4 = -1;

    if (trigParams != 0)
    {
        mTriggers[mNumTriggers].params.float1 = trigParams->float1;
        mTriggers[mNumTriggers].params.param1 = trigParams->param1;
        mTriggers[mNumTriggers].params.param2 = trigParams->param2;
        mTriggers[mNumTriggers].params.param3 = trigParams->param3;
        mTriggers[mNumTriggers].params.param4 = trigParams->param4;
    }

    mNumTriggers++;
}

bool Nis::GetMainCharacterHeadPosition(nlVector3& position) const
{
    int charIdx;
    if (mMainCharacterIndex >= 0)
    {
        charIdx = mMainCharacterIndex;
    }
    else
    {
        charIdx = TargetToIndex(mTarget, mWinnerType, false);
    }
    if (charIdx >= 0 && charIdx < MAX_NUM_CHARACTERS)
    {
        position = GetReplayDrawableCharacter(g_pCharacters[charIdx])->headPosition;
        return true;
    }
    return false;
}

void Nis::Trigger::FireEffect(const Nis& nis) const
{
    NisPlayer* player = 0;
    if (params.param1 == 0)
    {
        player = NisPlayer::Instance();
    }
    int charIdx = -1;
    if (nlStrICmp(target, "ball") == 0)
    {
        EmissionController* ctrl = fn_802E7DC4(EmissionManager::Instance(), name, 0, true, 0);
        if (ctrl != 0)
        {
            ctrl->m_uUserData = (u32)player;
            Function1<void, EmissionController&> update(UpdateEmitterFromBall);
            ctrl->SetUpdateCallback(update);
        }
    }
    if (nlStrICmp(target, "ballpos") == 0)
    {
        EmissionController* ctrl = fn_802E7DC4(EmissionManager::Instance(), name, 0, true, 0);
        if (ctrl != 0)
        {
            ReplayManager* manager = ReplayManager::Instance();
            ctrl->SetPosition(manager->mRender->mBall.mPosition);
        }
    }
    else if (nlStrICmp(target, "bip01") == 0)
    {
        if (nis.mMainCharacterIndex >= 0)
        {
            charIdx = nis.mMainCharacterIndex;
        }
        else
        {
            charIdx = nis.TargetToIndex(nis.mTarget, nis.mWinnerType, false);
        }
    }
    else
    {
        int idx = -1;
        for (int i = 0; i < nis.mHeader->numAnimProxies; ++i)
        {
            if (nlStrICmp(nis.mHeader->animProxyNames[i], target) == 0)
            {
                idx = i;
                break;
            }
        }
        if (idx >= 0)
        {
            for (int i = 0; i < MAX_NUM_CHARACTERS; ++i)
            {
                if (idx == nis.mCharacterAnimProxy[i])
                {
                    charIdx = i;
                    break;
                }
            }
        }
    }
    if (charIdx >= 0 && charIdx < MAX_NUM_CHARACTERS)
    {
        void* context = g_pCharacters[charIdx];
        EmissionController* ctrl = fn_802E7DC4(EmissionManager::Instance(), name, 0, true, 0);
        if (ctrl == 0)
            return;
        ctrl->SetAnimController(*nis.mCharacterControllers[charIdx]);
        ctrl->m_uUserData = (u32)player;
        Function<void(EmissionController&)> callback(
            Bind<void>(UpdateEmitterFromCharacterWithoutAnimController, placeholder0, context));
        ctrl->SetUpdateCallback(callback);
    }
    else
    {
        for (int i = 0; i < 8; ++i)
        {
            if (nis.mImpostorNames[i] != 0
                && nlStrNICmp(target, nis.mImpostorNames[i], nlStrLen(nis.mImpostorNames[i])) == 0
                && nis.mImpostorSuffixes[i] == target[nlStrLen(target) - 1])
            {
                EmissionController* ctrl = fn_802E7DC4(EmissionManager::Instance(), name, 0, true, 0);
                if (ctrl == 0)
                    return;
                ctrl->SetAnimController(*nis.mImpostors[i]->mAnimController);
                ctrl->m_uUserData = (u32)player;
                {
                    Function<void(EmissionController&)> callback(
                        Bind<void>(UpdateEmitterFromImpostorModel, placeholder0, (void*)nis.mImpostors[i]));
                    ctrl->SetUpdateCallback(callback);
                }
                ctrl->m_bVisible = nis.mImpostors[i]->mVisible;
                break;
            }
        }
    }
}

void Nis::Trigger::Fire(Nis& nis) const
{
    switch (type)
    {
    case NIS_TRIGGER_TYPE_TIME_DILATION:
        nlTaskManager::SetTimeDilation(params.float1);
        break;
    case NIS_TRIGGER_TYPE_EFFECT:
        FireEffect(nis);
        break;
    case NIS_TRIGGER_TYPE_UNIDENTIFIED_6:
        fn_802789A8(BasicStadium::GetCurrentStadium(), params.param1);
        break;
    case NIS_TRIGGER_TYPE_UNIDENTIFIED_7:
    {
        int charIdx;
        if (nis.mMainCharacterIndex >= 0)
        {
            charIdx = nis.mMainCharacterIndex;
        }
        else
        {
            charIdx = nis.TargetToIndex(nis.mTarget, nis.mWinnerType, false);
        }
        if (charIdx >= 0 && charIdx < MAX_NUM_CHARACTERS)
        {
            cCharacter* character = g_pCharacters[charIdx];
            character->m_Dirt = params.float1;
            character->fn_8001F1C0(2);
        }
        break;
    }
    case NIS_TRIGGER_TYPE_RAISE_EVENT:
    {
        NISData* pData = g_NISDataPool.Allocate();
        pData->Type = name;
        pData->Param = target;
        g_pGame->fn_8005E130(pData);
        break;
    }
    case NIS_TRIGGER_TYPE_PLAY_SOUND:
        if (nis.unknown_0x034 == 0)
        {
            PlaySound(params.param1, params.param2, 0, 0);
        }
        break;
    case NIS_TRIGGER_TYPE_UNIDENTIFIED_4:
        for (int i = 0; i < MAX_NUM_CHARACTERS; ++i)
        {
            cPlayer* player = (cPlayer*)g_pCharacters[i];
            if (player->m_pController != 0)
            {
                PlayRumbleAction(params.param1, player->m_pController->m_pGlobalPad);
            }
        }
        break;
    case NIS_TRIGGER_TYPE_UNIDENTIFIED_5:
        if (params.param1 != 0)
        {
            SetCrowdImpostorsExcited();
        }
        else
        {
            SetCrowdImpostorsIdle();
        }
        break;
    case NIS_TRIGGER_TYPE_UNIDENTIFIED_8:
        if (params.param1 != 0)
        {
            DepthOfFieldManager::instance.TurnOff();
        }
        else
        {
            DepthOfFieldManager::instance.TurnOn();
        }
        break;
    case NIS_TRIGGER_TYPE_UNIDENTIFIED_9:
        DisplayElectricFence();
        break;
    case NIS_TRIGGER_TYPE_UNIDENTIFIED_10:
        StopDisplayingElectricFence();
        break;
    }
}

static inline int FindAvailableSidekickIndex(int firstIndex)
{
    int index;
    for (index = 0; index < 3; ++index)
    {
        if (!g_bNisAnimatedCharacters[index + firstIndex])
        {
            break;
        }
    }
    return index + firstIndex;
}

int Nis::TargetToIndex(NisTarget target, NisWinnerType winnerType, bool findAvailableSidekick) const
{
    if (target == NIS_TARGET_HOME_CAPTAIN)
    {
        return 0;
    }
    if (target == NIS_TARGET_AWAY_CAPTAIN)
    {
        return 4;
    }
    if (target == NIS_TARGET_HOME_SIDEKICK)
    {
        if (findAvailableSidekick)
        {
            return FindAvailableSidekickIndex(1);
        }
        return 1;
    }
    if (target == NIS_TARGET_UNIDENTIFIED_5)
    {
        return 1;
    }
    if (target == NIS_TARGET_UNIDENTIFIED_6)
    {
        return 2;
    }
    if (target == NIS_TARGET_UNIDENTIFIED_7)
    {
        return 3;
    }
    if (target == NIS_TARGET_AWAY_SIDEKICK)
    {
        if (findAvailableSidekick)
        {
            return FindAvailableSidekickIndex(5);
        }
        return 5;
    }
    if (target == NIS_TARGET_UNIDENTIFIED_9)
    {
        return 5;
    }
    if (target == NIS_TARGET_UNIDENTIFIED_10)
    {
        return 6;
    }
    if (target == NIS_TARGET_UNIDENTIFIED_11)
    {
        return 7;
    }
    if (target == NIS_TARGET_HOME_GOALIE)
    {
        return 8;
    }
    if (target == NIS_TARGET_AWAY_GOALIE)
    {
        return 9;
    }
    if (target == NIS_TARGET_UNIDENTIFIED_14)
    {
        return NisPlayer::Instance()->mGoalScorerCharIndex;
    }
    if (target == NIS_TARGET_LOSER_SIDEKICK)
    {
        if (NisPlayer::Instance()->fn_8027E284(winnerType) == 0)
        {
            if (findAvailableSidekick)
            {
                return FindAvailableSidekickIndex(5);
            }
            return 5;
        }
        if (findAvailableSidekick)
        {
            return FindAvailableSidekickIndex(1);
        }
        return 1;
    }
    if (target == NIS_TARGET_WINNER_SIDEKICK)
    {
        if (NisPlayer::Instance()->fn_8027E284(winnerType) == 0)
        {
            if (findAvailableSidekick)
            {
                return FindAvailableSidekickIndex(1);
            }
            return 1;
        }
        if (findAvailableSidekick)
        {
            return FindAvailableSidekickIndex(5);
        }
        return 5;
    }
    if (target == NIS_TARGET_LOSER_GOALIE)
    {
        return (NisPlayer::Instance()->fn_8027E284(winnerType) == 0) ? 9 : 8;
    }
    if (target == NIS_TARGET_WINNER_GOALIE)
    {
        return (NisPlayer::Instance()->fn_8027E284(winnerType) == 0) ? 8 : 9;
    }
    if (target == NIS_TARGET_WINNER_CAPTAIN)
    {
        return (NisPlayer::Instance()->fn_8027E284(winnerType) == 0) ? 0 : 4;
    }
    if (target == NIS_TARGET_LOSER_CAPTAIN)
    {
        return (NisPlayer::Instance()->fn_8027E284(winnerType) == 0) ? 4 : 0;
    }
    if (target == NIS_TARGET_UNIDENTIFIED_21)
    {
        return (NisPlayer::Instance()->mMegaStrikeSide == 0) ? 0 : 4;
    }
    if (target == NIS_TARGET_UNIDENTIFIED_22)
    {
        return (NisPlayer::Instance()->mMegaStrikeSide == 0) ? 9 : 8;
    }
    return (target == NIS_TARGET_NONE) ? 0 : -1;
}

void Nis::PlayAnimProxy(const char* animName, const char* proxyName,
    NisTarget target, NisWinnerType winnerType, bool force)
{
    if (winnerType == (NisWinnerType)4)
    {
        winnerType = mHeader->winnerType;
    }
    for (int j = 0; j < mHeader->numAnimProxies; ++j)
    {
        if (nlStrICmp(mHeader->animProxyNames[j], proxyName) == 0)
        {
            int i = TargetToIndex(target, winnerType, true);
            mCharacterAnimProxy[i] = j;
            mAnimProxyPositions[j] = mHeader->animProxyPositions[j];
            mAnimProxyDirections[j] = mHeader->animProxyDirections[j];
            cCharacter* character = g_pCharacters[i];
            cSAnim* anim = character->GetAnimInventory()->m_pSAnimInventory->Find(nlStringLowerHash(animName));
            if (anim != 0)
            {
                if (force == true || !g_bNisAnimatedCharacters[i])
                {
                    cPN_SAnimController* controller = new cPN_SAnimController(anim, 0, PM_CYCLIC, 0, 0, false);
                    g_bNisAnimatedCharacters[i] = true;
                    mCharacterControllers[i] = controller;
                }
            }
            else
            {
                if (force == true || !g_bNisAnimatedCharacters[i])
                {
                    g_bNisAnimatedCharacters[i] = true;
                    LoadCharacterAnimation(animName, i);
                }
            }
            break;
        }
    }
}

void ClearNisAnimatedCharacters()
{
    for (int i = 0; i < Nis::MAX_NUM_CHARACTERS; ++i)
    {
        g_bNisAnimatedCharacters[i] = false;
    }
}

void Nis::AttachHeadImpostors()
{
    mDryBonesHead = AttachImpostorToCharacter((eCharacterClass)17, "DryBonesHead",
        "dryboneshead/drybones_mario", &mDryBonesHeadCharacter);
    mShyGuyMask = AttachImpostorToCharacter((eCharacterClass)19, "ShyGuyMask",
        "shyguymask/shyguy_mario", &mShyGuyMaskCharacter);
}

ImpostorModel* Nis::AttachImpostorToCharacter(eCharacterClass characterClass, const char* impostorName,
    const char* textureName, DrawableCharacter** outCharacter)
{
    ImpostorModel* model = 0;
    *outCharacter = 0;

    for (int i = 0; i < 8; ++i)
    {
        if (mImpostors[i] != 0 && nlStrCmp(mImpostorNames[i], impostorName) == 0)
        {
            model = mImpostors[i];
        }
    }

    if (model != 0)
    {
        RenderSnapshot& snapshot = ReplayManager::Instance()->GetMutableRenderSnapshot();
        for (int i = 0; i < MAX_NUM_CHARACTERS; ++i)
        {
            DrawableCharacter* pDC = &snapshot.GetCharacter(i);
            cCharacter* character = pDC->character;
            if (character->m_eClassType == FIELDER
                && characterClass == character->mUnidentified024.m_eCharacterClass
                && mCharacterControllers[i] != 0)
            {
                model->SetReplacementTexture(character->mUnidentified104);
                model->mOriginalTexture = glGetTexture(textureName);
                *outCharacter = pDC;
                break;
            }
        }
    }

    return model;
}

void Nis::ApplyDamageEffects(glModel* model, DrawableCharacter* character)
{
    glModelPacket* packet;
    static u32 hash1 = nlStringLowerHash("damage1Enabled");
    static u32 hash2 = nlStringLowerHash("damage2Enabled");

    int stadium = GameInfoManager::Instance()->GetStadium();
    u32 damageTexture = glGetTexture("global/scorch");
    char textureName[64];
    nlSNPrintf(textureName, sizeof(textureName), "%s/dirt", character->character->mUnidentified11C->mName);
    damageTexture = glGetTexture(textureName);

    if (character->character->mUnidentified16C != 1
        && (character->character->mUnidentified16C == 2
            || stadium == 0 || stadium == 5 || stadium == 7
            || stadium == 9 || stadium == 16 || stadium == 8
            || stadium == 14 || stadium == 11 || stadium == 15
            || stadium == 1 || stadium == 6))
    {
        damageTexture = glGetTexture("global/scorch");
    }

    if (character->character->GetDirt() > 0.0f || character->character->GetMinDirt() > 0.0f)
    {
        for (packet = model->packets; packet < model->packets + model->numPackets; ++packet)
        {
            if (glHasMaterialParameter(packet, hash1) && character->character->GetDirt() > 0.0f)
            {
                glSetMaterialUnsignedParameter(packet, hash1, 1);
                glTextureBinding* texture = (glTextureBinding*)packet->materialParameters;
                texture[4].texture = damageTexture;
                texture[4].textureIndex = 0xFFFF;
            }
            if (glHasMaterialParameter(packet, hash2) && character->character->GetMinDirt() > 0.0f)
            {
                glSetMaterialUnsignedParameter(packet, hash2, 1);
            }
        }
    }
}

ImpostorModel* Nis::FindImpostor(const char* name)
{
    for (int i = 0; i < 8; ++i)
    {
        if (mImpostorNames[i] != 0 && nlStrICmp(name, mImpostorNames[i]) == 0)
        {
            return mImpostors[i];
        }
    }
    return 0;
}

bool Nis::IsLoading()
{
    if (!mScriptStarted)
    {
        return true;
    }
    for (int i = 0; i < MAX_NUM_CHARACTERS; ++i)
    {
        if (mPendingAnimations[i].name != 0
            && mPendingAnimations[i].characterIndex != -1
            && !mPendingAnimations[i].loaded)
        {
            return true;
        }
    }
    return false;
}

void Nis::LoadCharacterAnimation(const char* animName, int characterIndex)
{
    cCharacter* character = g_pCharacters[characterIndex];
    PendingAnimation* entry = 0;
    for (int i = 0; i < MAX_NUM_CHARACTERS; ++i)
    {
        if (mPendingAnimations[i].name == 0
            && mPendingAnimations[i].characterIndex == -1)
        {
            entry = &mPendingAnimations[i];
        }
    }

    if (entry != 0)
    {
        eCharacterClass characterClass = character->mUnidentified024.m_eCharacterClass;
        entry->name = animName;
        entry->characterIndex = characterIndex;
        entry->loaded = false;
        tCharacterTemplateInfo* info = GetCharacterTemplateInfo(characterClass);
        char filename[100];
        nlSNPrintf(filename, sizeof(filename) - 1, "Art/Animation/%s/%s.sanim", info->szHierarchy, animName);

        PendingAnimationRequest* request = lbl_8057AB80.Allocate();
        request->active = true;
        request->animation = entry;
        entry->request = request;
        entry->loadHandle = nlLoadEntireFileAsync(filename, OnCharacterAnimationLoaded, request,
            32, AllocateEnd, 0, 0, &VirtualAllocator);
    }
}

void OnCharacterAnimationLoaded(void* data, unsigned long size, void* userData)
{
    PendingAnimationRequest* request = (PendingAnimationRequest*)userData;
    if (request->active == true)
    {
        Nis::PendingAnimation* entry = request->animation;
        entry->loadHandle = 0;
        entry->data = data;
        entry->size = size;
        entry->loaded = true;
    }
    else
    {
        ::operator delete(data);
    }
    lbl_8057AB80.Free(request);
}
