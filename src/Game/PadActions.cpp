#include "NL/plat/PlatPadManager.h"
#include "NL/plat/WiiPad.h"
#include "NL/plat/GameCubePad.h"
#include "Game/PadActions.h"

#include "Game/Event.h"
#include "Game/PadMonkey.h"
#include "Game/TweakRegistry.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/MemAlloc.h"
#include "Game/TweakValueInt.h"
#include "NL/globalpad.h"
#include "NL/nlFormat.h"
#include "NL/nlMemory.h"
#include "NL/platpad.h"
#include "NL/plat/WiiPad.h"
#include "types.h"

extern int* gGameCubePadButtonMap;

bool g_bEnableGamecubePadMonkey;

int PadMonkey::GetButtonMask(int buttonIndex)
{
    return GetPadButtonMask(buttonIndex);
}

int g_pPadRemapArray[51] = {
    0x00000020, 0x00000040, 0x00000800, 0x00000400, 0x00000001, 0x00000040, 0x00000020, 0x00000100, 0x00000010, 0x00000100, 0x00000200, 0x00000001, 0x00000002, 0x00000008, 0x00000004, 0x00000100, 0x00000200, 0x00000400, 0x00000800, 0x00001000, 0x00000020, 0x00000400, 0x00000010, 0x00000040, 0x00000800, 0x00000200, 0x00000100, 0x00000100, 0x00000200, 0x00000800, 0x00000100, 0x00000200, 0x00001000, 0x00000800, 0x00000800, 0x00000800, 0x00000200, 0x00000800, 0x00000040, 0x00000020, 0x00000800, 0x00000400, 0x00000040, 0x00000020, 0x00000040, 0x00000020, 0x00000010, 0x00001000, 0x00000020, 0x00000040, 0x00001000
};

static int sWiiRemoteButtonRemap[51] = {
    0x00000000, 0x00000000, 0x00000100, 0x00000200, 0x00000010, 0x00000000, 0x00000000, 0x00000800, 0x00001000, 0x00000200, 0x00000100, 0x00000001, 0x00000002, 0x00000008, 0x00000004, 0x00000800, 0x00000400, 0x00000200, 0x00000100, 0x00000010, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000800, 0x00000800, 0x00000400, 0x00000000, 0x00000800, 0x00000400, 0x00000010, 0x00000100, 0x00000100, 0x00000100, 0x00000400, 0x00000200, 0x00002000, 0x00004000, 0x00000200, 0x00000100, 0x00004000, 0x00002000, 0x00002000, 0x00004000, 0x00008000, 0x00000200, 0x00000010, 0x00001000, 0x00000010
};

static int sDefaultFreestyleButtonRemap[51] = {
    0x00004000, 0x00002000, 0x00002000, 0x00004000, 0x00000010, 0x00000001, 0x00000002, 0x00000800, 0x00001000, 0x00002000, 0x00000400, 0x00000001, 0x00000002, 0x00000008, 0x00000004, 0x00000800, 0x00000400, 0x00000200, 0x00000100, 0x00000010, 0x00002000, 0x00004000, 0x00000000, 0x00002000, 0x00000000, 0x00000000, 0x00000800, 0x00000800, 0x00000400, 0x00000000, 0x00000800, 0x00000400, 0x00000010, 0x00000100, 0x00000100, 0x00000100, 0x00000400, 0x00000200, 0x00002000, 0x00004000, 0x00000200, 0x00000100, 0x00004000, 0x00002000, 0x00002000, 0x00004000, 0x00008000, 0x00000200, 0x00000010, 0x00001000, 0x00000010
};

static int sFreestyleButtonRemapConfig1[51] = {
    0x00004000, 0x00002000, 0x00000100, 0x00000200, 0x00000010, 0x00002000, 0x00004000, 0x00000800, 0x00001000, 0x00002000, 0x00000400, 0x00000001, 0x00000002, 0x00000008, 0x00000004, 0x00000800, 0x00000400, 0x00000200, 0x00000100, 0x00000010, 0x00002000, 0x00004000, 0x00000000, 0x00002000, 0x00000000, 0x00000400, 0x00000800, 0x00000800, 0x00000400, 0x00000000, 0x00000800, 0x00000400, 0x00000010, 0x00000100, 0x00000100, 0x00000100, 0x00000400, 0x00000200, 0x00002000, 0x00004000, 0x00000200, 0x00000100, 0x00004000, 0x00002000, 0x00002000, 0x00004000, 0x00008000, 0x00000200, 0x00000010, 0x00001000, 0x00000010
};

static int sFreestyleButtonRemapConfig2[51] = {
    0x00004000, 0x00002000, 0x00000100, 0x00000200, 0x00000010, 0x00002000, 0x00004000, 0x00000800, 0x00001000, 0x00002000, 0x00000400, 0x00000001, 0x00000002, 0x00000008, 0x00000004, 0x00000800, 0x00000400, 0x00000200, 0x00000100, 0x00000010, 0x00004000, 0x00002000, 0x00000000, 0x00004000, 0x00000000, 0x00000000, 0x00000800, 0x00000800, 0x00000400, 0x00000000, 0x00000800, 0x00000400, 0x00000010, 0x00000100, 0x00000100, 0x00000100, 0x00000400, 0x00000200, 0x00002000, 0x00004000, 0x00000200, 0x00000100, 0x00004000, 0x00002000, 0x00002000, 0x00004000, 0x00008000, 0x00000200, 0x00000010, 0x00001000, 0x00000010
};

static TweakValueInt sControllerConfig(
    "giControllerConfig", "Controller Config", 0);

