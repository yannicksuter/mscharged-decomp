#include <RVLFaceLib/RFL_DataUtility.h>
#include <RVLFaceLib/RFL_Model.h>

#include "unclassified/tu_80264E98.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/DB/SaveLoad.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/feTextureResource.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/GameInfo.h"
#include "Game/Render/Presentation.h"
#include "Game/Task/ResetTask.h"
#include "NL/nlAlgorithm.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalization.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/nlBind.h"
#include "unclassified/tu_802196B0.h"
#include "unclassified/tu_80252180.h"
#include "unclassified/tu_802672D4.h"

#include <string.h>

extern "C" u16 RFLGetAvailableOfficialDataNum();
extern "C" int RFLSearchOfficialData(const RFLCreateID* id, u16* index);
extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern "C" int fn_802AA91C(
    unsigned short* buffer, unsigned long size, const unsigned short* format, ...);
extern "C" Presentation* fn_801FEEAC();
extern "C" void fn_80238A1C(TU802384AC* object, int index, bool enabled, bool visible);
extern "C" void fn_802392D4(TU802384AC* object, int index);
extern "C" void fn_80302420(TLInstance* instance, bool visible);

struct TU806E1908
{
    /* 0x00 */ unsigned char mPadding00[0x3C];
    /* 0x3C */ unsigned long mUnidentified3C[10];
};

extern "C" bool fn_8026F280(TU806E1908* object, int index, int slot, bool value);

extern BaseGameSceneManager* lbl_806E1838;
extern TLComponentInstance* lbl_80578450[4];
extern unsigned int lbl_806E18B0;
extern TLComponentInstance lbl_80580030;
extern TLComponentInstance lbl_80580138;
extern TLImageInstance lbl_80580248;
extern TU806E1908* lbl_806E1908;
extern char lbl_80520808[];
extern char lbl_80520814[];
extern char lbl_80520838[];
extern char lbl_80520888[];
extern char lbl_805208A8[];
extern char lbl_805208B4[];
extern char lbl_806DEC20[6];
extern char lbl_806DEC28[8];
extern char lbl_806DEC30[4];
extern char lbl_806DEC34[5];
extern char lbl_806DEC40[8];
extern char lbl_806DEC48[2];
extern char lbl_806DEC4C[7];
extern char lbl_806DEC54[4];
extern unsigned short lbl_806DEC58[3];
extern char lbl_806DEC60[6];

typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;

static inline const unsigned short* LookupLocString(const char* id)
{
    nlLocalization* localization = g_pLocalization;
    unsigned long hash = nlStringLowerHash(id);
    if (localization->m_LookupTable == 0)
    {
        return LocalizationTableNotFound;
    }

    nlLocalization::StringLookup* lookup
        = nlBSearch<nlLocalization::StringLookup, unsigned long>(
            hash, localization->m_LookupTable, (int)localization->m_pFile->StringCount);
    if (lookup != 0)
    {
        return localization->m_FirstString + lookup->StringOffset;
    }

    return MissingLocString;
}

TU80264E98Scene::TU80264E98Scene()
    : mUnidentified30(false)
    , mUnidentified38(0)
    , mUnidentified3C(false)
    , mUnidentified200()
    , mNavigation()
{
    mUnidentified1C = RFLGetAvailableOfficialDataNum();
    mUnidentified34 = mUnidentified1C / 10 + (mUnidentified1C % 10 != 0);
    if (mUnidentified34 == 0)
    {
        mUnidentified34 = 1;
    }

    mUnidentified20[0] = 0;
    mUnidentified20[1] = 0;
    mUnidentified20[2] = 0;
    mUnidentified20[3] = 0;

    for (int i = 0; i < 10; ++i)
    {
        mUnidentified200[i].mContext = (void*)i;
    }

    memset(mUnidentified40, -1, sizeof(mUnidentified40));
    mNavigation.fn_801D2BE8(false);
}

TU80264E98Scene::~TU80264E98Scene()
{
}

