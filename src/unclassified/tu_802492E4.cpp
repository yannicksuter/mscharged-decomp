#include "unclassified/tu_802492E4.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/DB/tu_8010A40C.h"
#include "Game/FE/feAsyncImage.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/Render/Presentation.h"
#include "Game/SH/SHSceneBase.h"
#include "NL/MemAlloc.h"
#include "NL/gl/glState.h"
#include "NL/nlAlgorithm.h"
#include "NL/nlBasicString.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalization.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "unclassified/tu_80216CB0.h"
#include "unclassified/tu_802196B0.h"
#include "unclassified/tu_8022EF84.h"

extern TLComponentInstance lbl_80580030;
extern BaseGameSceneManager* lbl_806E1838;
extern MemoryAllocator* AllocatorStack[16];
extern unsigned int AllocatorStackDepth;
AsyncImage* lbl_805785C8[12];
int lbl_806E18E0;
int lbl_806E18E4;
bool lbl_806E18E8;
int lbl_806E18EC;

extern void* lbl_8051F684[];
extern char lbl_8051EBB8[];
extern const char* lbl_806DE700;

extern bool fn_8010FE54(unsigned int flag);
extern "C" void fn_801FEA18(const char* name, void (*callback)(), float duration, bool value);
extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern "C" void fn_801CCEA0(int group, int index);
extern "C" Presentation* fn_801FEEAC();
extern "C" void fn_80209344(bool enabled);
extern "C" void fn_8022F858(UnidentifiedScrollWidget* widget);
extern "C" bool fn_8022FD80(UnidentifiedScrollWidget* widget, int direction, int value);
extern "C" void fn_80230468(UnidentifiedScrollWidget* widget, TU80300104Event event, float fDeltaT);
extern "C" void fn_802308D0(UnidentifiedScrollWidget* widget, TLInstance* instance);
extern "C" void fn_80230B90(UnidentifiedScrollWidget* widget, int mode);
extern "C" void fn_80230DE0(UnidentifiedScrollWidget* widget, int value);
class TU80252180Scene;
extern "C" void fn_80253474(TU80252180Scene* scene);
extern "C" void fn_802534BC(TU80252180Scene* scene, int value, bool enabled);
extern "C" TLComponentInstance* fn_80253D70(TU80252180Scene* scene, int value);
extern "C" TU80252180Scene* fn_80253E18();
extern "C" bool fn_80273B00();
extern "C" void fn_8024AF04(TU8024A368Scene* scene);
extern "C" void fn_8024BED8(TU8024A368Scene* scene);
extern "C" void fn_8024C368(TU8024A368Scene* scene);
extern "C" void fn_8024CBD8(TU8024A368Scene* scene);
extern "C" void fn_8024D074(TU8024A368Scene* scene);

extern TLComponentInstance* lbl_80578450[4];
extern unsigned int lbl_806E18B0;
extern "C" void* memcpy(void* dest, const void* src, unsigned long count);

static const int sFirstControllerModes[3] = { 0, 1, 2 };
static const int sSecondControllerModes[3] = { 14, 15, 16 };

struct UnidentifiedImageInfo
{
    const char* textureName;
    const char* unlockedPath;
    unsigned int unlockFlag;
};

static const UnidentifiedImageInfo sImageInfo[12] = {
    { "_main/screen", "fe/environments/main/screen_unlocked", 0x200 },
    { "_main/screen_bowser", "fe/environments/main/screen_bowser_unlocked", 0x400 },
    { "_main/screen_daisy", "fe/environments/main/screen_daisy_unlocked", 0x800 },
    { "_main/screen_donkeykong", "fe/environments/main/screen_donkeykong_unlocked", 0x1000 },
    { "_main/screen_luigi", "fe/environments/main/screen_luigi_unlocked", 0x2000 },
    { "_main/screen_peach", "fe/environments/main/screen_peach_unlocked", 0x4000 },
    { "_main/screen_waluigi", "fe/environments/main/screen_waluigi_unlocked", 0x8000 },
    { "_main/screen_wario", "fe/environments/main/screen_wario_unlocked", 0x10000 },
    { "_main/screen_yoshi", "fe/environments/main/screen_yoshi_unlocked", 0x20000 },
    { "_main/screen_bowserjr", "fe/environments/main/screen_bowserjr_unlocked", 0x40000 },
    { "_main/screen_diddykong", "fe/environments/main/screen_diddykong_unlocked", 0x80000 },
    { "_main/screen_petey", "fe/environments/main/screen_petey_unlocked", 0x100000 },
};

static inline TLImageInstance* FindBreadcrumb(TLComponentInstance* breadcrumbs, const char* name)
{
    TLImageInstance* image = (TLImageInstance*)FEFinder<TLImageInstance, 2>::_Find<TLSlide>(
        breadcrumbs->GetActiveSlide(), nlStringLowerHash(name), 0, 0, 0, 0, 0);
    if (image == 0)
    {
        image = (TLImageInstance*)&lbl_80580030;
    }
    return image;
}

template <typename T>
static inline T* CastFound(TLInstance* found)
{
    if (found == 0)
        return 0;
    return (T*)found;
}

typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;

extern "C" WideBasicString fn_8024EF24(const WideBasicString& string,
    const unsigned short (&t0)[5], const unsigned short (&t1)[5],
    const unsigned short (&t2)[5]);

template <>
WideBasicString Format<WideBasicString, unsigned short[4], unsigned short[4],
    unsigned short[4]>(const WideBasicString& string, const unsigned short (&t0)[4],
    const unsigned short (&t1)[4], const unsigned short (&t2)[4]);

template <>
WideBasicString Format<WideBasicString, unsigned short[4]>(
    const WideBasicString& string, const unsigned short (&t0)[4]);

static inline const unsigned short* LookupLocString(const char* id)
{
    nlLocalization* loc = g_pLocalization;
    unsigned long hash = nlStringLowerHash(id);
    if (loc->m_LookupTable == 0)
    {
        return LocalizationTableNotFound;
    }

    nlLocalization::StringLookup* entry
        = nlBSearch<nlLocalization::StringLookup, unsigned long>(
            hash, loc->m_LookupTable, (int)loc->m_pFile->StringCount);
    if (entry != 0)
    {
        return loc->m_FirstString + entry->StringOffset;
    }

    return MissingLocString;
}

extern "C" void fn_802492E4(int mode, TLComponentInstance* breadcrumbs)
{
    FindBreadcrumb(breadcrumbs, "breadcrumb_5")->m_bVisible = false;
    FindBreadcrumb(breadcrumbs, "breadcrumb_6")->m_bVisible = false;
    FindBreadcrumb(breadcrumbs, "breadcrumb_7")->m_bVisible = false;

    if (mode < 3)
    {
        for (int i = 0; i < 3; ++i)
        {
            if (mode == sFirstControllerModes[i])
            {
                fn_801CCEA0(3, i);
                break;
            }
        }
    }
    else
    {
        for (int i = 0; i < 3; ++i)
        {
            if (mode == sSecondControllerModes[i])
            {
                fn_801CCEA0(3, i);
                break;
            }
        }
    }
}

extern "C" void fn_8024960C(int mode, bool advance)
{
    bool wideScreen = fn_80273B00();
    int index = 0;

    for (int i = 0; i < 3; ++i)
    {
        if (mode == sFirstControllerModes[i])
        {
            index = i;
            break;
        }
    }

    if (advance == true)
    {
        ++index;
    }
    else
    {
        --index;
    }

    if (index >= 3)
    {
        index = 0;
    }
    else if (index < 0)
    {
        index = 2;
    }

    switch (sFirstControllerModes[index])
    {
    case 0:
        if (wideScreen)
        {
            fn_801FEA18("hofbronze", 0, 0.0f, true);
        }
        else
        {
            fn_801FEA18("43hofbronze", 0, 0.0f, true);
        }
        lbl_806E1838->Push((SceneList)62, SCREEN_NOTHING, true);
        break;
    case 1:
        if (wideScreen)
        {
            fn_801FEA18("hofgold", 0, 0.0f, true);
        }
        else
        {
            fn_801FEA18("43hofgold", 0, 0.0f, true);
        }
        lbl_806E1838->Push((SceneList)63, SCREEN_NOTHING, true);
        break;
    case 2:
        if (wideScreen)
        {
            fn_801FEA18("hofsilver", 0, 0.0f, true);
        }
        else
        {
            fn_801FEA18("43hofsilver", 0, 0.0f, true);
        }
        lbl_806E1838->Push((SceneList)64, SCREEN_NOTHING, true);
        break;
    }
}

extern "C" void fn_80249800(int camera)
{
    switch (camera)
    {
    case 4:
        fn_801FEA18("trophycentreofbronzehof", 0, 0.0f, true);
        lbl_806E1838->Push((SceneList)66, SCREEN_NOTHING, true);
        break;
    case 6:
        fn_801FEA18("trophycentreofgoldhof", 0, 0.0f, true);
        lbl_806E1838->Push((SceneList)69, SCREEN_NOTHING, true);
        break;
    case 5:
        fn_801FEA18("trophycentreofsilverhof", 0, 0.0f, true);
        lbl_806E1838->Push((SceneList)72, SCREEN_NOTHING, true);
        break;
    case 8:
        fn_801FEA18("trophyrightofbronze", 0, 0.0f, true);
        lbl_806E1838->Push((SceneList)67, SCREEN_NOTHING, true);
        break;
    case 7:
        fn_801FEA18("trophyleftofbronze", 0, 0.0f, true);
        lbl_806E1838->Push((SceneList)68, SCREEN_NOTHING, true);
        break;
    case 12:
        fn_801FEA18("trophyrightofgold", 0, 0.0f, true);
        lbl_806E1838->Push((SceneList)70, SCREEN_NOTHING, true);
        break;
    case 11:
        fn_801FEA18("trophyleftofgold", 0, 0.0f, true);
        lbl_806E1838->Push((SceneList)71, SCREEN_NOTHING, true);
        break;
    case 10:
        fn_801FEA18("trophyrightofsilver", 0, 0.0f, true);
        lbl_806E1838->Push((SceneList)73, SCREEN_NOTHING, true);
        break;
    case 9:
        fn_801FEA18("trophyleftofsilver", 0, 0.0f, true);
        lbl_806E1838->Push((SceneList)74, SCREEN_NOTHING, true);
        break;
    }

    fn_801CBCA0(0xA28F072D, 0, 0, 1);
}

