#ifndef NL_PLAT_NL_FLASH_H
#define NL_PLAT_NL_FLASH_H

#include "NL/nlTask.h"

struct NANDCommandBlock;

typedef void (*NANDResultCallback)(s32 result);

class FlashMemoryTask : public nlTask
{
public:
    FlashMemoryTask();
    virtual ~FlashMemoryTask() { }

    virtual void Run(float dt);
    virtual const char* GetName() { return "Flash Memory"; }
};

extern "C"
{
void nlFlashAsyncCallback(s32 result, NANDCommandBlock* block);
void nlFlashInitialize();
s32 nlFlashChangeDirectory(s32 directory, NANDResultCallback callback);
s32 nlFlashCreateDirectory(
    const char* name, u8 permissions, NANDResultCallback callback);
s32 nlFlashCreate(
    const char* name, u8 permissions, NANDResultCallback callback);
s32 nlFlashCheck(
    u32 blocks, u32 files, u32* answer, NANDResultCallback callback);
s32 nlFlashDelete(const char* name, NANDResultCallback callback);
s32 nlFlashWrite(
    const void* data, u32 size, NANDResultCallback callback);
s32 nlFlashGetLength(u32* size, NANDResultCallback callback);
s32 nlFlashRead(
    void** data, u32* size, NANDResultCallback callback, bool bufferProvided);
s32 nlFlashOpen(
    const char* name, u8 mode, NANDResultCallback callback);
s32 nlFlashClose(NANDResultCallback callback);
bool nlFlashCallbackPending();
}

#endif // NL_PLAT_NL_FLASH_H
