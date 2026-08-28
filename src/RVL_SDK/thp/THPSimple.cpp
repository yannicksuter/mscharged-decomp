#include "RVL_SDK/thp/THPSimple.h"

#include "NL/gl/glState.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlFile.h"
#include "NL/nlFileGC.h"

#include <revolution/ai.h>
#include <revolution/os/OSCache.h>
#include <revolution/os/OSInterrupt.h>
#include <revolution/os/OSThread.h>
#include <revolution/thp/THPFile.h>
#include <string.h>

extern "C"
{
    BOOL THPInit(void);
    s32 THPVideoDecode(void* file, void* tileY, void* tileU, void* tileV, void* work);
    u32 THPAudioDecode(s16* buffer, u8* audioFrame, s32 flag);
}

static void THPAudioMixCallback();

struct THPSimpleControlWork
{
    /* 0x000 */ nlFile* fileInfo;
    /* 0x004 */ char magic[4];
    /* 0x008 */ unsigned long version;
    /* 0x00C */ unsigned long bufSize;
    /* 0x010 */ unsigned long audioMaxSamples;
    /* 0x014 */ float frameRate;
    /* 0x018 */ unsigned long numFrames;
    /* 0x01C */ unsigned long firstFrameSize;
    /* 0x020 */ unsigned long movieDataSize;
    /* 0x024 */ unsigned long compInfoDataOffsets;
    /* 0x028 */ unsigned long offsetDataOffsets;
    /* 0x02C */ unsigned long movieDataOffsets;
    /* 0x030 */ unsigned long finalFrameDataOffsets;
    /* 0x034 */ THPFrameCompInfo compInfo;
    /* 0x048 */ THPVideoInfo videoInfo;
    /* 0x054 */ THPAudioInfo audioInfo;
    /* 0x064 */ void* thpWork;
    /* 0x068 */ int open;
    /* 0x06C */ unsigned char preFetchState;
    /* 0x06D */ unsigned char audioState;
    /* 0x06E */ unsigned char loop;
    /* 0x06F */ unsigned char audioExist;
    /* 0x070 */ long curOffset;
    /* 0x074 */ int dvdError;
    /* 0x078 */ unsigned long readProgress;
    /* 0x07C */ long nextDecodeIndex;
    /* 0x080 */ long readIndex;
    /* 0x084 */ long readSize;
    /* 0x088 */ long totalReadFrame;
    /* 0x08C */ float curVolume;
    /* 0x090 */ float targetVolume;
    /* 0x094 */ float deltaVolume;
    /* 0x098 */ long rampCount;
    /* 0x09C */ THPReadBuffer readBuffer[10];
    /* 0x114 */ THPTextureSet textureSet;
    /* 0x124 */ THPAudioBuffer audioBuffer[6];
    /* 0x16C */ long audioDecodeIndex;
    /* 0x170 */ long audioOutputIndex;
    /* 0x174 */ unsigned char mono;
};

static THPSimpleControlWork SimpleControl;
static int Initialized;
static s32 SoundBufferIndex;
static void (*OldAIDCallback)();
static s16* LastAudioBuffer;
static s16* CurAudioBuffer;
static s32 AudioSystem;
static long WorkBuffer[16] ALIGN(32);
static s16 SoundBuffer[2][192] ALIGN(32);