extern "C" void fn_80249A70(int camera)
{
    lbl_806E18E0 = camera;
    lbl_806E1838->Push((SceneList)65, SCREEN_NOTHING, true);
}

extern "C" void fn_80249A94(int mode, bool advance)
{
    switch (mode)
    {
    case 14:
        if (advance)
        {
            lbl_806E1838->Push((SceneList)60, SCREEN_NOTHING, true);
        }
        else
        {
            lbl_806E1838->Push((SceneList)61, SCREEN_NOTHING, true);
        }
        break;
    case 15:
        if (advance)
        {
            lbl_806E1838->Push((SceneList)61, SCREEN_NOTHING, true);
        }
        else
        {
            lbl_806E1838->Push((SceneList)59, SCREEN_NOTHING, true);
        }
        break;
    case 16:
        if (advance)
        {
            lbl_806E1838->Push((SceneList)59, SCREEN_NOTHING, true);
        }
        else
        {
            lbl_806E1838->Push((SceneList)60, SCREEN_NOTHING, true);
        }
        break;
    default:
        lbl_806E1838->Push((SceneList)59, SCREEN_NOTHING, true);
        lbl_806E18E4 = mode;
        break;
    }
}

extern "C" void fn_80249BF4(int mode)
{
    bool wideScreen = fn_80273B00();

    switch (mode)
    {
    case 0:
    case 1:
    case 2:
        fn_801CBCA0(0x4430B152, 0, 0, 1);
        lbl_806E1838->Pop();
        fn_80209344(true);
        fn_801FEEAC()->Call("TransitionHallOfFameToMainMenu");
        break;
    case 4:
    case 7:
    case 8:
        if (wideScreen)
        {
            fn_801FEA18("hofbronze", 0, 0.0f, true);
        }
        else
        {
            fn_801FEA18("43hofbronze", 0, 0.0f, true);
        }
        lbl_806E1838->Push((SceneList)62, SCREEN_BACK, true);
        break;
    case 6:
    case 11:
    case 12:
        if (wideScreen)
        {
            fn_801FEA18("hofgold", 0, 0.0f, true);
        }
        else
        {
            fn_801FEA18("43hofgold", 0, 0.0f, true);
        }
        lbl_806E1838->Push((SceneList)63, SCREEN_BACK, true);
        break;
    case 5:
    case 9:
    case 10:
        if (wideScreen)
        {
            fn_801FEA18("hofsilver", 0, 0.0f, true);
        }
        else
        {
            fn_801FEA18("43hofsilver", 0, 0.0f, true);
        }
        lbl_806E1838->Push((SceneList)64, SCREEN_BACK, true);
        break;
    case 13:
        if (lbl_806E18E0 < 4)
        {
            if (wideScreen)
            {
                fn_801FEA18("hofbronze", 0, 0.0f, true);
            }
            else
            {
                fn_801FEA18("43hofbronze", 0, 0.0f, true);
            }
            lbl_806E1838->Push((SceneList)62, SCREEN_BACK, true);
        }
        else if (lbl_806E18E0 < 8)
        {
            if (wideScreen)
            {
                fn_801FEA18("hofsilver", 0, 0.0f, true);
            }
            else
            {
                fn_801FEA18("43hofsilver", 0, 0.0f, true);
            }
            lbl_806E1838->Push((SceneList)64, SCREEN_BACK, true);
        }
        else
        {
            if (wideScreen)
            {
                fn_801FEA18("hofgold", 0, 0.0f, true);
            }
            else
            {
                fn_801FEA18("43hofgold", 0, 0.0f, true);
            }
            lbl_806E1838->Push((SceneList)63, SCREEN_BACK, true);
        }
        break;
    case 14:
    case 15:
    case 16:
        if (lbl_806E18E4 == 1)
        {
            lbl_806E1838->Push((SceneList)63, SCREEN_NOTHING, true);
        }
        else if (lbl_806E18E4 == 2)
        {
            lbl_806E1838->Push((SceneList)64, SCREEN_NOTHING, true);
        }
        else
        {
            lbl_806E1838->Push((SceneList)62, SCREEN_NOTHING, true);
        }
        break;
    }
}

extern "C" unsigned int fn_80249F50(int mode, int item)
{
    if (item == 0)
    {
        switch (mode)
        {
        case 2:
            item += 1;
            break;
        case 1:
            item += 2;
            break;
        case 0:
            break;
        }
    }
    else if (item <= 2)
    {
        item += 2;
        switch (mode)
        {
        case 2:
            item += 2;
            break;
        case 1:
            item += 4;
            break;
        case 0:
            break;
        }
    }
    else if (item <= 6)
    {
        item += 6;
        switch (mode)
        {
        case 2:
            item += 4;
            break;
        case 1:
            item += 8;
            break;
        case 0:
            break;
        }
    }

    return 1 << item;
}

