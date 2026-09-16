#include "Game/SH/SHStadiumSelect.h"
#include "NL/nlBasicString.inl"
#include "NL/nlFunction.inl"
#include "NL/nlBindMember.h"
#include "Game/EventRegistry.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/feScrollText.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feMusic.h"
#include "Game/FE/FEAudio.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/FE/feBackButton.h"
#include "Game/FE/feInput.h"
#include "NL/nlFunctionMemory.h"
#include "NL/nlBind.h"
#include "Game/GameInfo.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/feDPD.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/DB/SaveLoad.h"
#include "Game/GameSceneManager.h"
#include "Game/Render/Presentation.h"
#include "Game/SH/SHNavigation.h"
#include "Game/SH/SHOnlineInvitePlayers.h"
#include "Game/Camera/animcam.h"
#include "Game/FriendManager.h"

#include "Game/FE/tlImageInstance.h"
#include "Game/FE/feTextureResource.h"
#include "Game/Sys/movie.h"
#include "NL/gl/glState.h"
#include "NL/gl/glStruct.h"

#include "NL/nlConfig.h"
#include "NL/nlstring_tmpl.h"
#include "NL/nlLocalizationLookup.h"
#include "NL/nlFormat.h"
#include <string.h>

extern "C" unsigned char SCGetSoundMode();
extern "C" int THPSimpleSetVolume(long volume, long time);

extern Config gMovieConfig;
static const int STADIUM_ORDER[17] = {
    13, 11, 15, 16, 7, 14, 3, 4, 9, 5, 0, 1, 2, 6, 10, 8, 12,
};

inline void MoviePlayerControl::Stop()
{
    if (mMovieStarted && MovieStop())
    {
        if (mMovieInstance)
        {
            mMovieInstance->m_bVisible = false;
        }
        mMovieStarted = false;
    }
}

bool MoviePlayerControl::CheckMoviePlayerAbort()
{
    bool b = g_pFEInput->JustPressed(FE_ALL_PADS, 31, true, 0) || g_pFEInput->JustPressed(FE_ALL_PADS, 30, true, 0);
    return b;
}

StadiumSelectScene::StadiumSelectScene()
    : m_pTicker(0)
    , mControlsInitialized(false)
    , mPointerOverPlayButton(false)
    , mProceeding(false)
    , mPageControls(true)
    , mPlayButtonInstance(0)
{
    if (GameInfoManager::Instance()->UseAltRules())
    {
        mBackButton.SetBackScene(27);
    }
    else
    {
        mBackButton.SetBackScene(4);
    }
    SortStadiums();
    UnidentifiedFindEvent<UnidentifiedEventNoData>("HBMHide", -1)->Add(Function<FnVoidVoid>(BindMember(this, &StadiumSelectScene::OnHBMHide)), (unsigned int)&mHBMHideConnection, -1);
}

StadiumSelectScene::~StadiumSelectScene()
{
    if (m_pTicker != 0)
    {
        delete m_pTicker;
    }
    if (mPlayingStadiumIndex != -1)
    {
        FEAudio::StopAnimAudioEvent(GetStadiumUnknown0x28(mStadiumOrder[mPlayingStadiumIndex]), this);
    }
    mMoviePlayer.Stop();
}

inline void MoviePlayerControl::Initialize(const char* filename)
{
    mMovieInstance = 0;
    nlStrNCpy<char>(mMovieFilename, filename, sizeof(mMovieFilename));
    mSwappedTexture = false;
    mMovieStarted = false;
    mWithSound = true;
    mLoopMovie = true;
}

