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

GLView* g_pNisRenderView;

bool lbl_8057AB68[Nis::MAX_NUM_CHARACTERS];

struct Unidentified83B88
{
    Nis::Unidentified864* mUnidentified00;
    bool mUnidentified04;
};

SlotPool<Unidentified83B88> lbl_8057AB80(16, 16);

void fn_80283C00(void* data, unsigned long size, void* userData);

Nis::Nis(NisHeader& header, char* data, int size)
    : InterpreterCore(100)
    , mHeader(&header)
    , mTarget(header.target)
    , mWinnerType(header.winnerType)
    , unknown_0x034(header.unknown_0x180)
    , mData(data)
    , mSize(size)
    , mMirrored(header.mUnidentified195)
    , mCamera(0)
    , mNumCameras(0)
    , mNumTriggers(0)
    , mMainCharacterIndex(-1)
    , mAudioCharacterIndex(-1)
    , mUnidentified850(0)
    , mUnidentified854(0)
    , mUnidentified858(0)
    , mUnidentified85C(0)
    , mUnidentified860(0)
    , mUnidentifiedBAC(false)
{
    int i;
    for (int i = 0; i < MAX_NUM_CHARACTERS; ++i)
    {
        mUnidentified864[i].mUnidentified00 = 0;
        mUnidentified864[i].mUnidentified08 = false;
        mUnidentified864[i].mUnidentified04 = -1;
        mUnidentified864[i].mUnidentified0C = 0;
        mUnidentified864[i].mUnidentified10 = 0;
        mUnidentified864[i].mUnidentified14 = 0;
        mUnidentified864[i].mUnidentified18 = 0;
    }
    g_pNisRenderView = GetLayerView(eCLV_WorldShadowed);
    for (int i = 0; i < MAX_NUM_CHARACTERS; ++i)
    {
        mCharacterControllers[i] = 0;
        mBallId[i] = -1;
        mUnidentified090[i] = -1;
    }
    for (int i = 0; i < 8; ++i)
    {
        mUnidentified10C[i] = 0;
        mUnidentified12C[i] = 0;
    }
    for (int i = 0; i < 10; ++i)
    {
        mUnidentified158[i] = new (8, false) cCameraData();
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
            NPCTemplate* npcTemplate = gNPCManager->fn_801ABBDC(anim->m_szName);
            if (npcTemplate != 0)
            {
                for (i = 0; i < 8; ++i)
                {
                    if (mUnidentified10C[i] == 0)
                        break;
                }
                mUnidentified10C[i] = new (8, false) ImpostorModel(*npcTemplate->hierarchy, npcTemplate->modelID, npcTemplate->mResourcePool);
                mUnidentified10C[i]->PlayAnimation(*anim, PM_HOLD, 0);
                mUnidentified10C[i]->mVisible = true;
                mUnidentified10C[i]->mModelCallback = fn_80183F78;
                mUnidentified12C[i] = npcTemplate->mName;
                mUnidentified14C[i] = anim->m_szName[nlStrLen(anim->m_szName) - 1];
                char textureName[256];
                if (nlStrCmp(npcTemplate->mName, "vice_image_plane_top") == 0
                    || nlStrCmp(npcTemplate->mName, "vice_image_plane_bottom") == 0)
                {
                    nlSNPrintf(textureName, sizeof(textureName), "_%s/%s", fn_802772C4(), npcTemplate->mName + 5);
                    unsigned long texture = nlStringLowerHash(textureName);
                    if (glTextureLoad(texture))
                    {
                        mUnidentified10C[i]->SetReplacementTexture(texture);
                        nlSNPrintf(textureName, sizeof(textureName), "%s/%s", npcTemplate->mName, npcTemplate->mName);
                        mUnidentified10C[i]->mOriginalTexture = nlStringLowerHash(textureName);
                    }
                }
                else if (nlStrCmp(npcTemplate->mName, "mario_mega_orange_bg") == 0)
                {
                    int charIdx = fn_80282DD8(mTarget, mWinnerType, true);
                    int captain = GameInfoManager::Instance()->GetTeam((short)((charIdx < 4 || charIdx == 8) == false));
                    nlSNPrintf(textureName, sizeof(textureName), "%s/mega_cone_colour", GetCharacterInfo(GetCharacterIndexFromCaptain(captain)).mName);
                    unsigned long texture = nlStringLowerHash(textureName);
                    if (glTextureLoad(texture))
                    {
                        mUnidentified10C[i]->SetReplacementTexture(texture);
                        mUnidentified10C[i]->mOriginalTexture = nlStringLowerHash("mario_mega_orange_bg/mega_cone_colour");
                    }
                }
                else if (nlStrCmp(npcTemplate->mName, "NIS_ball") == 0)
                {
                    ++numBalls;
                    if (numBalls >= NisPlayer::Instance()->mMaxNumBallsVisible)
                    {
                        mUnidentified10C[i]->mVisible = false;
                    }
                }
            }
            else if (mTarget != NIS_TARGET_NONE && mTarget != NIS_TARGET_STADIUM)
            {
                i = fn_80282DD8(mTarget, mWinnerType, true);
                if (mCharacterControllers[i] != 0)
                {
                    i = fn_80282DD8(NIS_TARGET_HOME_CAPTAIN, mWinnerType, true);
                }
                if (mCharacterControllers[i] != 0)
                {
                    i = fn_80282DD8(NIS_TARGET_AWAY_CAPTAIN, mWinnerType, true);
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
                    lbl_8057AB68[i] = true;
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
            if (LoadAnimCameraData(cameraBegin, cameraEnd, mUnidentified158[mNumCameras], false)
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
void Nis::fn_802815E0()
{
    mUnidentifiedBAC = true;
    char name[64];
    nlStrNCpy(name, mHeader->name, sizeof(name));
    *nlStrChr(name, '.') = '\0';
    CallFunction(nlStringHash(name));
}

void Nis::fn_802816CC()
{
    for (int i = 0; i < MAX_NUM_CHARACTERS; ++i)
    {
        if (mUnidentified864[i].mUnidentified00 != 0
            && mUnidentified864[i].mUnidentified04 != -1)
        {
            cInventory<cSAnim>& inventory = mUnidentified97C[mUnidentified864[i].mUnidentified04];
            inventory.AddFile((char*)mUnidentified864[i].mUnidentified10,
                mUnidentified864[i].mUnidentified14);
            mUnidentified864[i].mUnidentified10 = 0;
            cSAnim* anim = inventory.Find(nlStringHash(mUnidentified864[i].mUnidentified00));
            mCharacterControllers[mUnidentified864[i].mUnidentified04] =
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
        if (mUnidentified864[i].mUnidentified0C != 0)
        {
            if (nlAsyncReadBusy((AsyncEntry*)mUnidentified864[i].mUnidentified0C))
            {
                ((Unidentified83B88*)mUnidentified864[i].mUnidentified18)->mUnidentified04 = false;
                ((Unidentified83B88*)mUnidentified864[i].mUnidentified18)->mUnidentified00 = 0;
            }
            else
            {
                nlCancelEntireFileLoad(mUnidentified864[i].mUnidentified0C, 0);
                lbl_8057AB80.Free((Unidentified83B88*)mUnidentified864[i].mUnidentified18);
            }
        }
        if (mUnidentified864[i].mUnidentified10 != 0)
        {
            ::operator delete(mUnidentified864[i].mUnidentified10);
        }
        mUnidentified864[i].mUnidentified0C = 0;
        mUnidentified864[i].mUnidentified00 = 0;
        mUnidentified864[i].mUnidentified08 = false;
        mUnidentified864[i].mUnidentified04 = -1;
        mUnidentified864[i].mUnidentified10 = 0;
        mUnidentified864[i].mUnidentified14 = 0;
        mUnidentified864[i].mUnidentified18 = 0;
    }

    for (int i = 0; i < 8; ++i)
    {
        if (mUnidentified10C[i] != 0)
        {
            delete mUnidentified10C[i];
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
        delete mUnidentified158[i];
        mUnidentified158[i] = 0;
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
        ImpostorModel* pModel = mUnidentified10C[i];
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
    camera.m_pActiveCameraData = mUnidentified158[index];
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

void Nis::fn_80281C70(cAnimCamera& camera)
{
    int randomIndex = nlRandom(mNumCameras, fn_80287B2C(GetPresentation()));
    SelectCamera(camera, randomIndex);
}

void Nis::Render(int param1)
{
    DrawableCharacter* pDC;
    RenderSnapshot& snapshot = ReplayManager::Instance()->GetMutableRenderSnapshot();
    nlVector3 offset = { 0.0f, 0.0f, 0.0f };
    int numBalls = 0;

    for (int i = 0; i < MAX_NUM_CHARACTERS; ++i)
    {
        pDC = &snapshot.GetCharacter(i);
        if (mCharacterControllers[i] == 0)
            continue;
        pDC->visible = true;

        nlVector3 rootTrans = { 0.0f, 0.0f, 0.0f };
        u16 angle = 0;
        int index = mUnidentified090[i];
        if (index >= 0)
        {
            mCharacterControllers[i]->GetRootTrans(&rootTrans, mUnidentified0F8[index], 1.0f);
            nlVec2Set(mUnidentified0B8[index],
                mUnidentified0B8[index].x + rootTrans.x,
                mUnidentified0B8[index].y + rootTrans.y);
            nlVec3Set(rootTrans, mUnidentified0B8[index].x, mUnidentified0B8[index].y, 0.0f);
            mCharacterControllers[i]->GetRootRot(&angle);
            mUnidentified0F8[index] += angle;
            angle = mUnidentified0F8[index];
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
        if (mUnidentified10C[i] == 0)
            continue;
        nlVector3 rootTrans = { 0.0f, 0.0f, 0.0f };
        u16 angle = 0;
        float fTime = mUnidentified10C[i]->mAnimController->get_fTime();
        mUnidentified10C[i]->mAnimController->m_pSAnim->GetRootTrans(fTime, &rootTrans);
        fTime = mUnidentified10C[i]->mAnimController->get_fTime();
        mUnidentified10C[i]->mAnimController->m_pSAnim->GetRootRot(fTime, &angle);
        if (mMirrored)
        {
            mUnidentified10C[i]->mAnimController->m_bMirror = true;
            rootTrans.x *= -1.0f;
            angle = angle + (0x4000 - angle) * 2;
        }
        nlMatrix4 matrix;
        nlMakeRotationMatrixZ(matrix, AngUnitsToRad_fromUnsignedShort(angle));
        matrix.SetTranslation(rootTrans);
        mUnidentified10C[i]->mWorldMatrix = matrix;

        GLView* view = GetLayerView(eCLV_MoreCharacters);
        if (param1 == 1 && (unknown_0x034 == 1 || unknown_0x034 == 2))
        {
            view = GetLayerView(eCLV_PictureInPicture);
        }
        GLSkinMesh* skinMesh = mUnidentified10C[i]->mSkinMesh;
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
            mUnidentified10C[i]->Render(view, GetLayerView(eCLV_WorldAlphaBlended));
        }
        else
        {
            mUnidentified10C[i]->Render(view, 0);
        }
        if (mUnidentified10C[i] == mUnidentified854)
        {
            fn_80283670(mUnidentified10C[i]->mLastModel, mUnidentified858);
        }
        if (mUnidentified10C[i] == mUnidentified85C)
        {
            fn_80283670(mUnidentified10C[i]->mLastModel, mUnidentified860);
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

bool Nis::fn_80282474(nlVector3& param1)
{
    int charIdx;
    if (mMainCharacterIndex >= 0)
    {
        charIdx = mMainCharacterIndex;
    }
    else
    {
        charIdx = fn_80282DD8(mTarget, mWinnerType, false);
    }
    if (charIdx >= 0 && charIdx < MAX_NUM_CHARACTERS)
    {
        param1 = GetReplayDrawableCharacter(g_pCharacters[charIdx])->headPosition;
        return true;
    }
    return false;
}

void Nis::Trigger::FireEffect(Nis& nis) const
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
            ctrl->SetPosition(ReplayManager::Instance()->mRender->mBall.mPosition);
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
            charIdx = nis.fn_80282DD8(nis.mTarget, nis.mWinnerType, false);
        }
    }
    else
    {
        int idx = -1;
        for (int i = 0; i < nis.mHeader->mUnidentified0A4; ++i)
        {
            if (nlStrICmp(nis.mHeader->mUnidentified0A8[i], target) == 0)
            {
                idx = i;
                break;
            }
        }
        if (idx >= 0)
        {
            for (int i = 0; i < MAX_NUM_CHARACTERS; ++i)
            {
                if (idx == nis.mUnidentified090[i])
                {
                    charIdx = i;
                    break;
                }
            }
        }
    }
    if (charIdx >= 0 && charIdx < MAX_NUM_CHARACTERS)
    {
        cCharacter* character = g_pCharacters[charIdx];
        EmissionController* ctrl = fn_802E7DC4(EmissionManager::Instance(), name, 0, true, 0);
        if (ctrl == 0)
            return;
        ctrl->SetAnimController(*nis.mCharacterControllers[charIdx]);
        ctrl->m_uUserData = (u32)player;
        Function<void(EmissionController&)> callback(
            Bind<void>(UpdateEmitterFromCharacterWithoutAnimController, placeholder0, character));
        ctrl->SetUpdateCallback(callback);
    }
    else
    {
        for (int i = 0; i < 8; ++i)
        {
            if (nis.mUnidentified12C[i] != 0
                && nlStrNICmp(target, nis.mUnidentified12C[i], nlStrLen(nis.mUnidentified12C[i])) == 0
                && nis.mUnidentified14C[i] == target[nlStrLen(target) - 1])
            {
                EmissionController* ctrl = fn_802E7DC4(EmissionManager::Instance(), name, 0, true, 0);
                if (ctrl == 0)
                    return;
                ctrl->SetAnimController(*nis.mUnidentified10C[i]->mAnimController);
                ctrl->m_uUserData = (u32)player;
                {
                    Function<void(EmissionController&)> callback(
                        Bind<void>(UpdateEmitterFromImpostorModel, placeholder0, nis.mUnidentified10C[i]));
                    ctrl->SetUpdateCallback(callback);
                }
                ctrl->m_bVisible = nis.mUnidentified10C[i]->mVisible;
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
            charIdx = nis.fn_80282DD8(nis.mTarget, nis.mWinnerType, false);
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

int Nis::fn_80282DD8(NisTarget target, NisWinnerType winnerType, bool param3)
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
        if (param3)
        {
            int index;
            for (index = 0; index < 3; ++index)
            {
                if (!lbl_8057AB68[index + 1])
                {
                    break;
                }
            }
            return index + 1;
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
        if (param3)
        {
            int index;
            for (index = 0; index < 3; ++index)
            {
                if (!lbl_8057AB68[index + 5])
                {
                    break;
                }
            }
            return index + 5;
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
            if (param3)
            {
                int index;
                for (index = 0; index < 3; ++index)
                {
                    if (!lbl_8057AB68[index + 5])
                    {
                        break;
                    }
                }
                return index + 5;
            }
            return 5;
        }
        if (param3)
        {
            int index;
            for (index = 0; index < 3; ++index)
            {
                if (!lbl_8057AB68[index + 1])
                {
                    break;
                }
            }
            return index + 1;
        }
        return 1;
    }
    if (target == NIS_TARGET_WINNER_SIDEKICK)
    {
        if (NisPlayer::Instance()->fn_8027E284(winnerType) == 0)
        {
            if (param3)
            {
                int index;
                for (index = 0; index < 3; ++index)
                {
                    if (!lbl_8057AB68[index + 1])
                    {
                        break;
                    }
                }
                return index + 1;
            }
            return 1;
        }
        if (param3)
        {
            int index;
            for (index = 0; index < 3; ++index)
            {
                if (!lbl_8057AB68[index + 5])
                {
                    break;
                }
            }
            return index + 5;
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
        return (NisPlayer::Instance()->mUnidentified34238 == 0) ? 0 : 4;
    }
    if (target == NIS_TARGET_UNIDENTIFIED_22)
    {
        return (NisPlayer::Instance()->mUnidentified34238 == 0) ? 9 : 8;
    }
    return (target == NIS_TARGET_NONE) ? 0 : -1;
}

void Nis::fn_80283200(const char* param1, const char* param2,
    NisTarget param3, NisWinnerType param4, bool param5)
{
    if (param4 == (NisWinnerType)4)
    {
        param4 = mHeader->winnerType;
    }
    for (int j = 0; j < mHeader->mUnidentified0A4; ++j)
    {
        if (nlStrICmp(mHeader->mUnidentified0A8[j], param2) == 0)
        {
            int i = fn_80282DD8(param3, param4, true);
            mUnidentified090[i] = j;
            mUnidentified0B8[j] = mHeader->mUnidentified128[j];
            mUnidentified0F8[j] = mHeader->mUnidentified168[j];
            cCharacter* character = g_pCharacters[i];
            cSAnim* anim = character->GetAnimInventory()->m_pSAnimInventory->Find(nlStringLowerHash(param1));
            if (anim != 0)
            {
                if (param5 == true || !lbl_8057AB68[i])
                {
                    cPN_SAnimController* controller = new cPN_SAnimController(anim, 0, PM_CYCLIC, 0, 0, false);
                    lbl_8057AB68[i] = true;
                    mCharacterControllers[i] = controller;
                }
            }
            else
            {
                if (param5 == true || !lbl_8057AB68[i])
                {
                    lbl_8057AB68[i] = true;
                    fn_80283A40(param1, i);
                }
            }
            break;
        }
    }
}

void fn_8028346C()
{
    for (int i = 0; i < Nis::MAX_NUM_CHARACTERS; ++i)
    {
        lbl_8057AB68[i] = false;
    }
}

void Nis::fn_802834A0()
{
    mUnidentified854 = fn_8028350C((eCharacterClass)17, "DryBonesHead",
        "dryboneshead/drybones_mario", &mUnidentified858);
    mUnidentified85C = fn_8028350C((eCharacterClass)19, "ShyGuyMask",
        "shyguymask/shyguy_mario", &mUnidentified860);
}

ImpostorModel* Nis::fn_8028350C(eCharacterClass param1, const char* param2,
    const char* param3, DrawableCharacter** param4)
{
    ImpostorModel* model = 0;
    *param4 = 0;

    for (int i = 0; i < 8; ++i)
    {
        if (mUnidentified10C[i] != 0 && nlStrCmp(mUnidentified12C[i], param2) == 0)
        {
            model = mUnidentified10C[i];
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
                && param1 == character->mUnidentified024.m_eCharacterClass
                && mCharacterControllers[i] != 0)
            {
                model->SetReplacementTexture(character->mUnidentified104);
                model->mOriginalTexture = glGetTexture(param3);
                *param4 = pDC;
                break;
            }
        }
    }

    return model;
}

void Nis::fn_80283670(glModel* model, DrawableCharacter* character)
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

    if (character->character->m_Dirt > 0.0f || character->character->m_MinDirt > 0.0f)
    {
        for (packet = model->packets; packet < model->packets + model->numPackets; ++packet)
        {
            if (glHasMaterialParameter(packet, hash1) && character->character->m_Dirt > 0.0f)
            {
                glSetMaterialUnsignedParameter(packet, hash1, 1);
                glTextureBinding* texture = (glTextureBinding*)packet->materialParameters;
                texture[4].texture = damageTexture;
                texture[4].textureIndex = 0xFFFF;
            }
            if (glHasMaterialParameter(packet, hash2) && character->character->m_MinDirt > 0.0f)
            {
                glSetMaterialUnsignedParameter(packet, hash2, 1);
            }
        }
    }
}

ImpostorModel* Nis::fn_80283884(const char* name)
{
    for (int i = 0; i < 8; ++i)
    {
        if (mUnidentified12C[i] != 0 && nlStrICmp(name, mUnidentified12C[i]) == 0)
        {
            return mUnidentified10C[i];
        }
    }
    return 0;
}

bool Nis::fn_80283930()
{
    if (!mUnidentifiedBAC)
    {
        return true;
    }
    for (int i = 0; i < MAX_NUM_CHARACTERS; ++i)
    {
        if (mUnidentified864[i].mUnidentified00 != 0
            && mUnidentified864[i].mUnidentified04 != -1
            && !mUnidentified864[i].mUnidentified08)
        {
            return true;
        }
    }
    return false;
}

void Nis::fn_80283A40(const char* param1, int param2)
{
    cCharacter* character = g_pCharacters[param2];
    Unidentified864* entry = 0;
    for (int i = 0; i < MAX_NUM_CHARACTERS; ++i)
    {
        if (mUnidentified864[i].mUnidentified00 == 0
            && mUnidentified864[i].mUnidentified04 == -1)
        {
            entry = &mUnidentified864[i];
        }
    }

    if (entry != 0)
    {
        entry->mUnidentified00 = param1;
        entry->mUnidentified04 = param2;
        entry->mUnidentified08 = false;
        tCharacterTemplateInfo* info = GetCharacterTemplateInfo(character->mUnidentified024.m_eCharacterClass);
        char filename[100];
        nlSNPrintf(filename, sizeof(filename) - 1, "Art/Animation/%s/%s.sanim", info->szHierarchy, param1);

        Unidentified83B88* callback = lbl_8057AB80.Allocate();
        callback->mUnidentified04 = true;
        callback->mUnidentified00 = entry;
        entry->mUnidentified18 = callback;
        entry->mUnidentified0C = nlLoadEntireFileAsync(filename, fn_80283C00, callback,
            32, AllocateEnd, 0, 0, &VirtualAllocator);
    }
}

void fn_80283C00(void* data, unsigned long size, void* userData)
{
    Unidentified83B88* callback = (Unidentified83B88*)userData;
    if (callback->mUnidentified04 == true)
    {
        Nis::Unidentified864* entry = callback->mUnidentified00;
        entry->mUnidentified0C = 0;
        entry->mUnidentified10 = data;
        entry->mUnidentified14 = size;
        entry->mUnidentified08 = true;
    }
    else
    {
        ::operator delete(data);
    }
    lbl_8057AB80.Free(callback);
}

void Nis::DoFunctionCall(unsigned int param1)
{
    switch (param1)
    {
    case 0:
    {
        NisPlayer* player = NisPlayer::Instance();
        player->mUnidentified343E0 = true;
        break;
    }
    case 1:
    {
        int param2 = m_SP[-1];
        m_SP[-1] = GameInfoManager::Instance()->GetStadium() == param2;
        if (m_RunState == 3)
        {
            m_SP[-1] = param2;
        }
        break;
    }
    case 2:
    {
        bool param6 = m_SP[-1] != 0;
        NisWinnerType param5 = (NisWinnerType)m_SP[-2];
        NisTarget param4 = (NisTarget)m_SP[-3];
        const char* param3 = (const char*)m_SP[-4];
        const char* param2 = (const char*)m_SP[-5];
        m_SP -= 5;
        fn_80283200(param2, param3, param4, param5, param6);
        break;
    }
    case 3:
    {
        float param3 = *(float*)(m_SP - 1);
        float param2 = *(float*)(m_SP - 2);
        m_SP -= 2;
        NisPlayer* player = NisPlayer::Instance();
        player->mUnidentified343E4 = param2;
        player->mUnidentified343EC = param3;
        break;
    }
    default:
        nlBreak();
        break;
    }
}