void TU80264E98Scene::SceneCreated()
{
    for (int i = 0; i < 10; ++i)
    {
        char componentName[16];
        nlSNPrintf(componentName, sizeof(componentName), lbl_80520838, i + 1);

        TLComponentInstance* component = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find<TLSlide>(
            mPresentation->GetActiveSlide(),
            nlStringLowerHash(lbl_806DEC20),
            nlStringLowerHash(lbl_80520808),
            nlStringLowerHash(componentName),
            0,
            0,
            0);
        if (component == 0)
        {
            component = &lbl_80580030;
        }
        mUnidentified9E4[i] = component;

        TLInstance* off = FEFinder<TLInstance, 2>::Find(component->GetActiveSlide(),
            nlStringLowerHash(lbl_806DEC30),
            nlStringLowerHash(lbl_806DEC28),
            0,
            0,
            0,
            0);
        if (off == 0)
        {
            off = &lbl_80580138;
        }

        TLInstance* over = FEFinder<TLInstance, 2>::Find(component->GetActiveSlide(),
            nlStringLowerHash(lbl_806DEC34),
            nlStringLowerHash(lbl_806DEC28),
            0,
            0,
            0,
            0);
        if (over == 0)
        {
            over = &lbl_80580138;
        }

        TLInstance* overBackground = FEFinder<TLInstance, 2>::_Find<TLInstance>(
            over, nlStringLowerHash(lbl_80520814), 0, 0, 0, 0, 0);
        overBackground->m_bVisible = false;
        fn_80302420(overBackground, false);

        TLInstance* offBackground = FEFinder<TLInstance, 2>::_Find<TLInstance>(
            off, nlStringLowerHash(lbl_80520814), 0, 0, 0, 0, 0);
        offBackground->m_bVisible = false;
        fn_80302420(offBackground, false);
    }

    fn_80265670();

    TU80252180Scene* scene = fn_80253E18();
    TLComponentInstance* screen = 0;
    if (scene != 0)
    {
        fn_802534BC(scene, 7, true);
        screen = fn_80253D70(scene, 4);
        mUnidentified9E0 = &scene->mUnidentified058;
        fn_80238A1C(mUnidentified9E0, 1, false, false);
        fn_80238A1C(mUnidentified9E0, 0, false, false);
    }

    mNavigation.fn_8022F194(screen);

    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide(lbl_806DEC40, true, false);
    }

    fn_80266014();
}

