#include <RVLFaceLib/RFL_Database.h>
#include <revolution/os/OSTime.h>

#include "unclassified/tu_802672D4.h"

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
#include "Game/Task/ResetTask.h"
#include "NL/nlBind.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalization.h"
#include "NL/nlString.h"
#include "unclassified/tu_802196B0.h"
#include "unclassified/tu_80252180.h"
#include "unclassified/tu_80264E98.h"
#include "unclassified/tu_806E1908.h"

#include <string.h>

extern BaseGameSceneManager* lbl_806E1838;
extern TLComponentInstance* lbl_80578450[4];
extern char lbl_80584360[12];
extern unsigned short lbl_8058436C[12];
extern RFLStoreData lbl_80584384;
extern unsigned char lbl_806E20DC;
extern int lbl_806E20E0;
extern const char* lbl_806DEC78[2];
extern unsigned int lbl_806E18B0;
extern TLImageInstance lbl_80580248;

extern "C" void fn_801CBCA0(
    unsigned long hash, int value0, int value1, int value2);
extern "C" TLComponentInstance* fn_801CA76C(TLSlide* pTopLevel, InlineHasher Level1,
    InlineHasher Level2 = InlineHasher(0UL), InlineHasher Level3 = InlineHasher(0UL),
    InlineHasher Level4 = InlineHasher(0UL), InlineHasher Level5 = InlineHasher(0UL),
    InlineHasher Level6 = InlineHasher(0UL));
extern "C" TLInstance* fn_801FA0C4(TLSlide* pTopLevel, InlineHasher Level1,
    InlineHasher Level2 = InlineHasher(0UL), InlineHasher Level3 = InlineHasher(0UL),
    InlineHasher Level4 = InlineHasher(0UL), InlineHasher Level5 = InlineHasher(0UL),
    InlineHasher Level6 = InlineHasher(0UL));
extern "C" TLTextInstance* fn_801F9EC0(TLInstance* pTopLevel, InlineHasher Level1,
    InlineHasher Level2 = InlineHasher(0UL), InlineHasher Level3 = InlineHasher(0UL),
    InlineHasher Level4 = InlineHasher(0UL), InlineHasher Level5 = InlineHasher(0UL),
    InlineHasher Level6 = InlineHasher(0UL));
extern "C" TLImageInstance* fn_80221F7C(TLInstance* pTopLevel, InlineHasher Level1,
    InlineHasher Level2 = InlineHasher(0UL), InlineHasher Level3 = InlineHasher(0UL),
    InlineHasher Level4 = InlineHasher(0UL), InlineHasher Level5 = InlineHasher(0UL),
    InlineHasher Level6 = InlineHasher(0UL));
extern "C" const unsigned short* fn_801CA950(nlLocalization* localization, const char* name);
extern "C" void fn_80302420(TLInstance* instance, bool value);
extern "C" void fn_80238A1C(TU802384AC*, int, bool, bool);
int fn_802AA91C(unsigned short* buffer, unsigned long size, const unsigned short* format, ...);

TU802672D4Scene::TU802672D4Scene()
    : mUnidentified30(false)
    , mUnidentified31(false)
    , mUnidentified70(-1)
    , mUnidentified74(-1)
    , mUnidentified78(0)
    , mUnidentified80(-1)
    , mComponents()
    , mNavigation()
{
    mUnidentified84[0] = 0;
    mUnidentified104[0] = 0;
    mUnidentified184[0] = 0;

    mUnidentified20[0] = 0;
    mUnidentified20[1] = 0;
    mUnidentified20[2] = 0;
    mUnidentified20[3] = 0;

    mComponents[0].mContext = (void*)0;
    mComponents[0].mIgnoreInputLock = true;
    mComponents[1].mContext = (void*)1;
    mComponents[1].mIgnoreInputLock = true;

    mNavigation.fn_801D2BE8(false);
    mNavigation.fn_801E6CB4(0x36);
    mNavigation.mIgnoreInputLock = true;

    g_pFEInput->PushExclusiveInputLock(this, -1);
}

TU802672D4Scene::~TU802672D4Scene()
{
    g_pFEInput->PopExclusiveInputLock(this);

    TU80252180Scene* scene = fn_80253E18();
    if (scene != 0)
    {
        fn_802533F0(scene);
    }
}

