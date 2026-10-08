#include "NL/nlDLListContainer.inl"
#include "Game/NisPlayer.h"
#include "Game/Blinker.h"
#include "NL/nlBasicString.h"
#include "Game/Render/StadiumLoading.h"
#include "Game/CharacterTemplate.h"
#include "Game/EventDataTypes.h"
#include "Game/EventRegistry.h"
#include "NL/nlFunction.inl"
#include "NL/nlBindMember.inl"
#include "Game/Render/Presentation.h"
#include "Game/Sys/tweak.h"
#include "Game/Player.h"
#include "Game/ReplayManager.h"
#include "Game/Game.h"
#include "Game/Render/ShootToScoreArrow.h"
#include "Game/Weather.h"
#include "Game/GameInfo.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "NL/nlConfig.h"

#include <string.h>
#include <stdio.h>

#include "Game/AI/Fielder.h"
#include "Game/Goalie.h"
#include "Game/Team.h"
#include "Game/TweakQuery.h"

#include "Game/Effects/EmissionManager.h"
#include "Game/Render/NisPlayerOverlay.h"
#include "NL/MemAlloc.h"
#include "NL/nlDebug.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlMath.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"
#include "NL/nlstring_tmpl.h"

#include "Game/Camera/CameraMan.h"
#include "Game/Render/depthoffield.h"
#include "Game/Sys/audio.h"
#include "Game/Sys/simpleparser.h"
#include "NL/glx/glxSend.h"
#include "NL/gl/glState.h"
#include "NL/gl/glTextureManager.h"
#include "NL/gl/glMaterialParameters.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glTexture.h"
#include "Game/Transitions/ModelTransition.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/UnidentifiedStaticStorage.h"

#include <revolution/os/OS_fwd.h>

void OnNisTriggersLoaded(void*, unsigned long, void*);
void OnNisAnimProxyLoaded(void*, unsigned long, void*);
void OnNisDoNotMirrorListLoaded(void*, unsigned long, void*);
void OnNisDoNotShowPIPListLoaded(void*, unsigned long, void*);
void OnNisDictionaryLoaded(void*, unsigned long, void*);
void ApplyNisTransitionTeamLogo(glModel*);

#include "src/Game/NisPlayer_interp.cpp"

float g_NisWorldDarkeningAmount = 0.65f;
float g_NisWorldDarkeningFadeTime = 0.65f;

namespace
{
static unsigned char useAsyncLoading = true;
}


NisPlayer* NisPlayer::Instance()
{
    static NisPlayer* instance;
    if (instance == 0)
    {
        instance = new NisPlayer;
    }
    return instance;
}

NisPlayer::NisPlayer()
    : InterpreterCore(100)
    , mLoadedAssetMask(0)
    , mActive(false)
    , mDictSize(0)
    , mMaxNumBallsVisible(1)
    , mLoadingFromBack(false)
    , mUsedFromFront(0)
    , mUsedFromBack(0x70800)
    , mGoalScorerCharIndex(-1)
    , mAnimProxyByteCode(0)
    , mOverlayMode(1)
    , mPlayingNisCue(0)
    , mPreparedNisCue(0)
    , mStopNisCueOnReset(true)
    , mDisplayNisInfo(false)
    , mUseViewMatrixOverride(false)
    , mInitialCameraRotationCached(false)
    , mSavedFogStart(-1.0f)
    , mSavedFogEnd(-1.0f)
    , mLastCelebrationIndex(-1)
    , mCameraOverrun(0.0f)
{
    mMemory = (char*)nlMalloc(0x70800, 8, false);
    mLoadedAssetMask = 0;
    mAnimProxyByteCode = NULL;
    nlLoadEntireFileAsync("art/Scripts/nis_triggers.byte_code", OnNisTriggersLoaded, this, 0x20, AllocateEnd, NULL, 0, &VirtualAllocator);
    nlLoadEntireFileAsync("art/Scripts/nis_anim_proxy.byte_code", OnNisAnimProxyLoaded, this, 0x20, AllocateEnd, NULL, 0, &VirtualAllocator);
    nlLoadEntireFileAsync("art/nis/do_not_mirror.txt", OnNisDoNotMirrorListLoaded, this, 0x20, AllocateEnd, NULL, 0, &VirtualAllocator);
    nlLoadEntireFileAsync("art/nis/do_not_showPiP.txt", OnNisDoNotShowPIPListLoaded, this, 0x20, AllocateEnd, NULL, 0, &VirtualAllocator);
    nlLoadEntireFileAsync("art/nis/nis_dict.txt", OnNisDictionaryLoaded, this, 0x20, AllocateEnd, NULL, 0, &VirtualAllocator);
    for (int i = 0; i < 8; i++)
    {
        mLoadQueue[i] = NULL;
        mPlaying[i] = NULL;
        mLoaded[i] = NULL;
        mAsyncStarted[i] = false;
    }
    Reset();
    for (int i = 0; i < 2; i++)
    {
        mCamera[i].m_LetManagerDoUpdate = false;
        mCamera[i].m_bCyclic = false;
    }
    mOverlays[0] = new (8, false) NisPlayerNoOverlay(this);
    mOverlays[1] = new (8, false) NisPlayerPIPOverlay(this);
    mOverlays[4] = new (8, false) NisPlayerHolotronOverlay(this);
    mOverlays[2] = new (8, false) NisPlayerCameraSwapOverlay(this);
    mOverlays[3] = new (8, false) NisPlayerPIPExpandOverlay(this, 1.0f);
    mDisplayNisInfo = GetTweakBool("/user/DisplayNISInfo", false);
    mSuppressBlinking = false;
    mRequestedFogStart = -1.0f;
    mRequestedFogEnd = -1.0f;
    for (int i = 0; i < 10; i++)
    {
        mTeamLogoPackets[i] = NULL;
    }
    g_ModelTransitionRenderCallback = ApplyNisTransitionTeamLogo;
    mLastCelebrationFilter[0] = '\0';
}

