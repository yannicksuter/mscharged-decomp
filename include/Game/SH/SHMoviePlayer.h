#ifndef GAME_SH_SHMOVIEPLAYER_H
#define GAME_SH_SHMOVIEPLAYER_H

#include "Game/BaseGameSceneManager.h"
#include "Game/BaseSceneHandler.h"

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

    /* 0x01C */ SceneList mNextScene;
    /* 0x020 */ bool mSwappedTexture;
    /* 0x021 */ bool mMovieStarted;
    /* 0x024 */ TLImageInstance* mMovieInstance;
    /* 0x028 */ char mMovieFilename[128];
    /* 0x0A8 */ bool mWithSound;
    /* 0x0A9 */ bool mLoopMovie;
    /* 0x0AA */ bool mPushWithPop;
    /* 0x0AC */ BaseGameSceneManager* mGameSceneManager;
    /* 0x0B0 */ void* mUnidentified0B0;
}; // size 0xB4

#endif // GAME_SH_SHMOVIEPLAYER_H