void StadiumSelectScene::SceneCreated()
{
    mMoviePlayer.Initialize("art/movies/stadiumtest.thp");

    mStadiumNames = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<>(mPresentation->m_currentSlide, "Layer", "stadium_names");
    mLockedIcon = FEFinder<TLInstance, TLAT_UNKNOWN>::Find<>(mPresentation->m_currentSlide, "Layer", "locked");
    mLockedIcon->m_bVisible = false;

    TLTextInstance* tickerText = FEFinder<TLTextInstance, TLAT_TEXT>::Find<>(mPresentation->m_currentSlide, "Layer", "TickerText");
    if (tickerText != 0)
    {
        glGetScreenInfo();
        m_pTicker = new (8, false) FEScrollText(0);
        m_pTicker->ApplyNewTextInstancePointer(tickerText, 0, 242, 0);
        m_pTicker->SetDisplayMessage(GetStadiumTickerStringID(0));
    }

    mPreviewState = 1;
    for (int i = 0; i < 17; ++i)
    {
        if (IsStadiumEnabled(mStadiumOrder[i]) && IsStadiumUnlocked(mStadiumOrder[i]))
        {
            mStadiumIndex = i;
            mPlayingStadiumIndex = -1;
            break;
        }
    }

    TLComponentInstance* backButton = 0;
    TLComponentInstance* plusButton = 0;
    TLComponentInstance* minusButton = 0;
    SHNavigation* navigation = GetNavigationScene();
    if (navigation != 0)
    {
        if (GameInfoManager::Instance()->UseAltRules())
        {
            navigation->SetButtons(0x2F, true);
            mPlayButtonInstance = navigation->GetButton(0x20);
        }
        else
        {
            navigation->SetButtons(0x1F, true);
            mPlayButtonInstance = navigation->GetButton(0x10);
        }
        backButton = navigation->GetButton(4);
        plusButton = navigation->GetButton(1);
        minusButton = navigation->GetButton(2);
        navigation->HideButtons();
    }
    mBackButton.SetButtonInstance(backButton);
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }
    mPageControls.SetPlusButton(plusButton);
    mPageControls.SetMinusButton(minusButton);
    FEMusic::StopStream();
    if (GameInfoManager::Instance()->UseAltRules())
    {
        Presentation::GetInstance()->Call("FromUnrankedMatchToStadiumSelect");
    }
    else
    {
        Presentation::GetInstance()->Call("FromGrudgeMatchToStadiumSelect");
    }
}

void StadiumSelectScene::SortStadiums()
{
    int unlockedCount = 0;
    int lockedCount = 0;
    int lockedStadiums[17];
    for (int i = 0; i < 17; ++i)
    {
        if (IsStadiumUnlocked(STADIUM_ORDER[i]))
        {
            mStadiumOrder[unlockedCount] = STADIUM_ORDER[i];
            ++unlockedCount;
        }
        else
        {
            lockedStadiums[lockedCount] = STADIUM_ORDER[i];
            ++lockedCount;
        }
    }
    int j = 0;
    for (int i = unlockedCount; i < 17; ++i)
    {
        mStadiumOrder[i] = lockedStadiums[j];
        ++j;
    }
}

static const unsigned short stadiumCount[] = { '1', '7', 0 };

static void UpdateStadiumLabel(StadiumSelectScene* scene);