NisPlayer::~NisPlayer()
{
    delete[] mDoNotMirrorNames;
    delete[] mDoNotShowPIPNames;
    for (int i = 0; i < 10; i++)
    {
        mTeamLogoPackets[i] = NULL;
    }
}

void NisPlayer::ParseDoNotMirrorList(char* data, unsigned long size)
{
    SimpleParser parser;
    const char* separators = "\n\r";
    parser.StartParsing(data, size + 1, separators);
    int count = 0;
    bool more = parser.AdvanceLine();
    while (more)
    {
        count++;
        more = parser.AdvanceLine();
    }
    mNumDoNotMirrorNames = count;
    parser.StartParsing(data, size + 1, separators);
    mDoNotMirrorNames = new (8, false) char*[count];
    char* token = parser.NextTokenOnLine(false);
    int i = 0;
    while (token != NULL)
    {
        char* name = new (8, false) char[parser.GetTokenLength() + 1];
        memcpy(name, token, parser.GetTokenLength());
        name[parser.GetTokenLength()] = 0;
        mDoNotMirrorNames[i] = name;
        parser.AdvanceLine();
        token = parser.NextTokenOnLine(false);
        i++;
    }
    nlFree(data);
}

void NisPlayer::ParseDoNotShowPIPList(char* data, unsigned long size)
{
    SimpleParser parser;
    const char* separators = "\n\r";
    parser.StartParsing(data, size + 1, separators);
    int count = 0;
    bool more = parser.AdvanceLine();
    while (more)
    {
        count++;
        more = parser.AdvanceLine();
    }
    mNumDoNotShowPIPNames = count;
    parser.StartParsing(data, size + 1, separators);
    mDoNotShowPIPNames = new (8, false) char*[count];
    char* token = parser.NextTokenOnLine(false);
    int i = 0;
    while (token != NULL)
    {
        char* name = new (8, false) char[parser.GetTokenLength() + 1];
        memcpy(name, token, parser.GetTokenLength());
        name[parser.GetTokenLength()] = 0;
        mDoNotShowPIPNames[i] = name;
        parser.AdvanceLine();
        token = parser.NextTokenOnLine(false);
        i++;
    }
    nlFree(data);
}

static inline void SkipLine(const char*& data)
{
    while (*data != '\n')
        data++;
    while (*data == '\n')
        data++;
}

void NisPlayer::ParseDictionary(char* data)
{
    if (data != NULL)
    {
        mDictSize = 0;
        const char* dictionaryCursor = data;
        while (mDictSize < 512)
        {
            NisHeader& header = mDict[mDictSize];
            if (sscanf(dictionaryCursor, "name %s", header.name) != 1)
                break;
            SkipLine(dictionaryCursor);
            if (sscanf(dictionaryCursor, "\tsize %d", &header.size) != 1)
                break;
            SkipLine(dictionaryCursor);
            if (sscanf(dictionaryCursor, "\thas_ball %d", &header.numBalls) != 1)
                break;
            SkipLine(dictionaryCursor);
            if (sscanf(dictionaryCursor, "\tnum_animations %d", &header.numAnimations) != 1)
                break;
            SkipLine(dictionaryCursor);
            if (sscanf(dictionaryCursor, "\tnum_cameras %d", &header.numCameras) != 1)
                break;
            SkipLine(dictionaryCursor);
            if (sscanf(dictionaryCursor, "\tcenter %f, %f, %f", &header.center.x, &header.center.y, &header.center.z) != 3)
                break;
            SkipLine(dictionaryCursor);
            if (sscanf(dictionaryCursor, "\tmin_bounds %f, %f, %f", &header.minBounds.x, &header.minBounds.y, &header.minBounds.z) != 3)
                break;
            SkipLine(dictionaryCursor);
            if (sscanf(dictionaryCursor, "\tmax_bounds %f, %f, %f", &header.maxBounds.x, &header.maxBounds.y, &header.maxBounds.z) != 3)
                break;
            SkipLine(dictionaryCursor);

            nlToLower<char>(header.name);

            for (int i = 0; i < header.numAnimations; i++)
            {
                nlVector3 beginPos = { { 0, 0, 0 } };
                int parsedValueCount = sscanf(dictionaryCursor, "\tbegin_pos %f, %f, %f", &beginPos.x, &beginPos.y, &beginPos.z);
                if (parsedValueCount != 3)
                    break;
                SkipLine(dictionaryCursor);
                header.beginPositions[i] = beginPos;
            }
            sscanf(dictionaryCursor, "\tnum_anim_proxies %d", &header.numAnimProxies);
            SkipLine(dictionaryCursor);
            for (int i = 0; i < header.numAnimProxies; i++)
            {
                sscanf(dictionaryCursor, "\tanim_proxy_name %s", header.animProxyNames[i]);
                SkipLine(dictionaryCursor);
                sscanf(dictionaryCursor, "\tanim_proxy_position %f %f", &header.animProxyPositions[i].x, &header.animProxyPositions[i].y);
                SkipLine(dictionaryCursor);
                int direction;
                sscanf(dictionaryCursor, "\tanim_proxy_direction %d", &direction);
                header.animProxyDirections[i] = direction;
                SkipLine(dictionaryCursor);
            }
            header.buffer = 0;
            header.bufferSize = 0;
            header.mUnidentified194 = false;
            mDictSize++;
        }
        nlFree(data);
    }
}

