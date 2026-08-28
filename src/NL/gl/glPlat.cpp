#include "NL/gl/glPlat.h"
#include "NL/gl/glView.h"

#include "NL/glx/glxGX.h"
#include "NL/glx/glxSend.h"
#include "NL/glx/glxSwap.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlFunction.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"

struct PlatformViewport
{
    s32 x;
    s32 y;
    s32 width;
    s32 height;
};

class PlatformRenderTarget
{
public:
    virtual void Reserved0() = 0;
    virtual void Begin(unsigned long mode) = 0;
    virtual void Reserved2() = 0;
    virtual void Reserved3() = 0;
    virtual void Reserved4() = 0;
    virtual void Reserved5() = 0;
    virtual void Configure(bool flag0, bool flag1, bool flag2) = 0;
};

class PlatformStartupObject
{
public:
    virtual void Reserved0() = 0;
    virtual void Reserved1() = 0;
    virtual void Reserved2() = 0;
    virtual void Reserved3() = 0;
    virtual void Reserved4() = 0;
    virtual void Reserved5() = 0;
    virtual void Startup() = 0;
};

struct PlatformStartupEntry
{
    PlatformStartupObject* object;
    void* key;
};

struct UnidentifiedStaticState
{
    UnidentifiedStaticState()
        : value(0)
    {
    }

    void* value;
};

template <typename T>
struct UnidentifiedStaticStorage
{
    static UnidentifiedStaticState state;
};

struct UnidentifiedStaticTag;

extern "C"
{
    void DCFlushRange(void* address, u32 length);
    void GXSetMisc(s32 token, s32 value);
    void GXSetCopyFilter(u8 aa, const u8 samplePattern[12][2], u8 vf, const u8 vfilter[7]);
    void GXFlush();
    void* GXInit(void* fifo, u32 size);

    void VIConfigure(GXRenderModeObj* mode);
    void VIFlush();
    void VIWaitForRetrace();
    u32 VIGetTvFormat();
    u32 VIGetDTVStatus();
    void VISetNextFrameBuffer(void* framebuffer);

    void SCInit();
    u32 SCCheckStatus();
    u8 SCGetProgressiveMode();
    u8 SCGetEuRgb60Mode();
    void OSReport(const char* format, ...);

    bool fn_802C2DBC(const char* path);
    s32 fn_802C2BE8(const char* path, s32 defaultValue);
    void fn_803A4084(const GXRenderModeObj* source, GXRenderModeObj* destination, u16 horizontal, u16 vertical);
    f32 fn_803A43C4(u16 efbHeight, u16 xfbHeight);
    void fn_803A7828(f32 x, f32 y, f32 width, f32 height, f32 nearZ, f32 farZ);
    void fn_803A78A4(u32 x, u32 y, u32 width, u32 height);
    void fn_803A41BC(u16 left, u16 top, u16 width, u16 height);
    void fn_803A423C(u16 width, u16 height);
    void fn_803A45F4(f32 scale);
    void fn_803A491C(s32 gamma);
    void fn_803A6FE8(u8 fieldMode, u8 halfAspectRatio);
    void fn_803A1D10(void* fifo, u32 highWatermark, u32 lowWatermark);
    void* fn_80372B30(u32 size, s32 arena);
    void fn_8004F594(s32 category, const char* format, ...);
    void fn_8036D89C();
    void fn_80376150();

    void fn_802CB848(Function2<bool, PlatformStartupEntry&, PlatformStartupEntry&>* callback);
    void* fn_80364020();
    void fn_803640E4(void* owner, u32 name);
    void fn_803640DC();
    void fn_8036DF24(PlatformRenderTarget* target, bool flag0, bool flag1);

    extern GXRenderModeObj lbl_8053BC30;
    extern GXRenderModeObj lbl_8053BC6C;
    extern GXRenderModeObj lbl_8053BCA8;
    extern GXRenderModeObj lbl_8053BCE4;
    extern GXRenderModeObj lbl_8053BD5C;
    extern GXRenderModeObj lbl_8053BD98;
    extern GXRenderModeObj lbl_8053BDD4;
}

