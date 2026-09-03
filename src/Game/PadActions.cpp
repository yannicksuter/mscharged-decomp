#include "Game/PadActions.h"

#include "Game/Event.h"
#include "Game/PadMonkey.h"
#include "Game/TweakRegistry.h"
#include "NL/MemAlloc.h"
#include "NL/glx/GXMaterialShadowTweaks.h"
#include "NL/globalpad.h"
#include "NL/nlFormat.h"
#include "NL/nlMemory.h"
#include "NL/platpad.h"
#include "types.h"

extern bool g_bEnableGamecubePadMonkey;
extern void* lbl_806E1E28;
extern MemoryAllocator* AllocatorStack[16];
extern unsigned int AllocatorStackDepth;
extern s32* lbl_806E2278;
extern s32* lbl_806E2288;
extern s32* lbl_806E22A8;

extern "C"
{
    void* fn_802C06D4(void* manager);
    void fn_802C06D8(void* manager, int padCount, int padSetCount);
    cGlobalPad* fn_802C082C(void* manager, int padIndex);
    void fn_802C084C(void* manager, int padSet);
    void fn_80375288(void* state);
}

struct DeviceChangedData_80137B40
{
};

class DeviceChangedEvent_80137B40
    : public UnidentifiedStaticEvent<DeviceChangedData_80137B40, 5>
{
public:
    DeviceChangedEvent_80137B40()
        : UnidentifiedStaticEvent<DeviceChangedData_80137B40, 5>(
              "DeviceChanged", -1)
    {
    }

    virtual ~DeviceChangedEvent_80137B40() { }
};

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

extern PadUpdateState_80137B40* lbl_806E2478;

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
            fn_802C084C(lbl_806E1E28, padSet);
            for (int padIndex = 0; padIndex < 4; ++padIndex)
            {
                PadMonkey* monkey = new (nlMalloc(0xFC, 8, false))
                    PadMonkey_80375EEC(padIndex);
                fn_802C082C(lbl_806E1E28, padIndex)->mBackend = monkey;
            }
        }
    }
    else
    {
        for (int padSet = 0; padSet < 2; ++padSet)
        {
            fn_802C084C(lbl_806E1E28, padSet);
            for (int padIndex = 0; padIndex < 4; ++padIndex)
            {
                cPlatPad* pad = new cPlatPad(padIndex);
                fn_802C082C(lbl_806E1E28, padIndex)->mBackend = pad;
            }
        }
    }
    fn_802C084C(lbl_806E1E28, 0);
}

void fn_801379AC()
{
    for (int padSet = 0; padSet < 2; ++padSet)
    {
        fn_802C084C(lbl_806E1E28, padSet);
        for (int padIndex = 0; padIndex < 4; ++padIndex)
        {
            fn_802C082C(lbl_806E1E28, padIndex)->StopRumble();
            delete fn_802C082C(lbl_806E1E28, padIndex)->mBackend;
            fn_802C082C(lbl_806E1E28, padIndex)->mBackend = 0;
        }
    }
    fn_802C084C(lbl_806E1E28, 0);
}

void InitPads()
{
    if (lbl_806E1E28 == 0)
    {
        void* manager = nlMalloc(0x10, 8, false);
        if (manager != 0)
        {
            manager = fn_802C06D4(manager);
        }
        lbl_806E1E28 = manager;
    }

    fn_802C06D8(lbl_806E1E28, 4, 2);
    fn_802C084C(lbl_806E1E28, 0);
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

    if (lbl_806E2478 == 0)
    {
        lbl_806E2478 = new (nlMalloc(
            sizeof(PadUpdateState_80137B40), 8, false))
            PadUpdateState_80137B40;
    }

    fn_80375288(lbl_806E2478);
    lbl_806E2478->mUnidentified305 = true;

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
            = (PadMonkey*)fn_802C082C(lbl_806E1E28, j)->mBackend;
        NLString monkeyPad = Format<NLString, int, int>(
            NLString("user/{0}_pad_monkey_{1}_"), monkeySet, j);

        monkey->m_unk_0x34
            = fn_802C2B48(monkeyPad.Append("connected").c_str(), 100.0f);
        monkey->SetButtonChance(0x0001,
            fn_802C2B48(monkeyPad.Append("button_left").c_str(), 10.0f));
        monkey->SetButtonChance(0x0002,
            fn_802C2B48(monkeyPad.Append("button_right").c_str(), 10.0f));
        monkey->SetButtonChance(0x0004,
            fn_802C2B48(monkeyPad.Append("button_down").c_str(), 10.0f));
        monkey->SetButtonChance(0x0008,
            fn_802C2B48(monkeyPad.Append("button_up").c_str(), 10.0f));
        monkey->SetButtonChance(0x0010,
            fn_802C2B48(monkeyPad.Append("button_plus").c_str(), 10.0f));
        monkey->SetButtonChance(0x0100,
            fn_802C2B48(monkeyPad.Append("button_2").c_str(), 10.0f));
        monkey->SetButtonChance(0x0200,
            fn_802C2B48(monkeyPad.Append("button_1").c_str(), 10.0f));
        monkey->SetButtonChance(0x0400,
            fn_802C2B48(monkeyPad.Append("button_b").c_str(), 10.0f));
        monkey->SetButtonChance(0x0800,
            fn_802C2B48(monkeyPad.Append("button_a").c_str(), 10.0f));
        monkey->SetButtonChance(0x1000,
            fn_802C2B48(monkeyPad.Append("button_minus").c_str(), 10.0f));
        monkey->SetButtonChance(0x8000,
            fn_802C2B48(monkeyPad.Append("button_home").c_str(), 10.0f));
        monkey->SetButtonChance(0x2000,
            fn_802C2B48(monkeyPad.Append("button_z").c_str(), 10.0f));
        monkey->SetButtonChance(0x4000,
            fn_802C2B48(monkeyPad.Append("button_c").c_str(), 10.0f));

        monkey->Update(0.0f);
    }
}