void NisPlayer::fn_8027BD60()
{
}

void NisPlayer::SelectCameras()
{
    for (int j = 0; j < 2; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            if (mPlaying[i] != 0
                && (j == mPlaying[i]->mRenderMode || (j == 1 && mPlaying[i]->mRenderMode == 2))
                && mPlaying[i]->mNumCameras != 0)
            {
                mPlaying[i]->SelectRandomCamera(mCamera[j]);
                break;
            }
        }
    }
}

static inline float MaximumCameraTimeLeft(float first, float second)
{
    if (first >= second)
        return first;
    return second;
}

float NisPlayer::TimeLeft() const
{
    float timeLeft = 0.0f;
    for (int i = 0; i < 2; i++)
    {
        float remaining = 0.0f;
        if (mCamera[i].m_pActiveCameraData != 0)
        {
            remaining = mCamera[i].GetTimeLeft();
        }
        timeLeft = MaximumCameraTimeLeft(timeLeft, remaining);
    }

    cCameraData* pCameraData = mCamera[0].m_pActiveCameraData;
    if (pCameraData != 0)
    {
        float remaining = mCamera[0].GetTimeLeft();
        if (remaining < timeLeft)
        {
            timeLeft = remaining;
        }
    }
    return timeLeft;
}

float NisPlayer::GetCameraTimeLeft(int cameraIndex) const
{
    cCameraData* pCameraData = mCamera[cameraIndex].m_pActiveCameraData;
    if (pCameraData != 0)
    {
        float timeLeft = mCamera[cameraIndex].GetTimeLeft();

        cCameraData* pCameraData = mCamera[0].m_pActiveCameraData;
        if (pCameraData != 0)
        {
            float remaining = mCamera[0].GetTimeLeft();
            if (remaining < timeLeft)
            {
                timeLeft = remaining;
            }
        }
        return timeLeft;
    }
    return -1.0f;
}

bool NisPlayer::WorldIsFrozen() const
{
    bool stateOK = (nlTaskManager::m_pInstance->mCurrentState == 0x10);
    if (stateOK)
    {
        stateOK = TimeLeft() == 0.0f;
    }
    return stateOK;
}

void NisPlayer::HandleAsyncs()
{
    for (int i = 0; i < 8; i++)
    {
        if (mLoadQueue[i] != 0)
        {
            if (!mAsyncStarted[i])
            {
                mAsyncStarted[i] = 1;
                if (mLoadingFromBack)
                {
                    mUsedFromBack -= mLoadQueue[i]->size;
                    mUsedFromBack -= 0x20;
                }

                int memoryOffset = mLoadingFromBack ? mUsedFromBack : mUsedFromFront;

                char* loadAt = mMemory + memoryOffset;
                loadAt = loadAt + (0x20 - ((unsigned int)loadAt & 0x1F));
                if (!mLoadingFromBack)
                {
                    mUsedFromFront += mLoadQueue[i]->size;
                    mUsedFromFront += 0x20;
                }
                if (mUsedFromFront >= mUsedFromBack)
                {
                    nlBreak();
                }

                char fileName[64];
                nlSNPrintf(fileName, sizeof(fileName), "art/nis/%s", mLoadQueue[i]->name);
                if (useAsyncLoading)
                {
                    nlFile* file = nlOpen(fileName);
                    OSGetConsoleType();
                    nlReadAsync(file, loadAt, mLoadQueue[i]->size, AsyncLoad, (unsigned long)mLoadQueue[i], 0);
                }
                else
                {
                    unsigned long size = 0;
                    nlLoadEntireFile(fileName, &size, 0x20, AllocateEnd, loadAt, AlignUp32(mLoadQueue[i]->size), 0);
                    AsyncLoad(0, loadAt, mLoadQueue[i]->size, (unsigned long)mLoadQueue[i]);
                }
            }
        }
    }
}