static unsigned short VolumeTable[128] = {
    0x0000,
    0x0002,
    0x0008,
    0x0012,
    0x0020,
    0x0032,
    0x0049,
    0x0063,
    0x0082,
    0x00A4,
    0x00CB,
    0x00F5,
    0x0124,
    0x0157,
    0x018E,
    0x01C9,
    0x0208,
    0x024B,
    0x0292,
    0x02DD,
    0x032C,
    0x037F,
    0x03D7,
    0x0432,
    0x0492,
    0x04F5,
    0x055D,
    0x05C9,
    0x0638,
    0x06AC,
    0x0724,
    0x07A0,
    0x0820,
    0x08A4,
    0x092C,
    0x09B8,
    0x0A48,
    0x0ADD,
    0x0B75,
    0x0C12,
    0x0CB2,
    0x0D57,
    0x0DFF,
    0x0EAC,
    0x0F5D,
    0x1012,
    0x10CA,
    0x1187,
    0x1248,
    0x130D,
    0x13D7,
    0x14A4,
    0x1575,
    0x164A,
    0x1724,
    0x1801,
    0x18E3,
    0x19C8,
    0x1AB2,
    0x1BA0,
    0x1C91,
    0x1D87,
    0x1E81,
    0x1F7F,
    0x2081,
    0x2187,
    0x2291,
    0x239F,
    0x24B2,
    0x25C8,
    0x26E2,
    0x2801,
    0x2923,
    0x2A4A,
    0x2B75,
    0x2CA3,
    0x2DD6,
    0x2F0D,
    0x3048,
    0x3187,
    0x32CA,
    0x3411,
    0x355C,
    0x36AB,
    0x37FF,
    0x3956,
    0x3AB1,
    0x3C11,
    0x3D74,
    0x3EDC,
    0x4048,
    0x41B7,
    0x432B,
    0x44A3,
    0x461F,
    0x479F,
    0x4923,
    0x4AAB,
    0x4C37,
    0x4DC7,
    0x4F5C,
    0x50F4,
    0x5290,
    0x5431,
    0x55D6,
    0x577E,
    0x592B,
    0x5ADC,
    0x5C90,
    0x5E49,
    0x6006,
    0x61C7,
    0x638C,
    0x6555,
    0x6722,
    0x68F4,
    0x6AC9,
    0x6CA2,
    0x6E80,
    0x7061,
    0x7247,
    0x7430,
    0x761E,
    0x7810,
    0x7A06,
    0x7C00,
    0x7DFE,
    0x8000,
};

static void __THPAsyncCancelCB(nlFile*, void*, unsigned int, unsigned long, void (*)(nlFile*, void*, unsigned int, unsigned long))
{
}

extern "C" int THPSimpleInit(long audioSystem)
{
    memset(&SimpleControl, 0, sizeof(SimpleControl));
    LCEnable();

    if (!THPInit())
    {
        return 0;
    }

    int old = OSDisableInterrupts();
    AudioSystem = audioSystem;
    SoundBufferIndex = 0;
    LastAudioBuffer = NULL;
    CurAudioBuffer = NULL;
    OldAIDCallback = AIRegisterDMACallback(THPAudioMixCallback);

    if (OldAIDCallback == NULL && AudioSystem != 0)
    {
        AIRegisterDMACallback(NULL);
        OSRestoreInterrupts(old);
        return 0;
    }

    OSRestoreInterrupts(old);

    if (AudioSystem == 0)
    {
        memset(SoundBuffer, 0, sizeof(SoundBuffer));
        DCFlushRange(SoundBuffer, sizeof(SoundBuffer));
        AIInitDMA(SoundBuffer[SoundBufferIndex], sizeof(SoundBuffer[0]));
        AIStartDMA();
    }

    Initialized = 1;
    return 1;
}

extern "C" void THPSimpleQuit()
{
    LCDisable();
    int old = OSDisableInterrupts();
    if (OldAIDCallback != NULL)
    {
        AIRegisterDMACallback(OldAIDCallback);
    }
    OSRestoreInterrupts(old);
    Initialized = 0;
}

