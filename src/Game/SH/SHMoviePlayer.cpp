#include <revolution/sc_fwd.h>
#include <revolution/thp_fwd.h>
#include "string.h"
#include "NL/nlDLListContainer.inl"
#include "Game/SH/SHMoviePlayer.h"
#include "NL/nlFunction.inl"
#include "Game/BasicStadium.h"
#include "Game/EventRegistry.h"
#include "Game/FE/feInlineHasher.h"
#include "Game/FE/feFinder_impl.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feTextureResource.h"
#include "Game/GameInfo.h"
#include "Game/GameSceneManager.h"
#include "Game/Render/FrontEndPresentation.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/Sys/movie.h"
#include "NL/gl/gl.h"
#include "NL/gl/glPlat.h"
#include "NL/gl/glState.h"
#include "NL/nlConfig.h"
#include "NL/nlString.h"
#include "Game/Task/GameRenderTask.h"
#include "Game/main.h"

Config gMovieConfig(Config::ALLOCATE_HIGH, 0x2800, 0x400);

MoviePlayerScene::MoviePlayerScene()
    : mNextScene(SCENE_INVALID)
    , mSwappedTexture(false)
    , mMovieStarted(false)
    , mMovieInstance(0)
    , mWithSound(false)
    , mLoopMovie(false)
    , mPushWithPop(true)
{
    typedef Detail::MemFunImpl<void, void (MoviePlayerScene::*)()> MemFunImpl_MoviePlayerScene_v;
    typedef BindExp1<void, MemFunImpl_MoviePlayerScene_v, MoviePlayerScene*> BindExp1_MoviePlayerScene_v;
    mMovieFilename[0] = 0;
    if (GameSceneManager::s_pInstance)
    {
        mGameSceneManager = GameSceneManager::s_pInstance;
    }
    else
    {
        mGameSceneManager = g_pOverlayManager;
    }
    UnidentifiedFindEvent<UnidentifiedEventNoData>("HBMHide", -1)->Add(Function<FnVoidVoid>(BindExp1_MoviePlayerScene_v(MemFun(&MoviePlayerScene::OnHBMHide), this)), (unsigned int)&mHBMHideConnection, -1);
}

MoviePlayerScene::~MoviePlayerScene()
{
    if (GameInfoManager::Instance()->mIsInStrikers101Mode)
    {
        g_bRenderWorld = true;
    }
    if (g_pFEInput->HasInputLock(this))
    {
        g_pFEInput->PopExclusiveInputLock(this);
    }
}

void MoviePlayerScene::SceneCreated()
{
    OverrideMovieDimensions();
}

void MoviePlayerScene::SetMovieDetails(const char* filename, bool withsound, bool loopmovie)
{
    nlStrNCpy<char>(mMovieFilename, filename, 0x80);
    mMovieFilename[127] = 0;
    mWithSound = withsound;
    mLoopMovie = loopmovie;
    mMovieStarted = false;
}

void MoviePlayerScene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    if (g_e3_Build)
    {
        OnMoviePlaybackEnded();
        return;
    }
    if (!mMovieStarted)
    {
        mMovieStarted = MovieStart(mMovieFilename, false, mLoopMovie, SCGetSoundMode() == 0);
        SetSyncedDecode(true);

        const char* streamName;
        if (strstr(mMovieFilename, "nlg"))
        {
            streamName = "FE_Eggman_Movie";
        }
        else
        {
            streamName = "FE_Intro_Movie";
        }
        char configKey[64];
        nlSNPrintf(configKey, 64, "%s/Volume", streamName);
        float volume = (float)GetConfigInt(gMovieConfig, configKey, 100) / 100.0f;
        nlSNPrintf(configKey, 64, "%s/FadeIn", streamName);
        int fadeIn = GetConfigInt(gMovieConfig, configKey, 500);
        THPSimpleSetVolume(0, 0);
        THPSimpleSetVolume((int)(127.0f * volume), fadeIn);
        if (GameInfoManager::Instance()->mIsInStrikers101Mode)
        {
            g_bRenderWorld = false;
        }
    }
    if (!mMovieStarted || CheckMoviePlayerAbort())
    {
        MovieStop();
        OnMoviePlaybackEnded();
        mMovieStarted = false;
        return;
    }
    if (!mSwappedTexture)
    {
        mMovieInstance = FEFinder<TLImageInstance, 2>::Find<TLSlide>(
            mPresentation->m_currentSlide,
            "Layer",
            "movie");
        unsigned long movieHandle = glGetTexture("movie");
        mMovieInstance->m_pTextureResource->SetTextureHandle(movieHandle);
        mSwappedTexture = true;
    }
    if (IsMovieFinished())
    {
        MovieStop();
        OnMoviePlaybackEnded();
    }
}