static GXRenderModeObj glPal480IntDf = {
    4,
    640,
    480,
    542,
    40,
    16,
    640,
    542,
    1,
    0,
    0,
    { 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6 },
    { 8, 8, 10, 12, 10, 8, 8 }
};

GXRenderModeObj glx_rmode;
static PlatformViewport glx_viewport;

static f32 glx_CopyDispScaleFactor = 1.0f;
static u32 glx_TargetFPS = 60;
static s32 glx_VIWidth = 680;
static bool glx_Widescreen = true;
static u32 glx_FIFOSize = 0x80000;
static u32 glx_ClearPixel = 0x10801080;

static s32 glx_VideoMode;
static s32 prev_VIWidth = glx_VIWidth;
static void* glx_FIFOMem;
static void* glx_FIFO;
static void* glx_FrameBuffer[2];
static s32 glx_FBSize;

static inline void ClearXFBInline(void* framebuffer)
{
    s32 offset = 0;
    u8* destination = (u8*)framebuffer;
    while (offset < glx_FBSize)
    {
        *(u32*)destination = glx_ClearPixel;
        destination += 4;
        offset += 4;
    }
    DCFlushRange(framebuffer, glx_FBSize);
}

extern "C" u32 fn_80369394()
{
    return glx_rmode.fbWidth;
}

extern "C" u32 fn_803693A4()
{
    return glx_rmode.efbHeight;
}

extern "C" s32 fn_803693B4()
{
    return glx_VideoMode;
}

extern "C" u32 fn_803693BC()
{
    return glx_VIWidth;
}

void glx_ClearXFB(void* framebuffer)
{
    ClearXFBInline(framebuffer);
}

static void glx_InitGX()
{
    gxInit();
    GXSetMisc(1, 8);
    GXRenderModeObj& mode = glx_rmode;
    fn_803A7828(0.0f, 0.0f, (f32)mode.fbWidth, (f32)mode.efbHeight, 0.0f, 1.0f);
    fn_803A78A4(0, 0, mode.fbWidth, mode.efbHeight);
    fn_803A41BC(0, 0, mode.fbWidth, mode.efbHeight);
    fn_803A423C(mode.fbWidth, mode.xfbHeight);
    fn_803A45F4(glx_CopyDispScaleFactor);
    GXSetCopyFilter(mode.aa, mode.sample_pattern, true, mode.vfilter);
    fn_803A6FE8(true, false);
    gxSetDither(true);
    gxSetColourUpdate(true);
    gxSetAlphaUpdate(true);
    fn_803A491C(0);

    for (int stage = 0; stage < 16; stage++)
    {
        gxSetTevColourOp(stage, 0, 0, 0, true, 0);
        gxSetTevAlphaOp(stage, 0, 0, 0, true, 0);
    }
    GXFlush();
}

bool glplatPreStartup()
{
    return true;
}

static bool StartupObject(PlatformStartupEntry&, PlatformStartupEntry& entry)
{
    entry.object->Startup();
    return true;
}

extern "C" void fn_80369574()
{
    Function2<bool, PlatformStartupEntry&, PlatformStartupEntry&> callback(StartupObject);
    fn_802CB848(&callback);
}

