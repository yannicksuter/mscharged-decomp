#include <RVLFaceLib/RFL_Database.h>
#include "NL/nlBasicString.inl"
#include <RVLFaceLib/RFL_DataUtility.h>
#include "Game/FE/FEAudio.h"
#include <RVLFaceLib/RFL_Model.h>

#include "Game/SH/SHOnlineMiiSelect.h"

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
#include "Game/Render/Presentation.h"
#include "Game/Task/ResetTask.h"
#include "NL/nlAlgorithm.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalization.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"
#include "Game/FE/feDPD.h"
#include "Game/SH/SHNavigation.h"
#include "Game/SH/SHOnlineMiiSelectOverlay.h"
#include "Game/MiiManager.h"

#include <string.h>
#include "Game/FE/fePageControls.h"
#include "NL/nlstring_tmpl.h"
#include "Game/FE/FEAudio.h"
#include "Game/UnidentifiedStaticStorage.h"

extern char lbl_80520808[];
extern char lbl_80520814[];
extern char lbl_80520838[];
extern char lbl_80520888[];
extern char lbl_805208A8[];
extern char lbl_805208B4[];

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

SHOnlineMiiSelect::SHOnlineMiiSelect()
    : mInitialized(false)
    , mCurrentPage(0)
    , mSuppressPageInput(false)
    , mMiiButtons()
    , mBackButton()
{
    mMiiCount = RFLGetAvailableOfficialDataNum();
    mPageCount = mMiiCount / 10 + (mMiiCount % 10 != 0);
    if (mPageCount == 0)
    {
        mPageCount = 1;
    }

    mHoverCounts[0] = 0;
    mHoverCounts[1] = 0;
    mHoverCounts[2] = 0;
    mHoverCounts[3] = 0;

    for (int i = 0; i < 10; ++i)
    {
        mMiiButtons[i].mContext = (void*)i;
    }

    memset(mOfficialIndices, -1, sizeof(mOfficialIndices));
    mBackButton.SetPushBackScene(false);
}

SHOnlineMiiSelect::~SHOnlineMiiSelect()
{
}

void SHOnlineMiiSelect::SceneCreated()
{
    for (int i = 0; i < 10; ++i)
    {
        char componentName[16];
        nlSNPrintf(componentName, sizeof(componentName), lbl_80520838, i + 1);

        TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find(
            mPresentation->m_currentSlide,
            nlStringLowerHash("Layer"),
            nlStringLowerHash(lbl_80520808),
            nlStringLowerHash(componentName),
            0,
            0,
            0);
        if (component == 0)
        {
            component = &UnidentifiedTLComponentDefault::sInstance;
        }
        mMiiInstances[i] = component;

        TLInstance* off = FEFinder<TLInstance, 5>::Find(mMiiInstances[i],
            nlStringLowerHash("off"),
            nlStringLowerHash("Mii_btn"),
            0,
            0,
            0,
            0);
        if (off == 0)
        {
            off = &UnidentifiedTLGroupDefault::sInstance;
        }

        TLInstance* over = FEFinder<TLInstance, 5>::Find(mMiiInstances[i],
            nlStringLowerHash("over"),
            nlStringLowerHash("Mii_btn"),
            0,
            0,
            0,
            0);
        if (over == 0)
        {
            over = &UnidentifiedTLGroupDefault::sInstance;
        }

        TLImageInstance* overBackground = FEFinder<TLImageInstance, 2>::Find(
            over, nlStringLowerHash(lbl_80520814), 0, 0, 0, 0, 0);
        overBackground->m_bVisible = false;
        overBackground->SetAssetVisible(false);

        TLImageInstance* offBackground = FEFinder<TLImageInstance, 2>::Find(
            off, nlStringLowerHash(lbl_80520814), 0, 0, 0, 0, 0);
        offBackground->m_bVisible = false;
        offBackground->SetAssetVisible(false);
    }

    BuildMiiList();

    SHNavigation* scene = GetNavigationScene();
    TLComponentInstance* screen = 0;
    if (scene != 0)
    {
        scene->SetButtons(7, true);
        screen = scene->GetButton(4);
        mPageControls = &scene->mPageControls;
        mPageControls->SetButtonState(1, false, false);
        mPageControls->SetButtonState(0, false, false);
    }

    mBackButton.SetButtonInstance(screen);

    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }

    UpdatePage();
}