extern "C" int THPSimpleOpen(const char* fileName)
{
    long offset;
    long i;

    if (!Initialized)
    {
        return 0;
    }

    if (SimpleControl.open)
    {
        return 0;
    }

    memset(&SimpleControl.videoInfo, 0, sizeof(THPVideoInfo));
    memset(&SimpleControl.audioInfo, 0, sizeof(THPAudioInfo));

    SimpleControl.fileInfo = nlOpen(fileName);
    if (!SimpleControl.fileInfo)
    {
        return 0;
    }

    nlRead(SimpleControl.fileInfo, WorkBuffer, sizeof(WorkBuffer), 0);
    memcpy(SimpleControl.magic, WorkBuffer, sizeof(THPHeader));

    if (strcmp(SimpleControl.magic, "THP") != 0)
    {
        nlClose(SimpleControl.fileInfo);
        SimpleControl.fileInfo = NULL;
        return 0;
    }

    if (SimpleControl.version != 0x00011000)
    {
        nlClose(SimpleControl.fileInfo);
        SimpleControl.fileInfo = NULL;
        return 0;
    }

    offset = SimpleControl.compInfoDataOffsets;
    nlSeek(SimpleControl.fileInfo, offset, 0);
    nlRead(SimpleControl.fileInfo, WorkBuffer, 0x20, 0);
    memcpy(&SimpleControl.compInfo, WorkBuffer, sizeof(THPFrameCompInfo));

    offset += sizeof(THPFrameCompInfo);
    SimpleControl.audioExist = 0;

    for (i = 0; i < SimpleControl.compInfo.numComponents; i++)
    {
        switch (SimpleControl.compInfo.frameComp[i])
        {
        case 0:
            nlSeek(SimpleControl.fileInfo, offset, 0);
            nlRead(SimpleControl.fileInfo, WorkBuffer, 0x20, 0);
            memcpy(&SimpleControl.videoInfo, WorkBuffer, sizeof(THPVideoInfo));
            offset += sizeof(THPVideoInfo);
            break;
        case 1:
            nlSeek(SimpleControl.fileInfo, offset, 0);
            nlRead(SimpleControl.fileInfo, WorkBuffer, 0x20, 0);
            memcpy(&SimpleControl.audioInfo, WorkBuffer, sizeof(THPAudioInfo));
            offset += sizeof(THPAudioInfo);
            SimpleControl.audioExist = 1;
            break;
        default:
            return 0;
        }
    }

    SimpleControl.curOffset = SimpleControl.movieDataOffsets;
    SimpleControl.readSize = SimpleControl.firstFrameSize;
    SimpleControl.readIndex = 0;
    SimpleControl.totalReadFrame = 0;
    SimpleControl.dvdError = 0;
    SimpleControl.textureSet.frameNumber = -1;
    SimpleControl.nextDecodeIndex = 0;
    SimpleControl.audioDecodeIndex = 0;
    SimpleControl.audioOutputIndex = 0;
    SimpleControl.preFetchState = 0;
    SimpleControl.audioState = 0;
    SimpleControl.loop = 0;
    SimpleControl.open = 1;
    SimpleControl.curVolume = 127.0f;
    SimpleControl.targetVolume = 127.0f;
    SimpleControl.rampCount = 0;

    return 1;
}

extern "C" int THPSimpleClose()
{
    THPSimpleControlWork* ctrl = &SimpleControl;

    if (ctrl->open && ctrl->preFetchState == 0)
    {
        if (ctrl->audioExist)
        {
            if (ctrl->audioState == 1)
            {
                return 0;
            }
        }
        else
        {
            ctrl->audioState = 0;
        }

        if (SimpleControl.readProgress == 0)
        {
            ctrl->open = 0;
            while (nlAsyncReadsPending(SimpleControl.fileInfo))
            {
                nlServiceFileSystem();
            }
            nlClose(SimpleControl.fileInfo);
            SimpleControl.fileInfo = NULL;
            return 1;
        }
    }

    return 0;
}

extern "C" unsigned long THPSimpleCalcNeedMemory()
{
    unsigned long size;
    THPSimpleControlWork* ctrl = &SimpleControl;

    if (ctrl->open)
    {
        unsigned long pixels = ctrl->videoInfo.xSize * ctrl->videoInfo.ySize;
        size = ((ctrl->bufSize + 31) & ~31) * 10;
        size += (pixels + 31) & ~31;
        size += ((pixels / 4) + 31) & ~31;
        size += ((pixels / 4) + 31) & ~31;

        if (ctrl->audioExist)
        {
            size += 6 * ((ctrl->audioMaxSamples * 4 + 31) & ~31);
        }

        return size + 0x1000;
    }

    return 0;
}