extern "C" unsigned int fn_80249FF0(int mode, int item)
{
    if (item == 0)
    {
        switch (mode)
        {
        case 0:
            item = 53;
            break;
        case 2:
            item = 67;
            break;
        case 1:
            item = 60;
            break;
        }
    }
    else if (item <= 2)
    {
        switch (mode)
        {
        case 0:
            item = item * 2 + 50;
            break;
        case 2:
            item = item * 2 + 64;
            break;
        case 1:
            item = item * 2 + 57;
            break;
        }
    }
    else if (item <= 6)
    {
        if (item < 5)
        {
            item -= 3;
        }

        switch (mode)
        {
        case 0:
            item += 50;
            if (item == 55)
            {
                item = 7;
            }
            break;
        case 2:
            item += 64;
            break;
        case 1:
            item += 57;
            break;
        }
    }

    return item;
}

extern "C" int fn_8024A0E4()
{
    return lbl_806E18E0;
}

extern "C" unsigned int fn_8024A0EC(unsigned int camera)
{
    switch (camera)
    {
    case 0:
        return 0x200;
    case 1:
        return 0x2000;
    case 2:
        return 0x1000;
    case 3:
        return 0x4000;
    case 4:
        return 0x800;
    case 5:
        return 0x10000;
    case 6:
        return 0x8000;
    case 7:
        return 0x20000;
    case 8:
        return 0x400;
    case 9:
        return 0x100000;
    case 10:
        return 0x40000;
    case 11:
        return 0x80000;
    default:
        return 0x200;
    }
}

extern "C" void fn_8024A178()
{
    if (!lbl_806E18E8)
    {
        lbl_806E18E8 = true;
        lbl_806E18EC = 0;

        for (int i = 0; i < 12; ++i)
        {
            if (fn_8010FE54(sImageInfo[i].unlockFlag))
            {
                CurrentAllocator = &VirtualAllocator;
                AllocatorStack[AllocatorStackDepth++] = &VirtualAllocator;
                lbl_805785C8[i] = new (nlMalloc(sizeof(AsyncImage), 8, false))
                    AsyncImage("art/fe/HallOfFameUI.res", 0);
                --AllocatorStackDepth;
                AllocatorStack[AllocatorStackDepth] = 0;
                CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];

                lbl_805785C8[i]->mUnidentified14 = glGetTexture(sImageInfo[i].textureName);
                lbl_805785C8[i]->QueueLoad(sImageInfo[i].unlockedPath, false);
                ++lbl_806E18EC;
            }
        }
    }
}

extern "C" void fn_8024A290(float fDeltaT)
{
    if (lbl_806E18EC > 0)
    {
        for (int i = 0; i < 12; ++i)
        {
            if (lbl_805785C8[i] != 0 && lbl_805785C8[i]->Update(true))
            {
                --lbl_806E18EC;
                delete lbl_805785C8[i];
                lbl_805785C8[i] = 0;
            }
        }
    }
}

extern "C" void fn_8024A33C()
{
    lbl_806E18E8 = false;
    lbl_806E18EC = 0;
}

extern "C" bool fn_8024A34C()
{
    return lbl_806E18E8;
}

extern "C" bool fn_8024A354()
{
    return lbl_806E18EC > 0;
}

struct TU8024A368HistoryRecord
{
    unsigned int mUnidentified00 : 4;
    unsigned int mUnidentified04 : 3;
    unsigned int mUnidentified07 : 3;
    unsigned int : 3;
    unsigned int mUnidentified0D : 5;
    unsigned int mUnidentified12 : 4;
    unsigned int mUnidentified16 : 10;
    unsigned int mUnidentified20 : 11;
    unsigned int mUnidentified2B : 7;
    unsigned int mUnidentified32 : 7;
    unsigned int mUnidentified39 : 7;
};

struct TU8024A368Scene
{
    /* 0x000 */ void** mVTable;
    /* 0x004 */ unsigned int mHashID;
    /* 0x008 */ bool mVisible;
    /* 0x009 */ unsigned char mPadding009[3];
    /* 0x00C */ BaseScreenHandler* mScreenHandlerList;
    /* 0x010 */ BaseScreenHandler* mActiveScreenHandler;
    /* 0x014 */ FEPresentation* mPresentation;
    /* 0x018 */ FEScene* mFEScene;
    /* 0x01C */ int mMode;
    /* 0x020 */ TU8022EF84Component mNavigation;
    /* 0x0F8 */ UnidentifiedScrollWidget mScrollWidget;
    /* 0x2AC */ TU8024A368HistoryRecord mHistory[12];
    /* 0x30C */ int mUnidentified30C;
    /* 0x310 */ int mHistoryCount;
    /* 0x314 */ int mUnidentified314[4];
    /* 0x324 */ bool mUnidentified324;
    /* 0x325 */ unsigned char mPadding325[3];
    /* 0x328 */ int mUnidentified328;
    /* 0x32C */ unsigned short mUnidentified32C[0x40];
    /* 0x3AC */ unsigned short mUnidentified3AC[0x80];
    /* 0x4AC */ unsigned short mUnidentified4AC[0x20];
    /* 0x4EC */ unsigned short mUnidentified4EC[0x20];
    /* 0x52C */ AsyncImage* mImages[5];
    /* 0x540 */ bool mImageReady[5];
    /* 0x545 */ unsigned char mPadding545[3];
};

