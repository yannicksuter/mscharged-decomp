#ifndef UNCLASSIFIED_TU_80376888_H
#define UNCLASSIFIED_TU_80376888_H

#include "NL/nlTask.h"

struct NANDCommandBlock;

typedef void (*NANDResultCallback)(s32 result);

class UnidentifiedTask_80376888 : public nlTask
{
public:
    UnidentifiedTask_80376888();
    virtual ~UnidentifiedTask_80376888() { }

    virtual void Run(float dt);
    virtual const char* GetName() { return "Flash Memory"; }
};

extern "C"
{
void fn_803768A0(s32 result, NANDCommandBlock* block);
void fn_803768F8();
s32 fn_80376934(s32 directory, NANDResultCallback callback);
s32 fn_80376B08(
    const char* name, u8 permissions, NANDResultCallback callback);
s32 fn_80376B68(
    const char* name, u8 permissions, NANDResultCallback callback);
s32 fn_80376BC8(
    u32 blocks, u32 files, u32* answer, NANDResultCallback callback);
s32 fn_80376C20(const char* name, NANDResultCallback callback);
s32 fn_80376C78(
    const void* data, u32 size, NANDResultCallback callback);
s32 fn_80376CF8(u32* size, NANDResultCallback callback);
s32 fn_80376D6C(
    void** data, u32* size, NANDResultCallback callback, bool bufferProvided);
s32 fn_80376E3C(
    const char* name, s32 mode, NANDResultCallback callback);
s32 fn_80376EB0(NANDResultCallback callback);
bool fn_80376F18();
}

#endif // UNCLASSIFIED_TU_80376888_H