void StadiumSelectScene::Update(float deltaTime)
{
    switch (mPreviewState)
    {
    case 0:
        break;
    case 1:
        if (!Presentation::GetInstance()->mCameraFinished)
        {
            return;
        }
        mMoviePlayer.Start(GetStadiumMoviePath(mStadiumOrder[mStadiumIndex]));
        mPreviewState = 4;
        mPlayingStadiumIndex = mStadiumIndex;
        if (m_pTicker != 0)
        {
            m_pTicker->SetDisplayMessage(GetStadiumTickerStringID(mStadiumOrder[mPlayingStadiumIndex]));
        }
        if (mStadiumNames != 0)
        {
            mStadiumNames->m_bVisible = true;
            mStadiumNames->SetActiveSlide(GetStadiumName(mStadiumOrder[mPlayingStadiumIndex]), true, false);
            UpdateStadiumLabel(this);
        }
        mPreviewState = 2;
        break;
    case 2:
    {
        cAnimCamera* camera = (cAnimCamera*)cCameraManager::PeekCamera();
        if (camera != 0 && camera->GetUnidentifiedAnimationTime() >= 1.0f)
        {
            mPreviewState = 4;
        }
        break;
    }
    case 3:
        mMoviePlayer.Start(GetStadiumMoviePath(mStadiumOrder[mStadiumIndex]));
        mPreviewState = 4;
        mPlayingStadiumIndex = mStadiumIndex;
        if (m_pTicker != 0)
        {
            m_pTicker->SetDisplayMessage(GetStadiumTickerStringID(mStadiumOrder[mPlayingStadiumIndex]));
        }
        if (mStadiumNames != 0)
        {
            mStadiumNames->m_bVisible = true;
            mStadiumNames->SetActiveSlide(GetStadiumName(mStadiumOrder[mPlayingStadiumIndex]), true, false);
            UpdateStadiumLabel(this);
        }
        break;
    case 4:
        if (mPlayingStadiumIndex != mStadiumIndex)
        {
            mPreviewState = 5;
        }
        break;
    case 5:
        mMoviePlayer.Stop();
        mPreviewState = 6;
        break;
    case 6:
        mPreviewState = 3;
        break;
    }

    mMoviePlayer.Update(deltaTime);
    BaseSceneHandler::Update(deltaTime);
    if (!mControlsInitialized)
    {
        TLInstance* titles = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<>(mPresentation->m_currentSlide, "Layer", "SCREEN_TITLES");
        TLSlide* slide = ((TLComponentInstance*)(titles == 0 ? &UnidentifiedTLComponentDefault::sInstance : titles))->GetActiveSlide();
        if (slide->GetCurrentTime() < slide->GetStartTime() + slide->GetDuration())
        {
            return;
        }
        for (int i = 0; i < 4; ++i)
        {
            TLComponentInstance* pointer = GetPointerInstance(i);
            if (i == gFEControllerIndex)
            {
                pointer->SetActiveSlide("cursor", true, false);
            }
            else
            {
                pointer->SetActiveSlide("waiting", true, false);
            }
        }
        SHNavigation* navigation = GetNavigationScene();
        if (navigation != 0)
        {
            navigation->RestoreButtonVisibility();
            SetBreadcrumbs(17, mStadiumIndex);
        }
        InitializeButtons();
        mControlsInitialized = true;
    }

    if (mPreviewState > 2)
    {
        TLSlide* activeSlide = mStadiumNames->GetActiveSlide();
        bool finished = true;
        if (activeSlide != 0)
        {
            TLSlide* slide = mStadiumNames->GetActiveSlide();
            float endTime = slide->GetStartTime() + slide->GetDuration();
            finished = mStadiumNames->GetActiveSlide()->GetCurrentTime() >= endTime;
        }
        if (finished)
        {
            unsigned char valid = 1;
            FEPointerEvent event;
            int pad = gFEControllerIndex;
            event.mIndex = pad;
            event.mPosition = GetPointerPosition(pad, &valid);
            event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 30, true, 0);
            if (mBackButton.UpdateBackButton(event, deltaTime))
            {
                if (GameInfoManager::Instance()->UseAltRules())
                {
                    FEAudio::PlayAnimAudioEvent(0xC385EFFB, 0, 0, true);
                    FEAudio::PlayAnimAudioEvent(0xBF2ED62D, 0, 0, true);
                    FEMusic::StartStreamIfDifferent(8);
                    Presentation::GetInstance()->Call("FromStadiumSelectToUnrankedMatch");
                }
                else
                {
                    FEAudio::PlayAnimAudioEvent(0xBF2ED62D, 0, 0, true);
                    FEMusic::StartStreamIfDifferent(2);
                    Presentation::GetInstance()->Call("FromStadiumSelectToGrudgeMatch");
                }
                mMoviePlayer.Stop();
                mBackButton.Disable();
                return;
            }
            mPageControls.Update(event, deltaTime);
            mPlayButton.HandlePointerEvent(&event);

            bool changed = false;
            bool previous = mPageControls.mPointerPressed[1] || mPageControls.mPadPressed[1];
            if (previous)
            {
                FEAudio::PlayAnimAudioEvent(0x375C885A, 0, 0, true);
                mStadiumIndex = (mStadiumIndex + 16) % 17;
                SetBreadcrumbs(17, mStadiumIndex);
                changed = true;
            }
            else
            {
                bool next = mPageControls.mPointerPressed[0] || mPageControls.mPadPressed[0];
                if (next)
                {
                    FEAudio::PlayAnimAudioEvent(0x375C885A, 0, 0, true);
                    mStadiumIndex = (mStadiumIndex + 1) % 17;
                    SetBreadcrumbs(17, mStadiumIndex);
                    changed = true;
                }
            }
            if (changed)
            {
                if (IsStadiumUnlocked(mStadiumOrder[mStadiumIndex]))
                {
                    mPlayButton.Enable();
                    mPlayButtonInstance->m_bVisible = true;
                    mLockedIcon->m_bVisible = false;
                }
                else
                {
                    mPlayButton.Disable();
                    mPlayButtonInstance->m_bVisible = false;
                    mPlayButtonInstance->SetActiveSlide("off", true, false);
                    mLockedIcon->m_bVisible = true;
                }
            }

            if (GameInfoManager::Instance()->UseAltRules()
                && !GameSceneManager::Instance()->IsOnStack((SceneList)10)
                && g_pFriendManager->FindHostInvitation())
            {
                FriendManager* manager = g_pFriendManager;
                manager->mReturnScene = 5;
                manager->mPreviousRankedMode = 0;
                Presentation::GetInstance()->Call("FromStadiumSelectToUnrankedMatch");
                GameSceneManager::Instance()->Push(SCENE_ONLINE_INVITE_RESPONSE, SCREEN_FORWARD, true);
                return;
            }
        }
    }

    for (int i = 0; i < 4; ++i)
    {
        TLComponentInstance* pointer = GetPointerInstance(i);
        if (i != gFEControllerIndex || mProceeding == true)
        {
            pointer->SetActiveSlide("waiting", true, false);
        }
        else if (mPointerOverPlayButton || mPageControls.mPointerInside[1]
            || mPageControls.mPointerInside[0] || mBackButton.mPointerInside[i])
        {
            pointer->SetActiveSlide("A", true, false);
        }
        else
        {
            pointer->SetActiveSlide("cursor", true, false);
        }
    }
    if (m_pTicker != 0)
    {
        m_pTicker->Update(deltaTime);
    }
}

