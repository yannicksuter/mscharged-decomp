#include "Game/SH/SHHallOfFame.h"
#include "Game/SH/SHNavigation.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/FE/feCupFlow.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/FEAudio.h"
#include "Game/SH/SHHallOfFameHistory.h"
#include "Game/SH/SHHallOfFamePlayerCard.h"

#include "Game/GameSceneManager.h"
#include "Game/DB/GameProgress.h"
#include <string.h>
#include "Game/BaseGameSceneManager.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/FE/feCupFlow.h"
#include "Game/FE/feAsyncImage.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/Render/Presentation.h"
#include "NL/MemAlloc.h"
#include "NL/gl/glState.h"
#include "NL/nlAlgorithm.h"
#include "NL/nlBasicString.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalization.h"
#include "NL/nlLocalizationLookup.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/feBackButton.h"
#include "Game/FE/FEAudio.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/SH/SHNavigation.h"

#include <string.h>
#include "NL/nlBasicString.inl"

AsyncImage* sHallOfFameImages[12];
int sHallOfFamePlayerCardIndex;
int sHallOfFameReturnMode;
bool sHallOfFameImagePreloadStarted;
int sHallOfFameImageLoadsPending;

extern const char* sHallOfFameResource;

static const int sFirstControllerModes[3] = { 0, 1, 2 };
static const int sSecondControllerModes[3] = { 14, 15, 16 };

struct HallOfFameImageInfo
{
    const char* textureName;
    const char* unlockedPath;
    unsigned int unlockFlag;
};

static const HallOfFameImageInfo sImageInfo[12] = {
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

typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;


void SetHallOfFameBreadcrumbs(int mode, TLComponentInstance* breadcrumbs)
{
    FEFinder<TLComponentInstance, 4>::FindOrDefault(breadcrumbs->GetActiveSlide(), nlStringLowerHash("breadcrumb_5"), 0, 0, 0, 0, 0)->m_bVisible = false;
    FEFinder<TLComponentInstance, 4>::FindOrDefault(breadcrumbs->GetActiveSlide(), nlStringLowerHash("breadcrumb_6"), 0, 0, 0, 0, 0)->m_bVisible = false;
    FEFinder<TLComponentInstance, 4>::FindOrDefault(breadcrumbs->GetActiveSlide(), nlStringLowerHash("breadcrumb_7"), 0, 0, 0, 0, 0)->m_bVisible = false;

    if (mode < 3)
    {
        for (int i = 0; i < 3; ++i)
        {
            if (mode == sFirstControllerModes[i])
            {
                SetBreadcrumbs(3, i);
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
                SetBreadcrumbs(3, i);
                break;
            }
        }
    }
}

void CycleHallOfFameCup(int mode, bool advance)
{
    bool wideScreen = IsWidescreen();
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
            PushPresentationCamera("hofbronze", 0, 0.0f, true);
        }
        else
        {
            PushPresentationCamera("43hofbronze", 0, 0.0f, true);
        }
        GameSceneManager::Instance()->Push((SceneList)62, SCREEN_NOTHING, true);
        break;
    case 1:
        if (wideScreen)
        {
            PushPresentationCamera("hofgold", 0, 0.0f, true);
        }
        else
        {
            PushPresentationCamera("43hofgold", 0, 0.0f, true);
        }
        GameSceneManager::Instance()->Push((SceneList)63, SCREEN_NOTHING, true);
        break;
    case 2:
        if (wideScreen)
        {
            PushPresentationCamera("hofsilver", 0, 0.0f, true);
        }
        else
        {
            PushPresentationCamera("43hofsilver", 0, 0.0f, true);
        }
        GameSceneManager::Instance()->Push((SceneList)64, SCREEN_NOTHING, true);
        break;
    }
}

void ShowHallOfFameTrophy(int camera)
{
    switch (camera)
    {
    case 4:
        PushPresentationCamera("trophycentreofbronzehof", 0, 0.0f, true);
        GameSceneManager::Instance()->Push((SceneList)66, SCREEN_NOTHING, true);
        break;
    case 6:
        PushPresentationCamera("trophycentreofgoldhof", 0, 0.0f, true);
        GameSceneManager::Instance()->Push((SceneList)69, SCREEN_NOTHING, true);
        break;
    case 5:
        PushPresentationCamera("trophycentreofsilverhof", 0, 0.0f, true);
        GameSceneManager::Instance()->Push((SceneList)72, SCREEN_NOTHING, true);
        break;
    case 8:
        PushPresentationCamera("trophyrightofbronze", 0, 0.0f, true);
        GameSceneManager::Instance()->Push((SceneList)67, SCREEN_NOTHING, true);
        break;
    case 7:
        PushPresentationCamera("trophyleftofbronze", 0, 0.0f, true);
        GameSceneManager::Instance()->Push((SceneList)68, SCREEN_NOTHING, true);
        break;
    case 12:
        PushPresentationCamera("trophyrightofgold", 0, 0.0f, true);
        GameSceneManager::Instance()->Push((SceneList)70, SCREEN_NOTHING, true);
        break;
    case 11:
        PushPresentationCamera("trophyleftofgold", 0, 0.0f, true);
        GameSceneManager::Instance()->Push((SceneList)71, SCREEN_NOTHING, true);
        break;
    case 10:
        PushPresentationCamera("trophyrightofsilver", 0, 0.0f, true);
        GameSceneManager::Instance()->Push((SceneList)73, SCREEN_NOTHING, true);
        break;
    case 9:
        PushPresentationCamera("trophyleftofsilver", 0, 0.0f, true);
        GameSceneManager::Instance()->Push((SceneList)74, SCREEN_NOTHING, true);
        break;
    }

    FEAudio::PlayAnimAudioEvent(0xA28F072D, 0, 0, 1);
}