extern "C" int THPSimpleSetBuffer(unsigned char* buffer)
{
    unsigned long i;
    unsigned char* ptr;
    PlatTexture* tex;

    if (SimpleControl.open && SimpleControl.preFetchState == 0)
    {
        if (SimpleControl.audioState == 1)
        {
            return 0;
        }

        ptr = buffer;
        SimpleControl.textureSet.ytexture = (u8*)glx_GetTex(glGetTexture("movie"))->m_SwizzledData;
        SimpleControl.textureSet.utexture = (u8*)glx_GetTex(glGetTexture("movie_u"))->m_SwizzledData;
        tex = glx_GetTex(glGetTexture("movie_v"));
        SimpleControl.textureSet.vtexture = (u8*)tex->m_SwizzledData;

        for (i = 0; i < 10; i++)
        {
            SimpleControl.readBuffer[i].ptr = ptr;
            ptr += (SimpleControl.bufSize + 31) & ~31;
            SimpleControl.readBuffer[i].isValid = 0;
        }

        if (SimpleControl.audioExist)
        {
            for (i = 0; i < 6; i++)
            {
                SimpleControl.audioBuffer[i].buffer = (s16*)ptr;
                SimpleControl.audioBuffer[i].curPtr = (s16*)ptr;
                SimpleControl.audioBuffer[i].validSample = 0;
                ptr += (SimpleControl.audioMaxSamples * 4 + 31) & ~31;
            }
        }

        SimpleControl.thpWork = ptr;
    }

    return 1;
}

static void __THPSimpleDVDCallback(nlFile*, void*, unsigned int, unsigned long)
{
    SimpleControl.readProgress = 0;
    SimpleControl.readBuffer[SimpleControl.readIndex].frameNumber = SimpleControl.totalReadFrame;
    SimpleControl.totalReadFrame++;
    SimpleControl.readBuffer[SimpleControl.readIndex].isValid = TRUE;
    SimpleControl.curOffset += SimpleControl.readSize;
    SimpleControl.readSize = *(u32*)SimpleControl.readBuffer[SimpleControl.readIndex].ptr;

    SimpleControl.readIndex = (SimpleControl.readIndex + 1 >= 10) ? 0 : SimpleControl.readIndex + 1;

    if (SimpleControl.readBuffer[SimpleControl.readIndex].isValid != 0)
        return;
    if (SimpleControl.dvdError != 0)
        return;
    if (SimpleControl.preFetchState != 1)
        return;

    if (SimpleControl.totalReadFrame > SimpleControl.numFrames - 1)
    {
        if (SimpleControl.loop != 1)
            return;
        SimpleControl.totalReadFrame = 0;
        SimpleControl.curOffset = SimpleControl.movieDataOffsets;
        SimpleControl.readSize = SimpleControl.firstFrameSize;
    }

    SimpleControl.readProgress = 1;
    nlSeek(SimpleControl.fileInfo, SimpleControl.curOffset, 0);
    nlReadAsync(SimpleControl.fileInfo, SimpleControl.readBuffer[SimpleControl.readIndex].ptr, SimpleControl.readSize, __THPSimpleDVDCallback, 0, 0);
}

