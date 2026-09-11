#include <RVLFaceLib/RFL_Database.h>

#include "Game/SH/SHMainMenu.h"

#include "Game/GameInfo.h"
#include "Game/GameSceneManager.h"
#include "Game/DB/SaveLoad.h"
#include "Game/DB/GameProgress.h"
#include "Game/HBMManager.h"
#include "Game/NetworkSession.h"
#include "Game/TweakRegistry.h"
#include "Game/Task/ResetTask.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/Render/Presentation.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"
#include "NL/nlLocalization.h"
#include "NL/plat/nlFlash.h"
#include "Game/FE/feCupFlow.h"
#include "Game/SH/SHHallOfFame.h"
#include "Game/SH/SHNavigation.h"
#include "Game/MiiManager.h"

/**
 * Offset/Address/Size: 0x0 | 0x8021F0F4 | size: 0x100
 */
SHMainMenu::SHMainMenu()
    : mMenuInitialized(false)
    , mItemSelected(false)
    , mWaitingForLoad(false)
{
    gpHBMManager->mBlocked = false;

    for (unsigned int i = 0; i < NUM_ITEMS; ++i)
    {
        mMenuItems[i].mContext = (void*)i;
    }

    for (int i = 0; i < 4; ++i)
    {
        mHighlightedItemCounts[i] = 0;
    }

    mBackButton.SetBackScene(0);
    GameInfoManager::Instance()->mIsOnlineMode = false;
}

/**
 * Offset/Address/Size: 0x100 | 0x8021F1F4 | size: 0x74
 */
SHMainMenu::~SHMainMenu()
{
}

/**
 * Offset/Address/Size: 0x5E0 | 0x8021F6D4 | size: 0x151C
 */
