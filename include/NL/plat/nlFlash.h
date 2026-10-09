#ifndef NL_PLAT_NL_FLASH_H
#define NL_PLAT_NL_FLASH_H

#include "NL/nlTask.h"

struct NANDCommandBlock;

typedef void (*NANDResultCallback)(long result);

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
void nlFlashAsyncCallback(long result, NANDCommandBlock* block);
void nlFlashInitialize();
long nlFlashChangeDirectory(long directory, NANDResultCallback callback);
long nlFlashCreateDirectory(
    const char* name, unsigned char permissions, NANDResultCallback callback);
long nlFlashCreate(
    const char* name, unsigned char permissions, NANDResultCallback callback);
long nlFlashCheck(
    unsigned long blocks, unsigned long files, unsigned long* answer, NANDResultCallback callback);
long nlFlashDelete(const char* name, NANDResultCallback callback);
long nlFlashWrite(
    const void* data, unsigned long size, NANDResultCallback callback);
long nlFlashGetLength(unsigned long* size, NANDResultCallback callback);
long nlFlashRead(
    void** data, unsigned long* size, NANDResultCallback callback, bool bufferProvided);
long nlFlashOpen(
    const char* name, unsigned char mode, NANDResultCallback callback);
long nlFlashClose(NANDResultCallback callback);
bool nlFlashCallbackPending();
}

#endif // NL_PLAT_NL_FLASH_H