void SHOnlineMiiSelect::ClearMissingMiiSaveSlots()
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

        SHNavigation* scene = GetNavigationScene();
        if (scene != 0)
        {
            scene->HideButtons();

            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
                (SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x8C,
                Function<FnVoidVoid>(
                    Bind<void>(MemFun(&SHOnlineMiiSelect::UpdatePage), this)));
        }
    }
}

void SHOnlineMiiSelect::BuildMiiList()
{
    unsigned int count = 0;
    RFLAdditionalInfo info;
    u64 id;

    for (unsigned int i = 0; i < RFL_DB_CHAR_MAX; ++i)
    {
        RFLErrcode result = RFLGetAdditionalInfo(&info, RFLDataSource_Official, 0, i);
        if (result == RFLErrcode_Broken)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
                (SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x87,
                Function<FnVoidVoid>(
                    Bind<void>(MemFun(&SHOnlineMiiSelect::ReturnToWiiMenu), this)));
            return;
        }

        if (result == RFLErrcode_Success)
        {
            memcpy(&id, &info.createID, sizeof(id));
            if (GameInfoManager::Instance()->HasSaveSlot(id) && info.favorite)
            {
                mOfficialIndices[count++] = (u16)i;
            }
        }
    }

    for (unsigned int i = 0; i < RFL_DB_CHAR_MAX; ++i)
    {
        RFLErrcode result = RFLGetAdditionalInfo(&info, RFLDataSource_Official, 0, i);
        if (result == RFLErrcode_Broken)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
                (SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x87,
                Function<FnVoidVoid>(
                    Bind<void>(MemFun(&SHOnlineMiiSelect::ReturnToWiiMenu), this)));
            return;
        }

        if (result == RFLErrcode_Success)
        {
            memcpy(&id, &info.createID, sizeof(id));
            if (GameInfoManager::Instance()->HasSaveSlot(id) && !info.favorite)
            {
                mOfficialIndices[count++] = (u16)i;
            }
        }
    }

    for (unsigned int i = 0; i < RFL_DB_CHAR_MAX; ++i)
    {
        RFLErrcode result = RFLGetAdditionalInfo(&info, RFLDataSource_Official, 0, i);
        if (result == RFLErrcode_Broken)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
                (SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x87,
                Function<FnVoidVoid>(
                    Bind<void>(MemFun(&SHOnlineMiiSelect::ReturnToWiiMenu), this)));
            return;
        }

        if (result == RFLErrcode_Success)
        {
            memcpy(&id, &info.createID, sizeof(id));
            if (!GameInfoManager::Instance()->HasSaveSlot(id) && info.favorite)
            {
                mOfficialIndices[count++] = (u16)i;
            }
        }
    }

    for (unsigned int i = 0; i < RFL_DB_CHAR_MAX; ++i)
    {
        RFLErrcode result = RFLGetAdditionalInfo(&info, RFLDataSource_Official, 0, i);
        if (result == RFLErrcode_Broken)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
                (SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x87,
                Function<FnVoidVoid>(
                    Bind<void>(MemFun(&SHOnlineMiiSelect::ReturnToWiiMenu), this)));
            return;
        }

        if (result == RFLErrcode_Success)
        {
            memcpy(&id, &info.createID, sizeof(id));
            if (!GameInfoManager::Instance()->HasSaveSlot(id) && !info.favorite)
            {
                mOfficialIndices[count++] = (u16)i;
            }
        }
    }
}

