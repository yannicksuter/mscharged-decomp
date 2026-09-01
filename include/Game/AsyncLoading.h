#ifndef GAME_ASYNC_LOADING_H
#define GAME_ASYNC_LOADING_H

#include "Game/InterpreterCore.h"

struct UnidentifiedOwnerTarget
{
    virtual ~UnidentifiedOwnerTarget();
    virtual void Release(void*) = 0;
};

struct UnidentifiedOwnerRecord
{
    /* 0x00 */ u32 mUnidentified00;
    /* 0x04 */ UnidentifiedOwnerTarget* mTarget;
    /* 0x08 */ u32 mFlags;
};

struct UnidentifiedOwnerHandle
{
    /* 0x00 */ UnidentifiedOwnerRecord* mOwner;
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
    AsyncLoadingManager()
        : InterpreterCore(100)
    {
        mLoadingHandle.mOwner = 0;
        mByteCode = 0;
        mSequenceState = ASYNC_LOADING_IDLE;
        mLoadingState = 0;
        mPreviousStageTick = 0;
        mStartingFreeMemory = 0;
        mStageStartTick = 0;
        mLoadingComment = "No Loading Comment";
    }

    virtual ~AsyncLoadingManager();
    virtual void DoFunctionCall(unsigned int functionIndex);

    /* 0x28 */ void* mByteCode;
    /* 0x2C */ u32 mSequenceState;
    /* 0x30 */ u32 mLoadingState;
    /* 0x34 */ const char* mLoadingComment;
    /* 0x38 */ u32 mPreviousStageTick;
    /* 0x3C */ u32 mUnidentified3C;
    /* 0x40 */ unsigned long long mStartingFreeMemory;
    /* 0x48 */ u32 mStageStartTick;
    /* 0x4C */ void* mUnidentified4C;
    /* 0x50 */ void* mUnidentified50;
    /* 0x54 */ UnidentifiedOwnerHandle mLoadingHandle;
}; // size 0x58

struct UnidentifiedLoadingStateProvider
{
    /* 0x00 */ u8 mUnidentified00[0x18];
    /* 0x18 */ void* mUnidentified18;
};

extern bool g_VerboseAudio;
extern bool g_bDumpMemoryStatsOnLoad;
extern float g_fScriptBlockingWarningMS;
extern float g_fYieldScriptBlockingTimeMS;

extern "C" {
void fn_80116988(void*, const char* bankName);
AsyncLoadingManager* fn_80118A74();
void fn_80118B38(void* data, unsigned long size, void* userData);
void fn_80118B50(AsyncLoadingManager* manager);
u32 fn_80118B7C(AsyncLoadingManager* manager);
void fn_80119054(AsyncLoadingManager* manager);
void fn_801190A0(AsyncLoadingManager* manager);
void fn_801190EC(AsyncLoadingManager* manager);
void fn_80119138(AsyncLoadingManager* manager);
void fn_80119184(AsyncLoadingManager* manager);
void fn_801191D4(AsyncLoadingManager* manager);
void fn_80119220(AsyncLoadingManager* manager);
void fn_8011926C(AsyncLoadingManager* manager);
void fn_80119454(AsyncLoadingManager* manager);
void fn_80119528(AsyncLoadingManager* manager);
void fn_80119B0C(AsyncLoadingManager* manager);
void fn_80119EC0(AsyncLoadingManager* manager);
void fn_8011A0A8(AsyncLoadingManager* manager);
void fn_8011A2DC(void* value0, void* value1);
void fn_8011A2E8(AsyncLoadingManager* manager);
void fn_8011A570(AsyncLoadingManager* manager);
void fn_8011A800(AsyncLoadingManager* manager);
void fn_8011A9DC(AsyncLoadingManager* manager);
void fn_8011B02C(AsyncLoadingManager* manager);
void fn_8011B178(AsyncLoadingManager* manager);
void fn_8011B2E4(AsyncLoadingManager* manager);
void fn_8011B40C(void*, unsigned long, unsigned long);
void fn_8011B418(void*, unsigned long, unsigned long);
void fn_8011B424(void*, unsigned long, unsigned long);
void fn_8011B430(AsyncLoadingManager* manager);
void fn_8011B6E8(AsyncLoadingManager* manager);
void* fn_8011B850();
UnidentifiedOwnerHandle* fn_8011B858(UnidentifiedOwnerHandle* handle,
    int shouldDelete);
void* fn_8011B8D8(UnidentifiedLoadingStateProvider* provider);
}

#endif // GAME_ASYNC_LOADING_H
