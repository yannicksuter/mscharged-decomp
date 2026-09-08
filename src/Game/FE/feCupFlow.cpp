#include "Game/FE/feCupFlow.h"

#include "Game/GameSceneManager.h"
#include "Game/DB/SaveLoad.h"
#include "Game/DB/GameProgress.h"
#include "Game/FE/feMusic.h"

extern "C" void fn_8010C5C0(CupManager* manager);
extern "C" void fn_8010D2D0(CupManager* manager);
extern "C" void fn_80207354(bool value);

bool gMainMenuInputResetPending;

extern const int sCupPageOrder[3] = { 4, 5, 6 };
extern const int sCupRoundPageOrderThree[3] = { 3, 2, 1 };
extern const int sCupRoundPageOrderTwo[2] = { 2, 1 };

void CycleCupPage(int currentPage, bool advance)
{
    if ((unsigned int)(currentPage - 4) > 2)
    {
        CycleCupRoundPage(currentPage, advance);
    }
    else
    {
        int currentIndex = 0;
        for (int i = 0; i < 3; ++i)
        {
            if (currentPage == sCupPageOrder[i])
            {
                currentIndex = i;
                break;
            }
        }

        int nextIndex = currentIndex - 1;
        if (advance)
        {
            nextIndex = currentIndex + 1;
        }
        if (nextIndex >= 3)
        {
            nextIndex = 0;
        }
        else if (nextIndex < 0)
        {
            nextIndex = 2;
        }

        switch (sCupPageOrder[nextIndex])
        {
        case 4:
            GameSceneManager::Instance()->Push((SceneList)36, SCREEN_NOTHING, true);
            break;
        case 5:
            GameSceneManager::Instance()->Push((SceneList)37, SCREEN_NOTHING, true);
            break;
        case 6:
            GameSceneManager::Instance()->Push((SceneList)38, SCREEN_NOTHING, true);
            break;
        }
    }
}

void CycleCupRoundPage(int currentPage, bool advance)
{
    int currentIndex = 0;
    int pageCount = 0;
    const int* pages = 0;

    int roundType = g_pCupManager->mUnidentified8680 == 0x10
                      ? 0
                      : g_pCupManager->GetCurrentRoundType();

    if (roundType != 0)
    {
        switch (roundType)
        {
        case 1:
            pageCount = 2;
            pages = sCupRoundPageOrderTwo;
            break;
        case 2:
            pageCount = 3;
            pages = sCupRoundPageOrderThree;
            break;
        }

        for (int i = 0; i < pageCount; ++i)
        {
            if (currentPage == pages[i])
            {
                currentIndex = i;
                break;
            }
        }

        if (advance == true)
        {
            ++currentIndex;
        }
        else
        {
            --currentIndex;
        }
        if (currentIndex >= pageCount)
        {
            currentIndex = 0;
        }
        else if (currentIndex < 0)
        {
            currentIndex = pageCount - 1;
        }

        switch (pages[currentIndex])
        {
        case 1:
            GameSceneManager::Instance()->Push((SceneList)32, SCREEN_NOTHING, true);
            break;
        case 2:
            GameSceneManager::Instance()->Push((SceneList)34, SCREEN_NOTHING, true);
            break;
        case 3:
            GameSceneManager::Instance()->Push((SceneList)35, SCREEN_NOTHING, true);
            break;
        }
    }
}

void ShowFirstCupPage()
{
    GameSceneManager::Instance()->Push((SceneList)36, SCREEN_NOTHING, true);
}

void ShowCurrentCupRoundPage()
{
    int scene = -2;
    int roundType = g_pCupManager->mUnidentified8680 == 0x10
                      ? 0
                      : g_pCupManager->GetCurrentRoundType();

    switch (roundType)
    {
    case 0:
        scene = 32;
        break;
    case 1:
        scene = 34;
        break;
    case 2:
        scene = 35;
        break;
    }

    GameSceneManager::Instance()->Push((SceneList)scene, SCREEN_NOTHING, true);
}

extern "C" void fn_80207B8C()
{
    fn_80207354(false);
}

void RequestMainMenuInputReset()
{
    gMainMenuInputResetPending = true;
}

void SaveAndShowCupHub()
{
    FEMusic::StartStreamIfDifferent(9);
    fn_8010C5C0(g_pCupManager);
    fn_8010D2D0(g_pCupManager);
    GameSceneManager::Instance()->Push((SceneList)31, SCREEN_NOTHING, true);
    SaveLoad::StartSave(false);
}

const char* GetCupWaitingSlide()
{
    return "waiting";
}

void ShowCupHub()
{
    GameSceneManager::Instance()->Push((SceneList)31, SCREEN_NOTHING, false);
}
