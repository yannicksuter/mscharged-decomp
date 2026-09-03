#include "unclassified/tu_802196B0.h"

#include "Game/FE/feInput.h"
#include "Game/TweakRegistry.h"
#include "NL/gl/glStruct.h"
#include "NL/globalpad.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "decomp.h"

class TLComponentInstance;

class TU802FA1C4
{
public:
    TU802FA1C4();
    ~TU802FA1C4();

    /* 0x00 */ u8 mUnidentified000[0x54];
    /* 0x54 */ int mUnidentified054;
}; // size 0x58

struct UnidentifiedPointerData
{
    /* 0x00 */ int mCount;
    /* 0x04 */ u8 mUnidentified004[8];
    /* 0x0C */ u16 mAngle;
    /* 0x0E */ u8 mUnidentified00E[0xA];
    /* 0x18 */ nlVector2 mPosition;
}; // size 0x20

typedef int (*GetDeviceType)(void* device);

extern TU802FA1C4* lbl_806E2030;
TLComponentInstance* lbl_80578450[4];
nlVector2 lbl_80578460[4];
bool lbl_806E18B4[4];
extern bool lbl_806E18B8;
extern bool g_bEnableGamecubePadMonkey;
extern void* lbl_806E1E28;
extern void* lbl_806E2478;
extern int lbl_806E227C;
extern int lbl_806E228C;
extern unsigned int nlDefaultSeed;

extern "C" cGlobalPad* fn_802C082C(void* owner, int pad);
extern "C" bool fn_80273B00();
extern "C" int fn_80364630(
    UnidentifiedPointerData* data, nlVector2* position, u16* angle);
extern "C" void fn_80375DF8(void* owner, int index, bool value);
extern "C" bool fn_80375E04(void* owner, int index);

UnidentifiedTask_802196B0::UnidentifiedTask_802196B0()
{
    if (lbl_806E2030 == 0)
    {
        lbl_806E2030 = new (nlMalloc(sizeof(TU802FA1C4), 8, false))
            TU802FA1C4;
    }

    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i] = 0;
        lbl_80578460[i].x = 0.0f;
        lbl_80578460[i].y = 0.0f;
        lbl_806E18B4[i] = false;
    }
    lbl_806E2030->mUnidentified054 = 30;
}

UnidentifiedTask_802196B0::~UnidentifiedTask_802196B0()
{
    if (lbl_806E2030 != 0)
    {
        delete lbl_806E2030;
        lbl_806E2030 = 0;
    }
}

extern "C" nlVector2 fn_802197FC(int pad, u8* valid)
{
    u16 angle;
    return fn_80219824(pad, &angle, valid);
}

extern "C" nlVector2 fn_80219824(int pad, u16* angle, u8* valid)
{
    cGlobalPad* globalPad = fn_802C082C(lbl_806E1E28, pad);
    void* device = *reinterpret_cast<void**>(
        reinterpret_cast<u8*>(globalPad) + 0x1C);
    nlVector2 position;
    nlVec2Set(position, 0.0f, 0.0f);

    gl_ScreenInfo* screenInfo = glGetScreenInfo();
    int width = fn_80273B00() ? 854 : screenInfo->ScreenWidth;
    int height = screenInfo->ScreenHeight;

    if (g_bEnableGamecubePadMonkey)
    {
        char path[32];
        nlSNPrintf(path, sizeof(path), "user/monkey_%d_offset", pad);
        float offset = fn_802C2B48(path, 50.0f);
        float randomX = nlRandomf(-offset, offset, &nlDefaultSeed);
        float randomY = nlRandomf(-offset, offset, &nlDefaultSeed);
        position.x = randomX + lbl_80578460[pad].x;
        position.y = randomY + lbl_80578460[pad].y;

        lbl_80578460[pad].x = CLAMP(
            (float)((-width / 2) + 20), (float)((width / 2) - 20), position.x);
        lbl_80578460[pad].y = CLAMP(
            (float)((-height / 2) + 5), (float)((height / 2) - 5), position.y);
        *angle = 0;
        *valid = true;
        return position;
    }

    UnidentifiedPointerData* data = 0;
    if (reinterpret_cast<GetDeviceType>(
            (*reinterpret_cast<void***>(device))[0x50 / sizeof(void*)])(device)
        == lbl_806E228C)
    {
        data = reinterpret_cast<UnidentifiedPointerData*>(
            *reinterpret_cast<u8**>(reinterpret_cast<u8*>(globalPad) + 0x1C)
            + 0x1D0);
    }
    else if (!lbl_806E18B8
             && reinterpret_cast<GetDeviceType>(
                    (*reinterpret_cast<void***>(device))[0x50 / sizeof(void*)])(device)
                    == lbl_806E227C)
    {
        data = reinterpret_cast<UnidentifiedPointerData*>(
            *reinterpret_cast<u8**>(reinterpret_cast<u8*>(globalPad) + 0x1C)
            + 0x1B0);
    }

    if (data != 0 && g_pFEInput->IsConnected((eFEINPUT_PAD)pad))
    {
        if (data->mCount > 0)
        {
            fn_80364630(data, &position, angle);
            position.x = -1.0f * (position.x * (float)width * 0.5f);
            position.y = position.y * (float)height * 0.5f;
            *valid = true;

            lbl_80578460[pad].x = CLAMP(
                (float)((-width / 2) + 20),
                (float)((width / 2) - 20),
                position.x);
            lbl_80578460[pad].y = CLAMP(
                (float)((-height / 2) + 5),
                (float)((height / 2) - 5),
                position.y);
        }
        else
        {
            *angle = 0;
            *valid = true;
        }
    }
    else
    {
        *valid = false;
        nlVec2Set(position, -999.0f, -999.0f);
        return position;
    }

    return lbl_80578460[pad];
}

void UnidentifiedTask_802196B0::Run(float)
{
    for (int i = 0; i < 4; ++i)
    {
        if (fn_802C082C(lbl_806E1E28, i) != 0
            && !fn_80375E04(lbl_806E2478, i))
        {
            fn_80375DF8(lbl_806E2478, i, true);
        }
    }
}

extern "C" void fn_80219E08(int, nlColour)
{
}

extern "C" bool fn_80219E0C(int index)
{
    cGlobalPad* globalPad = fn_802C082C(lbl_806E1E28, index);
    void* device = *reinterpret_cast<void**>(
        reinterpret_cast<u8*>(globalPad) + 0x1C);
    int type = reinterpret_cast<GetDeviceType>(
        (*reinterpret_cast<void***>(device))[0x50 / sizeof(void*)])(device);
    return type == lbl_806E228C;
}