void ShowHallOfFamePlayerCard(int camera)
{
    sHallOfFamePlayerCardIndex = camera;
    GameSceneManager::Instance()->Push((SceneList)65, SCREEN_NOTHING, true);
}

void CycleHallOfFameDetailPage(int mode, bool advance)
{
    switch (mode)
    {
    case 14:
        if (advance)
        {
            GameSceneManager::Instance()->Push((SceneList)60, SCREEN_NOTHING, true);
        }
        else
        {
            GameSceneManager::Instance()->Push((SceneList)61, SCREEN_NOTHING, true);
        }
        break;
    case 15:
        if (advance)
        {
            GameSceneManager::Instance()->Push((SceneList)61, SCREEN_NOTHING, true);
        }
        else
        {
            GameSceneManager::Instance()->Push((SceneList)59, SCREEN_NOTHING, true);
        }
        break;
    case 16:
        if (advance)
        {
            GameSceneManager::Instance()->Push((SceneList)59, SCREEN_NOTHING, true);
        }
        else
        {
            GameSceneManager::Instance()->Push((SceneList)60, SCREEN_NOTHING, true);
        }
        break;
    default:
        GameSceneManager::Instance()->Push((SceneList)59, SCREEN_NOTHING, true);
        sHallOfFameReturnMode = mode;
        break;
    }
}

void LeaveHallOfFamePage(int mode)
{
    bool wideScreen = IsWidescreen();

    switch (mode)
    {
    case 0:
    case 1:
    case 2:
        FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, 1);
        GameSceneManager::Instance()->Pop();
        SetLockedTrophyVisibility(true);
        Presentation::GetInstance()->Call("TransitionHallOfFameToMainMenu");
        break;
    case 4:
    case 7:
    case 8:
        if (wideScreen)
        {
            PushPresentationCamera("hofbronze", 0, 0.0f, true);
        }
        else
        {
            PushPresentationCamera("43hofbronze", 0, 0.0f, true);
        }
        GameSceneManager::Instance()->Push((SceneList)62, SCREEN_BACK, true);
        break;
    case 6:
    case 11:
    case 12:
        if (wideScreen)
        {
            PushPresentationCamera("hofgold", 0, 0.0f, true);
        }
        else
        {
            PushPresentationCamera("43hofgold", 0, 0.0f, true);
        }
        GameSceneManager::Instance()->Push((SceneList)63, SCREEN_BACK, true);
        break;
    case 5:
    case 9:
    case 10:
        if (wideScreen)
        {
            PushPresentationCamera("hofsilver", 0, 0.0f, true);
        }
        else
        {
            PushPresentationCamera("43hofsilver", 0, 0.0f, true);
        }
        GameSceneManager::Instance()->Push((SceneList)64, SCREEN_BACK, true);
        break;
    case 13:
        if (sHallOfFamePlayerCardIndex < 4)
        {
            if (wideScreen)
            {
                PushPresentationCamera("hofbronze", 0, 0.0f, true);
            }
            else
            {
                PushPresentationCamera("43hofbronze", 0, 0.0f, true);
            }
            GameSceneManager::Instance()->Push((SceneList)62, SCREEN_BACK, true);
        }
        else if (sHallOfFamePlayerCardIndex < 8)
        {
            if (wideScreen)
            {
                PushPresentationCamera("hofsilver", 0, 0.0f, true);
            }
            else
            {
                PushPresentationCamera("43hofsilver", 0, 0.0f, true);
            }
            GameSceneManager::Instance()->Push((SceneList)64, SCREEN_BACK, true);
        }
        else
        {
            if (wideScreen)
            {
                PushPresentationCamera("hofgold", 0, 0.0f, true);
            }
            else
            {
                PushPresentationCamera("43hofgold", 0, 0.0f, true);
            }
            GameSceneManager::Instance()->Push((SceneList)63, SCREEN_BACK, true);
        }
        break;
    case 14:
    case 15:
    case 16:
        if (sHallOfFameReturnMode == 1)
        {
            GameSceneManager::Instance()->Push((SceneList)63, SCREEN_NOTHING, true);
        }
        else if (sHallOfFameReturnMode == 2)
        {
            GameSceneManager::Instance()->Push((SceneList)64, SCREEN_NOTHING, true);
        }
        else
        {
            GameSceneManager::Instance()->Push((SceneList)62, SCREEN_NOTHING, true);
        }
        break;
    }
}

unsigned int GetHallOfFameUnlockFlag(int mode, int item)
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

unsigned int GetHallOfFameTrophyID(int mode, int item)
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

int GetHallOfFamePlayerCardIndex()
{
    return sHallOfFamePlayerCardIndex;
}

unsigned int GetHallOfFamePlayerUnlockFlag(unsigned int camera)
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