void SHOnlineMiiSelect::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    if (!mInitialized)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->GetStartTime() + slide->GetDuration())
        {
            return;
        }

        InitializeButtons();
        mInitialized = true;
        UpdatePage();
        ClearMissingMiiSaveSlots();
    }

    for (unsigned int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* controller = GetPointerInstance(pad);
        bool processInput = true;
        if (g_pFEInput->m_InputLockDepth == 0)
        {
            if (pad != gFEControllerIndex)
            {
                controller->SetActiveSlide("waiting", true, false);
                processInput = false;
            }
            else if (mHoverCounts[pad] > 0
                     || mBackButton.mPointerInside[pad]
                     || mPageControls->mPointerInside[0]
                     || mPageControls->mPointerInside[1])
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
            FEPointerEvent event;
            event.mIndex = pad;
            event.mPosition = GetPointerPosition(pad, &valid);
            event.mPressed
                = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
            event.mReleased
                = g_pFEInput->JustReleased((eFEINPUT_PAD)pad, 0x1E, true, 0);

            mPageControls->Update(event, fDeltaT);

            if (mBackButton.UpdateBackButton(event, fDeltaT))
            {
                FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, 1);
                Presentation::GetInstance()->Call(lbl_80520888);
                return;
            }

            for (int i = 0; i < 10; ++i)
            {
                mMiiButtons[i].HandlePointerEvent(&event);
            }

            bool previous = false;
            if (mPageControls->mPointerPressed[1]
                || mPageControls->mPadPressed[1])
            {
                previous = true;
            }
            if (previous && !mSuppressPageInput)
            {
                if (mCurrentPage > 0)
                {
                    --mCurrentPage;
                    UpdatePage();
                    FEAudio::PlayAnimAudioEvent(0x375C885A, 0, 0, 1);
                }
            }
            else
            {
                bool next = mPageControls->mPointerPressed[0]
                         || mPageControls->mPadPressed[0];
                if (next && !mSuppressPageInput
                    && mCurrentPage < mPageCount - 1)
                {
                    ++mCurrentPage;
                    UpdatePage();
                    FEAudio::PlayAnimAudioEvent(0x375C885A, 0, 0, 1);
                }
            }
            mSuppressPageInput = false;
        }
    }
}

void SHOnlineMiiSelect::UpdatePage()
{
    for (int i = 0; i < 10; ++i)
    {
        bool visible = i < mMiiCount - mCurrentPage * 10;
        mMiiInstances[i]->m_bVisible = visible;
        if (visible)
        {
            mMiiButtons[i].Enable();
        }
        else
        {
            mMiiButtons[i].Disable();
        }

        TLInstance* off = FEFinder<TLInstance, 5>::Find(
            mMiiInstances[i], nlStringLowerHash("off"), nlStringLowerHash("Mii_btn"), 0, 0, 0, 0);
        if (off == 0)
        {
            off = &UnidentifiedTLGroupDefault::sInstance;
        }

        TLInstance* over = FEFinder<TLInstance, 5>::Find(
            mMiiInstances[i], nlStringLowerHash("over"), nlStringLowerHash("Mii_btn"), 0, 0, 0, 0);
        if (over == 0)
        {
            over = &UnidentifiedTLGroupDefault::sInstance;
        }

        int officialIndex = mOfficialIndices[mCurrentPage * 10 + i];
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

        TLImageInstance* logo = FEFinder<TLImageInstance, 2>::Find(
            off, nlStringLowerHash(lbl_805208A8), 0, 0, 0, 0, 0);
        if (logo == 0)
        {
            logo = &UnidentifiedTLImageDefault::sInstance;
        }
        logo->SetAssetVisible(hasSaveSlot);

        logo = FEFinder<TLImageInstance, 2>::Find(
            over, nlStringLowerHash(lbl_805208A8), 0, 0, 0, 0, 0);
        if (logo == 0)
        {
            logo = &UnidentifiedTLImageDefault::sInstance;
        }
        logo->SetAssetVisible(hasSaveSlot);

        unsigned long textureReference = g_pMiiManager->mIconTextureIds[i];
        bool imageReady
            = g_pMiiManager->CreateIcon(officialIndex, i, RFLExp_Normal);

        TLImageInstance* image
            = FEFinder<TLImageInstance, 2>::Find(
                off, nlStringLowerHash("Mii"), 0, 0, 0, 0, 0);
        if (image == 0)
        {
            image = &UnidentifiedTLImageDefault::sInstance;
        }
        image->m_pTextureResource->SetTextureHandle(textureReference);
        image->SetAssetVisible(imageReady && mInitialized);

        image
            = FEFinder<TLImageInstance, 2>::Find(
                over, nlStringLowerHash("Mii"), 0, 0, 0, 0, 0);
        if (image == 0)
        {
            image = &UnidentifiedTLImageDefault::sInstance;
        }
        image->m_pTextureResource->SetTextureHandle(textureReference);
        image->SetAssetVisible(imageReady && mInitialized);

        TLImageInstance* background = FEFinder<TLImageInstance, 2>::Find(
            over, nlStringLowerHash(lbl_80520814), 0, 0, 0, 0, 0);
        background->m_bVisible = imageReady;
        background->SetAssetVisible(imageReady);

        background = FEFinder<TLImageInstance, 2>::Find(
            off, nlStringLowerHash(lbl_80520814), 0, 0, 0, 0, 0);
        background->m_bVisible = imageReady;
        background->SetAssetVisible(imageReady);
    }

    unsigned short currentPage[4];
    nlSNPrintf(currentPage, 4, (const unsigned short*)L"%d", mCurrentPage + 1);
    unsigned short pageCount[4];
    nlSNPrintf(pageCount, 4, (const unsigned short*)L"%d", mPageCount);

    WideBasicString formatted(Format(
        WideBasicString(LookupLocString(lbl_805208B4)), currentPage, pageCount));
    nlStrNCpy(mPageText, formatted.c_str(), 24);

    TLTextInstance* pages
        = FEFinder<TLTextInstance, 3>::Find(
            mPresentation->m_currentSlide, nlStringLowerHash("Layer"), nlStringLowerHash("PAGES"), 0, 0, 0, 0);
    if (pages == 0)
    {
        pages = &UnidentifiedTLTextDefault::sInstance;
    }
    pages->SetString(mPageText);

    SHNavigation* scene = GetNavigationScene();
    if (scene == 0)
    {
        return;
    }

    if (mPageCount == 1)
    {
        mPageControls->SetButtonState(1, false, false);
        mPageControls->SetButtonState(0, false, false);
        scene->SetButtons(4, true);
    }
    else if (mCurrentPage <= 0)
    {
        mPageControls->SetButtonState(1, true, true);
        mPageControls->SetButtonState(0, false, false);
        mPageControls->ClearButtonHighlight(1);
        scene->SetButtons(5, true);
    }
    else if (mCurrentPage >= mPageCount - 1)
    {
        mPageControls->SetButtonState(1, false, false);
        mPageControls->SetButtonState(0, true, true);
        mPageControls->ClearButtonHighlight(0);
        scene->SetButtons(6, true);
    }
    else
    {
        mPageControls->SetButtonState(1, true, true);
        mPageControls->SetButtonState(0, true, true);
        scene->SetButtons(7, true);
    }
}