static void UpdateStadiumLabel(StadiumSelectScene* scene)
{
    unsigned short number[4];
    nlSNPrintf(number, 4, (const unsigned short*)L"%d", scene->mStadiumIndex + 1);
    WideBasicString text(Format(WideBasicString(LookupLocString("X_OF_X")), number, stadiumCount));
    nlStrNCpy(scene->mStadiumCountText, text.c_str(), 16);
    FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(scene->mStadiumNames->GetActiveSlide(), "quantity")->SetString(scene->mStadiumCountText);
}

void StadiumSelectScene::InitializeButtons()
{
    typedef Detail::MemFunImpl<void, void (StadiumSelectScene::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, StadiumSelectScene*, Placeholder<0>, Placeholder<1> > PointerBinding;

    if (GameInfoManager::Instance()->UseAltRules())
    {
        SetDoneButtonBounds(&mPlayButton, mPlayButtonInstance, 0);
    }
    else
    {
        SetPlayButtonBounds(&mPlayButton, mPlayButtonInstance);
    }

    Function2<void, int, void*> callback = PointerBinding(MemFun(&StadiumSelectScene::OnPointerEnter), this, Placeholder<0>(), Placeholder<1>());
    mPlayButton.SetPointerEnterCallback(callback);
    callback = PointerBinding(MemFun(&StadiumSelectScene::OnPointerLeave), this, Placeholder<0>(), Placeholder<1>());
    mPlayButton.SetPointerLeaveCallback(callback);
    Function2<void, int, void*> pressCallback = PointerBinding(MemFun(&StadiumSelectScene::OnSelectStadium), this, Placeholder<0>(), Placeholder<1>());
    mPlayButton.SetPointerPressCallback(pressCallback);
}

void StadiumSelectScene::OnPointerEnter(int index, void*)
{
    mPlayButtonInstance->SetActiveSlide("over", true, false);
    mPlayButton.SetPointerState(1, index);
    FEAudio::PlayAnimAudioEvent(0xAA73EF34, 0, 0, true);
    mPointerOverPlayButton = true;
}

void StadiumSelectScene::OnPointerLeave(int index, void*)
{
    mPlayButtonInstance->SetActiveSlide("off", true, false);
    mPlayButton.SetPointerState(0, index);
    mPointerOverPlayButton = false;
}

void StadiumSelectScene::OnSelectStadium(int, void*)
{
    if (IsStadiumUnlocked(mStadiumOrder[mStadiumIndex]))
    {
        if (GameInfoManager::Instance()->UseAltRules())
        {
            FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, true);
            FEAudio::PlayAnimAudioEvent(0xBF2ED62D, 0, 0, true);
            Presentation::GetInstance()->Call("FromStadiumSelectToUnrankedMatch");
            GameInfoManager::Instance()->SetStadium(mStadiumOrder[mStadiumIndex]);
            SHOnlineInvitePlayers* scene = (SHOnlineInvitePlayers*)GameSceneManager::Instance()->Push((SceneList)44, SCREEN_NOTHING, true);
            scene->mIsHost = true;
            scene->mStartFriendServer = true;
        }
        else
        {
            FEAudio::PlayAnimAudioEvent(0x6E5C794C, 0, 0, true);
            for (int i = 0; i < 4; ++i)
            {
                GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
            }
            GameSceneManager::Instance()->PushLoadingScene(true);
            if (!GameInfoManager::Instance()->IsOnline())
            {
                SaveLoad::StartSave(false);
            }
            GameInfoManager::Instance()->SetStadium(mStadiumOrder[mStadiumIndex]);
            GetNavigationScene()->SetButtons(0, true);
        }
        mBackButton.Disable();
        mPlayButton.Disable();
        mProceeding = true;
    }
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }
}