void PreloadHallOfFameImages()
{
    if (!sHallOfFameImagePreloadStarted)
    {
        sHallOfFameImagePreloadStarted = true;
        sHallOfFameImageLoadsPending = 0;

        for (int i = 0; i < 12; ++i)
        {
            if (IsUnlockFlagSet(sImageInfo[i].unlockFlag))
            {
                CurrentAllocator = &VirtualAllocator;
                AllocatorStack[AllocatorStackDepth++] = &VirtualAllocator;
                sHallOfFameImages[i] = new (nlMalloc(sizeof(AsyncImage), 8, false))
                    AsyncImage("art/fe/HallOfFameUI.res", 0);
                --AllocatorStackDepth;
                AllocatorStack[AllocatorStackDepth] = 0;
                CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];

                sHallOfFameImages[i]->mTargetTextureHandle = glGetTexture(sImageInfo[i].textureName);
                sHallOfFameImages[i]->QueueLoad(sImageInfo[i].unlockedPath, false);
                ++sHallOfFameImageLoadsPending;
            }
        }
    }
}

void UpdateHallOfFameImagePreload(float fDeltaT)
{
    if (sHallOfFameImageLoadsPending > 0)
    {
        for (int i = 0; i < 12; ++i)
        {
            if (sHallOfFameImages[i] != 0 && sHallOfFameImages[i]->Update(true))
            {
                --sHallOfFameImageLoadsPending;
                delete sHallOfFameImages[i];
                sHallOfFameImages[i] = 0;
            }
        }
    }
}

void ResetHallOfFameImagePreload()
{
    sHallOfFameImagePreloadStarted = false;
    sHallOfFameImageLoadsPending = 0;
}

bool IsHallOfFameImagePreloadStarted()
{
    return sHallOfFameImagePreloadStarted;
}

bool IsHallOfFameImagePreloadPending()
{
    return sHallOfFameImageLoadsPending > 0;
}

SHHallOfFameHistory::SHHallOfFameHistory(int mode)
    : mMode(mode)
{
    mSelectedHistoryIndex = 0;
    mHistoryCount = 0;
    mUnidentified324 = false;
    mState = 0;
    mImages[0] = 0;
    mImages[1] = 0;
    mImages[2] = 0;
    mImages[3] = 0;
    mImages[4] = 0;
    mUnidentified314[0] = 0;
    mUnidentified314[1] = 0;
    mUnidentified314[2] = 0;
    mUnidentified314[3] = 0;

    mImages[0] = new (0x20, true) AsyncImage(sHallOfFameResource, 0);
    mImageReady[0] = false;
    mImages[1] = new (0x20, true) AsyncImage(sHallOfFameResource, 0);
    mImageReady[1] = false;
    mImages[2] = new (0x20, true) AsyncImage(sHallOfFameResource, 0);
    mImageReady[2] = false;
    mImages[3] = new (0x20, true) AsyncImage(sHallOfFameResource, 0);
    mImageReady[3] = false;
    mImages[4] = new (0x20, true) AsyncImage(sHallOfFameResource, 0);
    mImageReady[4] = false;

    memset(mHistory, 0, sizeof(mHistory));

    int modeIndex = mode - 4;
    unsigned char* historyBase = (unsigned char*)g_pCupManager + 0x86A0;
    int historyIndex = 11;
    unsigned char cursor = historyBase[0x370 + modeIndex];
    if (cursor != 0)
    {
        historyIndex = cursor - 1;
    }

    CupHistoryRecord* source
        = (CupHistoryRecord*)(historyBase + modeIndex * 0x60 + 0x10);
    while (mHistoryCount < 12)
    {
        CupHistoryRecord& record = source[historyIndex];
        bool emptyThrough04
            = record.mCaptain == 0 && record.mSidekick1 == 0;
        bool emptyThrough07 = emptyThrough04 && record.mSidekick2 == 0;
        bool emptyThrough0D = emptyThrough07 && record.mDay == 0;
        bool emptyThrough12 = emptyThrough0D && record.mMonth == 0;
        bool emptyThrough16 = emptyThrough12 && record.mYearOffset == 0;
        bool emptyThrough20 = emptyThrough16 && record.mGoals == 0;
        bool emptyThrough2B = emptyThrough20 && record.mUnidentified2B == 0;
        bool emptyThrough32 = emptyThrough2B && record.mUnidentified32 == 0;
        bool empty = emptyThrough32 && record.mUnidentified39 == 0;
        if (empty)
        {
            break;
        }

        mHistory[mHistoryCount] = record;
        ++mHistoryCount;
        historyIndex = historyIndex > 0 ? historyIndex - 1 : 11;
    }

}

SHHallOfFameHistory::~SHHallOfFameHistory()
{
    if (mImages[0] != 0)
        delete mImages[0];
    if (mImages[1] != 0)
        delete mImages[1];
    if (mImages[2] != 0)
        delete mImages[2];
    if (mImages[3] != 0)
        delete mImages[3];
    if (mImages[4] != 0)
        delete mImages[4];
}

char sHallOfFameResourcePath[] = "art/fe/HallOfFameUI.res";
const char* sHallOfFameResource = sHallOfFameResourcePath;

void SHHallOfFameHistory::SceneCreated()
{
    StopHallOfFameTrophyEffects();

    SHNavigation* object = GetNavigationScene();
    TLComponentInstance* screen = 0;
    if (object != 0)
    {
        object->HideButtons();
        screen = object->GetButton(4);
    }
    mNavigation.SetButtonInstance(screen);

    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    TLComponentInstance* scrollbar = FEFinder<TLComponentInstance, 4>::Find(presentation->m_currentSlide,
            nlStringLowerHash("Layer"), nlStringLowerHash("scrollbar"), 0, 0, 0, 0);
    mScrollWidget.SetComponent(scrollbar);
    mScrollWidget.SetRange(mHistoryCount - 1);
    mScrollWidget.SetValue(mSelectedHistoryIndex);

    UpdateTitle();
    switch (mMode)
    {
    case 4:
    case 5:
    case 6:
        UpdateCupRecordText();
        break;
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
        UpdateGoalsRecordText();
        break;
    }
    UpdateDateText();
    UpdateTeamDisplay();
}

