#ifndef GAME_ASYNC_LOADING_H
#define GAME_ASYNC_LOADING_H

#include "Game/InterpreterCore.h"

class GLResourcePool;
struct AudioResourceLoadOwner;
struct EventConnection;

struct GlobalEventConnectionOwner
{
    GlobalEventConnectionOwner()
        : mOwner(0)
    {
    }

    ~GlobalEventConnectionOwner();

    /* 0x00 */ EventConnection* mOwner;
};

enum AsyncLoadingSequenceState
{
    ASYNC_LOADING_IDLE = 0,
    ASYNC_LOADING_BOOT_TO_FE_BEGIN = 1,
    ASYNC_LOADING_BOOT_TO_FE_RUN = 2,
    ASYNC_LOADING_CLEAN_BOOT_BEGIN = 3,
    ASYNC_LOADING_CLEAN_BOOT_RUN = 4,
    ASYNC_LOADING_FE_TO_GAME_BEGIN = 5,
    ASYNC_LOADING_FE_TO_GAME_RUN = 6,
    ASYNC_LOADING_GAME_TO_FE_BEGIN = 7,
    ASYNC_LOADING_GAME_TO_FE_RUN = 8,
    ASYNC_LOADING_BOOT_TO_GAME_BEGIN = 9,
    ASYNC_LOADING_BOOT_TO_GAME_RUN = 10,
    ASYNC_LOADING_STADIUM_VIEWER_BEGIN = 11,
    ASYNC_LOADING_STADIUM_VIEWER_RUN = 12,
};

enum AsyncLoadingResourceState
{
    ASYNC_RESOURCES_UNLOADED = 0,
    ASYNC_RESOURCES_LOADED = 1,
    ASYNC_RESOURCES_LOADING = 2,
    ASYNC_RESOURCES_UNLOADING = 3,
};

enum AsyncLoadingResult
{
    ASYNC_LOADING_WAITING_FOR_BYTE_CODE = 0,
    ASYNC_LOADING_NO_TRANSITION = 1,
    ASYNC_LOADING_RUNNING = 2,
    ASYNC_LOADING_FE_READY = 3,
    ASYNC_LOADING_CLEAN_BOOT_COMPLETE = 4,
    ASYNC_LOADING_GAME_READY = 5,
    ASYNC_LOADING_RETURN_TO_FE = 6,
    ASYNC_LOADING_STADIUM_OR_GAME_READY = 7,
};

class AsyncLoadingManager : public InterpreterCore
{
public:
    AsyncLoadingManager();

    virtual ~AsyncLoadingManager();
    virtual void DoFunctionCall(unsigned int functionIndex);

    static AsyncLoadingManager* Instance();
    GLResourcePool* GetPersistentResourcePool();
    void LoadTrophyTemplates();
    void SetLoadingComment(const char* comment) { mLoadingComment = comment; }

    /* 0x28 */ void* mByteCode;
    /* 0x2C */ u32 mSequenceState;
    /* 0x30 */ u32 mLoadingState;
    /* 0x34 */ const char* mLoadingComment;
    /* 0x38 */ u32 mPreviousStageTick;
    /* 0x3C */ u32 m_pad3C;
    /* 0x40 */ unsigned long long mSequenceStartTime;
    /* 0x48 */ u32 mStageStartTick;
    /* 0x4C */ void* mPoolResourceMark;
    /* 0x50 */ void* mGameResourceMark;
    /* 0x54 */ EventConnection* mGameOverConnection;
}; // size 0x58

extern bool g_VerboseAudio;
extern bool g_bDumpMemoryStatsOnLoad;
extern float g_fScriptBlockingWarningMS;
extern float g_fYieldScriptBlockingTimeMS;

void OnAsyncSoundBankLoaded(AudioResourceLoadOwner*, void* bankName);
void OnAsyncLoadingByteCodeLoaded(void* data, unsigned long size, void* userData);
void LoadAsyncLoadingByteCode(AsyncLoadingManager* manager);
u32 UpdateAsyncLoadingSequence(AsyncLoadingManager* manager);
void BeginBootToFrontEndLoading(AsyncLoadingManager* manager);
void BeginCleanBootShutdown(AsyncLoadingManager* manager);
void BeginFrontEndToGameLoading(AsyncLoadingManager* manager);
void BeginGameToFrontEndLoading(AsyncLoadingManager* manager);
void BeginGameToTitleLoading(AsyncLoadingManager* manager);
void BeginBootToGameLoading(AsyncLoadingManager* manager);
void BeginStadiumViewerLoading(AsyncLoadingManager* manager);
void DestroyFrontEndFast(AsyncLoadingManager* manager);
void InitializeFrontEndState1(AsyncLoadingManager* manager);
void InitializeFrontEndState2(AsyncLoadingManager* manager);
void DestroyFrontEndState(AsyncLoadingManager* manager);
void InitializeGameState1(AsyncLoadingManager* manager);
void InitializeGameState2(AsyncLoadingManager* manager);
void OnFrontEndTextureBundleLoaded(void* value0, unsigned long value1, void*);
void StartAsyncGameWorldLoading(AsyncLoadingManager* manager);
void InitializeInGameFrontEnd(AsyncLoadingManager* manager);
void FinalizeGameStateLoading(AsyncLoadingManager* manager);
void DestroyGameState(AsyncLoadingManager* manager);
void InitializeLegalScreenAndMovies(AsyncLoadingManager* manager);
void BeginAsyncFrontEndResourceLoad(AsyncLoadingManager* manager);
void BeginAsyncInGameFrontEndResourceLoad(AsyncLoadingManager* manager);
void OnLoadingSoundBankLoaded(AudioResourceLoadOwner*, void*);
void OnPermanentFrontEndBundleLoaded();
void OnOnDemandFrontEndBundleLoaded(void*, unsigned long, unsigned long);
void InitializeStadiumViewer(AsyncLoadingManager* manager);

#endif // GAME_ASYNC_LOADING_H