void TU802672D4Scene::fn_8026748C()
{
    TU80300104Base::Callback over(
        Bind<void>(MemFun(&TU802672D4Scene::fn_80268ED0), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback off(
        Bind<void>(MemFun(&TU802672D4Scene::fn_80268F94), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback select(
        Bind<void>(MemFun(&TU802672D4Scene::fn_802677C4), this, Placeholder<0>(), Placeholder<1>()));

    for (int i = 0; i < 2; ++i)
    {
        mComponents[i].fn_80300D74(
            mUnidentified448[i], true, 0.0f, 0.0f, 1.0f, 1.0f);
        mComponents[i].fn_803007C0(over);
        mComponents[i].fn_80300864(off);
        mComponents[i].fn_803009AC(select);
    }
}

void TU802672D4Scene::fn_802677C4(int, void* context)
{
    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
    }

    switch ((int)context)
    {
    case 0:
        fn_801CBCA0(0xF0AFD586, 0, 0, 1);
        if (fn_80267A84())
        {
            if (lbl_806E20E0 >= 0)
            {
                void* saveTime = GameInfoManager::Instance()->GetUnknown0xA88(
                    lbl_806E20E0);
                long long time = OSGetTime();
                memcpy(saveTime, &time, sizeof(time));
            }

            SaveLoad::StartSave(true);
            mUnidentified31 = true;
        }
        break;
    case 1:
    {
        fn_801CBCA0(0xF0AFD586, 0, 0, 1);

        FEPopupMenu* popup = (FEPopupMenu*)lbl_806E1838->Push(
            (SceneList)10, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)0x89,
            Function<FnVoidVoid>(
                Bind<void>(MemFun(&TU802672D4Scene::fn_80267D84), this)),
            Function<FnVoidVoid>(
                Bind<void>(MemFun(&TU802672D4Scene::fn_80267DDC), this)));
        break;
    }
    }
}

bool TU802672D4Scene::fn_80267A84()
{
    int emptySlot = -1;
    lbl_806E20E0 = -1;

    for (int i = 0; i < 10; ++i)
    {
        void* saveId = GameInfoManager::Instance()->GetUnknown0xA80(i);
        unsigned long long id;
        memcpy(&id, saveId, sizeof(id));

        unsigned short index = 0;
        if (!RFLSearchOfficialData((const RFLCreateID*)&id, &index))
        {
            GameInfoManager::Instance()->ClearSaveSlot(i);
        }

        if (*(unsigned long long*)saveId == 0 && emptySlot < 0)
        {
            emptySlot = i;
        }

        if (mUnidentified78 == *(unsigned long long*)saveId)
        {
            lbl_806E20E0 = i;
        }
    }

    if (lbl_806E20E0 < 0)
    {
        if (emptySlot >= 0)
        {
            lbl_806E20E0 = emptySlot;
            void* saveId
                = GameInfoManager::Instance()->GetUnknown0xA80(emptySlot);
            *(unsigned long long*)saveId = mUnidentified78;
        }
        else
        {
            FEPopupMenu* popup = (FEPopupMenu*)lbl_806E1838->Push(
                (SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x8B);
            return false;
        }
    }

    nlStrNCpy(lbl_8058436C, (const unsigned short*)mUnidentified34.name, 11);
    fn_80267E40(lbl_8058436C);
    nlWcsToStr(lbl_8058436C, lbl_80584360, 11);

    RFLStoreData storeData;
    if (RFLGetStoreData(
            &storeData, RFLDataSource_Official, (unsigned short)mUnidentified70)
        == RFLErrcode_Success)
    {
        memcpy(&lbl_80584384, &storeData, sizeof(storeData));
    }

    unsigned short* savedName
        = (unsigned short*)GameInfoManager::Instance()->GetUnknown0xAA8(
            lbl_806E20E0);
    void* savedStoreData = GameInfoManager::Instance()->GetUnknown0xABE(
        lbl_806E20E0);

    if (nlStrCmp(lbl_8058436C, savedName) != 0)
    {
        nlStrNCpy(savedName, lbl_8058436C, 11);
        lbl_806E20DC = true;
    }

    if (memcmp(savedStoreData, &lbl_80584384, sizeof(lbl_80584384)) != 0)
    {
        memcpy(savedStoreData, &lbl_80584384, sizeof(lbl_80584384));
        lbl_806E20DC = true;
    }

    return true;
}

void TU802672D4Scene::fn_80267D84()
{
    GameInfoManager::Instance()->ClearSaveSlot(mUnidentified80);
    SaveLoad::StartSave(true);

    TU80264E98Scene* scene = (TU80264E98Scene*)lbl_806E1838->GetScene(
        (SceneList)0x36);
    scene->fn_80266014();
    lbl_806E1838->Pop();
}

void TU802672D4Scene::fn_80267DDC()
{
}

void TU802672D4Scene::fn_80267DE0()
{
    TU80264E98Scene* scene = (TU80264E98Scene*)lbl_806E1838->GetScene(
        (SceneList)0x36);
    scene->fn_80266014();
    lbl_806E1838->Pop();
}

void TU802672D4Scene::fn_80267E20()
{
    ResetTask::s_ResetMode = 3;
    ResetTask::s_ResetState = ResetTask::s_ResetState == RS_RUNNING
                                ? RS_STARTRESET
                                : ResetTask::s_ResetState;
}

void TU802672D4Scene::fn_80267E40(unsigned short* name)
{
    for (unsigned long i = 0; i < nlStrLen(name); i++)
    {
        switch (name[i])
        {
        case 0x037E:
            name[i] = ';';
            break;
        case 0x0384:
            name[i] = '\'';
            break;
        case 0x007B:
        case 0x0385:
        case 0x0386:
        case 0x0388:
        case 0x0389:
        case 0x038A:
        case 0x038C:
        case 0x038E:
        case 0x038F:
        case 0x0390:
        case 0x03B0:
        case 0x03C2:
        case 0x03CA:
        case 0x03CB:
        case 0x03CC:
        case 0x03CD:
        case 0x03CE:
            name[i] = '?';
            break;
        }

        if (g_pLocalization->m_CurrentLanguage != nlLocalization::LangJapanese)
        {
            switch (name[i])
            {
            case 0x0387:
            case 0x03AA:
            case 0x03AB:
            case 0x03AC:
            case 0x03AD:
            case 0x03AE:
            case 0x03AF:
                name[i] = '?';
                break;
            }
        }
    }
}

void TU802672D4Scene::SceneCreated()
{
    if (mUnidentified70 >= 0)
    {
        RFLErrcode error = RFLGetAdditionalInfo(&mUnidentified34,
            RFLDataSource_Official,
            0,
            (unsigned short)mUnidentified70);
        if (error == RFLErrcode_Success)
        {
            memcpy(&mUnidentified78, &mUnidentified34.createID, sizeof(mUnidentified78));
            mUnidentified80 = GameInfoManager::Instance()->FindSaveSlot(mUnidentified78);
        }
        else if (error == RFLErrcode_Broken)
        {
            FEPopupMenu* popup = (FEPopupMenu*)lbl_806E1838->Push(
                (SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x87,
                Function<FnVoidVoid>(Bind<void>(MemFun(&TU802672D4Scene::fn_80267E20), this)));
        }
    }
    else
    {
        FEPopupMenu* popup = (FEPopupMenu*)lbl_806E1838->Push(
            (SceneList)10, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)0x88,
            Function<FnVoidVoid>(Bind<void>(MemFun(&TU802672D4Scene::fn_80267DE0), this)));
    }

    for (int i = 0; i < 2; ++i)
    {
        mUnidentified448[i] = fn_801CA76C(GetPresentation()->GetActiveSlide(),
            InlineHasher("Layer"),
            InlineHasher(lbl_806DEC78[i]));
    }

    TLInstance* box = fn_801FA0C4(GetPresentation()->GetActiveSlide(),
        InlineHasher("Layer"),
        InlineHasher("PLAYER_BOX"));
    TLTextInstance* nameText = fn_801F9EC0(box, InlineHasher("NAME"));
    nlStrNCpy(mUnidentified104, (const unsigned short*)mUnidentified34.name, 11);
    fn_80267E40(mUnidentified104);
    nameText->SetString(mUnidentified104);

    if (!GameInfoManager::Instance()->HasSaveSlot(mUnidentified78))
    {
        FEPopupMenu* popup = (FEPopupMenu*)lbl_806E1838->Push(
            (SceneList)10, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)0x8A);
        mUnidentified448[1]->m_bVisible = false;
        mComponents[1].fn_80206B54();
        fn_801F9EC0(box, InlineHasher("DATE"))->m_bVisible = false;
        fn_801F9EC0(box, InlineHasher("lifetime_record"))->m_bVisible = false;
    }
    else
    {
        OSCalendarTime calendar;
        OSTicksToCalendarTime(
            *(OSTime*)GameInfoManager::Instance()->GetUnknown0xA88(mUnidentified80), &calendar);

        unsigned short day[5];
        unsigned short month[5];
        unsigned short year[5];
        fn_802AA91C(day, 5, (const unsigned short*)L"%.2d", calendar.mday);
        fn_802AA91C(month, 5, (const unsigned short*)L"%.2d", calendar.month + 1);
        fn_802AA91C(year, 5, (const unsigned short*)L"%.4d", calendar.year);

        typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;
        WideBasicString formattedDate = Format(
            WideBasicString(fn_801CA950(g_pLocalization, "ONLINE_MII_SELECT_DATE")), day, month, year);
        nlStrNCpy(mUnidentified84, formattedDate.c_str(), 0x40);
        fn_801F9EC0(box, InlineHasher("DATE"))->SetString(mUnidentified84);

        int wins = GameInfoManager::Instance()->GetUnknown0xAA0Total(mUnidentified80);
        int losses = GameInfoManager::Instance()->GetUnknown0xAA4Total(mUnidentified80);
        WideBasicString formattedRecord = Format(
            WideBasicString(fn_801CA950(g_pLocalization, "ONLINE_TOTAL_WINS_LOSSES")), wins, losses);
        nlStrNCpy(mUnidentified184, formattedRecord.c_str(), 0x40);
        fn_801F9EC0(box, InlineHasher("lifetime_record"))->SetString(mUnidentified184);
        fn_80302420(fn_80221F7C(box, InlineHasher("logo_32x32")), true);
    }

    TU80252180Scene* scene = fn_80253E18();
    TLComponentInstance* screen = 0;
    if (scene != 0)
    {
        fn_802534BC(scene, 4, true);
        screen = fn_80253D70(scene, 4);
        mUnidentified444 = &scene->mUnidentified058;
        fn_80238A1C(mUnidentified444, 1, true, false);
        fn_80238A1C(mUnidentified444, 0, true, false);
    }
    mNavigation.fn_8022F194(screen);

    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
    }
}

void TU802672D4Scene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    if (!mUnidentified30)
    {
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (slide->GetCurrentTime() < slide->m_start + slide->m_duration)
        {
            return;
        }

        fn_8026748C();
        mUnidentified30 = true;

        TLImageInstance* image = FEFinder<TLImageInstance, 2>::Find(
            mPresentation->GetActiveSlide(),
            nlStringLowerHash("Layer"),
            nlStringLowerHash("PLAYER_BOX"),
            nlStringLowerHash("Mii"),
            0,
            0,
            0);
        if (image == 0)
        {
            image = &lbl_80580248;
        }

        unsigned long textureReference = lbl_806E1908->mUnidentified3C[mUnidentified74];
        image->m_pTextureResource->fn_8030009C(textureReference);
        fn_80302420(image, true);

        for (int i = 0; i < 4; ++i)
        {
            lbl_80578450[i]->SetActiveSlide("cursor", true, false);
        }
    }

    if (mUnidentified31)
    {
        if (SaveEnabled && SaveLoad::CardBusy())
        {
            return;
        }
        lbl_806E1838->Pop();
        lbl_806E1838->Push((SceneList)0x33, SCREEN_FORWARD, true);
    }

    for (unsigned int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* controller = lbl_80578450[pad];
        if (pad != lbl_806E18B0)
        {
            controller->SetActiveSlide("waiting", true, false);
        }
        else
        {
            unsigned char valid = 1;
            TU80300104Event event;
            event.mIndex = pad;
            event.mPosition = fn_802197FC(pad, &valid);
            event.mFlag0 = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
            event.mFlag1 = g_pFEInput->JustReleased((eFEINPUT_PAD)pad, 0x1E, true, 0);

            for (int i = 0; i < 2; ++i)
            {
                mComponents[i].fn_80219608(&event);
            }

            if (mNavigation.fn_8022F2E0(event, fDeltaT))
            {
                TU80264E98Scene* scene = (TU80264E98Scene*)lbl_806E1838->GetScene((SceneList)0x36);
                scene->fn_80266014();
            }
        }
    }
}

void TU802672D4Scene::fn_80268ED0(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    unsigned int which = index;
    ++mUnidentified20[index];
    if (!mComponents[item].fn_802192FC(1, which))
    {
        mUnidentified448[item]->SetActiveSlide("over", true, false);
        mComponents[item].mValues[which] = 1;
        fn_801CBCA0(0xDE912775, 0, 0, 1);
    }
}