void SHHallOfFameHistory::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    int state = mState;
    if (state == 0 || (unsigned int)(state - 2) <= 1)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->m_start + slide->m_duration)
        {
            for (int pad = 0; pad < 4; ++pad)
            {
                GetPointerInstance(pad)->SetActiveSlide("waiting", true, false);
            }
            return;
        }

        if (state == 0)
        {
            GetNavigationScene()->SetButtons(4, true);
            mState = 1;
            if (!mScrollWidget.mInitialized)
            {
                TLComponentInstance* scrollbar = FEFinder<TLComponentInstance, 4>::Find(
                        mPresentation->m_currentSlide, nlStringLowerHash("Layer"),
                        nlStringLowerHash("scrollbar"), 0, 0, 0, 0);
                mScrollWidget.SetComponent(scrollbar);
                mScrollWidget.Initialize();
            }
        }
        else if (state == 2)
        {
            return;
        }
        else if (state == 3)
        {
            LeaveHallOfFamePage(mMode);
            return;
        }
    }

    if (mImages[0]->Update(true))
        mImageReady[0] = true;
    if (mImages[1]->Update(true))
        mImageReady[1] = true;
    if (mImages[2]->Update(true))
        mImageReady[2] = true;
    if (mImages[3]->Update(true))
        mImageReady[3] = true;
    if (mImages[4]->Update(true))
        mImageReady[4] = true;

    if (!mImageReady[0] || !mImageReady[1] || !mImageReady[2]
        || !mImageReady[3] || !mImageReady[4])
    {
        return;
    }

    mImages[0]->mImageInstance->m_bVisible = true;
    mImages[4]->mImageInstance->m_bVisible = true;
    mImages[1]->mImageInstance->m_bVisible = true;
    mImages[2]->mImageInstance->m_bVisible = true;
    mImages[3]->mImageInstance->m_bVisible = true;

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
            else if (mUnidentified314[pad] > 0)
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
            mScrollWidget.Update(event, fDeltaT);

            if (mNavigation.UpdateBackButton(event, fDeltaT))
            {
                FEAudio::PlayAnimAudioEvent(0x80BA0C86, 0, 0, 1);
                mState = 3;
                SHNavigation* object = GetNavigationScene();
                if (object != 0)
                {
                    object->HideButtons();
                }
                mPresentation->SetActiveSlide("OUT", true);
                return;
            }
        }
    }

    if (mScrollWidget.IsScrolling(1, 1))
    {
        ++mSelectedHistoryIndex;
        switch (mMode)
        {
        case 4:
        case 5:
        case 6:
            UpdateCupRecordText();
            break;
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
            UpdateGoalsRecordText();
            break;
        }
        UpdateDateText();
        UpdateTeamDisplay();
    }
    else if (mScrollWidget.IsScrolling(0, 1))
    {
        --mSelectedHistoryIndex;
        switch (mMode)
        {
        case 4:
        case 5:
        case 6:
            UpdateCupRecordText();
            break;
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
            UpdateGoalsRecordText();
            break;
        }
        UpdateDateText();
        UpdateTeamDisplay();
    }
}

void SHHallOfFameHistory::UpdateTitle()
{
    WideBasicString title;

    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    unsigned long titlesHash = nlStringLowerHash("TITLES");
    unsigned long titleHash = nlStringLowerHash("TITLE");
    unsigned long historyHash = nlStringLowerHash("HISTORY");
    TLTextInstance* titleText = FEFinder<TLTextInstance, 3>::Find(presentation->m_currentSlide,
            nlStringLowerHash("Layer"), historyHash, titleHash, titlesHash, 0, 0);
    if (titleText == 0)
    {
        titleText = &UnidentifiedTLTextDefault::sInstance;
    }

    switch (mMode)
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

    memcpy(mTitleText, title.c_str(), sizeof(mTitleText));
    titleText->SetString(mTitleText);
}

void SHHallOfFameHistory::UpdateCupRecordText()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    unsigned long recordHash = nlStringLowerHash("RECORD");
    unsigned long historyHash = nlStringLowerHash("HISTORY");
    TLTextInstance* recordText = FEFinder<TLTextInstance, 3>::Find(presentation->m_currentSlide,
            nlStringLowerHash("Layer"), historyHash, recordHash, 0, 0, 0);
    if (recordText == 0)
    {
        recordText = &UnidentifiedTLTextDefault::sInstance;
    }

    char monthString[4];
    char dayString[4];
    char yearString[4];
    unsigned short monthWideString[4];
    unsigned short dayWideString[4];
    unsigned short yearWideString[4];
    WideBasicString unformatted;
    WideBasicString formatted;

    CupHistoryRecord& record = mHistory[mSelectedHistoryIndex];
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

    memcpy(mRecordText, formatted.c_str(), sizeof(mRecordText));
    recordText->SetString(mRecordText);
}