extern "C" int THPSimplePreLoad(long loop)
{
    unsigned long i;
    unsigned long readNum;

    if (SimpleControl.open && SimpleControl.preFetchState == 0)
    {
        readNum = 10;
        if (loop == 0 && SimpleControl.numFrames < 10)
        {
            readNum = SimpleControl.numFrames;
        }

        for (i = 0; i < readNum; i++)
        {
            nlSeek(SimpleControl.fileInfo, SimpleControl.curOffset, 0);
            nlRead(SimpleControl.fileInfo, SimpleControl.readBuffer[SimpleControl.readIndex].ptr, SimpleControl.readSize, 0);

            long idx = SimpleControl.readIndex;
            SimpleControl.curOffset += SimpleControl.readSize;
            SimpleControl.readSize = *(long*)SimpleControl.readBuffer[idx].ptr;
            SimpleControl.readBuffer[idx].isValid = 1;
            SimpleControl.readBuffer[SimpleControl.readIndex].frameNumber = SimpleControl.totalReadFrame;
            SimpleControl.readIndex = (SimpleControl.readIndex + 1 >= 10) ? 0 : SimpleControl.readIndex + 1;
            SimpleControl.totalReadFrame++;

            if ((unsigned long)SimpleControl.totalReadFrame > SimpleControl.numFrames - 1)
            {
                if (SimpleControl.loop == 1)
                {
                    SimpleControl.totalReadFrame = 0;
                    SimpleControl.curOffset = SimpleControl.movieDataOffsets;
                    SimpleControl.readSize = SimpleControl.firstFrameSize;
                }
            }
        }

        SimpleControl.loop = loop;
        SimpleControl.preFetchState = 1;
        return 1;
    }

    return 0;
}

extern "C" void THPSimpleAudioStart()
{
    SimpleControl.audioState = 1;
}

extern "C" void THPSimpleAudioStop()
{
    SimpleControl.audioState = 0;
}

extern "C" int THPSimpleLoadStop()
{
    long i;

    if (SimpleControl.open && SimpleControl.audioState == 0)
    {
        SimpleControl.preFetchState = 0;
        if (SimpleControl.readProgress != 0)
        {
            nlCancelPendingAsyncReads(SimpleControl.fileInfo, __THPAsyncCancelCB);
            while (nlAsyncReadsPending(SimpleControl.fileInfo))
            {
                nlServiceFileSystem();
                OSYieldThread();
            }
            SimpleControl.readProgress = 0;
        }

        for (i = 0; i < 10; i++)
        {
            SimpleControl.readBuffer[i].isValid = 0;
        }

        SimpleControl.audioBuffer[0].validSample = 0;
        SimpleControl.audioBuffer[1].validSample = 0;
        SimpleControl.audioBuffer[2].validSample = 0;
        SimpleControl.audioBuffer[3].validSample = 0;
        SimpleControl.audioBuffer[4].validSample = 0;
        SimpleControl.audioBuffer[5].validSample = 0;
        SimpleControl.textureSet.frameNumber = -1;
        SimpleControl.curOffset = SimpleControl.movieDataOffsets;
        SimpleControl.readSize = SimpleControl.firstFrameSize;
        SimpleControl.readIndex = 0;
        SimpleControl.totalReadFrame = 0;
        SimpleControl.dvdError = 0;
        SimpleControl.nextDecodeIndex = 0;
        SimpleControl.audioDecodeIndex = 0;
        SimpleControl.audioOutputIndex = 0;
        SimpleControl.curVolume = SimpleControl.targetVolume;
        SimpleControl.rampCount = 0;
        return 1;
    }

    return 0;
}

static inline int VideoDecode(unsigned char* videoFrame)
{
    long ret = THPVideoDecode(videoFrame, SimpleControl.textureSet.ytexture, SimpleControl.textureSet.utexture, SimpleControl.textureSet.vtexture, SimpleControl.thpWork);
    if (ret == 0)
    {
        SimpleControl.textureSet.frameNumber = SimpleControl.readBuffer[SimpleControl.nextDecodeIndex].frameNumber;
        return 1;
    }
    return 0;
}

