#ifndef GAME_SH_SHMOVIEPLAYER_H
#define GAME_SH_SHMOVIEPLAYER_H

#include "Game/EventConnection.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/BaseSceneHandler.h"
#include "Game/FE/feButtonComponent.h"
#include "Game/Render/RLViewLayers.h"

class TLImageInstance;

class MoviePlayerScene : public BaseSceneHandler
{
public:
    MoviePlayerScene();
    virtual ~MoviePlayerScene();
    virtual void SceneCreated();
    void SetMovieDetails(const char* filename, bool withsound, bool loopmovie);
    virtual void Update(float fDeltaT);
    virtual bool CheckMoviePlayerAbort();
    virtual void PlayScreenForwardSFX();
    virtual void PlayScreenBackSFX();
    virtual void OverrideMovieDimensions();
    virtual void MoviePlayerVirtual3C();
    void OnHBMHide();

    /* 0x01C */ SceneList mNextScene;
    /* 0x020 */ bool mSwappedTexture;
    /* 0x021 */ bool mMovieStarted;
    /* 0x024 */ TLImageInstance* mMovieInstance;
    /* 0x028 */ char mMovieFilename[128];
    /* 0x0A8 */ bool mWithSound;
    /* 0x0A9 */ bool mLoopMovie;
    /* 0x0AA */ bool mPushWithPop;
    /* 0x0AC */ BaseGameSceneManager* mGameSceneManager;
    /* 0x0B0 */ EventConnectionOwner mHBMHideConnection;
}; // size 0xB4

class LessonMoviePlayerScene : public MoviePlayerScene
{
public:
    virtual ~LessonMoviePlayerScene() { }
    virtual void SceneCreated();
    virtual bool CheckMoviePlayerAbort();
    virtual void Update(float fDeltaT);

    ButtonComponent mButtonComponent;
};

class NLGLogoMovieScene : public MoviePlayerScene
{
public:
    NLGLogoMovieScene()
    {
        if (IsWidescreen())
        {
            SetMovieDetails("movies/nlgintrowide.thp", true, false);
        }
        else
        {
            SetMovieDetails("movies/nlgintrofull.thp", true, false);
        }
        mNextScene = (SceneList)23;
    }
    virtual ~NLGLogoMovieScene() { }
    virtual void PlayScreenForwardSFX() { }
    virtual void PlayScreenBackSFX() { }
    virtual void OverrideMovieDimensions();
};

class IntroMovieScene : public MoviePlayerScene
{
public:
    IntroMovieScene();
    virtual ~IntroMovieScene() { }
    virtual void PlayScreenForwardSFX() { }
    virtual void PlayScreenBackSFX() { }
    virtual void MoviePlayerVirtual3C();
    virtual void SceneCreated();
    virtual void Update(float fDeltaT);
    void ResetMoviePlayer();

    /* 0xB4 */ float mUnidentifiedB4;
    /* 0xB8 */ bool mMovieFinished;
    /* 0xB9 */ bool mTransitionPending;
};

#endif // GAME_SH_SHMOVIEPLAYER_H
