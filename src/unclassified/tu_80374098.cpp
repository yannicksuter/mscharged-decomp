#include <revolution/os/OSThread.h>
#include <revolution/so.h>
#include <string.h>

#include "Game/TweakValue.h"
#include "NL/MemAlloc.h"
#include "NL/nlMemory.h"
#include "types.h"

extern MemoryAllocator* AllocatorStack[16];
extern unsigned int AllocatorStackDepth;

extern "C" int fn_8004F594(int category, const char* format, ...);

int g_nHardcodeIPAddr[4] = { 0x42, 0x77, 0xA7, 0x68 };
int g_nHardcodeGatewayAddr[4] = { 0x42, 0x77, 0xA7, 0x61 };
int g_nHardcodedDNSAddr[4] = { 0x41, 0x27, 0x98, 0xED };

static bool lbl_806E2468;
static bool lbl_806E2469;
static int lbl_806E246C;
bool g_bHardcodeIP;
static void* lbl_806E2474;

namespace
{
extern MemoryAllocator lbl_8059C5D8;
extern OSThread lbl_8059C5F0;
extern u8 lbl_8059C908[0x4000];
}

static inline void PushAllocator(MemoryAllocator* pAllocator)
{
    AllocatorStack[AllocatorStackDepth++] = pAllocator;
    CurrentAllocator = pAllocator;
}

static inline void PopAllocator()
{
    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
}

static inline void fn_803740B8_impl()
{
    if (!lbl_806E2468)
    {
        PushAllocator(&VirtualAllocator);
        lbl_806E2474 = nlMalloc(0x30D40, 8, false);
        lbl_8059C5D8.Initialize(lbl_806E2474, 0x30D40);
        lbl_806E2468 = true;
        PopAllocator();
    }
}

extern "C" void* fn_80374098(u32, s32 size)
{
    return lbl_8059C5D8.Allocate(size, 32, false);
}

extern "C" void fn_803740AC(u32, void* memory, s32)
{
    lbl_8059C5D8.Free(memory);
}

extern "C" void fn_803740B8()
{
    fn_803740B8_impl();
}

extern "C" void fn_80374174()
{
    if (!lbl_806E2469)
    {
        if (!lbl_806E2468)
        {
            fn_803740B8_impl();
        }

        SOLibraryConfig config;
        memset(&config, 0, sizeof(config));
        config.alloc = fn_80374098;
        config.free = fn_803740AC;

        lbl_806E246C = SOInit(&config);
        if (lbl_806E246C != 0)
        {
            fn_8004F594(16, "SOInit failed error %d\n", lbl_806E246C);
        }
        else
        {
            lbl_806E246C = SOStartup();
            if (lbl_806E246C != 0)
            {
                fn_8004F594(
                    16, "SOStartup failed error %d\n", lbl_806E246C);
                SOFinish();
            }
            else
            {
                lbl_806E2469 = true;
            }
        }
    }
}

extern "C" void fn_803742D0()
{
    if (lbl_806E2469)
    {
        SOCleanup();
        SOFinish();
        lbl_806E2469 = false;
    }
}

extern "C" bool fn_80374308()
{
    return lbl_806E2469;
}

extern "C" int fn_80374310()
{
    return lbl_806E246C;
}

extern "C" void* fn_80374318(void*)
{
    fn_80374174();
    return 0;
}

extern "C" void fn_8037433C()
{
    OSCreateThread(&lbl_8059C5F0, fn_80374318, 0,
        lbl_8059C908 + sizeof(lbl_8059C908), sizeof(lbl_8059C908), 14,
        OS_THREAD_DETACHED);
    OSResumeThread(&lbl_8059C5F0);
}

extern "C" bool fn_80374398()
{
    return OSIsThreadTerminated(&lbl_8059C5F0) != 0;
}

static TweakValueBoolImpl_804F4538 lbl_8059C448(
    "Network", "g_bHardcodeIP", &g_bHardcodeIP, true);
static TweakValueIntImpl_804FD898 lbl_8059C468(
    "g_nHardcodeIPAddr0", "Network", &g_nHardcodeIPAddr[0], true);
static TweakValueIntImpl_804FD898 lbl_8059C488(
    "g_nHardcodeIPAddr1", "Network", &g_nHardcodeIPAddr[1], true);
static TweakValueIntImpl_804FD898 lbl_8059C4A8(
    "g_nHardcodeIPAddr2", "Network", &g_nHardcodeIPAddr[2], true);
static TweakValueIntImpl_804FD898 lbl_8059C4C8(
    "g_nHardcodeIPAddr3", "Network", &g_nHardcodeIPAddr[3], true);
static TweakValueIntImpl_804FD898 lbl_8059C4E8(
    "g_nHardcodeGatewayAddr0", "Network", &g_nHardcodeGatewayAddr[0], true);
static TweakValueIntImpl_804FD898 lbl_8059C508(
    "g_nHardcodeGatewayAddr1", "Network", &g_nHardcodeGatewayAddr[1], true);
static TweakValueIntImpl_804FD898 lbl_8059C528(
    "g_nHardcodeGatewayAddr2", "Network", &g_nHardcodeGatewayAddr[2], true);
static TweakValueIntImpl_804FD898 lbl_8059C548(
    "g_nHardcodeGatewayAddr3", "Network", &g_nHardcodeGatewayAddr[3], true);
static TweakValueIntImpl_804FD898 lbl_8059C568(
    "g_nHardcodedDNSAddr0", "Network", &g_nHardcodedDNSAddr[0], true);
static TweakValueIntImpl_804FD898 lbl_8059C588(
    "g_nHardcodedDNSAddr1", "Network", &g_nHardcodedDNSAddr[1], true);
static TweakValueIntImpl_804FD898 lbl_8059C5A8(
    "g_nHardcodedDNSAddr2", "Network", &g_nHardcodedDNSAddr[2], true);
static TweakValueIntImpl_804FD898 lbl_8059C5C8(
    "g_nHardcodedDNSAddr3", "Network", &g_nHardcodedDNSAddr[3], true);

namespace
{
MemoryAllocator lbl_8059C5D8;
OSThread lbl_8059C5F0;
u8 lbl_8059C908[0x4000];
}
