#ifndef GAME_NIS_PLAYER_H
#define GAME_NIS_PLAYER_H

#include "Game/InterpreterCore.h"
#include "Game/Camera/animcam.h"
#include "Game/Render/Nis.h"
#include "types.h"

class NisPlayerOverlay;
class nlFile;
class cPlayer;
struct glModel;
struct glModelPacket;
struct GoalScoredData;
struct GoalieSaveData;

enum NisUseFilter
{
    NIS_NO_FILTER = 0,
    NIS_FILTER = 1,
};

extern bool g_ForceDoubleBallTransition;

class NisPlayer : public InterpreterCore
{
public:
    NisPlayer();
    virtual ~NisPlayer();
    virtual void DoFunctionCall(unsigned int);

    void Load(char* buffer, unsigned int size, NisHeader& nisHeader);
    void Load(const char* nisType, NisTarget target, NisUseStadiumOffset useStadiumOffset, NisUseFilter useFilter, NisWinnerType winnerType, int renderMode, int variantIndex);
    void LoadRelatedNis(const char* nisName, const char* relation, NisTarget target, NisUseStadiumOffset useStadiumOffset, NisWinnerType winnerType, bool mirrored, int renderMode);
    void LoadTriggers(Nis& nis);
    const char* GetTargetFilter(NisTarget target, NisWinnerType winnerType) const;
    void SetTeamLogo(NisTarget target, NisWinnerType winnerType);

    bool HasTeamLogoPacket(const glModelPacket* packet) const
    {
        for (int i = 0; i < 10; i++)
        {
            if (mTeamLogoPackets[i] == packet)
            {
                return true;
            }
        }
        return false;
    }

    void AddTeamLogoPacket(glModelPacket* packet)
    {
        for (int i = 0; i < 10; i++)
        {
            if (mTeamLogoPackets[i] == NULL)
            {
                mTeamLogoPackets[i] = packet;
                return;
            }
        }
    }

    void HandleAsyncs();
    void Update(float deltaT);
    void StartLoadedScripts();
    bool IsReadyToPlay();
    void QueueNisLoad(NisHeader& nisHeader, NisTarget target, NisUseStadiumOffset useStadiumOffset, NisWinnerType winnerType, int renderMode, bool preserveMirroring);
    void ParseDictionary(char* data);
    void ParseDoNotMirrorList(char* data, unsigned long size);
    void ParseDoNotShowPIPList(char* data, unsigned long size);
    void ResetEffects();
    void fn_8027BD60();
    void SelectCameras();
    void ReleaseNisCue();
    void PrepareNisCue(unsigned long cue);
    void Reset();
    void Play();
    void RegisterEventHandlers();
    void OnPauseGame();
    void PreserveNisCueOnReset();
    void ClearLoadQueue();
    void SetupNisPlayback();
    void OnGoalScored(GoalScoredData* goalScoredData);
    void RandomizeBeginPositions();
    void OnGoalieSave(GoalieSaveData*);
    void ResetPlayerEffects();
    void OnMegaStrikeIntro(cPlayer* player);
    void ReleaseCachedNisBuffers();
    void SetExtraNameFilter(const char* filter);
    bool WorldIsFrozen() const;
    bool HasSecondaryNis() const;
    unsigned int IsPIPOverlayMode() const
    {
        return mOverlayMode == 0;
    }
    float TimeLeft() const;
    float GetCameraTimeLeft(int cameraIndex) const;
    cAnimCamera* GetSecondaryCamera();
    void StartNisCue();
    void Render(int pass) const;
    void HideAllActors() const;
    void FadeWorldDarkening(float duration);
    void EnableWorldDarkening(bool fade);
    void ResetToPIPOverlay();
    void ClearSecondaryNis();
    void fn_8027E5D0();
    void SwapCameras();
    void StopNisCue();
    int GetWinnerSide(NisWinnerType winnerType) const;
    bool IsMirrored(NisTarget target, const char* name, NisWinnerType winnerType) const;
    bool AllowsPIP();
    bool AllowsPIP(const char* name) const;
    static NisPlayer* Instance();
    static void AsyncLoad(nlFile* file, void* buffer, unsigned int size, unsigned long param);

    /* 0x00028 */ int mLoadedAssetMask;
    /* 0x0002C */ bool mActive;
    /* 0x00030 */ int mDictSize;
    /* 0x00034 */ NisHeader mDict[512];
    /* 0x34034 */ char* mMemory;
    /* 0x34038 */ int mMaxNumBallsVisible;
    /* 0x3403C */ Nis* mPlaying[8];
    /* 0x3405C */ Nis* mLoaded[8];
    /* 0x3407C */ NisHeader* mLoadQueue[8];
    /* 0x3409C */ bool mAsyncStarted[8];
    /* 0x340A4 */ char** mDoNotMirrorNames;
    /* 0x340A8 */ int mNumDoNotMirrorNames;
    /* 0x340AC */ char** mDoNotShowPIPNames;
    /* 0x340B0 */ int mNumDoNotShowPIPNames;
    /* 0x340B4 */ bool mLoadingFromBack;
    /* 0x340B8 */ int mUsedFromFront;
    /* 0x340BC */ int mUsedFromBack;
    /* 0x340C0 */ int mGoalScorerCharIndex;
    /* 0x340C4 */ cAnimCamera mCamera[2];
    /* 0x3422C */ Nis* mNisForTriggerLoading;
    /* 0x34230 */ int mWinnerSide[NIS_NUM_WINNER_TYPES];
    /* 0x34238 */ int mMegaStrikeSide;
    /* 0x3423C */ nlVector3 mBeginPositions[10];
    /* 0x342B4 */ char mExtraNameFilter[128];
    /* 0x34334 */ void* mAnimProxyByteCode;
    /* 0x34338 */ int mOverlayMode;
    /* 0x3433C */ NisPlayerOverlay* mOverlays[5];
    /* 0x34350 */ unsigned long mPlayingNisCue;
    /* 0x34354 */ unsigned long mPreparedNisCue;
    /* 0x34358 */ bool mStopNisCueOnReset;
    /* 0x34359 */ bool mDisplayNisInfo;
    /* 0x3435A */ bool mUseViewMatrixOverride;
    /* 0x3435C */ nlMatrix4 mViewMatrixOverride;
    /* 0x3439C */ bool mInitialCameraRotationCached;
    /* 0x343A0 */ nlMatrix4 mInitialCameraRotationInverse;
    /* 0x343E0 */ bool mSuppressBlinking;
    /* 0x343E4 */ float mRequestedFogStart;
    /* 0x343E8 */ float mSavedFogStart;
    /* 0x343EC */ float mRequestedFogEnd;
    /* 0x343F0 */ float mSavedFogEnd;
    /* 0x343F4 */ int mLastCelebrationIndex;
    /* 0x343F8 */ char mLastCelebrationFilter[64];
    /* 0x34438 */ unsigned long mTeamLogoTexture;
    /* 0x3443C */ unsigned long mTeamLogoTextureIndex;
    /* 0x34440 */ glModelPacket* mTeamLogoPackets[10];
    /* 0x34468 */ float mCameraOverrun;
};

#endif // GAME_NIS_PLAYER_H