void SHMainMenu::SceneCreated()
{
    GetPresentation()->SetActiveSlide("MAIN", true);
    TLComponentInstance* hiddenItem;
    if (g_pLocalization->GetCurrentLanguage() == nlLocalization::LangJapanese)
    {
        hiddenItem = FEFinder<TLComponentInstance, 4>::Find(GetPresentation()->GetActiveSlide(), "Layer", "101_Menu_Item");
        mMenuItemInstances[4] = FEFinder<TLComponentInstance, 4>::Find(GetPresentation()->GetActiveSlide(), "Layer", "101_Menu_Item_jp");
    }
    else
    {
        hiddenItem = FEFinder<TLComponentInstance, 4>::Find(GetPresentation()->GetActiveSlide(), "Layer", "101_Menu_Item_jp");
        mMenuItemInstances[4] = FEFinder<TLComponentInstance, 4>::Find(GetPresentation()->GetActiveSlide(), "Layer", "101_Menu_Item");
    }

    hiddenItem->SetVisible(false);

    mMenuItemInstances[0] = FEFinder<TLComponentInstance, 4>::FindOrDefault(GetPresentation()->GetActiveSlide(), "Layer", "Domination_Menu_Item");
    mMenuItemInstances[1] = FEFinder<TLComponentInstance, 4>::FindOrDefault(GetPresentation()->GetActiveSlide(), "Layer", "Online _Menu_Item");
    mMenuItemInstances[2] = FEFinder<TLComponentInstance, 4>::FindOrDefault(GetPresentation()->GetActiveSlide(), "Layer", "RTSC_Menu_Item");
    mMenuItemInstances[3] = FEFinder<TLComponentInstance, 4>::FindOrDefault(GetPresentation()->GetActiveSlide(), "Layer", "Challenges_Menu_Item");
    mMenuItemInstances[6] = FEFinder<TLComponentInstance, 4>::FindOrDefault(GetPresentation()->GetActiveSlide(), "Layer", "Options_Menu_Item");
    mMenuItemInstances[5] = FEFinder<TLComponentInstance, 4>::FindOrDefault(GetPresentation()->GetActiveSlide(), "Layer", "HOF_Menu_Item");

    mMenuHighlights[0] = FEFinder<TLComponentInstance, 4>::FindOrDefault(mMenuItemInstances[0]->GetActiveSlide(), "GRUDGE");
    mMenuHighlights[1] = FEFinder<TLComponentInstance, 4>::FindOrDefault(mMenuItemInstances[1]->GetActiveSlide(), "online");
    mMenuHighlights[2] = FEFinder<TLComponentInstance, 4>::FindOrDefault(mMenuItemInstances[2]->GetActiveSlide(), "quest");
    mMenuHighlights[3] = FEFinder<TLComponentInstance, 4>::FindOrDefault(mMenuItemInstances[3]->GetActiveSlide(), "scenario");
    mMenuHighlights[6] = FEFinder<TLComponentInstance, 4>::FindOrDefault(mMenuItemInstances[6]->GetActiveSlide(), "HOF");
    mMenuHighlights[4] = FEFinder<TLComponentInstance, 4>::FindOrDefault(mMenuItemInstances[4]->GetActiveSlide(), "HOF");
    mMenuHighlights[5] = FEFinder<TLComponentInstance, 4>::FindOrDefault(mMenuItemInstances[5]->GetActiveSlide(), "HOF");

    mMenuGroups[0] = FEFinder<TLInstance, 5>::Find(mMenuHighlights[0], "off", "group");
    mMenuGroups[1] = FEFinder<TLInstance, 5>::Find(mMenuHighlights[1], "off", "group");
    mMenuGroups[2] = FEFinder<TLInstance, 5>::Find(mMenuHighlights[2], "off", "group");
    mMenuGroups[3] = FEFinder<TLInstance, 5>::Find(mMenuHighlights[3], "off", "group");
    mMenuGroups[6] = FEFinder<TLInstance, 5>::Find(mMenuHighlights[6], "off", "group");
    mMenuGroups[4] = FEFinder<TLInstance, 5>::Find(mMenuHighlights[4], "off", "group");
    mMenuGroups[5] = FEFinder<TLInstance, 5>::Find(mMenuHighlights[5], "off", "group");

    mMenuHitImages[0] = FEFinder<TLImageInstance, 2>::FindOrDefault(mMenuGroups[0], "btn_main_domination");
    mMenuHitImages[1] = FEFinder<TLImageInstance, 2>::FindOrDefault(mMenuGroups[1], "btn_main_online");
    mMenuHitImages[2] = FEFinder<TLImageInstance, 2>::FindOrDefault(mMenuGroups[2], "btn_main_RTSC");
    mMenuHitImages[3] = FEFinder<TLImageInstance, 2>::FindOrDefault(mMenuGroups[3], "btn_main_challenges");
    mMenuHitImages[6] = FEFinder<TLImageInstance, 2>::FindOrDefault(mMenuGroups[6], "btn_main_OPTIONS");
    mMenuHitImages[4] = FEFinder<TLImageInstance, 2>::FindOrDefault(mMenuGroups[4], "btn_main_101");
    mMenuHitImages[5] = FEFinder<TLImageInstance, 2>::FindOrDefault(mMenuGroups[5], "btn_main_HOF");

    mItemDescription = FEFinder<TLTextInstance, 3>::FindOrDefault(GetPresentation()->GetActiveSlide(), "Layer", "Text");
    SHNavigation* scene = GetNavigationScene();
    TLComponentInstance* backButton = 0;
    if (scene != 0)
    {
        scene->SetButtons(4, true);
        backButton = scene->GetButton(4);
    }

    for (int i = 0; i < NUM_ITEMS; ++i)
    {
        mMenuArrows[i] = FEFinder<TLComponentInstance, 4>::FindOrDefault(mMenuItemInstances[i]->GetActiveSlide(), "arrows");
    }
    if (g_pNetworkSessionBase->GetSessionMode() != 0)
    {
        g_pNetworkSessionBase->Shutdown();
    }
    mBackButton.SetButtonInstance(backButton);

    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }
    FEMusic::StartStreamIfDifferent(1);
    if (SaveEnabled && !NormalSaveLoaded && !nlFlashCallbackPending())
    {
        SaveLoad::StartLoad(false);
    }
}

/**
 * Offset/Address/Size: 0x1AFC | 0x80220BF0 | size: 0x31C
 */
