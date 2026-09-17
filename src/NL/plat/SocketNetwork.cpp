#include "NL/plat/SocketNetwork.h"
#include <revolution/os/OSThread.h>
#include "Game/Sys/debug.h"
#include <revolution/so.h>
#include <string.h>

#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/MemAlloc.h"
#include "NL/nlMemory.h"
#include "types.h"

extern MemoryAllocator* AllocatorStack[16];
extern unsigned int AllocatorStackDepth;


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
extern u8 sSocketStartupThreadStack[0x4000];
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

static inline void InitializeSocketAllocatorImpl()
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

void* SocketAlloc(u32, s32 size)
{
    return sSocketAllocator.Allocate(size, 32, false);
}

void SocketFree(u32, void* memory, s32)
{
    sSocketAllocator.Free(memory);
}

void SocketNetworkInitializeMemory()
{
    InitializeSocketAllocatorImpl();
}

void SocketNetworkStartup()
{
    if (!sSocketNetworkStarted)
    {
        if (!sSocketMemoryInitialized)
        {
            InitializeSocketAllocatorImpl();
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

void* SocketNetworkStartupThread(void*)
{
    SocketNetworkStartup();
    return 0;
}

void SocketNetworkStartupAsync()
{
    OSCreateThread(&sSocketStartupThread, SocketNetworkStartupThread, 0,
        sSocketStartupThreadStack + sizeof(sSocketStartupThreadStack), sizeof(sSocketStartupThreadStack), 14,
        OS_THREAD_DETACHED);
    OSResumeThread(&sSocketStartupThread);
}

bool SocketNetworkIsStartupComplete()
{
    return OSIsThreadTerminated(&sSocketStartupThread) != 0;
}

static TweakBoolBinding sHardcodeIPTweak(
    "g_bHardcodeIP", "Network", &g_bHardcodeIP, true);
static TweakIntBinding sHardcodeIPAddr0Tweak(
    "g_nHardcodeIPAddr0", "Network", &g_nHardcodeIPAddr[0], true);
static TweakIntBinding sHardcodeIPAddr1Tweak(
    "g_nHardcodeIPAddr1", "Network", &g_nHardcodeIPAddr[1], true);
static TweakIntBinding sHardcodeIPAddr2Tweak(
    "g_nHardcodeIPAddr2", "Network", &g_nHardcodeIPAddr[2], true);
static TweakIntBinding sHardcodeIPAddr3Tweak(
    "g_nHardcodeIPAddr3", "Network", &g_nHardcodeIPAddr[3], true);
static TweakIntBinding sHardcodeGatewayAddr0Tweak(
    "g_nHardcodeGatewayAddr0", "Network", &g_nHardcodeGatewayAddr[0], true);
static TweakIntBinding sHardcodeGatewayAddr1Tweak(
    "g_nHardcodeGatewayAddr1", "Network", &g_nHardcodeGatewayAddr[1], true);
static TweakIntBinding sHardcodeGatewayAddr2Tweak(
    "g_nHardcodeGatewayAddr2", "Network", &g_nHardcodeGatewayAddr[2], true);
static TweakIntBinding sHardcodeGatewayAddr3Tweak(
    "g_nHardcodeGatewayAddr3", "Network", &g_nHardcodeGatewayAddr[3], true);
static TweakIntBinding sHardcodedDNSAddr0Tweak(
    "g_nHardcodedDNSAddr0", "Network", &g_nHardcodedDNSAddr[0], true);
static TweakIntBinding sHardcodedDNSAddr1Tweak(
    "g_nHardcodedDNSAddr1", "Network", &g_nHardcodedDNSAddr[1], true);
static TweakIntBinding sHardcodedDNSAddr2Tweak(
    "g_nHardcodedDNSAddr2", "Network", &g_nHardcodedDNSAddr[2], true);
static TweakIntBinding sHardcodedDNSAddr3Tweak(
    "g_nHardcodedDNSAddr3", "Network", &g_nHardcodedDNSAddr[3], true);

namespace
{
MemoryAllocator sSocketAllocator;
OSThread sSocketStartupThread;
u8 sSocketStartupThreadStack[0x4000];
}
