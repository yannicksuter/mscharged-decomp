#include "Game/PadActions.h"

#include "Game/Event.h"
#include "Game/PadMonkey.h"
#include "Game/TweakRegistry.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/MemAlloc.h"
#include "NL/glx/GXMaterialShadowTweaks.h"
#include "NL/globalpad.h"
#include "NL/nlFormat.h"
#include "NL/nlMemory.h"
#include "NL/platpad.h"
#include "types.h"

extern bool g_bEnableGamecubePadMonkey;
extern MemoryAllocator* AllocatorStack[16];
extern unsigned int AllocatorStackDepth;
extern s32* lbl_806E2278;
extern s32* lbl_806E2288;
extern s32* lbl_806E22A8;

extern "C"
{
    void fn_80375288(void* state);
}

struct PadUpdateState_80137B40
{
    PadUpdateState_80137B40()
        : mUnidentified304(false)
        , mUnidentified305(false)
        , mDeviceChanged()
    {
    }

    /* 0x000 */ u8 mUnidentified000[0x304];
    /* 0x304 */ bool mUnidentified304;
    /* 0x305 */ bool mUnidentified305;
    /* 0x306 */ u8 mUnidentified306[0xE];
    /* 0x314 */ DeviceChangedEvent_80137B40 mDeviceChanged;
}; // size 0x3B8

extern PadUpdateState_80137B40* g_pPlatPadManager;

bool g_bEnableGamecubePadMonkey;

int PadMonkey::GetButtonMask(int buttonIndex)
{
    return fn_802C06C8(buttonIndex);
}

s32 g_pPadRemapArray[51] = {
    0x00000020, 0x00000040, 0x00000800, 0x00000400, 0x00000001, 0x00000040, 0x00000020, 0x00000100, 0x00000010, 0x00000100, 0x00000200, 0x00000001, 0x00000002, 0x00000008, 0x00000004, 0x00000100, 0x00000200, 0x00000400, 0x00000800, 0x00001000, 0x00000020, 0x00000400, 0x00000010, 0x00000040, 0x00000800, 0x00000200, 0x00000100, 0x00000100, 0x00000200, 0x00000800, 0x00000100, 0x00000200, 0x00001000, 0x00000800, 0x00000800, 0x00000800, 0x00000200, 0x00000800, 0x00000040, 0x00000020, 0x00000800, 0x00000400, 0x00000040, 0x00000020, 0x00000040, 0x00000020, 0x00000010, 0x00001000, 0x00000020, 0x00000040, 0x00001000
};

static s32 remapArray_8050DB2C[51] = {
    0x00000000, 0x00000000, 0x00000100, 0x00000200, 0x00000010, 0x00000000, 0x00000000, 0x00000800, 0x00001000, 0x00000200, 0x00000100, 0x00000001, 0x00000002, 0x00000008, 0x00000004, 0x00000800, 0x00000400, 0x00000200, 0x00000100, 0x00000010, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000800, 0x00000800, 0x00000400, 0x00000000, 0x00000800, 0x00000400, 0x00000010, 0x00000100, 0x00000100, 0x00000100, 0x00000400, 0x00000200, 0x00002000, 0x00004000, 0x00000200, 0x00000100, 0x00004000, 0x00002000, 0x00002000, 0x00004000, 0x00008000, 0x00000200, 0x00000010, 0x00001000, 0x00000010
};

static s32 remapArray_8050DBF8[51] = {
    0x00004000, 0x00002000, 0x00002000, 0x00004000, 0x00000010, 0x00000001, 0x00000002, 0x00000800, 0x00001000, 0x00002000, 0x00000400, 0x00000001, 0x00000002, 0x00000008, 0x00000004, 0x00000800, 0x00000400, 0x00000200, 0x00000100, 0x00000010, 0x00002000, 0x00004000, 0x00000000, 0x00002000, 0x00000000, 0x00000000, 0x00000800, 0x00000800, 0x00000400, 0x00000000, 0x00000800, 0x00000400, 0x00000010, 0x00000100, 0x00000100, 0x00000100, 0x00000400, 0x00000200, 0x00002000, 0x00004000, 0x00000200, 0x00000100, 0x00004000, 0x00002000, 0x00002000, 0x00004000, 0x00008000, 0x00000200, 0x00000010, 0x00001000, 0x00000010
};