void NisPlayer::Update(float deltaT)
{
    float realTimeDelta = nlTaskManager::m_pInstance->mRealTimeDelta;
    float timeDilation = nlTaskManager::m_pInstance->mTimeDilation;
    deltaT = realTimeDelta * timeDilation;
    if (deltaT > 0.5f)
    {
        deltaT = 0.5f;
    }
    deltaT += mCameraOverrun;
    mCameraOverrun = 0.0f;
    HandleAsyncs();

    for (int i = 0; i < 8; i++)
    {
        if (mLoadQueue[i] != 0)
        {
            mLoadQueue[i]->mTime += deltaT;
        }
    }

    if (nlTaskManager::m_pInstance->mCurrentState == 0x10)
    {
        UpdateStadium(deltaT);
        StartLoadedScripts();

        float animTime[2];
        for (int i = 0; i < 2; i++)
        {
            animTime[i] = mCamera[i].GetAnimationTime();
            if (mCamera[i].m_pActiveCameraData != 0)
            {
                float overrun = mCamera[i].ManualUpdate(deltaT);
                if (i == 0 && overrun > 0.0f)
                {
                    mCameraOverrun = overrun;
                }
            }
        }
        DepthOfFieldManager::instance.m_fDistanceFromCamera = mCamera[0].GetFocalLength();

        for (int i = 0; i < 8; i++)
        {
            if (mPlaying[i] == 0)
            {
                continue;
            }
            int cameraIndex = mPlaying[i]->mRenderMode;
            if (cameraIndex == 2)
            {
                cameraIndex = 1;
            }
            mPlaying[i]->Update(deltaT);
            float duration = mCamera[cameraIndex].GetDuration();
            mPlaying[i]->UpdateTriggers(animTime[cameraIndex], mCamera[cameraIndex].GetAnimationTime(), duration);
            mCamera[cameraIndex].m_OffsetPos = mPlaying[i]->Offset();
        }

        int overlay = mOverlays[mOverlayMode]->Update(deltaT);
        if (mOverlayMode != overlay)
        {
            mOverlayMode = overlay;
            mOverlays[mOverlayMode]->Reset();
            mOverlays[mOverlayMode]->GetOverlayType();
        }
    }
}

void NisPlayer::Reset()
{
    if (!mActive)
    {
        return;
    }

    ResetPlayerEffects();
    mOverlayMode = 0;
    if (mPlayingNisCue != 0 && mStopNisCueOnReset)
    {
        StopSound(mPlayingNisCue, (void*)-1);
        mPlayingNisCue = 0;
    }
    if (mPreparedNisCue != 0)
    {
        StopSound(mPreparedNisCue, (void*)-1);
        mPreparedNisCue = 0;
    }

    for (int i = 0; i < 8; i++)
    {
        delete mPlaying[i];
        delete mLoaded[i];
        mPlaying[i] = 0;
        mLoaded[i] = 0;
        mLoadQueue[i] = 0;
        mAsyncStarted[i] = false;
    }

    mActive = false;
    mLoadingFromBack = false;
    mUsedFromFront = 0;
    mUsedFromBack = 0x70800;
    for (int i = 0; i < 2; i++)
    {
        mCamera[i].UnselectCameraAnimation();
    }
    cCameraManager::Remove(mCamera[0]);
    ClearNisAnimatedCharacters();
    gBlinkingEnabled = true;
    if (mSavedFogStart != -1.0f)
    {
        glx_SetFogStart(mSavedFogStart);
    }
    if (mSavedFogEnd != -1.0f)
    {
        glx_SetFogEnd(mSavedFogEnd);
    }
    mSavedFogStart = -1.0f;
    mSavedFogEnd = -1.0f;
    DepthOfFieldManager::instance.TurnOn();
    mCameraOverrun = 0.0f;
}

void NisPlayer::StartLoadedScripts()
{
    for (int i = 0; i < 8; i++)
    {
        if (mLoadQueue[i] != 0)
        {
            return;
        }
    }
    for (int i = 0; i < 8; i++)
    {
        if (mLoaded[i] != 0 && !mLoaded[i]->mScriptStarted)
        {
            mLoaded[i]->StartScript();
        }
    }
}

bool NisPlayer::IsReadyToPlay()
{
    if (mLoadedAssetMask != 0x1F)
    {
        return false;
    }
    for (int i = 0; i < 8; i++)
    {
        if (mLoadQueue[i] != 0)
        {
            return false;
        }
    }
    for (int i = 0; i < 8; i++)
    {
        if (mLoaded[i] != 0)
        {
            if (!mLoaded[i]->mScriptStarted)
            {
                mLoaded[i]->StartScript();
            }
            if (mLoaded[i]->IsLoading() == true)
            {
                return false;
            }
        }
    }
    if (mPreparedNisCue != 0 && GetSoundState(mPreparedNisCue, (void*)-1) == 2)
    {
        return false;
    }
    return true;
}

void NisPlayer::ClearLoadQueue()
{
    for (int i = 0; i < 8; i++)
    {
        mLoadQueue[i] = 0;
    }
}