bool glplatStartup(gl_ScreenInfo* screenInfo)
{
    SCInit();
    while (SCCheckStatus() == 1)
    {
    }

    if (fn_802C2DBC("/user/gpu fifo"))
    {
        glx_FIFOSize = (u32)fn_802C2BE8("/user/gpu fifo", 0) << 10;
    }

    screenInfo->ScreenWidth = 640;
    screenInfo->ScreenHeight = 448;
    screenInfo->ColourDepth[0] = 6;
    screenInfo->ColourDepth[1] = 6;
    screenInfo->ColourDepth[2] = 6;
    screenInfo->ColourDepth[3] = 6;
    screenInfo->ZDepth = 24;
    screenInfo->StencilDepth = 0;
    screenInfo->PixelCentre = 0.0f;
    screenInfo->FSAA = false;
    glx_CopyDispScaleFactor = 1.0f;

    GXRenderModeObj* renderMode;
    u32 tvFormat = VIGetTvFormat();
    switch (tvFormat)
    {
    case 0:
        glx_VideoMode = 0;
        renderMode = &lbl_8053BC30;
        break;
    case 1:
        glx_VideoMode = 1;
        renderMode = &glPal480IntDf;
        break;
    case 2:
        glx_VideoMode = 2;
        renderMode = &lbl_8053BCE4;
        break;
    case 5:
        glx_VideoMode = 4;
        renderMode = &lbl_8053BD5C;
        break;
    default:
        extern void nlBreak();
        nlBreak();
        break;
    }

    tvFormat = VIGetTvFormat();
    if (SCGetProgressiveMode() == 1 && VIGetDTVStatus() == 1)
    {
        if (glx_VideoMode == 0)
        {
            renderMode = glx_Widescreen ? &lbl_8053BCA8 : &lbl_8053BC6C;
            OSReport("Setting Progressive NTSC Mode\n");
        }
        if (glx_VideoMode == 1 || tvFormat == 5)
        {
            renderMode = glx_Widescreen ? &lbl_8053BDD4 : &lbl_8053BD98;
            OSReport("Setting Progressive EURGB60 Mode\n");
        }
    }
    else if (tvFormat == 5 || SCGetEuRgb60Mode() == 1)
    {
        renderMode = &lbl_8053BD5C;
        OSReport("Setting Interlaced EURGB60 Mode\n");
    }

    fn_803A4084(renderMode, &glx_rmode, 0, 16);
    if (glx_VideoMode == 1)
    {
        glx_rmode.efbHeight = 448;
        glx_CopyDispScaleFactor = fn_803A43C4(448, glx_rmode.xfbHeight);
        glx_TargetFPS = 50;
    }
    else
    {
        glx_TargetFPS = 60;
        glx_CopyDispScaleFactor = 1.0f;
    }

    glx_rmode.viWidth = (u16)glx_VIWidth;
    glx_rmode.viXOrigin = (u16)((720 - glx_VIWidth) / 2);
    VIConfigure(&glx_rmode);
    VIFlush();
    VIConfigure(&glx_rmode);

    glx_FIFOMem = fn_80372B30(glx_FIFOSize, 0);
    if (glx_FIFOMem == 0)
    {
        return false;
    }
    glx_FIFO = GXInit(glx_FIFOMem, glx_FIFOSize);
    fn_803A1D10(glx_FIFO, glx_FIFOSize - 0x10000, glx_FIFOSize - 0x40000);

    u32 fbSize = ((glx_rmode.fbWidth + 15) & 0xFFF0) * glx_rmode.xfbHeight * 2;
    if (fbSize < 0x9F600)
    {
        fbSize = 0x9F600;
    }
    u32 totalSize = fbSize * 2;
    void* framebufferMemory = nlMalloc(totalSize, 32, false);
    glx_FrameBuffer[0] = framebufferMemory;
    glx_FrameBuffer[1] = (u8*)framebufferMemory + fbSize;
    glx_FBSize = fbSize;
    ClearXFBInline(glx_FrameBuffer[0]);
    ClearXFBInline(glx_FrameBuffer[1]);

    fn_8004F594(1, "%uKB used for FB and FIFO\n", totalSize >> 10, glx_FIFOSize >> 10);
    glx_InitGX();
    VISetNextFrameBuffer(glx_FrameBuffer[0]);
    glxSwapSetBlack(true);
    VIFlush();
    VIWaitForRetrace();
    if ((glx_rmode.tvInfo & 1) != 0)
    {
        VIWaitForRetrace();
    }
    glxInitSwap(glx_FrameBuffer[0], glx_FrameBuffer[1]);
    glxInitTex();
    fn_8036D89C();
    return true;
}

bool glplatPostStartup()
{
    return true;
}

void glplatBeginFrame()
{
    if (glx_VIWidth != prev_VIWidth)
    {
        const s32 widthDifference = 720 - glx_VIWidth;
        prev_VIWidth = glx_VIWidth;
        glx_rmode.viWidth = (u16)glx_VIWidth;
        glx_rmode.viXOrigin = (u16)(widthDifference / 2);
        VIConfigure(&glx_rmode);
        VIFlush();
    }
}