static s32 remapArray_8050DCC4[51] = {
    0x00004000, 0x00002000, 0x00000100, 0x00000200, 0x00000010, 0x00002000, 0x00004000, 0x00000800, 0x00001000, 0x00002000, 0x00000400, 0x00000001, 0x00000002, 0x00000008, 0x00000004, 0x00000800, 0x00000400, 0x00000200, 0x00000100, 0x00000010, 0x00002000, 0x00004000, 0x00000000, 0x00002000, 0x00000000, 0x00000400, 0x00000800, 0x00000800, 0x00000400, 0x00000000, 0x00000800, 0x00000400, 0x00000010, 0x00000100, 0x00000100, 0x00000100, 0x00000400, 0x00000200, 0x00002000, 0x00004000, 0x00000200, 0x00000100, 0x00004000, 0x00002000, 0x00002000, 0x00004000, 0x00008000, 0x00000200, 0x00000010, 0x00001000, 0x00000010
};

static s32 remapArray_8050DD90[51] = {
    0x00004000, 0x00002000, 0x00000100, 0x00000200, 0x00000010, 0x00002000, 0x00004000, 0x00000800, 0x00001000, 0x00002000, 0x00000400, 0x00000001, 0x00000002, 0x00000008, 0x00000004, 0x00000800, 0x00000400, 0x00000200, 0x00000100, 0x00000010, 0x00004000, 0x00002000, 0x00000000, 0x00004000, 0x00000000, 0x00000000, 0x00000800, 0x00000800, 0x00000400, 0x00000000, 0x00000800, 0x00000400, 0x00000010, 0x00000100, 0x00000100, 0x00000100, 0x00000400, 0x00000200, 0x00002000, 0x00004000, 0x00000200, 0x00000100, 0x00004000, 0x00002000, 0x00002000, 0x00004000, 0x00008000, 0x00000200, 0x00000010, 0x00001000, 0x00000010
};

static GXMaterialColourTweak_804FC520 lbl_8056FE90(
    "giControllerConfig", "Controller Config", 0);

void fn_80137824(bool useDefaultRemap)
{
    if (useDefaultRemap)
    {
        lbl_806E2288 = remapArray_8050DBF8;
        return;
    }

    switch (lbl_8056FE90.value)
    {
    case 1:
        lbl_806E2288 = remapArray_8050DCC4;
        break;
    case 2:
        lbl_806E2288 = remapArray_8050DD90;
        break;
    default:
        lbl_806E2288 = remapArray_8050DBF8;
        break;
    }
}

void fn_80137890()
{
    if (g_bEnableGamecubePadMonkey)
    {
        for (int padSet = 0; padSet < 2; ++padSet)
        {
            g_pPadManager->fn_802C084C(padSet);
            for (int padIndex = 0; padIndex < 4; ++padIndex)
            {
                PadMonkey* monkey = new (nlMalloc(0xFC, 8, false))
                    PadMonkey_80375EEC(padIndex);
                g_pPadManager->GetPad(padIndex)->mBackend = monkey;
            }
        }
    }
    else
    {
        for (int padSet = 0; padSet < 2; ++padSet)
        {
            g_pPadManager->fn_802C084C(padSet);
            for (int padIndex = 0; padIndex < 4; ++padIndex)
            {
                cPlatPad* pad = new cPlatPad(padIndex);
                g_pPadManager->GetPad(padIndex)->mBackend = pad;
            }
        }
    }
    g_pPadManager->fn_802C084C(0);
}

void fn_801379AC()
{
    for (int padSet = 0; padSet < 2; ++padSet)
    {
        g_pPadManager->fn_802C084C(padSet);
        for (int padIndex = 0; padIndex < 4; ++padIndex)
        {
            g_pPadManager->GetPad(padIndex)->StopRumble();
            delete g_pPadManager->GetPad(padIndex)->mBackend;
            g_pPadManager->GetPad(padIndex)->mBackend = 0;
        }
    }
    g_pPadManager->fn_802C084C(0);
}

void InitPads()
{
    if (g_pPadManager == 0)
    {
        g_pPadManager = new (8, false) PadManager_802C06D4;
    }

    g_pPadManager->fn_802C06D8(4, 2);
    g_pPadManager->fn_802C084C(0);
    lbl_806E22A8 = g_pPadRemapArray;
    lbl_806E2278 = remapArray_8050DB2C;

    switch (lbl_8056FE90.value)
    {
    case 1:
        lbl_806E2288 = remapArray_8050DCC4;
        break;
    case 2:
        lbl_806E2288 = remapArray_8050DD90;
        break;
    default:
        lbl_806E2288 = remapArray_8050DBF8;
        break;
    }
}

void fn_80137B40()
{
    CurrentAllocator = &VirtualAllocator;
    AllocatorStack[AllocatorStackDepth++] = &VirtualAllocator;

    if (g_pPlatPadManager == 0)
    {
        g_pPlatPadManager = new (nlMalloc(
            sizeof(PadUpdateState_80137B40), 8, false))
            PadUpdateState_80137B40;
    }

    fn_80375288(g_pPlatPadManager);
    g_pPlatPadManager->mUnidentified305 = true;

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