extern "C" long THPSimpleDecode(long audioTrack)
{
    int old;
    unsigned long i;
    unsigned char* ptr;
    unsigned long* compSizePtr;
    unsigned long sample;

    do
    {
        if (SimpleControl.readBuffer[SimpleControl.nextDecodeIndex].isValid == 0)
            break;

        compSizePtr = (unsigned long*)(SimpleControl.readBuffer[SimpleControl.nextDecodeIndex].ptr + 8);
        ptr = SimpleControl.readBuffer[SimpleControl.nextDecodeIndex].ptr + SimpleControl.compInfo.numComponents * 4 + 8;

        if (SimpleControl.audioExist != 0)
        {
            if (audioTrack < 0 || (unsigned long)audioTrack >= SimpleControl.audioInfo.sndNumTracks)
                return 4;

            if (SimpleControl.audioBuffer[SimpleControl.audioDecodeIndex].validSample == 0)
            {
                for (i = 0; i < SimpleControl.compInfo.numComponents; i++)
                {
                    switch (SimpleControl.compInfo.frameComp[i])
                    {
                    case 0:
                        if (!VideoDecode(ptr))
                            return 1;
                        break;
                    case 1:
                        sample = THPAudioDecode(SimpleControl.audioBuffer[SimpleControl.audioDecodeIndex].buffer, ptr + *compSizePtr * audioTrack, 0);
                        old = OSDisableInterrupts();
                        SimpleControl.audioBuffer[SimpleControl.audioDecodeIndex].validSample = sample;
                        SimpleControl.audioBuffer[SimpleControl.audioDecodeIndex].curPtr = SimpleControl.audioBuffer[SimpleControl.audioDecodeIndex].buffer;
                        OSRestoreInterrupts(old);
                        if (++SimpleControl.audioDecodeIndex >= 6)
                            SimpleControl.audioDecodeIndex = 0;
                        break;
                    }
                    ptr += *compSizePtr;
                    compSizePtr++;
                }
            }
            else
            {
                return 3;
            }
        }
        else
        {
            for (i = 0; i < SimpleControl.compInfo.numComponents; i++)
            {
                switch (SimpleControl.compInfo.frameComp[i])
                {
                case 0:
                    if (!VideoDecode(ptr))
                        return 1;
                    break;
                }
                ptr += *compSizePtr;
                compSizePtr++;
            }
        }

        SimpleControl.readBuffer[SimpleControl.nextDecodeIndex].isValid = 0;
        SimpleControl.nextDecodeIndex = (SimpleControl.nextDecodeIndex + 1 >= 10) ? 0 : SimpleControl.nextDecodeIndex + 1;

        old = OSDisableInterrupts();
        do
        {
            if (SimpleControl.readBuffer[SimpleControl.readIndex].isValid == 0 && SimpleControl.readProgress == 0 && SimpleControl.dvdError == 0 && SimpleControl.preFetchState == 1)
            {
                if ((unsigned long)SimpleControl.totalReadFrame > SimpleControl.numFrames - 1)
                {
                    if (SimpleControl.loop != 1)
                        break;
                    SimpleControl.totalReadFrame = 0;
                    SimpleControl.curOffset = SimpleControl.movieDataOffsets;
                    SimpleControl.readSize = SimpleControl.firstFrameSize;
                }

                SimpleControl.readProgress = 1;
                nlSeek(SimpleControl.fileInfo, SimpleControl.curOffset, 0);
                nlReadAsync(SimpleControl.fileInfo, SimpleControl.readBuffer[SimpleControl.readIndex].ptr, SimpleControl.readSize, __THPSimpleDVDCallback, 0, 0);
            }
        } while (false);

        OSRestoreInterrupts(old);
        return 0;
    } while (false);

    return 2;
}