void StadiumSelectScene::OnHBMHide()
{
    mMoviePlayer.mMovieStarted = false;
    mPreviewState = 3;
}

void MoviePlayerControl::Start(const char* filename)
{
    if (!mMovieStarted)
    {
        nlStrNCpy<char>(mMovieFilename, filename, sizeof(mMovieFilename));
        mMovieStarted = MovieStart(mMovieFilename, mWithSound, mLoopMovie, SCGetSoundMode() == 0);
        fn_80370E90(false);

        const char* streamName;
        if (strstr(mMovieFilename, "nlg"))
        {
            streamName = "FE_Eggman_Movie";
        }
        else
        {
            streamName = "FE_Intro_Movie";
        }
        char key[64];
        nlSNPrintf(key, sizeof(key), "%s/Volume", streamName);
        float volume = (float)GetConfigInt(gMovieConfig, key, 100) / 100.0f;
        nlSNPrintf(key, sizeof(key), "%s/FadeIn", streamName);
        int fadeIn = GetConfigInt(gMovieConfig, key, 500);
        THPSimpleSetVolume(0, 0);
        THPSimpleSetVolume((int)(127.0f * volume), fadeIn);
        if (mMovieInstance)
        {
            mMovieInstance->m_bVisible = true;
        }
        mEndFrameCount = 0;
    }
}

const char* gMovieTextureName = "movie";

void MoviePlayerControl::Update(float)
{
    if (fn_803713C4())
    {
        if (!mMovieStarted)
        {
            Stop();
        }
        else
        {
            if (!mSwappedTexture)
            {
                unsigned long texture = glGetTexture(gMovieTextureName);
                if (mMovieInstance)
                {
                    mMovieInstance->m_pTextureResource->SetTextureHandle(texture);
                }
                mSwappedTexture = true;
            }
            if (fn_803713CC())
            {
                ++mEndFrameCount;
                fn_803713D4();
                if (mEndFrameCount >= 5)
                {
                    Stop();
                }
            }
            else
            {
                mEndFrameCount = 0;
            }
        }
    }
}

void FEBackButton::SetBackScene(int value)
{
    mBackScene = value;
}
