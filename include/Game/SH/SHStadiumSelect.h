#ifndef GAME_SH_SHSTADIUMSELECT_H
#define GAME_SH_SHSTADIUMSELECT_H

#include "Game/BaseSceneHandler.h"
#include "Game/EventConnection.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feBackButton.h"
#include "Game/FE/fePageControls.h"

class TLComponentInstance;
class TLImageInstance;
class TLInstance;
class FEScrollText;

class MoviePlayerControl
{
public:
    virtual bool CheckMoviePlayerAbort();
    void Initialize(const char* filename);
    void Start(const char* filename);
    void Update(float deltaTime);
    void Stop();

    /* 0x04 */ bool mSwappedTexture;
    /* 0x05 */ bool mMovieStarted;
    /* 0x06 */ u8 mPadding06[2];
    /* 0x08 */ TLImageInstance* mMovieInstance;
    /* 0x0C */ char mMovieFilename[128];
    /* 0x8C */ bool mWithSound;
    /* 0x8D */ bool mLoopMovie;
    /* 0x8E */ u8 mPadding8E[2];
    /* 0x90 */ int mEndFrameCount;
}; // size 0x94

class StadiumSelectScene : public BaseSceneHandler
{
public:
    StadiumSelectScene();
    virtual ~StadiumSelectScene();
    virtual void Update(float deltaTime);
    virtual void SceneCreated();

    void OnHBMHide();
    void SortStadiums();
    void InitializeButtons();
    void OnPointerEnter(int index, void* context);
    void OnPointerLeave(int index, void* context);
    void OnSelectStadium(int index, void* context);

    /* 0x01C */ MoviePlayerControl mMoviePlayer;
    /* 0x0B0 */ FEScrollText* m_pTicker;
    /* 0x0B4 */ bool mControlsInitialized;
    /* 0x0B5 */ bool mPointerOverPlayButton;
    /* 0x0B6 */ bool mProceeding;
    /* 0x0B7 */ u8 mPaddingB7;
    /* 0x0B8 */ UnidentifiedEventConnectionOwner mHBMHideConnection;
    /* 0x0BC */ int mPreviewState;
    /* 0x0C0 */ int mPlayingStadiumIndex;
    /* 0x0C4 */ int mStadiumIndex;
    /* 0x0C8 */ int mStadiumOrder[17];
    /* 0x10C */ u8 mReserved10C[4];
    /* 0x110 */ FEPointerButton mPlayButton;
    /* 0x1C4 */ FEBackButton mBackButton;
    /* 0x29C */ FEPageControls mPageControls;
    /* 0x424 */ unsigned short mStadiumCountText[16];
    /* 0x444 */ u8 mReserved444[4];
    /* 0x448 */ TLComponentInstance* mStadiumNames;
    /* 0x44C */ TLComponentInstance* mPlayButtonInstance;
    /* 0x450 */ TLInstance* mLockedIcon;
}; // size 0x454

#endif // GAME_SH_SHSTADIUMSELECT_H