static void MixAudio(short* destination, short* source, unsigned long sample)
{
    unsigned long requestSample;
    unsigned long i;
    unsigned short vol;
    long mix;
    short* dst;
    short* libsrc;
    short* thpsrc;

    if (source != NULL)
    {
        if (SimpleControl.open != 0 && SimpleControl.audioState == 1 && SimpleControl.audioExist != 0)
        {
            while (1)
            {
                if (SimpleControl.audioBuffer[SimpleControl.audioOutputIndex].validSample == 0)
                    break;

                if (SimpleControl.audioBuffer[SimpleControl.audioOutputIndex].validSample >= sample)
                    requestSample = sample;
                else
                    requestSample = SimpleControl.audioBuffer[SimpleControl.audioOutputIndex].validSample;

                thpsrc = SimpleControl.audioBuffer[SimpleControl.audioOutputIndex].curPtr;
                dst = destination;
                libsrc = source;

                for (i = 0; i < requestSample; i++)
                {
                    if (SimpleControl.rampCount != 0)
                    {
                        SimpleControl.rampCount--;
                        SimpleControl.curVolume += SimpleControl.deltaVolume;
                    }
                    else
                    {
                        SimpleControl.curVolume = SimpleControl.targetVolume;
                    }

                    vol = VolumeTable[(long)SimpleControl.curVolume];
                    if (SimpleControl.mono)
                    {
                        mix = *libsrc++ + ((vol * *thpsrc++) >> 15);
                        mix += *libsrc++ + ((vol * *thpsrc++) >> 15);
                        mix >>= 1;
                        if (mix < -0x8000)
                            mix = -0x8000;
                        if (mix > 0x7FFF)
                            mix = 0x7FFF;
                        dst[0] = mix;
                        dst[1] = mix;
                        dst += 2;
                    }
                    else
                    {
                        mix = libsrc[0] + ((vol * thpsrc[0]) >> 15);
                        if (mix < -0x8000)
                            mix = -0x8000;
                        if (mix > 0x7FFF)
                            mix = 0x7FFF;
                        dst[0] = mix;

                        mix = libsrc[1] + ((vol * thpsrc[1]) >> 15);
                        if (mix < -0x8000)
                            mix = -0x8000;
                        if (mix > 0x7FFF)
                            mix = 0x7FFF;
                        dst[1] = mix;
                        dst += 2;
                        libsrc += 2;
                        thpsrc += 2;
                    }
                }

                sample -= requestSample;
                SimpleControl.audioBuffer[SimpleControl.audioOutputIndex].validSample -= requestSample;
                SimpleControl.audioBuffer[SimpleControl.audioOutputIndex].curPtr = thpsrc;
                if (SimpleControl.audioBuffer[SimpleControl.audioOutputIndex].validSample == 0 && ++SimpleControl.audioOutputIndex >= 6)
                    SimpleControl.audioOutputIndex = 0;
                if (sample == 0)
                    return;
                destination = dst;
                source = libsrc;
            }

            memcpy(destination, source, sample << 2);
        }
        else
        {
            memcpy(destination, source, sample << 2);
        }
    }
    else
    {
        if (SimpleControl.open != 0 && SimpleControl.audioState == 1 && SimpleControl.audioExist != 0)
        {
            while (1)
            {
                requestSample = SimpleControl.audioBuffer[SimpleControl.audioOutputIndex].validSample;
                if (requestSample == 0)
                    break;
                if (requestSample >= sample)
                    requestSample = sample;

                thpsrc = SimpleControl.audioBuffer[SimpleControl.audioOutputIndex].curPtr;
                dst = destination;
                for (i = 0; i < requestSample; i++)
                {
                    if (SimpleControl.rampCount != 0)
                    {
                        SimpleControl.rampCount--;
                        SimpleControl.curVolume += SimpleControl.deltaVolume;
                    }
                    else
                    {
                        SimpleControl.curVolume = SimpleControl.targetVolume;
                    }

                    vol = VolumeTable[(long)SimpleControl.curVolume];
                    mix = (vol * thpsrc[0]) >> 15;
                    if (mix < -0x8000)
                        mix = -0x8000;
                    if (mix > 0x7FFF)
                        mix = 0x7FFF;
                    dst[0] = mix;

                    mix = (vol * thpsrc[1]) >> 15;
                    if (mix < -0x8000)
                        mix = -0x8000;
                    if (mix > 0x7FFF)
                        mix = 0x7FFF;
                    dst[1] = mix;

                    dst += 2;
                    thpsrc += 2;
                }

                sample -= requestSample;
                SimpleControl.audioBuffer[SimpleControl.audioOutputIndex].validSample -= requestSample;
                SimpleControl.audioBuffer[SimpleControl.audioOutputIndex].curPtr = thpsrc;
                if (SimpleControl.audioBuffer[SimpleControl.audioOutputIndex].validSample == 0 && ++SimpleControl.audioOutputIndex >= 6)
                    SimpleControl.audioOutputIndex = 0;
                if (sample == 0)
                    return;
                destination = dst;
            }

            memset(destination, 0, sample << 2);
        }
        else
        {
            memset(destination, 0, sample << 2);
        }
    }
}