void SHOnlineMiiSelect::InitializeButtons()
{
    typedef Detail::MemFunImpl<void, void (SHOnlineMiiSelect::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, SHOnlineMiiSelect*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback over(
        PointerBinding(MemFun(&SHOnlineMiiSelect::OpenItem), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback off(
        PointerBinding(MemFun(&SHOnlineMiiSelect::CloseItem), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback select(
        PointerBinding(MemFun(&SHOnlineMiiSelect::SelectMii), this, Placeholder<0>(), Placeholder<1>()));

    for (int i = 0; i < 10; ++i)
    {
        mMiiButtons[i].SetInstanceBounds(
            mMiiInstances[i], true, 0.0f, 0.0f, 1.0f, 1.0f);
        mMiiButtons[i].SetPointerEnterCallback(over);
        mMiiButtons[i].SetPointerLeaveCallback(off);
        mMiiButtons[i].SetPointerPressCallback(select);
    }
}

void SHOnlineMiiSelect::OpenItem(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    ++mHoverCounts[index];
    if (!mMiiButtons[item].HasOtherPointerState(1, index))
    {
        mMiiInstances[item]->SetActiveSlide("over", true, false);
        mMiiButtons[item].SetPointerState(1, index);
        FEAudio::PlayAnimAudioEvent(0xFFC8A55D, 0, 0, 1);
    }
}

void SHOnlineMiiSelect::CloseItem(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    --mHoverCounts[index];
    mMiiInstances[item]->SetActiveSlide("off", true, false);
    mMiiButtons[item].SetPointerState(0, index);
}

void SHOnlineMiiSelect::SelectMii(int, void* context)
{
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }

    mPageControls->SetButtonState(1, false, false);
    mPageControls->SetButtonState(0, false, false);

    SHOnlineMiiSelectOverlay* scene = (SHOnlineMiiSelectOverlay*)GameSceneManager::Instance()->Push(
        SCENE_ONLINE_MII_SELECT_OVERLAY, SCREEN_FORWARD, false);
    int item = (int)context;
    scene->mOfficialIndex = mOfficialIndices[mCurrentPage * 10 + item];
    scene->mIconIndex = item;

    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    mSuppressPageInput = true;
}

void SHOnlineMiiSelect::ReturnToWiiMenu()
{
    ResetTask::s_ResetMode = 3;
    ResetTask::s_ResetState = ResetTask::s_ResetState == RS_RUNNING
                                ? RS_STARTRESET
                                : ResetTask::s_ResetState;
}
