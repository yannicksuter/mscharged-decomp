#include "NL/glx/glxSwap.h"

#include "NL/glx/glxGX.h"
#include "NL/MemAlloc.h"
#include "NL/nlEndian.h"
#include "NL/nlPrint.h"
#include "NL/nlTicker.h"

// Revolution SDK and neighbouring renderer entry points retained as automatic
// objects in R4QE01. The SDK's own <revolution/gx.h> cannot be included from
// game code yet, because "types.h" and <revolution/types.h> spell the
// fixed-width typedefs differently, so the declarations are repeated here.
// Only identities established from the target are spelled out; the rest stay
// address-named.
extern "C"
{
    void VIFlush();
    void VISetBlack(u8 black);
    void VISetNextFrameBuffer(void* fb);
    u32 VIGetRetraceCount();
    void VISetPreRetraceCallback(void (*cb)(u32));
    void VISetPostRetraceCallback(void (*cb)(u32));
    u32 OSGetTick();
    void OSReport(const char* format, ...);

    void GXGetGPStatus(u8* overhi, u8* underlow, u8* readIdle, u8* cmdIdle, u8* brkpt);
    void GXFlush();
    void GXAbortFrame();
    u16 GXReadDrawSync();
    void GXSetDrawDone();
    void GXWaitDrawDone();
    void GXDrawDone();
    void GXPeekARGB(u16 x, u16 y, u32* colour);
    void (*GXSetDrawDoneCallback(void (*cb)()))();
    void GXCopyDisp(void* dest, u8 clear);
    void nlBreak__Fv();

    // Clears one external framebuffer; owned by the still-unsplit platform TU.
    void fn_803693C4(void* fb);
    // View/token name lookups owned by an unsplit debug TU.
    void VIWaitForRetrace();
    void* fn_80364020();
    const char* fn_803640E8(void* view, u16 token);
    const char* fn_803640F0(void* view, u16 token);
    // Allocator-state helpers retained in the following automatic range.
    void fn_8036D6F8(MemoryAllocator* allocator);
    void fn_8036D71C();
    // C stdio entry points retained in the MSL region.
    typedef struct _FILE FILE;
    FILE* fopen(const char* path, const char* mode);
    unsigned long fwrite(const void* buffer, unsigned long size, unsigned long count, FILE* file);
    int fclose(FILE* file);
}

struct GXColor
{
    u8 r;
    u8 g;
    u8 b;
    u8 a;
};

extern "C" void GXSetCopyClear(GXColor clear_clr, u32 clear_z);

// R4QE01 keeps one copy of the aligned nlMalloc forwarding thunk, emitted by
// the nlConfig translation unit and called from here as well.
void* ConfigParserAllocate(unsigned long size, unsigned int alignment, bool fromEnd);

enum
{
    GX_LEQUAL = 3
};

#define OS_BUS_CLOCK                 (*(u32*)0x800000F8)
#define OS_TIMER_CLOCK               (OS_BUS_CLOCK / 4)
#define OSTicksToMicroseconds(ticks) (((ticks) * 8) / (OS_TIMER_CLOCK / 125000))

#pragma push
#pragma pack(1)
struct TargaHeader
{
    /* 0x00 */ u8 imageIDLength;
    /* 0x01 */ u8 colorMapType;
    /* 0x02 */ u8 imageType;
    /* 0x03 */ u16 firstEntry;
    /* 0x05 */ u16 mapLength;
    /* 0x07 */ u8 paletteBitsPerPixel;
    /* 0x08 */ u16 xOrigin;
    /* 0x0A */ u16 yOrigin;
    /* 0x0C */ u16 width;
    /* 0x0E */ u16 height;
    /* 0x10 */ u8 bitsPerPixel;
    /* 0x11 */ u8 imageDescriptor;
};
#pragma pop

static void HandleSoftReset();
static void glx_ScreenCapture(bool isMovie);
static void PutPixel(u8* dst, u32 argbColor);
static void loading_indicator();
static void vi_pre_cb(u32);
static void vi_post_cb(u32);
static void draw_done_cb();
static void AdvanceFrame();
static void ReportGPStatus();
static void WaitDrawDone();
static void swap_Post();
static void swap_Pre();

static u8 glx_bAllowDrawSync = true;
static s32 glx_SwapMode = 1;

static void (*glx_pSoftReset)();
static int _shotno;
u8 glx_bLoadingIndicator;
s32 glx_nBlitXor;
static volatile int glx_nFrame;
u8 glx_MovieOutput;
u8 glx_ScreenShot;
static void* glx_FrameBuffer[2];
static int glx_nBuffer;
static int nFirstFrame;
static volatile u8 bInRetrace;
static unsigned long long glx_LastRetraceTick;
static int glx_nLoadFrame;
static int glx_nLoadWaitFrames;
static void (*glx_pLoadingIndicator)(void*);
static u8 glx_bAborted;
static u32 glx_nRetraceAtSwap;
static f32 glx_DrawSyncTimeout;
static GXColor glx_ClearColour;