extern "C" int THPSimpleGetVideoInfo(THPVideoInfo* videoInfo)
{
    if (SimpleControl.open)
    {
        memcpy(videoInfo, &SimpleControl.videoInfo, sizeof(THPVideoInfo));
        return 1;
    }
    return 0;
}

extern "C" s32 THPSimpleGetTotalFrame()
{
    if (SimpleControl.open)
        return SimpleControl.numFrames;
    return 0;
}

static void THPAudioMixCallback()
{
    if (AudioSystem == 0)
    {
        SoundBufferIndex ^= 1;
        AIInitDMA(SoundBuffer[SoundBufferIndex], sizeof(SoundBuffer[0]));
        BOOL old = OSEnableInterrupts();
        MixAudio(SoundBuffer[SoundBufferIndex], NULL, 0x60);
        DCFlushRange(SoundBuffer[SoundBufferIndex], sizeof(SoundBuffer[0]));
        OSRestoreInterrupts(old);
    }
    else
    {
        if (AudioSystem == 1)
        {
            if (LastAudioBuffer != NULL)
                CurAudioBuffer = LastAudioBuffer;
            OldAIDCallback();
            LastAudioBuffer = (s16*)(AIGetDMAStartAddr() + 0x80000000);
        }
        else
        {
            OldAIDCallback();
            CurAudioBuffer = (s16*)(AIGetDMAStartAddr() + 0x80000000);
        }

        SoundBufferIndex ^= 1;
        AIInitDMA(SoundBuffer[SoundBufferIndex], sizeof(SoundBuffer[0]));
        BOOL old = OSEnableInterrupts();
        if (CurAudioBuffer != NULL)
            DCInvalidateRange(CurAudioBuffer, sizeof(SoundBuffer[0]));
        MixAudio(SoundBuffer[SoundBufferIndex], CurAudioBuffer, 0x60);
        DCFlushRange(SoundBuffer[SoundBufferIndex], sizeof(SoundBuffer[0]));
        OSRestoreInterrupts(old);
    }
}

extern "C" int THPSimpleSetVolume(long vol, long time)
{
    THPSimpleControlWork* ctrl = &SimpleControl;

    if (ctrl->open && ctrl->audioExist)
    {
        u32 rate = AIGetDSPSampleRate();
        long samplePerMs = 0x30;
        if (!rate)
            samplePerMs = 0x20;

        if (vol > 127)
            vol = 127;
        if (vol < 0)
            vol = 0;
        if (time > 60000)
            time = 60000;
        if (time < 0)
            time = 0;

        int old = OSDisableInterrupts();
        ctrl = &SimpleControl;
        ctrl->targetVolume = (float)vol;
        if (time != 0)
        {
            ctrl->rampCount = samplePerMs * time;
            ctrl->deltaVolume = (ctrl->targetVolume - ctrl->curVolume) / (float)ctrl->rampCount;
        }
        else
        {
            ctrl->curVolume = ctrl->targetVolume;
            ctrl->rampCount = 0;
        }

        OSRestoreInterrupts(old);
        return 1;
    }
    return 0;
}

extern "C" void fn_80372970(unsigned char mono)
{
    SimpleControl.mono = mono;
}
