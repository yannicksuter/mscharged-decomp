#include <revolution/os/OSThread.h>
#include "NL/plat/SocketNetwork.h"
#include "Game/Sys/debug.h"
#include <revolution/so.h>
#include <string.h>

#include "Game/TweakValue.h"
#include "NL/MemAlloc.h"
#include "NL/nlMemory.h"
#include "types.h"

int g_nHardcodeIPAddr[4] = { 0x42, 0x77, 0xA7, 0x68 };
int g_nHardcodeGatewayAddr[4] = { 0x42, 0x77, 0xA7, 0x61 };
int g_nHardcodedDNSAddr[4] = { 0x41, 0x27, 0x98, 0xED };

static bool sSocketMemoryInitialized;
static bool sSocketNetworkStarted;
static int sSocketNetworkLastError;
bool g_bHardcodeIP;
static void* sSocketMemoryPool;

namespace
{
extern MemoryAllocator sSocketAllocator;
extern OSThread sSocketStartupThread;
extern u8 sSocketStartupStack[0x4000];
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

static void* SocketAlloc(u32, s32 size)
{
    return sSocketAllocator.Allocate(size, 32, false);
}

static void SocketFree(u32, void* memory, s32)
{
    sSocketAllocator.Free(memory);
}

void SocketNetworkInitializeMemory()
{
    if (!sSocketMemoryInitialized)
    {
        PushAllocator(&VirtualAllocator);
        sSocketMemoryPool = nlMalloc(0x30D40, 8, false);
        sSocketAllocator.Initialize(sSocketMemoryPool, 0x30D40);
        sSocketMemoryInitialized = true;
        PopAllocator();
    }
}

void SocketNetworkStartup()
{
    if (!sSocketNetworkStarted)
    {
        if (!sSocketMemoryInitialized)
        {
            SocketNetworkInitializeMemory();
        }

        SOLibraryConfig config;
        memset(&config, 0, sizeof(config));
        config.alloc = SocketAlloc;
        config.free = SocketFree;

        sSocketNetworkLastError = SOInit(&config);
        if (sSocketNetworkLastError != 0)
        {
            tDebugPrintManager::Print(DC_NETWORK, "SOInit failed error %d\n", sSocketNetworkLastError);
        }
        else
        {
            sSocketNetworkLastError = SOStartup();
            if (sSocketNetworkLastError != 0)
            {
                tDebugPrintManager::Print(DC_NETWORK, "SOStartup failed error %d\n", sSocketNetworkLastError);
                SOFinish();
            }
            else
            {
                sSocketNetworkStarted = true;
            }
        }
    }
}

void SocketNetworkShutdown()
{
    if (sSocketNetworkStarted)
    {
        SOCleanup();
        SOFinish();
        sSocketNetworkStarted = false;
    }
}

bool SocketNetworkIsStarted()
{
    return sSocketNetworkStarted;
}

int SocketNetworkGetLastError()
{
    return sSocketNetworkLastError;
}

static void* SocketNetworkStartupThread(void*)
{
    SocketNetworkStartup();
    return 0;
}

void SocketNetworkStartupAsync()
{
    OSCreateThread(&sSocketStartupThread, SocketNetworkStartupThread, 0,
        sSocketStartupStack + sizeof(sSocketStartupStack), sizeof(sSocketStartupStack), 14,
        OS_THREAD_DETACHED);
    OSResumeThread(&sSocketStartupThread);
}

bool SocketNetworkIsStartupComplete()
{
    return OSIsThreadTerminated(&sSocketStartupThread) != 0;
}

static TweakValueBoolImpl_804F4538 sHardcodeIPTweak(
    "g_bHardcodeIP", "Network", &g_bHardcodeIP, true);
static TweakValueIntImpl_804FD898 sHardcodeIPAddr0Tweak(
    "g_nHardcodeIPAddr0", "Network", &g_nHardcodeIPAddr[0], true);
static TweakValueIntImpl_804FD898 sHardcodeIPAddr1Tweak(
    "g_nHardcodeIPAddr1", "Network", &g_nHardcodeIPAddr[1], true);
static TweakValueIntImpl_804FD898 sHardcodeIPAddr2Tweak(
    "g_nHardcodeIPAddr2", "Network", &g_nHardcodeIPAddr[2], true);
static TweakValueIntImpl_804FD898 sHardcodeIPAddr3Tweak(
    "g_nHardcodeIPAddr3", "Network", &g_nHardcodeIPAddr[3], true);
static TweakValueIntImpl_804FD898 sHardcodeGatewayAddr0Tweak(
    "g_nHardcodeGatewayAddr0", "Network", &g_nHardcodeGatewayAddr[0], true);
static TweakValueIntImpl_804FD898 sHardcodeGatewayAddr1Tweak(
    "g_nHardcodeGatewayAddr1", "Network", &g_nHardcodeGatewayAddr[1], true);
static TweakValueIntImpl_804FD898 sHardcodeGatewayAddr2Tweak(
    "g_nHardcodeGatewayAddr2", "Network", &g_nHardcodeGatewayAddr[2], true);
static TweakValueIntImpl_804FD898 sHardcodeGatewayAddr3Tweak(
    "g_nHardcodeGatewayAddr3", "Network", &g_nHardcodeGatewayAddr[3], true);
static TweakValueIntImpl_804FD898 sHardcodedDNSAddr0Tweak(
    "g_nHardcodedDNSAddr0", "Network", &g_nHardcodedDNSAddr[0], true);
static TweakValueIntImpl_804FD898 sHardcodedDNSAddr1Tweak(
    "g_nHardcodedDNSAddr1", "Network", &g_nHardcodedDNSAddr[1], true);
static TweakValueIntImpl_804FD898 sHardcodedDNSAddr2Tweak(
    "g_nHardcodedDNSAddr2", "Network", &g_nHardcodedDNSAddr[2], true);
static TweakValueIntImpl_804FD898 sHardcodedDNSAddr3Tweak(
    "g_nHardcodedDNSAddr3", "Network", &g_nHardcodedDNSAddr[3], true);

namespace
{
MemoryAllocator sSocketAllocator;
OSThread sSocketStartupThread;
u8 sSocketStartupStack[0x4000];
}
