#include <RVLFaceLib/RFL_Database.h>
#include "Game/FE/FEAudio.h"
#include <revolution/os/OSTime.h>

#include "Game/SH/SHOnlineMiiSelectOverlay.h"

#include "Game/GameSceneManager.h"
#include "Game/DB/SaveLoad.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/feTextureResource.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/GameInfo.h"
#include "Game/NetworkSession.h"
#include "Game/Task/ResetTask.h"
#include "NL/nlPrint.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"
#include "NL/nlFormat.h"
#include "NL/nlLocalization.h"
#include "NL/nlString.h"
#include "Game/FE/feDPD.h"
#include "Game/SH/SHNavigation.h"
#include "Game/SH/SHOnlineMiiSelect.h"
#include "Game/MiiManager.h"

#include <string.h>
#include "Game/FE/fePageControls.h"
#include "Game/NetworkSession.h"
#include "NL/nlstring_tmpl.h"

static const char* sButtonNames[2] = { "USE", "UNLINK" };


SHOnlineMiiSelectOverlay::SHOnlineMiiSelectOverlay()
    : mInitialized(false)
    , mSavePending(false)
    , mOfficialIndex(-1)
    , mIconIndex(-1)
    , mCreateID(0)
    , mSaveSlot(-1)
    , mButtons()
    , mBackButton()
{
    mDateText[0] = 0;
    mNameText[0] = 0;
    mRecordText[0] = 0;

    mHoverCounts[0] = 0;
    mHoverCounts[1] = 0;
    mHoverCounts[2] = 0;
    mHoverCounts[3] = 0;

    mButtons[0].mContext = (void*)0;
    mButtons[0].mIgnoreInputLock = true;
    mButtons[1].mContext = (void*)1;
    mButtons[1].mIgnoreInputLock = true;

    mBackButton.SetPushBackScene(false);
    mBackButton.SetBackScene(SCENE_ONLINE_MII_SELECT);
    mBackButton.mIgnoreInputLock = true;

    g_pFEInput->PushExclusiveInputLock(this, -1);
}

SHOnlineMiiSelectOverlay::~SHOnlineMiiSelectOverlay()
{
    g_pFEInput->PopExclusiveInputLock(this);

    SHNavigation* scene = GetNavigationScene();
    if (scene != 0)
    {
        scene->RestoreButtonVisibility();
    }
}

void SHOnlineMiiSelectOverlay::InitializeButtons()
{
    FEPointerListener::Callback over(
        Bind<void>(MemFun(&SHOnlineMiiSelectOverlay::OpenItem), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback off(
        Bind<void>(MemFun(&SHOnlineMiiSelectOverlay::CloseItem), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback select(
        Bind<void>(MemFun(&SHOnlineMiiSelectOverlay::SelectOption), this, Placeholder<0>(), Placeholder<1>()));

    for (int i = 0; i < 2; ++i)
    {
        mButtons[i].SetInstanceBounds(
            mButtonInstances[i], true, 0.0f, 0.0f, 1.0f, 1.0f);
        mButtons[i].SetPointerEnterCallback(over);
        mButtons[i].SetPointerLeaveCallback(off);
        mButtons[i].SetPointerPressCallback(select);
    }
}

void SHOnlineMiiSelectOverlay::SelectOption(int, void* context)
{
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }

    switch ((int)context)
    {
    case 0:
        FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
        if (SelectMii())
        {
            if (gNetworkSaveSlotIndex >= 0)
            {
                void* saveTime = GameInfoManager::Instance()->GetUnknown0xA88(
                    gNetworkSaveSlotIndex);
                long long time = OSGetTime();
                memcpy(saveTime, &time, sizeof(time));
            }

            SaveLoad::StartSave(true);
            mSavePending = true;
        }
        break;
    case 1:
    {
        FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);

        FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
            (SceneList)10, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)0x89,
            Function<FnVoidVoid>(
                Bind<void>(MemFun(&SHOnlineMiiSelectOverlay::DeleteSaveSlot), this)),
            Function<FnVoidVoid>(
                Bind<void>(MemFun(&SHOnlineMiiSelectOverlay::CancelDeleteSaveSlot), this)));
        break;
    }
    }
}