void TU80264E98Scene::fn_802654AC()
{
    bool changed = false;
    for (int i = 0; i < 10; ++i)
    {
        void* saveId = GameInfoManager::Instance()->GetUnknown0xA80(i);
        if (*(u64*)saveId != 0)
        {
            u64 id;
            memcpy(&id, saveId, sizeof(id));

            u16 index = 0;
            if (!RFLSearchOfficialData((const RFLCreateID*)&id, &index))
            {
                GameInfoManager::Instance()->ClearSaveSlot(i);
                changed = true;
            }
        }
    }

    if (changed)
    {
        SaveLoad::StartSave(true);

        TU80252180Scene* scene = fn_80253E18();
        if (scene != 0)
        {
            fn_80253474(scene);

            FEPopupMenu* popup = (FEPopupMenu*)lbl_806E1838->Push(
                (SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x8C,
                Function<FnVoidVoid>(
                    Bind<void>(MemFun(&TU80264E98Scene::fn_80266014), this)));
        }
    }
}

void TU80264E98Scene::fn_80265670()
{
    unsigned int count = 0;
    RFLAdditionalInfo info;
    u64 id;

    for (unsigned int i = 0; i < RFL_DB_CHAR_MAX; ++i)
    {
        RFLErrcode result = RFLGetAdditionalInfo(&info, RFLDataSource_Official, 0, i);
        if (result == RFLErrcode_Broken)
        {
            FEPopupMenu* popup = (FEPopupMenu*)lbl_806E1838->Push(
                (SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x87,
                Function<FnVoidVoid>(
                    Bind<void>(MemFun(&TU80264E98Scene::fn_802670D4), this)));
            return;
        }

        if (result == RFLErrcode_Success)
        {
            memcpy(&id, &info.createID, sizeof(id));
            if (GameInfoManager::Instance()->HasSaveSlot(id) && info.favorite)
            {
                mUnidentified40[count++] = (u16)i;
            }
        }
    }

    for (unsigned int i = 0; i < RFL_DB_CHAR_MAX; ++i)
    {
        RFLErrcode result = RFLGetAdditionalInfo(&info, RFLDataSource_Official, 0, i);
        if (result == RFLErrcode_Broken)
        {
            FEPopupMenu* popup = (FEPopupMenu*)lbl_806E1838->Push(
                (SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x87,
                Function<FnVoidVoid>(
                    Bind<void>(MemFun(&TU80264E98Scene::fn_802670D4), this)));
            return;
        }

        if (result == RFLErrcode_Success)
        {
            memcpy(&id, &info.createID, sizeof(id));
            if (GameInfoManager::Instance()->HasSaveSlot(id) && !info.favorite)
            {
                mUnidentified40[count++] = (u16)i;
            }
        }
    }

    for (unsigned int i = 0; i < RFL_DB_CHAR_MAX; ++i)
    {
        RFLErrcode result = RFLGetAdditionalInfo(&info, RFLDataSource_Official, 0, i);
        if (result == RFLErrcode_Broken)
        {
            FEPopupMenu* popup = (FEPopupMenu*)lbl_806E1838->Push(
                (SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x87,
                Function<FnVoidVoid>(
                    Bind<void>(MemFun(&TU80264E98Scene::fn_802670D4), this)));
            return;
        }

        if (result == RFLErrcode_Success)
        {
            memcpy(&id, &info.createID, sizeof(id));
            if (!GameInfoManager::Instance()->HasSaveSlot(id) && info.favorite)
            {
                mUnidentified40[count++] = (u16)i;
            }
        }
    }

    for (unsigned int i = 0; i < RFL_DB_CHAR_MAX; ++i)
    {
        RFLErrcode result = RFLGetAdditionalInfo(&info, RFLDataSource_Official, 0, i);
        if (result == RFLErrcode_Broken)
        {
            FEPopupMenu* popup = (FEPopupMenu*)lbl_806E1838->Push(
                (SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x87,
                Function<FnVoidVoid>(
                    Bind<void>(MemFun(&TU80264E98Scene::fn_802670D4), this)));
            return;
        }

        if (result == RFLErrcode_Success)
        {
            memcpy(&id, &info.createID, sizeof(id));
            if (!GameInfoManager::Instance()->HasSaveSlot(id) && !info.favorite)
            {
                mUnidentified40[count++] = (u16)i;
            }
        }
    }
}

void TU80264E98Scene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    if (!mUnidentified30)
    {
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (slide->GetCurrentTime() < slide->m_start + slide->m_duration)
        {
            return;
        }

        fn_80266B68();
        mUnidentified30 = true;
        fn_80266014();
        fn_802654AC();
    }

    for (unsigned int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* controller = lbl_80578450[pad];
        bool processInput = true;
        if (g_pFEInput->m_InputLockDepth == 0)
        {
            if (pad != lbl_806E18B0)
            {
                controller->SetActiveSlide(lbl_806DEC40, true, false);
                processInput = false;
            }
            else if (mUnidentified20[pad] > 0
                     || mNavigation.mUnidentifiedD2[pad]
                     || mUnidentified9E0->mUnidentified007[0]
                     || mUnidentified9E0->mUnidentified007[1])
            {
                controller->SetActiveSlide(lbl_806DEC48, true, false);
            }
            else
            {
                controller->SetActiveSlide(lbl_806DEC4C, true, false);
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

            mUnidentified9E0->fn_802385D0(event, fDeltaT);

            if (mNavigation.fn_8022F2E0(event, fDeltaT))
            {
                fn_801CBCA0(0x4430B152, 0, 0, 1);
                fn_801FEEAC()->Call(lbl_80520888);
                return;
            }

            for (int i = 0; i < 10; ++i)
            {
                mUnidentified200[i].fn_80219608(&event);
            }

            bool previous = false;
            if (mUnidentified9E0->mUnidentified005[1]
                || mUnidentified9E0->mUnidentified009[1])
            {
                previous = true;
            }
            if (previous && !mUnidentified3C)
            {
                if (mUnidentified38 > 0)
                {
                    --mUnidentified38;
                    fn_80266014();
                    fn_801CBCA0(0x375C885A, 0, 0, 1);
                }
            }
            else
            {
                bool next = mUnidentified9E0->mUnidentified005[0]
                         || mUnidentified9E0->mUnidentified009[0];
                if (next && !mUnidentified3C
                    && mUnidentified38 < mUnidentified34 - 1)
                {
                    ++mUnidentified38;
                    fn_80266014();
                    fn_801CBCA0(0x375C885A, 0, 0, 1);
                }
            }
            mUnidentified3C = false;
        }
    }
}

void TU80264E98Scene::fn_80266014()
{
    for (int i = 0; i < 10; ++i)
    {
        bool visible = i < mUnidentified1C - mUnidentified38 * 10;
        mUnidentified9E4[i]->m_bVisible = visible;
        if (visible)
        {
            mUnidentified200[i].mDisabled = true;
            TU80300104Event event;
            mUnidentified200[i].mPreviousEvents[0] = event;
            mUnidentified200[i].mPreviousEvents[1] = event;
            mUnidentified200[i].mPreviousEvents[2] = event;
            mUnidentified200[i].mPreviousEvents[3] = event;
        }
        else
        {
            mUnidentified200[i].mDisabled = false;
        }

        TLInstance* off = FEFinder<TLInstance, 2>::Find(
            mUnidentified9E4[i], nlStringLowerHash(lbl_806DEC30), nlStringLowerHash(lbl_806DEC28), 0, 0, 0, 0);
        if (off == 0)
        {
            off = &lbl_80580138;
        }

        TLInstance* over = FEFinder<TLInstance, 2>::Find(
            mUnidentified9E4[i], nlStringLowerHash(lbl_806DEC34), nlStringLowerHash(lbl_806DEC28), 0, 0, 0, 0);
        if (over == 0)
        {
            over = &lbl_80580138;
        }

        int officialIndex = mUnidentified40[mUnidentified38 * 10 + i];
        bool hasSaveSlot = false;
        if (officialIndex >= 0)
        {
            RFLAdditionalInfo info;
            if (RFLGetAdditionalInfo(&info, RFLDataSource_Official, 0, (u16)officialIndex)
                == RFLErrcode_Success)
            {
                u64 id;
                memcpy(&id, &info.createID, sizeof(id));
                if (GameInfoManager::Instance()->HasSaveSlot(id))
                {
                    hasSaveSlot = true;
                }
                RFLGetFavoriteColor((RFLFavoriteColor)info.color);
            }
        }

        TLInstance* logo = FEFinder<TLInstance, 2>::_Find<TLInstance>(
            off, nlStringLowerHash(lbl_805208A8), 0, 0, 0, 0, 0);
        if (logo == 0)
        {
            logo = &lbl_80580248;
        }
        fn_80302420(logo, hasSaveSlot);

        logo = FEFinder<TLInstance, 2>::_Find<TLInstance>(
            over, nlStringLowerHash(lbl_805208A8), 0, 0, 0, 0, 0);
        if (logo == 0)
        {
            logo = &lbl_80580248;
        }
        fn_80302420(logo, hasSaveSlot);

        unsigned long textureReference = lbl_806E1908->mUnidentified3C[i];
        bool imageReady
            = fn_8026F280(lbl_806E1908, officialIndex, i, false);

        TLImageInstance* image
            = (TLImageInstance*)FEFinder<TLImageInstance, 2>::_Find<TLInstance>(
                off, nlStringLowerHash(lbl_806DEC54), 0, 0, 0, 0, 0);
        if (image == 0)
        {
            image = &lbl_80580248;
        }
        image->m_pTextureResource->fn_8030009C(textureReference);
        fn_80302420(image, imageReady && mUnidentified30);

        image
            = (TLImageInstance*)FEFinder<TLImageInstance, 2>::_Find<TLInstance>(
                over, nlStringLowerHash(lbl_806DEC54), 0, 0, 0, 0, 0);
        if (image == 0)
        {
            image = &lbl_80580248;
        }
        image->m_pTextureResource->fn_8030009C(textureReference);
        fn_80302420(image, imageReady && mUnidentified30);

        TLInstance* background = FEFinder<TLInstance, 2>::_Find<TLInstance>(
            over, nlStringLowerHash(lbl_80520814), 0, 0, 0, 0, 0);
        background->m_bVisible = imageReady;
        fn_80302420(background, imageReady);

        background = FEFinder<TLInstance, 2>::_Find<TLInstance>(
            off, nlStringLowerHash(lbl_80520814), 0, 0, 0, 0, 0);
        background->m_bVisible = imageReady;
        fn_80302420(background, imageReady);
    }

    unsigned short currentPage[4];
    fn_802AA91C(currentPage, 4, lbl_806DEC58, mUnidentified38 + 1);
    unsigned short pageCount[4];
    fn_802AA91C(pageCount, 4, lbl_806DEC58, mUnidentified34);

    WideBasicString formatted(Format(
        WideBasicString(LookupLocString(lbl_805208B4)), currentPage, pageCount));
    nlStrNCpy(mUnidentified1D0, formatted.c_str(), 24);

    TLTextInstance* pages
        = (TLTextInstance*)FEFinder<TLTextInstance, 2>::_Find<TLSlide>(
            mPresentation->GetActiveSlide(), nlStringLowerHash(lbl_806DEC20), nlStringLowerHash(lbl_806DEC60), 0, 0, 0, 0);
    if (pages == 0)
    {
        pages = &UnidentifiedFallbackTextInstance;
    }
    pages->SetString(mUnidentified1D0);

    TU80252180Scene* scene = fn_80253E18();
    if (scene == 0)
    {
        return;
    }

    if (mUnidentified34 == 1)
    {
        fn_80238A1C(mUnidentified9E0, 1, false, false);
        fn_80238A1C(mUnidentified9E0, 0, false, false);
        fn_802534BC(scene, 4, true);
    }
    else if (mUnidentified38 <= 0)
    {
        fn_80238A1C(mUnidentified9E0, 1, true, true);
        fn_80238A1C(mUnidentified9E0, 0, false, false);
        fn_802392D4(mUnidentified9E0, 1);
        fn_802534BC(scene, 5, true);
    }
    else if (mUnidentified38 >= mUnidentified34 - 1)
    {
        fn_80238A1C(mUnidentified9E0, 1, false, false);
        fn_80238A1C(mUnidentified9E0, 0, true, true);
        fn_802392D4(mUnidentified9E0, 0);
        fn_802534BC(scene, 6, true);
    }
    else
    {
        fn_80238A1C(mUnidentified9E0, 1, true, true);
        fn_80238A1C(mUnidentified9E0, 0, true, true);
        fn_802534BC(scene, 7, true);
    }
}

void TU80264E98Scene::fn_80266B68()
{
    TU80300104Base::Callback over(
        Bind<void>(MemFun(&TU80264E98Scene::fn_80266EA0), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback off(
        Bind<void>(MemFun(&TU80264E98Scene::fn_80266F64), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback select(
        Bind<void>(MemFun(&TU80264E98Scene::fn_80266FEC), this, Placeholder<0>(), Placeholder<1>()));

    for (int i = 0; i < 10; ++i)
    {
        mUnidentified200[i].fn_80300D74(
            mUnidentified9E4[i], true, 0.0f, 0.0f, 1.0f, 1.0f);
        mUnidentified200[i].fn_803007C0(over);
        mUnidentified200[i].fn_80300864(off);
        mUnidentified200[i].fn_803009AC(select);
    }
}

void TU80264E98Scene::fn_80266EA0(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    unsigned int which = index;
    ++mUnidentified20[index];
    if (!mUnidentified200[item].fn_802192FC(1, which))
    {
        mUnidentified9E4[item]->SetActiveSlide(lbl_806DEC34, true, false);
        mUnidentified200[item].mValues[which] = 1;
        fn_801CBCA0(0xFFC8A55D, 0, 0, 1);
    }
}

void TU80264E98Scene::fn_80266F64(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    unsigned int which = index;
    --mUnidentified20[index];
    mUnidentified9E4[item]->SetActiveSlide(lbl_806DEC30, true, false);
    mUnidentified200[item].mValues[which] = 0;
}

void TU80264E98Scene::fn_80266FEC(int, void* context)
{
    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide(lbl_806DEC40, true, false);
    }

    fn_80238A1C(mUnidentified9E0, 1, false, false);
    fn_80238A1C(mUnidentified9E0, 0, false, false);

    TU802672D4Scene* scene = (TU802672D4Scene*)lbl_806E1838->Push(
        (SceneList)0x37, SCREEN_FORWARD, false);
    int item = (int)context;
    scene->mUnidentified70 = mUnidentified40[mUnidentified38 * 10 + item];
    scene->mUnidentified74 = item;

    fn_801CBCA0(0xF0AFD586, 0, 0, 1);
    mUnidentified3C = true;
}

void TU80264E98Scene::fn_802670D4()
{
    ResetTask::s_ResetMode = 3;
    ResetTask::s_ResetState = ResetTask::s_ResetState == RS_RUNNING
                                ? RS_STARTRESET
                                : ResetTask::s_ResetState;
}