void SHMainMenu::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    if (SaveEnabled && !NormalSaveLoaded)
    {
        return;
    }

    if (mWaitingForLoad)
    {
        if (!SaveEnabled)
        {
            mWaitingForLoad = false;
        }
        else
        {
            if (SaveLoad::CardBusy())
            {
                return;
            }
            ApplyItem(1);
        }
    }

    if (!IsHallOfFameImagePreloadStarted())
    {
        PreloadHallOfFameImages();
    }
    if (IsHallOfFameImagePreloadPending())
    {
        UpdateHallOfFameImagePreload(fDeltaT);
        return;
    }

    if (!mMenuInitialized)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->GetStartTime() + slide->GetDuration())
        {
            return;
        }
        InitializeMenuItems();
        mMenuInitialized = true;
        for (int i = 0; i < 4; ++i)
        {
            GetPointerInstance(i)->SetActiveSlide("cursor", true, false);
        }
    }

    if (gMainMenuInputResetPending)
    {
        gMainMenuInputResetPending = false;
        for (int i = 0; i < NUM_ITEMS; ++i)
        {
            mMenuItems[i].Enable();
        }
        mItemSelected = false;
        for (int i = 0; i < 4; ++i)
        {
            GetPointerInstance(i)->SetActiveSlide("cursor", true, false);
        }
    }

    for (int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* cursor = GetPointerInstance(pad);
        if (pad != gFEControllerIndex)
        {
            cursor->SetActiveSlide("waiting", true, false);
        }
        else
        {
            u8 valid = true;
            FEPointerEvent event;
            event.mIndex = pad;
            event.mPosition = GetPointerPosition(pad, &valid);
            event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
            if (mBackButton.UpdateBackButton(event, fDeltaT))
            {
                FEAudio::PlayAnimAudioEvent(0x29267FA1, 0, 0, true);
                FEAudio::PlayAnimAudioEvent(0xD1A8E50D, 0, 0, true);
                Presentation::GetInstance()->Call("TransitionMainMenuToTitleScreen");
                return;
            }
            for (int i = 0; i < NUM_ITEMS; ++i)
            {
                mMenuItems[i].HandlePointerEvent(&event);
            }
            if (!mItemSelected)
            {
                if (mHighlightedItemCounts[pad] > 0)
                {
                    mItemDescription->m_bVisible = false;
                }
                else
                {
                    mItemDescription->m_bVisible = true;
                }
            }
        }
    }
}

/**
 * Offset/Address/Size: 0x174 | 0x8021F268 | size: 0x388
 */
void SHMainMenu::InitializeMenuItems()
{
    FEPointerListener::Callback openFunc(Bind<void>(MemFun(&SHMainMenu::OpenItem), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback closeFunc(Bind<void>(MemFun(&SHMainMenu::CloseItem), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback applyFunc(Bind<void>(MemFun(&SHMainMenu::SelectItem), this, Placeholder<0>(), Placeholder<1>()));

    for (int i = 0; i < NUM_ITEMS; ++i)
    {
        feVector3 position = mMenuItemInstances[i]->GetAssetPosition();
        feVector3 groupPosition = mMenuGroups[i]->GetAssetPosition();
        float offsetX = position.f.x + groupPosition.f.x;
        float offsetY = position.f.y + groupPosition.f.y;
        mMenuItems[i].SetInstanceBounds(mMenuHitImages[i], true, offsetX, offsetY, 1.0f, 1.0f);
        mMenuItems[i].SetPointerEnterCallback(openFunc);
        mMenuItems[i].SetPointerLeaveCallback(closeFunc);
        mMenuItems[i].SetPointerPressCallback(applyFunc);
    }
}

/**
 * Offset/Address/Size: 0x4FC | 0x8021F5F0 | size: 0xC4
 */
void SHMainMenu::CloseItem(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    --mHighlightedItemCounts[index];
    if (!mMenuItems[item].HasOtherPointerState(1, index))
    {
        mMenuHighlights[item]->SetActiveSlide("off", true, false);
        mMenuArrows[item]->SetActiveSlide("off", true, false);
        mMenuItems[item].SetPointerState(0, index);
    }
}

/**
 * Offset/Address/Size: 0x5C0 | 0x8021F6B4 | size: 0x20
 */
void SHMainMenu::ReturnToWiiMenu()
{
    ResetTask::s_ResetMode = 3;
    ResetTask::s_ResetState = ResetTask::s_ResetState == RS_RUNNING ? RS_STARTRESET : ResetTask::s_ResetState;
}

/**
 * Offset/Address/Size: 0x1E18 | 0x80220F0C | size: 0xDC
 */
void SHMainMenu::OpenItem(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    ++mHighlightedItemCounts[index];
    if (!mMenuItems[item].HasOtherPointerState(1, index))
    {
        mMenuHighlights[item]->SetActiveSlide("over", true, false);
        mMenuArrows[item]->SetActiveSlide("over", true, false);
        mMenuItems[item].SetPointerState(1, index);
        FEAudio::PlayAnimAudioEvent(0x6B0689D4, 0, 0, true);
    }
}

/**
 * Offset/Address/Size: 0x1EF4 | 0x80220FE8 | size: 0x2D0
 */
void SHMainMenu::SelectItem(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (GetTweakBool("/user/media_build", false) && (item == 3 || item == 5 || item == 6))
    {
        FEAudio::PlayAnimAudioEvent(0x7326DD54, 0, 0, true);
        return;
    }

    FEAudio::PlayAnimAudioEvent(0x0A93E9A0, 0, 0, true);
    if (item == 1)
    {
        if (!SaveEnabled)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x46);
        }
        else if (!g_pMiiManager->mInitialized)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x87, Bind<void>(MemFun(&SHMainMenu::ReturnToWiiMenu), this));
        }
        else if (RFLGetAvailableOfficialDataNum() == 0)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x86);
        }
        else
        {
            SaveLoad::StartLoad(true);
            mWaitingForLoad = true;
        }
    }
    else
    {
        ApplyItem(item);
    }
}