static void HandleSoftReset()
{
    if (glx_pSoftReset)
    {
        glx_pSoftReset();
    }
}

s32 glxGetFrameCount()
{
    return glx_nFrame;
}

void glxSwapSetBlack(bool black)
{
    if (black)
    {
        VISetBlack(1);
        nFirstFrame = 3;
    }
    else
    {
        VISetBlack(0);
        nFirstFrame = 0;
    }
}

void glxSetSwapMode(s32 mode)
{
    glx_SwapMode = mode;
}

void glxRequestScreenShot()
{
    glx_ScreenShot = 1;
}

static void glx_ScreenCapture(bool isMovie)
{
    char filename[0x40];
    void* file;
    TargaHeader header;
    u32 argbColor;
    s32 pixelOffset;
    s32 y, x;
    u8* imageData;

    if (isMovie != 0)
    {
        nlSNPrintf(filename, 0x40, "../shot%03d.tga", _shotno);
    }
    else
    {
        nlSNPrintf(filename, 0x40, "shot%03d.tga", _shotno);
    }

    _shotno++;
    file = fopen(filename, "wb");

    if (file != 0)
    {
        header.imageIDLength = 0;
        header.colorMapType = 0;
        header.imageType = 2;
        header.firstEntry = 0;
        header.mapLength = 0;
        header.paletteBitsPerPixel = 0;
        header.xOrigin = 0;
        header.yOrigin = 0;
        header.width = 0x280;
        header.height = 0x1C0;
        header.bitsPerPixel = 0x18;
        header.imageDescriptor = 0x20;

        nlSwapEndian(header.width, &(header.width));
        nlSwapEndian(header.height, &(header.height));

        fn_8036D6F8(&VirtualAllocator);
        imageData = (u8*)ConfigParserAllocate(0xD2000, 8, false);
        fn_8036D71C();

        GXDrawDone();

        for (y = 0; y < 0x1C0; y++)
        {
            for (x = 0; x < 0x280; x++)
            {
                GXPeekARGB((u16)x, (u16)y, &argbColor);
                pixelOffset = (y * 0x280 + x) * 3;
                PutPixel(&imageData[pixelOffset], argbColor);
            }
        }

        fwrite(&header, 1, sizeof(TargaHeader), (FILE*)file);
        fwrite(imageData, 3, 0x46000, (FILE*)file);
        fclose((FILE*)file);
        delete[] imageData;
    }
}

static void PutPixel(u8* dst, u32 argbColor)
{
    union
    {
        u32 word;
        u8 bytes[4];
    } colorBytes;

    colorBytes.word = argbColor;
    dst[0] = colorBytes.bytes[3];
    dst[1] = colorBytes.bytes[2];
    dst[2] = colorBytes.bytes[1];
}

void* glxGetDisplayedBuffer()
{
    return glx_FrameBuffer[glx_nBuffer ^ 1];
}

void* glxGetBackBuffer()
{
    return glx_FrameBuffer[glx_nBuffer];
}

static void loading_indicator()
{
    if (nFirstFrame == 0)
    {
        glx_pLoadingIndicator(glx_FrameBuffer[glx_nBuffer ^ glx_nBlitXor]);
        VISetNextFrameBuffer(glx_FrameBuffer[glx_nBuffer]);
        VIFlush();
        glx_nBuffer ^= 1;
    }
    if (nFirstFrame > 0)
    {
        nFirstFrame--;
        if (nFirstFrame == 0)
        {
            fn_803693C4(glx_FrameBuffer[0]);
            fn_803693C4(glx_FrameBuffer[1]);
            VISetBlack(0);
            VIFlush();
        }
    }
}

static void vi_pre_cb(u32)
{
    bInRetrace = 1;
}

static void vi_post_cb(u32)
{
    HandleSoftReset();
    if (glx_bLoadingIndicator != 0)
    {
        glx_nLoadFrame++;
        if (glx_nLoadFrame >= glx_nLoadWaitFrames)
        {
            loading_indicator();
        }
    }
    glx_nFrame++;
    bInRetrace = 0;
    glx_LastRetraceTick = OSGetTick();
}

void glxSwapWaitDrawDone()
{
    GXSetDrawDone();
    GXFlush();
    GXWaitDrawDone();
}

static void draw_done_cb()
{
    glx_bAborted = 1;
}

