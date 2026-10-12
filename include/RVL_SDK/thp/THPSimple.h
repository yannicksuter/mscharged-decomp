#ifndef RVL_SDK_THP_THPSIMPLE_H
#define RVL_SDK_THP_THPSIMPLE_H

#include <revolution/thp/THPBuffer.h>
#include <revolution/thp/THPInfo.h>
#include <revolution/thp_fwd.h>

#ifdef __cplusplus
extern "C"
{
#endif

    int THPSimpleInit(long audioSystem);
#if !defined(VERSION_R4QP01_REV1)
    void THPSimpleSetMono(bool mono);
#endif
    void THPSimpleQuit(void);
    int THPSimpleOpen(const char* fileName);
    int THPSimpleClose(void);
    unsigned long THPSimpleCalcNeedMemory(void);
    int THPSimpleSetBuffer(unsigned char* buffer);
    int THPSimplePreLoad(long loop);
    void THPSimpleAudioStart(void);
    void THPSimpleAudioStop(void);
    int THPSimpleLoadStop(void);
    long THPSimpleDecode(long audioTrack);
    int THPSimpleGetVideoInfo(THPVideoInfo* videoInfo);
    s32 THPSimpleGetTotalFrame(void);

#ifdef __cplusplus
}
#endif

#endif // RVL_SDK_THP_THPSIMPLE_H