extern "C" TU8024A368Scene* fn_8024A368(TU8024A368Scene* scene, int mode)
{
    scene->mVisible = true;
    scene->mScreenHandlerList = 0;
    scene->mActiveScreenHandler = 0;
    scene->mPresentation = 0;
    scene->mFEScene = 0;
    scene->mVTable = lbl_8051F684;
    scene->mMode = mode;

    new (&scene->mNavigation) TU8022EF84Component();
    new (&scene->mScrollWidget) UnidentifiedScrollWidget();

    scene->mUnidentified30C = 0;
    scene->mHistoryCount = 0;
    scene->mUnidentified324 = false;
    scene->mUnidentified328 = 0;
    scene->mImages[0] = 0;
    scene->mImages[1] = 0;
    scene->mImages[2] = 0;
    scene->mImages[3] = 0;
    scene->mImages[4] = 0;
    scene->mUnidentified314[0] = 0;
    scene->mUnidentified314[1] = 0;
    scene->mUnidentified314[2] = 0;
    scene->mUnidentified314[3] = 0;

    scene->mImages[0] = new (0x20, true) AsyncImage(lbl_806DE700, 0);
    scene->mImageReady[0] = false;
    scene->mImages[1] = new (0x20, true) AsyncImage(lbl_806DE700, 0);
    scene->mImageReady[1] = false;
    scene->mImages[2] = new (0x20, true) AsyncImage(lbl_806DE700, 0);
    scene->mImageReady[2] = false;
    scene->mImages[3] = new (0x20, true) AsyncImage(lbl_806DE700, 0);
    scene->mImageReady[3] = false;
    scene->mImages[4] = new (0x20, true) AsyncImage(lbl_806DE700, 0);
    scene->mImageReady[4] = false;

    memset(scene->mHistory, 0, sizeof(scene->mHistory));

    int modeIndex = mode - 4;
    unsigned char* historyBase = (unsigned char*)lbl_806E0F90 + 0x86A0;
    int historyIndex = 11;
    unsigned char cursor = historyBase[0x370 + modeIndex];
    if (cursor != 0)
    {
        historyIndex = cursor - 1;
    }

    TU8024A368HistoryRecord* source
        = (TU8024A368HistoryRecord*)(historyBase + modeIndex * 0x60 + 0x10);
    while (scene->mHistoryCount < 12)
    {
        TU8024A368HistoryRecord& record = source[historyIndex];
        bool emptyThrough04
            = record.mUnidentified00 == 0 && record.mUnidentified04 == 0;
        bool emptyThrough07 = emptyThrough04 && record.mUnidentified07 == 0;
        bool emptyThrough0D = emptyThrough07 && record.mUnidentified0D == 0;
        bool emptyThrough12 = emptyThrough0D && record.mUnidentified12 == 0;
        bool emptyThrough16 = emptyThrough12 && record.mUnidentified16 == 0;
        bool emptyThrough20 = emptyThrough16 && record.mUnidentified20 == 0;
        bool emptyThrough2B = emptyThrough20 && record.mUnidentified2B == 0;
        bool emptyThrough32 = emptyThrough2B && record.mUnidentified32 == 0;
        bool empty = emptyThrough32 && record.mUnidentified39 == 0;
        if (empty)
        {
            break;
        }

        scene->mHistory[scene->mHistoryCount] = record;
        ++scene->mHistoryCount;
        historyIndex = historyIndex > 0 ? historyIndex - 1 : 11;
    }

    return scene;
}

extern "C" TU8024A368Scene* fn_8024A6B0(TU8024A368Scene* scene, int deleteFlag)
{
    if (scene != 0)
    {
        scene->mVTable = lbl_8051F684;
        if (scene->mImages[0] != 0)
            delete scene->mImages[0];
        if (scene->mImages[1] != 0)
            delete scene->mImages[1];
        if (scene->mImages[2] != 0)
            delete scene->mImages[2];
        if (scene->mImages[3] != 0)
            delete scene->mImages[3];
        if (scene->mImages[4] != 0)
            delete scene->mImages[4];
        scene->mScrollWidget.~UnidentifiedScrollWidget();
        scene->mNavigation.TU8022EF84Component::~TU8022EF84Component();

        if (deleteFlag > 0)
        {
            operator delete(scene);
        }
    }

    return scene;
}

char lbl_8051EBB8[] = "art/fe/HallOfFameUI.res";
const char* lbl_806DE700 = lbl_8051EBB8;