bool MoviePlayerScene::CheckMoviePlayerAbort()
{
    return g_pFEInput->JustPressed(FE_ALL_PADS, 0x1E, true, 0);
}

void MoviePlayerScene::OnMoviePlaybackEnded()
{
    if (mPushWithPop)
    {
        mGameSceneManager->Push(mNextScene, SCREEN_NOTHING, true);
        PlayScreenForwardSFX();
    }
    else
    {
        mGameSceneManager->Pop();
        PlayScreenBackSFX();
    }
}

void MoviePlayerScene::PlayScreenForwardSFX()
{
}

void MoviePlayerScene::PlayScreenBackSFX()
{
}

void MoviePlayerScene::OverrideMovieDimensions()
{
}

void MoviePlayerScene::OnHBMHide()
{
    OnMoviePlaybackEnded();
    mMovieStarted = false;
}

void LessonMoviePlayerScene::SceneCreated()
{
    OverrideMovieDimensions();
    mButtonComponent.mButtonInstance = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(
        mPresentation->m_currentSlide,
        "Layer",
        "buttons");
    mButtonComponent.SetState(ButtonComponent::BS_B_ONLY);
}

bool LessonMoviePlayerScene::CheckMoviePlayerAbort()
{
    return g_pFEInput->JustPressed(FE_ALL_PADS, 0x1F, true, 0);
}

void LessonMoviePlayerScene::Update(float fDeltaT)
{
    MoviePlayerScene::Update(fDeltaT);
    if (mMovieStarted)
    {
        mButtonComponent.CentreButtons();
        return;
    }
    if (mButtonComponent.mButtonInstance != 0)
    {
        mButtonComponent.mButtonInstance->m_bVisible = false;
    }
    glDiscardFrame(2);
}

void NLGLogoMovieScene::OverrideMovieDimensions()
{
}

IntroMovieScene::IntroMovieScene()
    : mUnidentifiedB4(0.0f)
    , mMovieFinished(false)
    , mTransitionPending(false)
{
    if (glx_GetVideoMode() == 1)
    {
        SetMovieDetails("art/movies/introtest_pal.thp", true, false);
    }
    else
    {
        SetMovieDetails("art/movies/introtest.thp", true, false);
    }
    mNextScene = SCENE_TITLE;
    ResetMoviePlayer();
}

void IntroMovieScene::ResetMoviePlayer()
{
    if (mMovieStarted)
    {
        MovieStop();
        mMovieStarted = false;
    }
    BasicStadium* pStadium = BasicStadium::GetCurrentStadium();
    pStadium->m_bRenderingEnabled = false;
    mUnidentifiedB4 = 0.0f;
    mMovieFinished = false;
    mTransitionPending = false;
}

void IntroMovieScene::OnMoviePlaybackEnded()
{
    GameSceneManager::s_pInstance->Push(mNextScene, SCREEN_NOTHING, true);
    BasicStadium* pStadium = BasicStadium::GetCurrentStadium();
    pStadium->m_bRenderingEnabled = true;
    SetSyncedDecode(false);
}

void IntroMovieScene::SceneCreated()
{
    OverrideMovieDimensions();
    if (IsWidescreen())
    {
        mPresentation->SetActiveSlide("16:9", true);
    }
}

void IntroMovieScene::Update(float fDeltaT)
{
    int frame = glx_GetVideoMode() == 1 ? 0x9E5 : 0xBE0;
    if (mMovieStarted)
    {
        if (!mMovieFinished)
        {
            if ((int)GetMovieFrame() >= frame)
            {
                mMovieFinished = true;
                FrontEndPresentation::GetInstance()->Call("TransitionFromMovieToTitleScreen");
                mTransitionPending = true;
                BasicStadium::GetCurrentStadium()->m_bRenderingEnabled = true;
                SetSyncedDecode(false);
            }
        }
        else if (mTransitionPending)
        {
            mTransitionPending = false;
            if (IsWidescreen())
            {
                mPresentation->SetActiveSlide("transition16:9", true);
            }
            else
            {
                mPresentation->SetActiveSlide("transition", true);
            }
        }
    }
    MoviePlayerScene::Update(fDeltaT);
}
