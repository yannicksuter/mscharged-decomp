#include "Game/SH/SHHallOfFame.h"
#include "Game/SH/SHNavigation.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/FE/feCupFlow.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/FEAudio.h"

#include "Game/GameSceneManager.h"
#include "Game/DB/GameProgress.h"
#include <string.h>
#include "Game/BaseGameSceneManager.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/FE/feCupFlow.h"
#include "Game/FE/feAsyncImage.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/Render/FrontEndPresentation.h"
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

AsyncImage* sHallOfFameImages[12];
int sHallOfFamePlayerCardIndex;
int sHallOfFameReturnMode;
bool sHallOfFameImagePreloadStarted;
int sHallOfFameImageLoadsPending;

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

void SetHallOfFameBreadcrumbs(int mode, TLComponentInstance* breadcrumbs)
{
    FEFinder<TLComponentInstance, 4>::FindOrDefault(breadcrumbs->GetActiveSlide(), "breadcrumb_5")->m_bVisible = false;
    FEFinder<TLComponentInstance, 4>::FindOrDefault(breadcrumbs->GetActiveSlide(), "breadcrumb_6")->m_bVisible = false;
    FEFinder<TLComponentInstance, 4>::FindOrDefault(breadcrumbs->GetActiveSlide(), "breadcrumb_7")->m_bVisible = false;

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
            PushPresentationCamera("hofbronze", 0, 0.5f, true);
        }
        else
        {
            PushPresentationCamera("43hofbronze", 0, 0.5f, true);
        }
        GameSceneManager::Instance()->Push((SceneList)62, SCREEN_NOTHING, true);
        break;
    case 1:
        if (wideScreen)
        {
            PushPresentationCamera("hofgold", 0, 0.5f, true);
        }
        else
        {
            PushPresentationCamera("43hofgold", 0, 0.5f, true);
        }
        GameSceneManager::Instance()->Push((SceneList)63, SCREEN_NOTHING, true);
        break;
    case 2:
        if (wideScreen)
        {
            PushPresentationCamera("hofsilver", 0, 0.5f, true);
        }
        else
        {
            PushPresentationCamera("43hofsilver", 0, 0.5f, true);
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
        PushPresentationCamera("trophycentreofbronzehof", 0, 0.5f, true);
        GameSceneManager::Instance()->Push((SceneList)66, SCREEN_NOTHING, true);
        break;
    case 6:
        PushPresentationCamera("trophycentreofgoldhof", 0, 0.5f, true);
        GameSceneManager::Instance()->Push((SceneList)69, SCREEN_NOTHING, true);
        break;
    case 5:
        PushPresentationCamera("trophycentreofsilverhof", 0, 0.5f, true);
        GameSceneManager::Instance()->Push((SceneList)72, SCREEN_NOTHING, true);
        break;
    case 8:
        PushPresentationCamera("trophyrightofbronze", 0, 0.5f, true);
        GameSceneManager::Instance()->Push((SceneList)67, SCREEN_NOTHING, true);
        break;
    case 7:
        PushPresentationCamera("trophyleftofbronze", 0, 0.5f, true);
        GameSceneManager::Instance()->Push((SceneList)68, SCREEN_NOTHING, true);
        break;
    case 12:
        PushPresentationCamera("trophyrightofgold", 0, 0.5f, true);
        GameSceneManager::Instance()->Push((SceneList)70, SCREEN_NOTHING, true);
        break;
    case 11:
        PushPresentationCamera("trophyleftofgold", 0, 0.5f, true);
        GameSceneManager::Instance()->Push((SceneList)71, SCREEN_NOTHING, true);
        break;
    case 10:
        PushPresentationCamera("trophyrightofsilver", 0, 0.5f, true);
        GameSceneManager::Instance()->Push((SceneList)73, SCREEN_NOTHING, true);
        break;
    case 9:
        PushPresentationCamera("trophyleftofsilver", 0, 0.5f, true);
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
        FrontEndPresentation::GetInstance()->Call("TransitionHallOfFameToMainMenu");
        break;
    case 4:
    case 7:
    case 8:
        if (wideScreen)
        {
            PushPresentationCamera("hofbronze", 0, 0.5f, true);
        }
        else
        {
            PushPresentationCamera("43hofbronze", 0, 0.5f, true);
        }
        GameSceneManager::Instance()->Push((SceneList)62, SCREEN_BACK, true);
        break;
    case 6:
    case 11:
    case 12:
        if (wideScreen)
        {
            PushPresentationCamera("hofgold", 0, 0.5f, true);
        }
        else
        {
            PushPresentationCamera("43hofgold", 0, 0.5f, true);
        }
        GameSceneManager::Instance()->Push((SceneList)63, SCREEN_BACK, true);
        break;
    case 5:
    case 9:
    case 10:
        if (wideScreen)
        {
            PushPresentationCamera("hofsilver", 0, 0.5f, true);
        }
        else
        {
            PushPresentationCamera("43hofsilver", 0, 0.5f, true);
        }
        GameSceneManager::Instance()->Push((SceneList)64, SCREEN_BACK, true);
        break;
    case 13:
        if (sHallOfFamePlayerCardIndex < 4)
        {
            if (wideScreen)
            {
                PushPresentationCamera("hofbronze", 0, 0.5f, true);
            }
            else
            {
                PushPresentationCamera("43hofbronze", 0, 0.5f, true);
            }
            GameSceneManager::Instance()->Push((SceneList)62, SCREEN_BACK, true);
        }
        else if (sHallOfFamePlayerCardIndex < 8)
        {
            if (wideScreen)
            {
                PushPresentationCamera("hofsilver", 0, 0.5f, true);
            }
            else
            {
                PushPresentationCamera("43hofsilver", 0, 0.5f, true);
            }
            GameSceneManager::Instance()->Push((SceneList)64, SCREEN_BACK, true);
        }
        else
        {
            if (wideScreen)
            {
                PushPresentationCamera("hofgold", 0, 0.5f, true);
            }
            else
            {
                PushPresentationCamera("43hofgold", 0, 0.5f, true);
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