extern "C" void fn_8024A7DC(TU8024A368Scene* scene)
{
    fn_802172C4();

    TU80252180Scene* object = fn_80253E18();
    TLComponentInstance* screen = 0;
    if (object != 0)
    {
        fn_80253474(object);
        screen = fn_80253D70(object, 4);
    }
    scene->mNavigation.fn_8022F194(screen);

    FEPresentation* presentation = scene->mFEScene->m_pFEPackage->GetPresentation();
    TLComponentInstance* scrollbar = CastFound<TLComponentInstance>(
        FEFinder<TLComponentInstance, 2>::_Find<TLSlide>(presentation->GetActiveSlide(),
            nlStringLowerHash("Layer"), nlStringLowerHash("scrollbar"), 0, 0, 0, 0));
    fn_802308D0(&scene->mScrollWidget, scrollbar);
    fn_80230B90(&scene->mScrollWidget, scene->mHistoryCount - 1);
    fn_80230DE0(&scene->mScrollWidget, scene->mUnidentified30C);

    fn_8024AF04(scene);
    switch (scene->mMode)
    {
    case 4:
    case 5:
    case 6:
        fn_8024BED8(scene);
        break;
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
        fn_8024C368(scene);
        break;
    }
    fn_8024CBD8(scene);
    fn_8024D074(scene);
}

extern "C" void fn_8024A990(TU8024A368Scene* scene, float fDeltaT)
{
    ((BaseSceneHandler*)scene)->BaseSceneHandler::Update(fDeltaT);

    int state = scene->mUnidentified328;
    if (state == 0 || (unsigned int)(state - 2) <= 1)
    {
        TLSlide* slide = scene->mPresentation->GetActiveSlide();
        if (slide->GetCurrentTime() < slide->m_start + slide->m_duration)
        {
            for (int pad = 0; pad < 4; ++pad)
            {
                lbl_80578450[pad]->SetActiveSlide("waiting", true, false);
            }
            return;
        }

        if (state == 0)
        {
            fn_802534BC(fn_80253E18(), 4, true);
            scene->mUnidentified328 = 1;
            if (!scene->mScrollWidget.mUnidentified00[0x18])
            {
                TLComponentInstance* scrollbar = CastFound<TLComponentInstance>(
                    FEFinder<TLComponentInstance, 2>::_Find<TLSlide>(
                        scene->mPresentation->GetActiveSlide(), nlStringLowerHash("Layer"),
                        nlStringLowerHash("scrollbar"), 0, 0, 0, 0));
                fn_802308D0(&scene->mScrollWidget, scrollbar);
                fn_8022F858(&scene->mScrollWidget);
            }
        }
        else if (state == 2)
        {
            return;
        }
        else if (state == 3)
        {
            fn_80249BF4(scene->mMode);
            return;
        }
    }

    if (scene->mImages[0]->Update(true))
        scene->mImageReady[0] = true;
    if (scene->mImages[1]->Update(true))
        scene->mImageReady[1] = true;
    if (scene->mImages[2]->Update(true))
        scene->mImageReady[2] = true;
    if (scene->mImages[3]->Update(true))
        scene->mImageReady[3] = true;
    if (scene->mImages[4]->Update(true))
        scene->mImageReady[4] = true;

    if (!scene->mImageReady[0] || !scene->mImageReady[1] || !scene->mImageReady[2]
        || !scene->mImageReady[3] || !scene->mImageReady[4])
    {
        return;
    }

    scene->mImages[0]->mImageInstance->m_bVisible = true;
    scene->mImages[4]->mImageInstance->m_bVisible = true;
    scene->mImages[1]->mImageInstance->m_bVisible = true;
    scene->mImages[2]->mImageInstance->m_bVisible = true;
    scene->mImages[3]->mImageInstance->m_bVisible = true;

    for (unsigned int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* controller = lbl_80578450[pad];
        bool processInput = true;
        if (g_pFEInput->m_InputLockDepth == 0)
        {
            if (pad != lbl_806E18B0)
            {
                controller->SetActiveSlide("waiting", true, false);
                processInput = false;
            }
            else if (scene->mUnidentified314[pad] > 0)
            {
                controller->SetActiveSlide("A", true, false);
            }
            else
            {
                controller->SetActiveSlide("cursor", true, false);
            }
        }

        if (processInput)
        {
            unsigned char valid = 1;
            TU80300104Event event;
            event.mIndex = pad;
            event.mPosition = fn_802197FC(pad, &valid);
            event.mFlag0
                = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
            event.mFlag1
                = g_pFEInput->JustReleased((eFEINPUT_PAD)pad, 0x1E, true, 0);
            fn_80230468(&scene->mScrollWidget, event, fDeltaT);

            if (scene->mNavigation.fn_8022F2E0(event, fDeltaT))
            {
                fn_801CBCA0(0x80BA0C86, 0, 0, 1);
                scene->mUnidentified328 = 3;
                TU80252180Scene* object = fn_80253E18();
                if (object != 0)
                {
                    fn_80253474(object);
                }
                scene->mPresentation->SetActiveSlide("OUT", true);
                return;
            }
        }
    }

    if (fn_8022FD80(&scene->mScrollWidget, 1, 1))
    {
        ++scene->mUnidentified30C;
        switch (scene->mMode)
        {
        case 4:
        case 5:
        case 6:
            fn_8024BED8(scene);
            break;
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
            fn_8024C368(scene);
            break;
        }
        fn_8024CBD8(scene);
        fn_8024D074(scene);
    }
    else if (fn_8022FD80(&scene->mScrollWidget, 0, 1))
    {
        --scene->mUnidentified30C;
        switch (scene->mMode)
        {
        case 4:
        case 5:
        case 6:
            fn_8024BED8(scene);
            break;
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
            fn_8024C368(scene);
            break;
        }
        fn_8024CBD8(scene);
        fn_8024D074(scene);
    }
}