void glxInitSwap(void* fb0, void* fb1)
{
    glx_FrameBuffer[0] = fb0;
    glx_FrameBuffer[1] = fb1;
    glx_nBuffer = 0;
    nFirstFrame = 3;
    glx_bAllowDrawSync = 1;

    GXSetDrawDone();
    GXSetDrawDoneCallback(draw_done_cb);
    GXFlush();
    VISetPreRetraceCallback(vi_pre_cb);
    VISetPostRetraceCallback(vi_post_cb);
}

void glxSwapPost()
{
    if (glx_bLoadingIndicator == 0)
    {
        swap_Post();
    }
}

void glxSwapPre()
{
    if (glx_bLoadingIndicator == 0)
    {
        swap_Pre();
    }
}

static void AdvanceFrame()
{
    VISetNextFrameBuffer(glx_FrameBuffer[glx_nBuffer]);
    if (nFirstFrame > 0)
    {
        nFirstFrame--;
        if (nFirstFrame == 0)
        {
            VISetBlack(0);
        }
    }
    VIFlush();
}

void glxSwapBuffers()
{
    glx_nBuffer ^= 1;
}

void glxSetDrawSyncTimeout(f32 seconds)
{
    glx_DrawSyncTimeout = seconds;
}

static void ReportGPStatus()
{
    u8 overhi;
    u8 underlow;
    u8 readIdle;
    u8 cmdIdle;
    u8 brkpt;

    GXGetGPStatus(&overhi, &underlow, &readIdle, &cmdIdle, &brkpt);
    OSReport("overhi = %s\n", overhi ? "true" : "false");
    OSReport("underlow = %s\n", underlow ? "true" : "false");
    OSReport("readIdle = %s\n", readIdle ? "true" : "false");
    OSReport("cmdIdle = %s\n", cmdIdle ? "true" : "false");
    OSReport("brkpt = %s\n", brkpt ? "true" : "false");

    u16 token = GXReadDrawSync();
    const char* view = fn_803640E8(fn_80364020(), token);
    const char* name = fn_803640F0(fn_80364020(), token);
    OSReport("current view = %s\n", view);
    OSReport("token string = %s\n", name);
}

static void WaitDrawDone()
{
    static u8 glx_bAllowAbortFrame = true;

    if (glx_DrawSyncTimeout == 0.0f)
    {
        GXWaitDrawDone();
        return;
    }

    if (glx_bAborted != 0)
    {
        return;
    }

    u32 start = nlGetTicker();
    while (glx_bAborted == 0)
    {
        if (nlGetTickerDifference(start, nlGetTicker()) > glx_DrawSyncTimeout)
        {
            glx_bAborted = 1;
            OSReport("Warning: Hung in WaitDrawDone, had to abort frame.\n");
            ReportGPStatus();
            if (glx_bAllowAbortFrame != 0)
            {
                GXAbortFrame();
                gxInit();
                return;
            }
            nlBreak__Fv();
        }
    }
}

static void swap_Post()
{
    GXSetDrawDone();
    if (glx_DrawSyncTimeout == 0.0f)
    {
        GXWaitDrawDone();
    }
    else
    {
        WaitDrawDone();
    }

    // R4QE01 tests the four swap modes in this order and keeps no jump table.
    // The stripped DOL does not preserve their names, so they stay numeric.
    u32 retrace = VIGetRetraceCount();
    if (glx_SwapMode == 2)
    {
        AdvanceFrame();
        VIWaitForRetrace();
        if (VIGetRetraceCount() < glx_nRetraceAtSwap + 2)
        {
            VIWaitForRetrace();
        }
    }
    else if (glx_SwapMode == 1)
    {
        AdvanceFrame();
        VIWaitForRetrace();
    }
    else if (glx_SwapMode == 3)
    {
        AdvanceFrame();
        if (OSTicksToMicroseconds(OSGetTick() - (u32)glx_LastRetraceTick) > 0x2EE0 || retrace == glx_nRetraceAtSwap)
        {
            VIWaitForRetrace();
        }
    }
    else if (glx_SwapMode == 0)
    {
        AdvanceFrame();
    }
    else
    {
        nlBreak__Fv();
    }

    glxSwapBuffers();
}

static void swap_Pre()
{
    if (glx_ScreenShot != 0)
    {
        glx_ScreenCapture(false);
        glx_ScreenShot = 0;
    }
    if (glx_MovieOutput != 0)
    {
        glx_ScreenCapture(true);
    }

    GXSetCopyClear(glx_ClearColour, 0xFFFFFF);
    gxSetZMode(true, GX_LEQUAL, true);
    gxSetColourUpdate(true);
    gxSetAlphaUpdate(true);
    GXCopyDisp(glx_FrameBuffer[glx_nBuffer], 1);
    GXSetDrawDone();
    glx_bAborted = 0;
    GXFlush();
    glx_nRetraceAtSwap = VIGetRetraceCount();
}