void NisPlayer::Play()
{
    mUseViewMatrixOverride = false;
    mInitialCameraRotationCached = false;
    mActive = true;
    gBlinkingEnabled = true;
    if (mSavedFogStart != -1.0f)
    {
        glx_SetFogStart(mSavedFogStart);
    }
    if (mSavedFogEnd != -1.0f)
    {
        glx_SetFogEnd(mSavedFogEnd);
    }
    mSavedFogStart = -1.0f;
    mSavedFogEnd = -1.0f;
    DepthOfFieldManager::instance.TurnOn();

    for (int j = 0; j < 8; j++)
    {
        delete mPlaying[j];
        mPlaying[j] = mLoaded[j];
        mLoaded[j] = 0;
        if (mPlaying[j] != 0)
        {
            mPlaying[j]->ApplyLoadedAnimations();
            mPlaying[j]->AttachHeadImpostors();
        }
    }

    if (mRequestedFogStart != -1.0f)
    {
        mSavedFogStart = glx_GetFogStart();
        glx_SetFogStart(mRequestedFogStart);
    }
    if (mRequestedFogEnd != -1.0f)
    {
        mSavedFogEnd = glx_GetFogEnd();
        glx_SetFogEnd(mRequestedFogEnd);
    }
    if (mSuppressBlinking == true)
    {
        gBlinkingEnabled = false;
    }
    mSuppressBlinking = false;
    mRequestedFogStart = -1.0f;
    mRequestedFogEnd = -1.0f;
    if (mOverlayMode != 4)
    {
        mOverlayMode = 0;
    }

    if (mLoadingFromBack)
    {
        mLoadingFromBack = false;
        mUsedFromFront = 0;
    }
    else
    {
        mLoadingFromBack = true;
        mUsedFromBack = 0x70800;
    }
    SelectCameras();
    StartNisCue();
    ClearNisAnimatedCharacters();
}

void NisPlayer::HideAllActors() const
{
    RenderSnapshot& snapshot = ReplayManager::Instance()->GetMutableRenderSnapshot();
    for (int i = 0; i < 150; i++)
    {
        snapshot.GetPowerup(i).SetUnidentifiedVisible(false);
    }
    for (int i = 0; i < 10; i++)
    {
        snapshot.mCharacters[i].visible = false;
    }
    for (int i = 0; i < 15; i++)
    {
        snapshot.mHammers[i].mVisible = false;
    }
    for (int i = 0; i < 3; i++)
    {
        snapshot.mWindDebris[i].visible = false;
    }
    for (int i = 0; i < 8; i++)
    {
        snapshot.mDaisyFists[i].mVisible = false;
    }
    for (int i = 0; i < 6; i++)
    {
        snapshot.mBulletBills[i].mVisible = false;
    }
    for (int i = 0; i < 8; i++)
    {
        snapshot.mThwomps[i].mVisible = false;
    }
    snapshot.mYoshiEgg.mVisible = false;
    snapshot.mBall.mFlags.bits.visible = false;
    snapshot.mChainChomp.visible = false;
}

void NisPlayer::SetupNisPlayback()
{
    mCameraOverrun = 0.0f;
    ResetEffects();
    ResetPlayerEffects();
    g_pGame->mpWeatherManager->Stop(true);
    if (mOverlayMode != 4)
    {
        WorldDarkening::Instance().fn_801AF550();
    }
    if (cCameraManager::PeekCamera() != &mCamera[0])
    {
        cCameraManager::Remove(mCamera[0]);
        cCameraManager::PushCamera(&mCamera[0]);
    }
}

void NisPlayer::ResetPlayerEffects()
{
    for (int side = 0; side < 2; side++)
    {
        cTeam* team = g_pTeams[side];
        team->GetGoalie()->ResetEffects();
        for (int i = 0; i < 4; i++)
        {
            cFielder* fielder = team->GetFielder(i);
            fielder->ResetEffects();
            fielder->fn_8001EE74(1.0f, 0.0f, 1.0f);
            fielder->EndFrozenOrDazed();
            fielder->fn_800974B0();
        }
    }
}

static inline void PrintPlayingNisInfo(const NisPlayer& player)
{
    int line = 0;
    for (int i = 0; i < 8; i++)
    {
        if (player.mPlaying[i] != NULL)
        {
            nlScreenPrintf(0, line++, false, 4, "Mirrored: %s", player.mPlaying[i]->mMirrored ? "True" : "False");
            if (player.mPlaying[i]->mCamera != NULL && player.mPlaying[i]->mCamera->m_pActiveCameraData != NULL)
            {
                nlScreenPrintf(0, line++, false, 4, "Camera: %s", player.mPlaying[i]->mCamera->m_pActiveCameraData->m_szName);
            }
            nlScreenPrintf(0, line++, false, 4, "Name: %s", player.mPlaying[i]->Name());
        }
    }
}