extern "C" void fn_8024AF04(TU8024A368Scene* scene)
{
    WideBasicString title;

    FEPresentation* presentation = scene->mFEScene->m_pFEPackage->GetPresentation();
    unsigned long titlesHash = nlStringLowerHash("TITLES");
    unsigned long titleHash = nlStringLowerHash("TITLE");
    unsigned long historyHash = nlStringLowerHash("HISTORY");
    TLTextInstance* titleText = CastFound<TLTextInstance>(
        FEFinder<TLTextInstance, 4>::_Find<TLSlide>(presentation->GetActiveSlide(),
            nlStringLowerHash("Layer"), historyHash, titleHash, titlesHash, 0, 0));
    if (titleText == 0)
    {
        titleText = &UnidentifiedFallbackTextInstance;
    }

    switch (scene->mMode)
    {
    case 4:
        title = WideBasicString(LookupLocString("HOF_HISTORY_TITLE_FIRE_CUP"));
        break;
    case 6:
        title = WideBasicString(LookupLocString("HOF_HISTORY_TITLE_STRIKER_CUP"));
        break;
    case 5:
        title = WideBasicString(LookupLocString("HOF_HISTORY_TITLE_CRYSTAL_CUP"));
        break;
    case 7:
        title = WideBasicString(LookupLocString("HOF_HISTORY_TITLE_FIRE_BRICK_WALL"));
        break;
    case 8:
        title = WideBasicString(LookupLocString("HOF_HISTORY_TITLE_FIRE_GOLDEN_BOOT"));
        break;
    case 11:
        title = WideBasicString(LookupLocString("HOF_HISTORY_TITLE_STRIKER_BRICK_WALL"));
        break;
    case 12:
        title = WideBasicString(LookupLocString("HOF_HISTORY_TITLE_STRIKER_GOLDEN_BOOT"));
        break;
    case 9:
        title = WideBasicString(LookupLocString("HOF_HISTORY_TITLE_CRYSTAL_BRICK_WALL"));
        break;
    case 10:
        title = WideBasicString(LookupLocString("HOF_HISTORY_TITLE_CRYSTAL_GOLDEN_BOOT"));
        break;
    }

    memcpy(scene->mUnidentified32C, title.c_str(), sizeof(scene->mUnidentified32C));
    titleText->SetString(scene->mUnidentified32C);
}

extern "C" void fn_8024BED8(TU8024A368Scene* scene)
{
    FEPresentation* presentation = scene->mFEScene->m_pFEPackage->GetPresentation();
    unsigned long recordHash = nlStringLowerHash("RECORD");
    unsigned long historyHash = nlStringLowerHash("HISTORY");
    TLTextInstance* recordText = CastFound<TLTextInstance>(
        FEFinder<TLTextInstance, 3>::_Find<TLSlide>(presentation->GetActiveSlide(),
            nlStringLowerHash("Layer"), historyHash, recordHash, 0, 0, 0));
    if (recordText == 0)
    {
        recordText = &UnidentifiedFallbackTextInstance;
    }

    char monthString[4];
    char dayString[4];
    char yearString[4];
    unsigned short monthWideString[4];
    unsigned short dayWideString[4];
    unsigned short yearWideString[4];
    WideBasicString unformatted;
    WideBasicString formatted;

    TU8024A368HistoryRecord& record = scene->mHistory[scene->mUnidentified30C];
    int month = record.mUnidentified2B;
    int day = record.mUnidentified32;
    int year = record.mUnidentified39;

    nlSNPrintf(monthString, 4, "%d", month);
    nlStrToWcs(monthString, monthWideString, 4);
    nlSNPrintf(dayString, 4, "%d", day);
    nlStrToWcs(dayString, dayWideString, 4);
    nlSNPrintf(yearString, 4, "%d", year);
    nlStrToWcs(yearString, yearWideString, 4);

    unformatted = WideBasicString(LookupLocString("ROAD_HUB_TEAM_RECORD_STATS"));
    formatted = Format(unformatted, monthWideString, dayWideString, yearWideString);

    memcpy(scene->mUnidentified3AC, formatted.c_str(), sizeof(scene->mUnidentified3AC));
    recordText->SetString(scene->mUnidentified3AC);
}