bool SHOnlineMiiSelectOverlay::SelectMii()
{
    int emptySlot = -1;
    gNetworkSaveSlotIndex = -1;

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

        if (mCreateID == *(unsigned long long*)saveId)
        {
            gNetworkSaveSlotIndex = i;
        }
    }

    if (gNetworkSaveSlotIndex < 0)
    {
        if (emptySlot >= 0)
        {
            gNetworkSaveSlotIndex = emptySlot;
            void* saveId
                = GameInfoManager::Instance()->GetUnknown0xA80(emptySlot);
            *(unsigned long long*)saveId = mCreateID;
        }
        else
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
                (SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x8B);
            return false;
        }
    }

    nlStrNCpy(gNetworkMiiNameWide, (const unsigned short*)mMiiInfo.name, 11);
    SanitizeMiiName(gNetworkMiiNameWide);
    nlWcsToStr(gNetworkMiiNameWide, gNetworkMiiName, 11);

    RFLStoreData storeData;
    if (RFLGetStoreData(
            &storeData, RFLDataSource_Official, (unsigned short)mOfficialIndex)
        == RFLErrcode_Success)
    {
        memcpy(&gNetworkMiiData, &storeData, sizeof(storeData));
    }

    unsigned short* savedName
        = (unsigned short*)GameInfoManager::Instance()->GetUnknown0xAA8(
            gNetworkSaveSlotIndex);
    void* savedStoreData = GameInfoManager::Instance()->GetUnknown0xABE(
        gNetworkSaveSlotIndex);

    if (nlStrCmp(gNetworkMiiNameWide, savedName) != 0)
    {
        nlStrNCpy(savedName, gNetworkMiiNameWide, 11);
        gNetworkMiiChanged = true;
    }

    if (memcmp(savedStoreData, &gNetworkMiiData, sizeof(gNetworkMiiData)) != 0)
    {
        memcpy(savedStoreData, &gNetworkMiiData, sizeof(gNetworkMiiData));
        gNetworkMiiChanged = true;
    }

    return true;
}

void SHOnlineMiiSelectOverlay::DeleteSaveSlot()
{
    GameInfoManager::Instance()->ClearSaveSlot(mSaveSlot);
    SaveLoad::StartSave(true);

    SHOnlineMiiSelect* scene = (SHOnlineMiiSelect*)GameSceneManager::Instance()->GetScene(
        SCENE_ONLINE_MII_SELECT);
    scene->UpdatePage();
    GameSceneManager::Instance()->Pop();
}

void SHOnlineMiiSelectOverlay::CancelDeleteSaveSlot()
{
}

void SHOnlineMiiSelectOverlay::ReturnToMiiSelect()
{
    SHOnlineMiiSelect* scene = (SHOnlineMiiSelect*)GameSceneManager::Instance()->GetScene(
        SCENE_ONLINE_MII_SELECT);
    scene->UpdatePage();
    GameSceneManager::Instance()->Pop();
}

void SHOnlineMiiSelectOverlay::ReturnToWiiMenu()
{
    ResetTask::s_ResetMode = 3;
    ResetTask::s_ResetState = ResetTask::s_ResetState == RS_RUNNING
                                ? RS_STARTRESET
                                : ResetTask::s_ResetState;
}

void SHOnlineMiiSelectOverlay::SanitizeMiiName(unsigned short* name)
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

void SHOnlineMiiSelectOverlay::SceneCreated()
{
    if (mOfficialIndex >= 0)
    {
        RFLErrcode error = RFLGetAdditionalInfo(&mMiiInfo,
            RFLDataSource_Official,
            0,
            (unsigned short)mOfficialIndex);
        if (error == RFLErrcode_Success)
        {
            memcpy(&mCreateID, &mMiiInfo.createID, sizeof(mCreateID));
            mSaveSlot = GameInfoManager::Instance()->FindSaveSlot(mCreateID);
        }
        else if (error == RFLErrcode_Broken)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
                (SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x87,
                Function<FnVoidVoid>(Bind<void>(MemFun(&SHOnlineMiiSelectOverlay::ReturnToWiiMenu), this)));
        }
    }
    else
    {
        FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
            (SceneList)10, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)0x88,
            Function<FnVoidVoid>(Bind<void>(MemFun(&SHOnlineMiiSelectOverlay::ReturnToMiiSelect), this)));
    }

    for (int i = 0; i < 2; ++i)
    {
        mButtonInstances[i] = FEFinder<TLComponentInstance, 4>::FindOrDefault<TLSlide>(GetPresentation()->GetActiveSlide(),
            InlineHasher("Layer"),
            InlineHasher(sButtonNames[i]));
    }

    TLInstance* box = FEFinder<TLInstance, 5>::FindOrDefault<TLSlide>(GetPresentation()->GetActiveSlide(),
        InlineHasher("Layer"),
        InlineHasher("PLAYER_BOX"));
    TLTextInstance* nameText = FEFinder<TLTextInstance, 3>::FindOrDefault<TLInstance>(box, InlineHasher("NAME"));
    nlStrNCpy(mNameText, (const unsigned short*)mMiiInfo.name, 11);
    SanitizeMiiName(mNameText);
    nameText->SetString(mNameText);

    if (!GameInfoManager::Instance()->HasSaveSlot(mCreateID))
    {
        FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
            (SceneList)10, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)0x8A);
        mButtonInstances[1]->SetVisible(false);
        mButtons[1].Disable();
        FEFinder<TLTextInstance, 3>::FindOrDefault<TLInstance>(box, InlineHasher("DATE"))->SetVisible(false);
        FEFinder<TLTextInstance, 3>::FindOrDefault<TLInstance>(box, InlineHasher("lifetime_record"))->SetVisible(false);
    }
    else
    {
        OSCalendarTime calendar;
        OSTicksToCalendarTime(
            *(OSTime*)GameInfoManager::Instance()->GetUnknown0xA88(mSaveSlot), &calendar);

        unsigned short day[5];
        unsigned short month[5];
        unsigned short year[5];
        nlSNPrintf(day, 5, (const unsigned short*)L"%.2d", calendar.mday);
        nlSNPrintf(month, 5, (const unsigned short*)L"%.2d", calendar.month + 1);
        nlSNPrintf(year, 5, (const unsigned short*)L"%.4d", calendar.year);

        typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;
        WideBasicString formattedDate = Format(
            WideBasicString(g_pLocalization->GetString("ONLINE_MII_SELECT_DATE")), day, month, year);
        nlStrNCpy(mDateText, formattedDate.c_str(), 0x40);
        FEFinder<TLTextInstance, 3>::FindOrDefault<TLInstance>(box, InlineHasher("DATE"))->SetString(mDateText);

        int wins = GameInfoManager::Instance()->GetUnknown0xAA0Total(mSaveSlot);
        int losses = GameInfoManager::Instance()->GetUnknown0xAA4Total(mSaveSlot);
        WideBasicString formattedRecord = Format(
            WideBasicString(g_pLocalization->GetString("ONLINE_TOTAL_WINS_LOSSES")), wins, losses);
        nlStrNCpy(mRecordText, formattedRecord.c_str(), 0x40);
        FEFinder<TLTextInstance, 3>::FindOrDefault<TLInstance>(box, InlineHasher("lifetime_record"))->SetString(mRecordText);
        FEFinder<TLImageInstance, 2>::FindOrDefault<TLInstance>(box, InlineHasher("logo_32x32"))->SetAssetVisible(true);
    }

    SHNavigation* scene = GetNavigationScene();
    TLComponentInstance* screen = 0;
    if (scene != 0)
    {
        scene->SetButtons(4, true);
        screen = scene->GetButton(4);
        mPageControls = scene->GetPageControls();
        mPageControls->SetButtonState(1, true, false);
        mPageControls->SetButtonState(0, true, false);
    }
    mBackButton.SetButtonInstance(screen);

    for (int i = 0; i < 4; ++i)
    {
        gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
    }
}