void NisPlayer::Render(int pass) const
{
    nlTaskManager* taskManager = nlTaskManager::m_pInstance;
    unsigned long currentState = taskManager->mCurrentState;

    if (currentState != 0x10 || ((taskManager->mPreviousState == 0x10) && (currentState != 1)))
    {
        return;
    }

    HideAllActors();

    bool renderPass = false;
    if (HasSecondaryNis() && pass == 0)
    {
        renderPass = true;
    }
    for (int i = 0; i < 8; i++)
    {
        if (mPlaying[i] != NULL)
        {
            if (mPlaying[i]->mRenderMode == renderPass || mPlaying[i]->mRenderMode == 2)
            {
                mPlaying[i]->Render(renderPass);
            }
        }
    }
    if (mDisplayNisInfo && renderPass == 0)
    {
        PrintPlayingNisInfo(*this);
    }
    if (pass == 0 && HasSecondaryNis())
    {
        mOverlays[mOverlayMode]->Render();
    }
}

void NisPlayer::Load(char* buffer, unsigned int size, NisHeader& nisHeader)
{
    if (!mActive)
        return;

    for (int i = 0; i < 8; i++)
    {
        if (mLoaded[i] != 0)
            continue;

        if (nisHeader.buffer == 0)
        {
            for (int j = 0; j < 8; j++)
            {
                if (&nisHeader == mLoadQueue[j])
                {
                    mLoadQueue[j] = 0;
                    mAsyncStarted[j] = false;
                    break;
                }
            }
        }

        Nis* nis = new (nlMalloc(sizeof(Nis), 8, false))
            Nis(nisHeader, buffer, size);
        mLoaded[i] = nis;
        LoadTriggers(*mLoaded[i]);
        return;
    }
}

void NisPlayer::LoadTriggers(Nis& nis)
{
    BasicString<char, Detail::TempStringAllocator> name(nis.Name());
    for (int i = name.size() - 1; i >= 0; --i)
    {
        if (name[i] == '.')
        {
            name[i] = '\0';
            break;
        }
    }
    unsigned long nisHash = nlStringHash(name.c_str());
    if (!FunctionExists(nisHash))
    {
        for (int i = 0; i < name.size(); ++i)
        {
            if (name[i] == '_')
            {
                name.erase(name.begin(), name.begin() + i);
                char fallbackPrefix[] = "all";
                BasicString<char, Detail::TempStringAllocator> all("all");
                name.insert(name.begin(), fallbackPrefix, fallbackPrefix + sizeof(fallbackPrefix) - 1);
                break;
            }
        }
        nisHash = nlStringHash(name.c_str());
        if (!FunctionExists(nisHash))
        {
            return;
        }
    }
    mNisForTriggerLoading = &nis;
    CallFunction(nisHash);
    mNisForTriggerLoading = NULL;
}

#include "src/Game/NisPlayerLoading.cpp"

void NisPlayer::AsyncLoad(nlFile* file, void* buffer, unsigned int size, unsigned long param)
{
    if (file != NULL)
    {
        nlClose(file);
    }
    Instance()->Load((char*)buffer, size, *(NisHeader*)param);
}

void NisPlayer::RandomizeBeginPositions()
{
    for (int i = 0; i < 10; i++)
    {
        mBeginPositions[i].x = nlRandomf(-8.0f, 8.0f, GetPresentationRandomSeed(GetPresentation()));
        mBeginPositions[i].y = nlRandomf(-4.0f, 4.0f, GetPresentationRandomSeed(GetPresentation()));
        mBeginPositions[i].z = 0.0f;
    }
}

bool g_ForceDoubleBallTransition;