void glplatEndFrame()
{
}

extern "C" PlatformViewport* fn_80369A30()
{
    return &glx_viewport;
}

static void glx_SendViews()
{
    PlatformRenderTarget* target;
    GLView* view;

    glx_viewport.x = 0;
    glx_viewport.y = 0;
    glx_viewport.width = 640;
    glx_viewport.height = 448;
    fn_803A7828((f32)glx_viewport.x,
        (f32)glx_viewport.y,
        (f32)glx_viewport.width,
        (f32)glx_viewport.height,
        0.0f,
        1.0f);
    fn_803A78A4(0, 0, 640, 448);

    GLViewIterator iterator(&lbl_8057F250);
    for (; !iterator.IsDone(); iterator.Next())
    {
        view = iterator.Current();
        if (view->m_ViewportWidth != 0 && view->m_ViewportHeight != 0)
        {
            GLRenderPair renderPair = view->GetRenderPair();
            target = (PlatformRenderTarget*)renderPair.target;
            target->Begin(0);

            glx_viewport.x = view->m_ViewportX;
            glx_viewport.y = view->m_ViewportY;
            glx_viewport.width = view->m_ViewportWidth;
            glx_viewport.height = view->m_ViewportHeight;
            const s32 viewportHeight = view->m_ViewportHeight;
            const s32 viewportWidth = view->m_ViewportWidth;
            const s32 viewportY = view->m_ViewportY;
            const s32 viewportX = view->m_ViewportX;
            fn_803A7828((f32)viewportX, (f32)viewportY, (f32)viewportWidth, (f32)viewportHeight, 0.0f, 1.0f);
            fn_803A78A4(viewportX, viewportY, viewportWidth, viewportHeight);
            fn_803640E4(fn_80364020(), (u32)view->m_Name);

            if (view->m_ClearColour || view->m_Unknown33 || view->m_ClearDepth)
            {
                bool hasRenderTarget = false;
                if (renderPair.hash != 0)
                {
                    if (target != 0)
                    {
                        hasRenderTarget = true;
                    }
                }
                if (hasRenderTarget)
                {
                    target->Configure(view->m_Unknown33, view->m_ClearColour, view->m_ClearDepth);
                }
            }

            if (view->m_Visible)
            {
                view->Iterate(glx_SendFrame_cb);
                const s32 targetMode = view->m_Target;
                if (targetMode != 8)
                {
                    if (targetMode != 9)
                    {
                        if (targetMode != 10)
                        {
                            continue;
                        }
                    }
                }
                fn_8036DF24(target, targetMode != 8, targetMode == 10);
            }
        }
    }

    fn_80364020();
    fn_803640DC();
    glx_SendEnd();
}

void glplatSendFrame()
{
    glxSwapPost(true);
    glx_SendViews();
    glxSwapPre(true);
    fn_80376150();
}

void glplatAbortFrame()
{
    fn_80376150();
    glxSwapWaitDrawDone();
    VIWaitForRetrace();
}

void glplatFinish()
{
    glxSwapWaitDrawDone();
}

extern "C" u32 fn_80369D4C()
{
    return 640;
}

extern "C" u32 fn_80369D54()
{
    return 448;
}

extern "C" u32 fn_80369D5C()
{
    return 640;
}

extern "C" u32 fn_80369D64()
{
    return 480;
}

extern "C" void fn_80369D6C(GLView* view, const nlVector3& world, nlVector3& ndc)
{
    nlMatrix4 viewMatrix;
    nlMatrix4 projectionMatrix;
    nlVector3 viewPosition;

    view->m_Interface->GetViewMatrix(viewMatrix);
    view->m_Interface->GetProjectionMatrix(projectionMatrix);
    nlMultPosVectorMatrix(viewPosition, world, viewMatrix);
    nlMultPosVectorMatrix(ndc, viewPosition, projectionMatrix);

    const f32 reciprocalW = 1.0f / -viewPosition.z;
    ndc.x *= reciprocalW;
    ndc.y = -ndc.y * reciprocalW;
    ndc.z *= reciprocalW;
}

template <typename T>
UnidentifiedStaticState UnidentifiedStaticStorage<T>::state;

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