void SHOnlineMiiSelectOverlay::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    if (!mInitialized)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->m_start + slide->m_duration)
        {
            return;
        }

        InitializeButtons();
        mInitialized = true;

        TLImageInstance* image = FEFinder<TLImageInstance, 2>::Find(
            mPresentation->m_currentSlide,
            nlStringLowerHash("Layer"),
            nlStringLowerHash("PLAYER_BOX"),
            nlStringLowerHash("Mii"),
            0,
            0,
            0);
        if (image == 0)
        {
            image = &UnidentifiedTLImageDefault::sInstance;
        }

        unsigned long textureReference = g_pMiiManager->mIconTextureIds[mIconIndex];
        image->m_pTextureResource->SetTextureHandle(textureReference);
        image->SetAssetVisible(true);

        for (int i = 0; i < 4; ++i)
        {
            GetPointerInstance(i)->SetActiveSlide("cursor", true, false);
        }
    }

    if (mSavePending)
    {
        if (SaveEnabled && SaveLoad::CardBusy())
        {
            return;
        }
        GameSceneManager::Instance()->Pop();
        GameSceneManager::Instance()->Push(SCENE_ONLINE_LOGIN, SCREEN_FORWARD, true);
    }

    for (unsigned int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* controller = GetPointerInstance(pad);
        if (pad != gFEControllerIndex)
        {
            controller->SetActiveSlide("waiting", true, false);
        }
        else
        {
            unsigned char valid = 1;
            FEPointerEvent event;
            event.mIndex = pad;
            event.mPosition = GetPointerPosition(pad, &valid);
            event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
            event.mReleased = g_pFEInput->JustReleased((eFEINPUT_PAD)pad, 0x1E, true, 0);

            for (int i = 0; i < 2; ++i)
            {
                mButtons[i].HandlePointerEvent(&event);
            }

            if (mBackButton.UpdateBackButton(event, fDeltaT))
            {
                SHOnlineMiiSelect* scene = (SHOnlineMiiSelect*)GameSceneManager::Instance()->GetScene(SCENE_ONLINE_MII_SELECT);
                scene->UpdatePage();
            }
        }
    }
}

void SHOnlineMiiSelectOverlay::OpenItem(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    ++mHoverCounts[index];
    if (!mButtons[item].HasOtherPointerState(1, index))
    {
        mButtonInstances[item]->SetActiveSlide("over", true, false);
        mButtons[item].SetPointerState(1, index);
        FEAudio::PlayAnimAudioEvent(0xDE912775, 0, 0, 1);
    }
}

void SHOnlineMiiSelectOverlay::CloseItem(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    --mHoverCounts[index];
    mButtonInstances[item]->SetActiveSlide("off", true, false);
    mButtons[item].SetPointerState(0, index);
}

#include "Game/FE/feFinder_impl.h"