void SHHallOfFameHistory::UpdateGoalsRecordText()
{
    char goalsString[4];
    unsigned short goalsWideString[4];
    WideBasicString unformatted;
    WideBasicString formatted;

    CupHistoryRecord& record = mHistory[mSelectedHistoryIndex];
    int goals = record.mGoals;
    nlSNPrintf(goalsString, 4, "%d", goals);
    nlStrToWcs(goalsString, goalsWideString, 4);

    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    TLTextInstance* goalsText = 0;
    switch (mMode)
    {
    case 7:
    case 9:
    case 11:
    {
        unformatted = WideBasicString(LookupLocString("HOF_GOALS_AGAINST"));

        unsigned long goalsForHash = nlStringLowerHash("GOALS FOR");
        unsigned long historyHash = nlStringLowerHash("HISTORY");
        goalsText = FEFinder<TLTextInstance, 3>::Find(presentation->m_currentSlide,
                nlStringLowerHash("Layer"), historyHash, goalsForHash, 0, 0, 0);
        if (goalsText == 0)
        {
            goalsText = &UnidentifiedTLTextDefault::sInstance;
        }
        goalsText->m_bVisible = false;

        unsigned long goalsAgainstHash = nlStringLowerHash("GOALS AGAINST");
        historyHash = nlStringLowerHash("HISTORY");
        goalsText = FEFinder<TLTextInstance, 3>::Find(presentation->m_currentSlide,
                nlStringLowerHash("Layer"), historyHash, goalsAgainstHash, 0, 0, 0);
        if (goalsText == 0)
        {
            goalsText = &UnidentifiedTLTextDefault::sInstance;
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
        goalsText = FEFinder<TLTextInstance, 3>::Find(presentation->m_currentSlide,
                nlStringLowerHash("Layer"), historyHash, goalsAgainstHash, 0, 0, 0);
        if (goalsText == 0)
        {
            goalsText = &UnidentifiedTLTextDefault::sInstance;
        }
        goalsText->m_bVisible = false;

        unsigned long goalsForHash = nlStringLowerHash("GOALS FOR");
        historyHash = nlStringLowerHash("HISTORY");
        goalsText = FEFinder<TLTextInstance, 3>::Find(presentation->m_currentSlide,
                nlStringLowerHash("Layer"), historyHash, goalsForHash, 0, 0, 0);
        if (goalsText == 0)
        {
            goalsText = &UnidentifiedTLTextDefault::sInstance;
        }
        break;
    }
    }

    formatted = Format(unformatted, goalsWideString);
    memcpy(mRecordText, formatted.c_str(), sizeof(mRecordText));
    goalsText->SetString(mRecordText);
}

void SHHallOfFameHistory::UpdateDateText()
{
    char yearString[5];
    char monthString[5];
    char dayString[5];
    unsigned short yearWideString[5];
    unsigned short monthWideString[5];
    unsigned short dayWideString[5];

    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    unsigned long dateHash = nlStringLowerHash("DATE");
    unsigned long historyHash = nlStringLowerHash("HISTORY");
    TLTextInstance* dateText = FEFinder<TLTextInstance, 3>::Find(presentation->m_currentSlide,
            nlStringLowerHash("Layer"), historyHash, dateHash, 0, 0, 0);
    if (dateText == 0)
    {
        dateText = &UnidentifiedTLTextDefault::sInstance;
    }

    WideBasicString unformatted;
    WideBasicString formatted;

    CupHistoryRecord& record = mHistory[mSelectedHistoryIndex];
    int month = record.mMonth + 1;
    int day = record.mDay + 1;
    int year = record.mYearOffset + 2000;

    nlSNPrintf(yearString, 5, "%.4d", year);
    nlStrToWcs(yearString, yearWideString, 5);
    nlSNPrintf(monthString, 5, "%.2d", month);
    nlStrToWcs(monthString, monthWideString, 5);
    nlSNPrintf(dayString, 5, "%.2d", day);
    nlStrToWcs(dayString, dayWideString, 5);

    unformatted = WideBasicString(LookupLocString("DATE_STAMP"));
    formatted
        = Format(unformatted, dayWideString, monthWideString, yearWideString);
    memcpy(mDateText, formatted.c_str(), sizeof(mDateText));
    dateText->SetString(mDateText);
}

void SHHallOfFameHistory::UpdateTeamDisplay()
{
    int value0 = mHistory[mSelectedHistoryIndex].mCaptain;
    int value1 = mHistory[mSelectedHistoryIndex].mSidekick1;
    int value2 = mHistory[mSelectedHistoryIndex].mSidekick2;
    int value3 = mHistory[mSelectedHistoryIndex].mSidekick3;
    WideBasicString string;
    switch (value0)
    {
    case 0:
        string = g_pLocalization->GetString("NAME_MARIO");
        break;
    case 1:
        string = g_pLocalization->GetString("NAME_BOWSER");
        break;
    case 2:
        string = g_pLocalization->GetString("NAME_DAISY");
        break;
    case 3:
        string = g_pLocalization->GetString("NAME_DK");
        break;
    case 4:
        string = g_pLocalization->GetString("NAME_LUIGI");
        break;
    case 5:
        string = g_pLocalization->GetString("NAME_PEACH");
        break;
    case 6:
        string = g_pLocalization->GetString("NAME_WALUIGI");
        break;
    case 7:
        string = g_pLocalization->GetString("NAME_WARIO");
        break;
    case 8:
        string = g_pLocalization->GetString("NAME_YOSHI");
        break;
    case 9:
        string = g_pLocalization->GetString("NAME_BOWSERJR");
        break;
    case 10:
        string = g_pLocalization->GetString("NAME_DIDDYKONG");
        break;
    case 11:
        string = g_pLocalization->GetString("NAME_PETEY");
        break;
    }

    FEPresentation* presentation = mFEScene->GetPackage()->GetPresentation();
    TLTextInstance* text = FEFinder<TLTextInstance, 3>::FindOrDefault(presentation->GetActiveSlide(),
        "Layer", "HISTORY", "NAMES", "NAMES");
    memcpy(mTeamNameText, string.c_str(), sizeof(mTeamNameText));
    text->SetString(mTeamNameText);

    const CharacterInfo& captain = GetCharacterInfo(GetCharacterIndexFromCaptain(value0));
    TLImageInstance* image;
    char path[0x80];

    image = FEFinder<TLImageInstance, 2>::FindOrDefault<TLSlide>(presentation->GetActiveSlide(), "Layer", "HISTORY", "CAPTAIN");
    mImages[0]->SetImageInstance(image);
    image->SetVisible(false);
    nlSNPrintf(path, sizeof(path), "fe/hof_history_images/hof_history_%s", captain.GetName());
    mImages[0]->QueueLoad(path, false);

    image = FEFinder<TLImageInstance, 2>::FindOrDefault<TLSlide>(presentation->GetActiveSlide(), "Layer", "HISTORY", "KRITTER");
    mImages[4]->SetImageInstance(image);
    image->SetVisible(false);
    nlSNPrintf(path, sizeof(path), "fe/hof_history_images/hof_history_kritter_%s", captain.GetName());
    mImages[4]->QueueLoad(path, false);

    const CharacterInfo& sidekick1 = GetCharacterInfo(GetCharacterIndexFromSidekick(value1));
    image = FEFinder<TLImageInstance, 2>::FindOrDefault<TLSlide>(presentation->GetActiveSlide(), "Layer", "HISTORY", "SK1");
    mImages[1]->SetImageInstance(image);
    image->SetVisible(false);
    nlSNPrintf(path, sizeof(path), "fe/hof_history_images/hof_history_sidekicks_%s_%s",
        sidekick1.GetName(), captain.GetName());
    mImages[1]->QueueLoad(path, false);

    const CharacterInfo& sidekick2 = GetCharacterInfo(GetCharacterIndexFromSidekick(value2));
    image = FEFinder<TLImageInstance, 2>::FindOrDefault<TLSlide>(presentation->GetActiveSlide(), "Layer", "HISTORY", "SK2");
    mImages[2]->SetImageInstance(image);
    image->SetVisible(false);
    nlSNPrintf(path, sizeof(path), "fe/hof_history_images/hof_history_sidekicks_%s_%s",
        sidekick2.GetName(), captain.GetName());
    mImages[2]->QueueLoad(path, false);

    const CharacterInfo& sidekick3 = GetCharacterInfo(GetCharacterIndexFromSidekick(value3));
    image = FEFinder<TLImageInstance, 2>::FindOrDefault<TLSlide>(presentation->GetActiveSlide(), "Layer", "HISTORY", "SK3");
    mImages[3]->SetImageInstance(image);
    image->SetVisible(false);
    nlSNPrintf(path, sizeof(path), "fe/hof_history_images/hof_history_sidekicks_%s_%s",
        sidekick3.GetName(), captain.GetName());
    mImages[3]->QueueLoad(path, false);

    mImageReady[0] = false;
    mImageReady[1] = false;
    mImageReady[2] = false;
    mImageReady[3] = false;
    mImageReady[4] = false;
}

SHHallOfFamePlayerCard::SHHallOfFamePlayerCard(int mode)
    : mMode(mode)
    , mNavigation()
    , mSlideFinished(false)
    , mFrontImage(sHallOfFameResource, 0)
    , mBackImage(sHallOfFameResource, 0)
{
    mFrontImageReady = false;
    mBackImageReady = false;
    mUnidentified0FC[0] = 0;
    mUnidentified0FC[1] = 0;
    mUnidentified0FC[2] = 0;
    mUnidentified0FC[3] = 0;

    mCardIndex = GetHallOfFamePlayerCardIndex();
    mIsUnlocked = IsUnlockFlagSet(GetHallOfFamePlayerUnlockFlag(mCardIndex));
}

SHHallOfFamePlayerCard::~SHHallOfFamePlayerCard()
{
}

void SHHallOfFamePlayerCard::SceneCreated()
{
    StopHallOfFameTrophyEffects();

    SHNavigation* object = GetNavigationScene();
    TLComponentInstance* screen = 0;
    if (object != 0)
    {
        object->SetButtons(4, true);
        screen = object->GetButton(4);
    }
    mNavigation.SetButtonInstance(screen);

    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    TLComponentInstance* playerCard = FEFinder<TLComponentInstance, 4>::Find(presentation->m_currentSlide,
            nlStringLowerHash("Layer"), nlStringLowerHash("player card"), 0, 0, 0, 0);
    if (playerCard == 0)
    {
        playerCard = &UnidentifiedTLComponentDefault::sInstance;
    }

    if (mIsUnlocked)
    {
        playerCard->SetActiveSlide("HAVE", true, false);
    }
    else
    {
        playerCard->SetActiveSlide("NOT HAVE", true, false);
    }

    UpdateText();
    UpdateImages();
}

void SHHallOfFamePlayerCard::Update(float fDeltaT)
{
    if (!mFrontImageReady || !mBackImageReady)
    {
        if (!mFrontImageReady)
        {
            mFrontImageReady = mFrontImage.Update(true);
        }
        if (!mBackImageReady)
        {
            mBackImageReady = mBackImage.Update(true);
        }
        if (mFrontImageReady && mBackImageReady)
        {
            FEAudio::PlayAnimAudioEvent(0x12057B21, 0, 0, 1);
        }
        return;
    }

    BaseSceneHandler::Update(fDeltaT);

    if (!mSlideFinished)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->m_time < slide->m_duration + slide->m_start)
        {
            return;
        }
        mSlideFinished = true;
    }

    for (unsigned int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* controller = GetPointerInstance(pad);
        bool processInput;
        if (g_pFEInput->m_InputLockDepth == 0)
        {
            if (pad != gFEControllerIndex)
            {
                controller->SetActiveSlide("waiting", true, false);
                processInput = false;
            }
            else
            {
                if (mUnidentified0FC[pad] > 0)
                {
                    controller->SetActiveSlide("A", true, false);
                }
                else
                {
                    controller->SetActiveSlide("cursor", true, false);
                }
                processInput = true;
            }
        }
        else
        {
            processInput = true;
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

            if (mNavigation.UpdateBackButton(event, fDeltaT))
            {
                LeaveHallOfFamePage(mMode);
                return;
            }
        }
    }
}