void UseDefaultFreestyleButtonRemap(bool useDefaultRemap)
{
    if (useDefaultRemap)
    {
        gWiiFreestyleButtonRemap = sDefaultFreestyleButtonRemap;
        return;
    }

    switch (sControllerConfig.value)
    {
    case 1:
        gWiiFreestyleButtonRemap = sFreestyleButtonRemapConfig1;
        break;
    case 2:
        gWiiFreestyleButtonRemap = sFreestyleButtonRemapConfig2;
        break;
    default:
        gWiiFreestyleButtonRemap = sDefaultFreestyleButtonRemap;
        break;
    }
}

void CreatePadBackends()
{
    if (g_bEnableGamecubePadMonkey)
    {
        for (int padSet = 0; padSet < 2; ++padSet)
        {
            g_pPadManager->SetActivePadSet(padSet);
            for (int padIndex = 0; padIndex < 4; ++padIndex)
            {
                PadMonkey* monkey = new (nlMalloc(0xFC, 8, false))
                    WiiPadMonkey(padIndex);
                g_pPadManager->GetPad(padIndex)->mBackend = monkey;
            }
        }
    }
    else
    {
        for (int padSet = 0; padSet < 2; ++padSet)
        {
            g_pPadManager->SetActivePadSet(padSet);
            for (int padIndex = 0; padIndex < 4; ++padIndex)
            {
                cPlatPad* pad = new cPlatPad(padIndex);
                g_pPadManager->GetPad(padIndex)->mBackend = pad;
            }
        }
    }
    g_pPadManager->SetActivePadSet(0);
}

void DestroyPadBackends()
{
    for (int padSet = 0; padSet < 2; ++padSet)
    {
        g_pPadManager->SetActivePadSet(padSet);
        for (int padIndex = 0; padIndex < 4; ++padIndex)
        {
            g_pPadManager->GetPad(padIndex)->StopRumble();
            delete g_pPadManager->GetPad(padIndex)->mBackend;
            g_pPadManager->GetPad(padIndex)->mBackend = 0;
        }
    }
    g_pPadManager->SetActivePadSet(0);
}

void InitPads()
{
    if (g_pPadManager == 0)
    {
        g_pPadManager = new (8, false) PadManager;
    }

    g_pPadManager->Initialize(4, 2);
    g_pPadManager->SetActivePadSet(0);
    gGameCubePadButtonMap = g_pPadRemapArray;
    gWiiRemoteButtonRemap = sWiiRemoteButtonRemap;

    switch (sControllerConfig.value)
    {
    case 1:
        gWiiFreestyleButtonRemap = sFreestyleButtonRemapConfig1;
        break;
    case 2:
        gWiiFreestyleButtonRemap = sFreestyleButtonRemapConfig2;
        break;
    default:
        gWiiFreestyleButtonRemap = sDefaultFreestyleButtonRemap;
        break;
    }
}

void InitPlatPad()
{
    CurrentAllocator = &VirtualAllocator;
    AllocatorStack[AllocatorStackDepth++] = &VirtualAllocator;

    if (g_pPlatPadManager == 0)
    {
        g_pPlatPadManager = new (nlMalloc(
            sizeof(PlatPadManager), 8, false))
            PlatPadManager;
    }

    g_pPlatPadManager->Initialize();
    g_pPlatPadManager->disableClassic = true;

    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
}

void UpdateMonkeyState(int monkeySet)
{
    if (!g_bEnableGamecubePadMonkey)
    {
        return;
    }

    for (int j = 0; j < 4; ++j)
    {
        PadMonkey* monkey
            = (PadMonkey*)g_pPadManager->GetPad(j)->mBackend;
        NLString monkeyPad = Format<NLString, int, int>(
            NLString("user/{0}_pad_monkey_{1}_"), monkeySet, j);

        monkey->m_unk_0x34
            = GetTweakFloat(monkeyPad.Append("connected").c_str(), 100.0f);
        monkey->SetButtonChance(0x0001,
            GetTweakFloat(monkeyPad.Append("button_left").c_str(), 10.0f));
        monkey->SetButtonChance(0x0002,
            GetTweakFloat(monkeyPad.Append("button_right").c_str(), 10.0f));
        monkey->SetButtonChance(0x0004,
            GetTweakFloat(monkeyPad.Append("button_down").c_str(), 10.0f));
        monkey->SetButtonChance(0x0008,
            GetTweakFloat(monkeyPad.Append("button_up").c_str(), 10.0f));
        monkey->SetButtonChance(0x0010,
            GetTweakFloat(monkeyPad.Append("button_plus").c_str(), 10.0f));
        monkey->SetButtonChance(0x0100,
            GetTweakFloat(monkeyPad.Append("button_2").c_str(), 10.0f));
        monkey->SetButtonChance(0x0200,
            GetTweakFloat(monkeyPad.Append("button_1").c_str(), 10.0f));
        monkey->SetButtonChance(0x0400,
            GetTweakFloat(monkeyPad.Append("button_b").c_str(), 10.0f));
        monkey->SetButtonChance(0x0800,
            GetTweakFloat(monkeyPad.Append("button_a").c_str(), 10.0f));
        monkey->SetButtonChance(0x1000,
            GetTweakFloat(monkeyPad.Append("button_minus").c_str(), 10.0f));
        monkey->SetButtonChance(0x8000,
            GetTweakFloat(monkeyPad.Append("button_home").c_str(), 10.0f));
        monkey->SetButtonChance(0x2000,
            GetTweakFloat(monkeyPad.Append("button_z").c_str(), 10.0f));
        monkey->SetButtonChance(0x4000,
            GetTweakFloat(monkeyPad.Append("button_c").c_str(), 10.0f));

        monkey->Update(0.0f);
    }
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