void NisPlayer::RegisterEventHandlers()
{
    UnidentifiedFindEvent<GoalScoredData>("GoalScored", -1)->Add(Function<GoalScoredData*>(BindMember(this, &NisPlayer::OnGoalScored)), 0, -1);
    UnidentifiedFindEvent<GoalieSaveData>("GoalieSave", -1)->Add(Function<GoalieSaveData*>(BindMember(this, &NisPlayer::OnGoalieSave)), 0, -1);
    UnidentifiedFindEvent<cPlayer>("MegaStrikeIntro", -1)->Add(Function<cPlayer*>(BindMember(this, &NisPlayer::OnMegaStrikeIntro)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("PauseGame", -1)->Add(Function<FnVoidVoid>(BindMember(this, &NisPlayer::OnPauseGame)), 0, -1);
}

void NisPlayer::OnGoalScored(GoalScoredData* goalScoredData)
{
    if (g_pGame == NULL)
    {
        return;
    }
    if (goalScoredData != NULL)
    {
        mGoalScorerCharIndex = GetCharacterIndex(goalScoredData->pLastTouch[goalScoredData->uTeamIndex]);
        mWinnerSide[NIS_GOAL_WINNER] = goalScoredData->uTeamIndex;
    }
}

void NisPlayer::OnGoalieSave(GoalieSaveData*)
{
}

void NisPlayer::OnMegaStrikeIntro(cPlayer* player)
{
    if (g_pGame == NULL)
    {
        return;
    }
    if (g_pGame->m_eGameState == 3)
    {
        return;
    }
    g_ForceDoubleBallTransition = false;
    if (player != NULL)
    {
        mMegaStrikeSide = player->m_pTeam->m_nSide;
        mGoalScorerCharIndex = GetCharacterIndex(player);
    }
}

void NisPlayer::OnPauseGame()
{
    StopNisCue();
}

inline bool NisPlayer::AllowsPIP(const char* name) const
{
    for (int i = 0; i < mNumDoNotShowPIPNames; i++)
    {
        if (nlStrICmp(name, mDoNotShowPIPNames[i]) == 0)
        {
            return false;
        }
    }
    return true;
}

bool NisPlayer::AllowsPIP()
{
    for (int i = 0; i < 8; i++)
    {
        if (mLoaded[i] != NULL && !AllowsPIP(mLoaded[i]->Name()))
        {
            return false;
        }
        if (mLoadQueue[i] != NULL && !AllowsPIP(mLoadQueue[i]->name))
        {
            return false;
        }
    }
    return true;
}

int NisPlayer::GetWinnerSide(NisWinnerType winnerType) const
{
    if (winnerType < NIS_NUM_WINNER_TYPES)
    {
        return mWinnerSide[winnerType];
    }
    return 0;
}

bool NisPlayer::IsMirrored(NisTarget target, const char* name, NisWinnerType winnerType) const
{
    for (int i = 0; i < mNumDoNotMirrorNames; i++)
    {
        if (nlStrICmp(name, mDoNotMirrorNames[i]) == 0)
        {
            return false;
        }
    }
    bool mirrored = true;
    if (strstr(name, "_goal_") == NULL && strstr(name, "goalie_") == NULL)
    {
        mirrored = false;
    }
    if (target == NIS_TARGET_SCORER || target == NIS_TARGET_WINNER_CAPTAIN || target == NIS_TARGET_WINNER_SIDEKICK || target == NIS_TARGET_LOSER_GOALIE)
    {
        if (GetWinnerSide(winnerType) == 0)
        {
            return mirrored;
        }
        return !mirrored;
    }
    if (target == NIS_TARGET_LOSER_CAPTAIN || target == NIS_TARGET_WINNER_GOALIE || target == NIS_TARGET_LOSER_SIDEKICK)
    {
        if (GetWinnerSide(winnerType) == 0)
        {
            return !mirrored;
        }
        return mirrored;
    }
    if (target == NIS_TARGET_MEGASTRIKE_CAPTAIN)
    {
        if (strstr(name, "_megastrike_") != NULL)
        {
            return false;
        }
        return mMegaStrikeSide == 0;
    }
    if (target == NIS_TARGET_MEGASTRIKE_DEFENDING_GOALIE)
    {
        return mMegaStrikeSide == 0;
    }
    if (strstr(name, "cup_win_home") != NULL)
    {
        return false;
    }
    if (strstr(name, "home") != NULL || strstr(name, "run_to_center") != NULL)
    {
        if (target == NIS_TARGET_AWAY_CAPTAIN)
        {
            return true;
        }
        if (target == NIS_TARGET_AWAY_SIDEKICK)
        {
            return true;
        }
        if (target == NIS_TARGET_NONE)
        {
            return true;
        }
    }
    return false;
}

void NisPlayer::ResetEffects()
{
    EmissionManager::Instance()->Destroy((unsigned long)this, 0);
    EmissionManager::Instance()->DestroyAll(0, true);
    EmissionManager::Instance()->DestroyAll(3, true);
}

void NisPlayer::SetExtraNameFilter(const char* filter)
{
    nlStrNCpy(mExtraNameFilter, filter, 128);
}

void NisPlayer::fn_8027E5D0()
{
}

void NisPlayer::ReleaseCachedNisBuffers()
{
    for (int i = 0; i < mDictSize; i++)
    {
        delete mDict[i].buffer;
        mDict[i].mUnidentified194 = false;
        mDict[i].buffer = 0;
        mDict[i].bufferSize = 0;
    }
}

bool NisPlayer::HasSecondaryNis() const
{
    for (int i = 0; i < 8; i++)
    {
        if (mPlaying[i] != 0 && mPlaying[i]->mRenderMode != 0)
        {
            return true;
        }
    }
    return false;
}

cAnimCamera* NisPlayer::GetSecondaryCamera()
{
    return &mCamera[1];
}

void NisPlayer::SwapCameras()
{
    for (int i = 0; i < 8; i++)
    {
        if (mPlaying[i] != NULL)
        {
            if (mPlaying[i]->mRenderMode == 0)
            {
                mPlaying[i]->mRenderMode = 1;
            }
            else if (mPlaying[i]->mRenderMode == 1)
            {
                mPlaying[i]->mRenderMode = 0;
            }
        }
    }

    cCameraManager::Remove(mCamera[0]);
    cAnimCamera camera = mCamera[0];
    mCamera[0] = mCamera[1];
    mCamera[1] = camera;
    cCameraManager::PushCamera(&mCamera[0]);
}

void NisPlayer::PreserveNisCueOnReset()
{
    mStopNisCueOnReset = false;
}

void NisPlayer::StopNisCue()
{
    mStopNisCueOnReset = true;
    if (mPlayingNisCue != 0)
    {
        StopSound(mPlayingNisCue, (void*)-1);
        mPlayingNisCue = 0;
    }
}

void NisPlayer::ReleaseNisCue()
{
    mStopNisCueOnReset = true;
    if (mPlayingNisCue != 0)
    {
        SetSoundCallbackEnabled(mPlayingNisCue, (void*)-1, 1);
        mPlayingNisCue = 0;
    }
}

void NisPlayer::PrepareNisCue(unsigned long cue)
{
    StopSound(cue, (void*)-1);
    if (PrepareTrackedSound(19, cue, 0, "Nis Cue", (void*)-1, true))
    {
        mPreparedNisCue = cue;
    }
}

void NisPlayer::StartNisCue()
{
    if (mPreparedNisCue != 0)
    {
        mPlayingNisCue = mPreparedNisCue;
        mPreparedNisCue = 0;
        StartTrackedSound(mPlayingNisCue, (void*)-1);
    }
}

void NisPlayer::EnableWorldDarkening(bool fade)
{
    mOverlayMode = 4;
    if (fade)
    {
        WorldDarkening::Instance().Fade(g_NisWorldDarkeningAmount, g_NisWorldDarkeningFadeTime);
    }
}

void NisPlayer::FadeWorldDarkening(float duration)
{
    if (mOverlayMode == 4)
    {
        WorldDarkening::Instance().Fade(g_NisWorldDarkeningAmount, duration);
    }
}

void NisPlayer::ResetToPIPOverlay()
{
    WorldDarkening::Instance().fn_801AF550();
    mOverlayMode = 0;
    ClearSecondaryNis();
}

void NisPlayer::ClearSecondaryNis()
{
    for (int i = 0; i < 8; i++)
    {
        if (mPlaying[i] != 0 && mPlaying[i]->mRenderMode != 0)
        {
            delete mPlaying[i];
            mPlaying[i] = 0;
        }
    }
}

void OnNisTriggersLoaded(void* data, unsigned long size, void* userData)
{
    NisPlayer* player = (NisPlayer*)userData;
    if (size != 0)
    {
        sTriggerByteCode = data;
        player->LoadByteCode(data);
        player->mLoadedAssetMask |= 1;
    }
}

void OnNisAnimProxyLoaded(void* data, unsigned long size, void* userData)
{
    NisPlayer* player = (NisPlayer*)userData;
    if (size != 0)
    {
        player->mAnimProxyByteCode = data;
        player->mLoadedAssetMask |= 2;
    }
}

void OnNisDoNotMirrorListLoaded(void* data, unsigned long size, void* userData)
{
    NisPlayer* player = (NisPlayer*)userData;
    if (size != 0)
    {
        player->ParseDoNotMirrorList((char*)data, size);
        player->mLoadedAssetMask |= 4;
    }
}

void OnNisDoNotShowPIPListLoaded(void* data, unsigned long size, void* userData)
{
    NisPlayer* player = (NisPlayer*)userData;
    if (size != 0)
    {
        player->ParseDoNotShowPIPList((char*)data, size);
        player->mLoadedAssetMask |= 8;
    }
}

void OnNisDictionaryLoaded(void* data, unsigned long size, void* userData)
{
    NisPlayer* player = (NisPlayer*)userData;
    if (size != 0)
    {
        player->ParseDictionary((char*)data);
        player->mLoadedAssetMask |= 16;
    }
}

void ApplyNisTransitionTeamLogo(glModel* model)
{
    static const unsigned long defaultTransitionLogo = glGetTexture("transitions/waluigi_logo");
    for (glModelPacket* packet = model->packets; packet < model->packets + model->numPackets; packet++)
    {
        unsigned long diffuseTexture = glGetMaterialUnsignedParameter(packet, gDiffuseTextureSemantic);
        if (NisPlayer::Instance()->HasTeamLogoPacket(packet))
        {
            glSetMaterialTextureParameter(packet, gDiffuseTextureSemantic, NisPlayer::Instance()->mTeamLogoTexture);
            unsigned long textureIndex = NisPlayer::Instance()->mTeamLogoTextureIndex;
            glSetMaterialTextureIndexParameter(packet, gDiffuseTextureSemantic, &textureIndex);
        }
        if (diffuseTexture == defaultTransitionLogo)
        {
            glSetMaterialTextureParameter(packet, gDiffuseTextureSemantic, NisPlayer::Instance()->mTeamLogoTexture);
            unsigned long textureIndex = NisPlayer::Instance()->mTeamLogoTextureIndex;
            glSetMaterialTextureIndexParameter(packet, gDiffuseTextureSemantic, &textureIndex);
            NisPlayer::Instance()->AddTeamLogoPacket(packet);
        }
    }
}

void NisPlayer::SetTeamLogo(NisTarget target, NisWinnerType winnerType)
{
    const char* filter = NisPlayer::Instance()->GetTargetFilter(target, winnerType);
    char textureName[64];
    nlSNPrintf(textureName, sizeof(textureName), "%s/%s_logo", filter, filter);
    mTeamLogoTexture = glGetTexture(textureName);
    mTeamLogoTextureIndex = glGetTextureManager()->GetTextureIndex(mTeamLogoTexture);
}

#include "Game/Render/NisPlayerOverlay.inl"