void SHHallOfFamePlayerCard::UpdateText()
{
    typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;

    WideBasicString title;
    WideBasicString description;
    WideBasicString name;

    FEPresentation* presentation = mFEScene->GetPackage()->GetPresentation();
    TLTextInstance* titleText = FEFinder<TLTextInstance, 3>::Find(
        presentation->GetActiveSlide(), InlineHasher("Layer"), InlineHasher("player card"),
        InlineHasher("title"));
    TLTextInstance* nameText = FEFinder<TLTextInstance, 3>::Find(
        presentation->GetActiveSlide(), InlineHasher("Layer"), InlineHasher("player card"),
        InlineHasher("name"));
    TLTextInstance* descriptionText = FEFinder<TLTextInstance, 3>::Find(
        presentation->GetActiveSlide(), InlineHasher("Layer"), InlineHasher("player card"),
        InlineHasher("description"));

    switch (mCardIndex)
    {
    case 0:
        title = g_pLocalization->GetString("PLAYER_CARDS_MARIO_TITLE");
        name = g_pLocalization->GetString("NAME_MARIO");
        if (mIsUnlocked)
            description = g_pLocalization->GetString("PLAYER_CARDS_MARIO_DESCRIPTION");
        else
            description = g_pLocalization->GetString("PLAYER_CARDS_MARIO_LOCKED");
        break;
    case 1:
        title = g_pLocalization->GetString("PLAYER_CARDS_LUIGI_TITLE");
        name = g_pLocalization->GetString("NAME_LUIGI");
        if (mIsUnlocked)
            description = g_pLocalization->GetString("PLAYER_CARDS_LUIGI_DESCRIPTION");
        else
            description = g_pLocalization->GetString("PLAYER_CARDS_LUIGI_LOCKED");
        break;
    case 2:
        title = g_pLocalization->GetString("PLAYER_CARDS_DONKEYKONG_TITLE");
        name = g_pLocalization->GetString("NAME_DK");
        if (mIsUnlocked)
            description
                = g_pLocalization->GetString("PLAYER_CARDS_DONKEYKONG_DESCRIPTION");
        else
            description = g_pLocalization->GetString("PLAYER_CARDS_DONKEYKONG_LOCKED");
        break;
    case 3:
        title = g_pLocalization->GetString("PLAYER_CARDS_PEACH_TITLE");
        name = g_pLocalization->GetString("NAME_PEACH");
        if (mIsUnlocked)
            description = g_pLocalization->GetString("PLAYER_CARDS_PEACH_DESCRIPTION");
        else
            description = g_pLocalization->GetString("PLAYER_CARDS_PEACH_LOCKED");
        break;
    case 4:
        title = g_pLocalization->GetString("PLAYER_CARDS_DAISY_TITLE");
        name = g_pLocalization->GetString("NAME_DAISY");
        if (mIsUnlocked)
            description = g_pLocalization->GetString("PLAYER_CARDS_DAISY_DESCRIPTION");
        else
            description = g_pLocalization->GetString("PLAYER_CARDS_DAISY_LOCKED");
        break;
    case 5:
        title = g_pLocalization->GetString("PLAYER_CARDS_WARIO_TITLE");
        name = g_pLocalization->GetString("NAME_WARIO");
        if (mIsUnlocked)
            description = g_pLocalization->GetString("PLAYER_CARDS_WARIO_DESCRIPTION");
        else
            description = g_pLocalization->GetString("PLAYER_CARDS_WARIO_LOCKED");
        break;
    case 6:
        title = g_pLocalization->GetString("PLAYER_CARDS_WALUIGI_TITLE");
        name = g_pLocalization->GetString("NAME_WALUIGI");
        if (mIsUnlocked)
            description = g_pLocalization->GetString("PLAYER_CARDS_WALUIGI_DESCRIPTION");
        else
            description = g_pLocalization->GetString("PLAYER_CARDS_WALUIGI_LOCKED");
        break;
    case 7:
        title = g_pLocalization->GetString("PLAYER_CARDS_YOSHI_TITLE");
        name = g_pLocalization->GetString("NAME_YOSHI");
        if (mIsUnlocked)
            description = g_pLocalization->GetString("PLAYER_CARDS_YOSHI_DESCRIPTION");
        else
            description = g_pLocalization->GetString("PLAYER_CARDS_YOSHI_LOCKED");
        break;
    case 8:
        title = g_pLocalization->GetString("PLAYER_CARDS_BOWSER_TITLE");
        name = g_pLocalization->GetString("NAME_BOWSER");
        if (mIsUnlocked)
            description = g_pLocalization->GetString("PLAYER_CARDS_BOWSER_DESCRIPTION");
        else
            description = g_pLocalization->GetString("PLAYER_CARDS_BOWSER_LOCKED");
        break;
    case 9:
        title = g_pLocalization->GetString("PLAYER_CARDS_PETEY_TITLE");
        name = g_pLocalization->GetString("NAME_PETEY");
        if (mIsUnlocked)
            description = g_pLocalization->GetString("PLAYER_CARDS_PETEY_DESCRIPTION");
        else
            description = g_pLocalization->GetString("PLAYER_CARDS_PETEY_LOCKED");
        break;
    case 10:
        title = g_pLocalization->GetString("PLAYER_CARDS_BOWSERJR_TITLE");
        name = g_pLocalization->GetString("NAME_BOWSERJR");
        if (mIsUnlocked)
            description = g_pLocalization->GetString("PLAYER_CARDS_BOWSERJR_DESCRIPTION");
        else
            description = g_pLocalization->GetString("PLAYER_CARDS_BOWSERJR_LOCKED");
        break;
    case 11:
        title = g_pLocalization->GetString("PLAYER_CARDS_DIDDYKONG_TITLE");
        name = g_pLocalization->GetString("NAME_DIDDYKONG");
        if (mIsUnlocked)
            description
                = g_pLocalization->GetString("PLAYER_CARDS_DIDDYKONG_DESCRIPTION");
        else
            description = g_pLocalization->GetString("PLAYER_CARDS_DIDDYKONG_LOCKED");
        break;
    }

    memcpy(mTitleText, title.c_str(),
        sizeof(mTitleText));
    titleText->SetString(mTitleText);
    memcpy(mNameText, name.c_str(), sizeof(mNameText));
    nameText->SetString(mNameText);
    memcpy(mDescriptionText, description.c_str(),
        sizeof(mDescriptionText));
    descriptionText->SetString(mDescriptionText);
}