/**
 * Offset/Address/Size: 0x21C4 | 0x802212B8 | size: 0xB04
 */
void SHMainMenu::ApplyItem(unsigned int item)
{
    if (item < NUM_ITEMS)
    {
        for (int i = 0; i < NUM_ITEMS; ++i)
        {
            mMenuItems[i].Disable();
        }
    }
    mItemSelected = true;
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }

    switch (item)
    {
    case 0:
    {
        GameInfoManager::Instance()->SetMode(0, 0);
        GameInfoManager::Instance()->SetTeam(0, 0);
        GameInfoManager::Instance()->SetTeam(1, 0);
        GameInfoManager::Instance()->ResetPlayingSides();
        GameSceneManager::Instance()->Pop();
        SHNavigation* scene = GetNavigationScene();
        if (scene != 0)
        {
            scene->HideButtons();
        }
        FEAudio::PlayAnimAudioEvent(0x6C2AE132, 0, 0, true);
        gNextFETransition = "TransitionMainMenuToGrudgeMatch";
        Presentation::GetInstance()->Call("TransitionFromMainMenu");
        break;
    }
    case 1:
    {
        GameInfoManager::Instance()->mIsOnlineMode = true;
        GameSceneManager::Instance()->Pop();
        if (g_pNetworkSessionBase->GetSessionMode() == 0)
        {
            gNextFETransition = "TransitionMainMenuToOnlineMatch";
        }
        SHNavigation* scene = GetNavigationScene();
        if (scene != 0)
        {
            scene->SetButtons(0, true);
        }
        Presentation::GetInstance()->Call("TransitionFromMainMenu");
        FEAudio::PlayAnimAudioEvent(0xB19DBC20, 0, 0, true);
        break;
    }
    case 2:
        GameInfoManager::Instance()->SetMode(3, 0);
        if (g_pCupManager->GetCurrentMode() == -1)
        {
            StartNewCup();
        }
        else
        {
            ShowCupStartOptions();
        }
        break;
    case 3:
    {
        GameInfoManager::Instance()->SetMode(4, 0);
        FEAudio::PlayAnimAudioEvent(0xB19DBC20, 0, 0, true);
        GameSceneManager::Instance()->Pop();
        SHNavigation* scene = GetNavigationScene();
        if (scene != 0)
        {
            scene->HideButtons();
        }
        gNextFETransition = "TransitionMainMenuToStrikerChallenges";
        Presentation::GetInstance()->Call("TransitionFromMainMenu");
        break;
    }
    case 4:
    {
        GameInfoManager::Instance()->SetMode(4, 0);
        FEAudio::PlayAnimAudioEvent(0xB19DBC20, 0, 0, true);
        GameSceneManager::Instance()->Pop();
        SHNavigation* scene = GetNavigationScene();
        if (scene != 0)
        {
            scene->HideButtons();
        }
        gNextFETransition = "TransitionMainMenuToTutorial";
        Presentation::GetInstance()->Call("TransitionFromMainMenu");
        break;
    }
    case 5:
    {
        GameSceneManager::Instance()->Pop();
        FEAudio::PlayAnimAudioEvent(0xB19DBC20, 0, 0, true);
        SHNavigation* scene = GetNavigationScene();
        if (scene != 0)
        {
            scene->HideButtons();
        }
        SetLockedTrophyVisibility(false);
        gNextFETransition = "TransitionMainMenuToHallOfFame";
        Presentation::GetInstance()->Call("TransitionFromMainMenu");
        break;
    }
    case 6:
    {
        GameSceneManager::Instance()->Pop();
        SHNavigation* scene = GetNavigationScene();
        if (scene != 0)
        {
            scene->HideButtons();
        }
        FEAudio::PlayAnimAudioEvent(0xB19DBC20, 0, 0, true);
        gNextFETransition = "TransitionMainMenuToOptions";
        Presentation::GetInstance()->Call("TransitionFromMainMenu");
        break;
    }
    }
}

#include "Game/FE/feFinder_impl.h"