extern "C" void fn_8024C368(TU8024A368Scene* scene)
{
    char goalsString[4];
    unsigned short goalsWideString[4];
    WideBasicString unformatted;
    WideBasicString formatted;

    TU8024A368HistoryRecord& record = scene->mHistory[scene->mUnidentified30C];
    int goals = record.mUnidentified20;
    nlSNPrintf(goalsString, 4, "%d", goals);
    nlStrToWcs(goalsString, goalsWideString, 4);

    FEPresentation* presentation = scene->mFEScene->m_pFEPackage->GetPresentation();
    TLTextInstance* goalsText = 0;
    switch (scene->mMode)
    {
    case 7:
    case 9:
    case 11:
    {
        unformatted = WideBasicString(LookupLocString("HOF_GOALS_AGAINST"));

        unsigned long goalsForHash = nlStringLowerHash("GOALS FOR");
        unsigned long historyHash = nlStringLowerHash("HISTORY");
        goalsText = CastFound<TLTextInstance>(
            FEFinder<TLTextInstance, 3>::_Find<TLSlide>(presentation->GetActiveSlide(),
                nlStringLowerHash("Layer"), historyHash, goalsForHash, 0, 0, 0));
        if (goalsText == 0)
        {
            goalsText = &UnidentifiedFallbackTextInstance;
        }
        goalsText->m_bVisible = false;

        unsigned long goalsAgainstHash = nlStringLowerHash("GOALS AGAINST");
        historyHash = nlStringLowerHash("HISTORY");
        goalsText = CastFound<TLTextInstance>(
            FEFinder<TLTextInstance, 3>::_Find<TLSlide>(presentation->GetActiveSlide(),
                nlStringLowerHash("Layer"), historyHash, goalsAgainstHash, 0, 0, 0));
        if (goalsText == 0)
        {
            goalsText = &UnidentifiedFallbackTextInstance;
        }
        break;
    }
    case 8:
    case 10:
    case 12:
    {
        unformatted = WideBasicString(LookupLocString("HOF_GOALS_FOR"));

        unsigned long goalsAgainstHash = nlStringLowerHash("GOALS AGAINST");
        unsigned long historyHash = nlStringLowerHash("HISTORY");
        goalsText = CastFound<TLTextInstance>(
            FEFinder<TLTextInstance, 3>::_Find<TLSlide>(presentation->GetActiveSlide(),
                nlStringLowerHash("Layer"), historyHash, goalsAgainstHash, 0, 0, 0));
        if (goalsText == 0)
        {
            goalsText = &UnidentifiedFallbackTextInstance;
        }
        goalsText->m_bVisible = false;

        unsigned long goalsForHash = nlStringLowerHash("GOALS FOR");
        historyHash = nlStringLowerHash("HISTORY");
        goalsText = CastFound<TLTextInstance>(
            FEFinder<TLTextInstance, 3>::_Find<TLSlide>(presentation->GetActiveSlide(),
                nlStringLowerHash("Layer"), historyHash, goalsForHash, 0, 0, 0));
        if (goalsText == 0)
        {
            goalsText = &UnidentifiedFallbackTextInstance;
        }
        break;
    }
    }

    formatted = Format(unformatted, goalsWideString);
    memcpy(scene->mUnidentified3AC, formatted.c_str(), sizeof(scene->mUnidentified3AC));
    goalsText->SetString(scene->mUnidentified3AC);
}

extern "C" void fn_8024CBD8(TU8024A368Scene* scene)
{
    char yearString[5];
    char monthString[5];
    char dayString[5];
    unsigned short yearWideString[5];
    unsigned short monthWideString[5];
    unsigned short dayWideString[5];

    FEPresentation* presentation = scene->mFEScene->m_pFEPackage->GetPresentation();
    unsigned long dateHash = nlStringLowerHash("DATE");
    unsigned long historyHash = nlStringLowerHash("HISTORY");
    TLTextInstance* dateText = CastFound<TLTextInstance>(
        FEFinder<TLTextInstance, 3>::_Find<TLSlide>(presentation->GetActiveSlide(),
            nlStringLowerHash("Layer"), historyHash, dateHash, 0, 0, 0));
    if (dateText == 0)
    {
        dateText = &UnidentifiedFallbackTextInstance;
    }

    WideBasicString unformatted;
    WideBasicString formatted;

    TU8024A368HistoryRecord& record = scene->mHistory[scene->mUnidentified30C];
    int month = record.mUnidentified12 + 1;
    int day = record.mUnidentified0D + 1;
    int year = record.mUnidentified16 + 2000;

    nlSNPrintf(yearString, 5, "%.4d", year);
    nlStrToWcs(yearString, yearWideString, 5);
    nlSNPrintf(monthString, 5, "%.2d", month);
    nlStrToWcs(monthString, monthWideString, 5);
    nlSNPrintf(dayString, 5, "%.2d", day);
    nlStrToWcs(dayString, dayWideString, 5);

    unformatted = WideBasicString(LookupLocString("DATE_STAMP"));
    formatted
        = fn_8024EF24(unformatted, dayWideString, monthWideString, yearWideString);
    memcpy(scene->mUnidentified4AC, formatted.c_str(), sizeof(scene->mUnidentified4AC));
    dateText->SetString(scene->mUnidentified4AC);
}