void SHHallOfFamePlayerCard::UpdateImages()
{
    FEPresentation* presentation;
    TLImageInstance* image0;
    TLImageInstance* image1;
    const char* name1;
    const char* name0;
    char path0[64];
    char path1[64];

    presentation = mFEScene->m_pFEPackage->GetPresentation();
    {
        unsigned long playerCardHash;
        unsigned long imageHash;
        imageHash = nlStringLowerHash("00_dummy_texture");
        playerCardHash = nlStringLowerHash("player card");
        image0 = FEFinder<TLImageInstance, 2>::Find(presentation->m_currentSlide,
                nlStringLowerHash("Layer"), playerCardHash, imageHash, 0, 0, 0);
    }
    if (image0 == 0)
    {
        image0 = &UnidentifiedTLImageDefault::sInstance;
    }

    {
        unsigned long imageHash = nlStringLowerHash("00_dummy_texture_positions");
        unsigned long playerCardHash = nlStringLowerHash("player card");
        image1 = FEFinder<TLImageInstance, 2>::Find(presentation->m_currentSlide,
                nlStringLowerHash("Layer"), playerCardHash, imageHash, 0, 0, 0);
    }
    if (image1 == 0)
    {
        image1 = &UnidentifiedTLImageDefault::sInstance;
    }

    name0 = 0;
    name1 = 0;
    switch (mCardIndex)
    {
    case 0:
        name0 = "PLAYER_CARDS_MARIO";
        name1 = "PLAYER_CARDS_BACK_MARIO";
        break;
    case 1:
        name0 = "PLAYER_CARDS_LUIGI";
        name1 = "PLAYER_CARDS_BACK_LUIGI";
        break;
    case 2:
        name0 = "PLAYER_CARDS_DONKEYKONG";
        name1 = "PLAYER_CARDS_BACK_DONKEYKONG";
        break;
    case 3:
        name0 = "PLAYER_CARDS_PEACH";
        name1 = "PLAYER_CARDS_BACK_PEACH";
        break;
    case 4:
        name0 = "PLAYER_CARDS_DAISY";
        name1 = "PLAYER_CARDS_BACK_DAISY";
        break;
    case 5:
        name0 = "PLAYER_CARDS_WARIO";
        name1 = "PLAYER_CARDS_BACK_WARIO";
        break;
    case 6:
        name0 = "PLAYER_CARDS_WALUIGI";
        name1 = "PLAYER_CARDS_BACK_WALUIGI";
        break;
    case 7:
        name0 = "PLAYER_CARDS_YOSHI";
        name1 = "PLAYER_CARDS_BACK_YOSHI";
        break;
    case 8:
        name0 = "PLAYER_CARDS_BOWSER";
        name1 = "PLAYER_CARDS_BACK_BOWSER";
        break;
    case 9:
        name0 = "PLAYER_CARDS_PETEY";
        name1 = "PLAYER_CARDS_BACK_PETEY";
        break;
    case 10:
        name0 = "PLAYER_CARDS_BOWSERJR";
        name1 = "PLAYER_CARDS_BACK_BOWSERJR";
        break;
    case 11:
        name0 = "PLAYER_CARDS_DIDDYKONG";
        name1 = "PLAYER_CARDS_BACK_DIDDYKONG";
        break;
    }

    nlSNPrintf(path0, sizeof(path0), "fe/screens/images/%s", name0);
    nlSNPrintf(path1, sizeof(path1), "fe/screens/images/%s", name1);

    mFrontImage.mImageInstance = image0;
    mBackImage.mImageInstance = image1;
    mFrontImage.QueueLoad(path0, false);
    mBackImage.QueueLoad(path1, false);
}

#include "Game/FE/feFinder_impl.h"
