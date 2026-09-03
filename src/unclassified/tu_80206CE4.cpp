#include "unclassified/tu_80206CE4.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/DB/SaveLoad.h"
#include "Game/DB/tu_8010A40C.h"
#include "Game/FE/feMusic.h"

extern BaseGameSceneManager* lbl_806E1838;
extern "C" void fn_8010C5C0(UnidentifiedCupManager* manager);
extern "C" void fn_8010D2D0(UnidentifiedCupManager* manager);
extern "C" void fn_80207354(bool value);

bool lbl_806E18A8;

extern const int lbl_804E84A8[3] = { 4, 5, 6 };
extern const int lbl_804E84B4[3] = { 3, 2, 1 };
extern const int lbl_806E5838[2] = { 2, 1 };

extern "C" void fn_80206CE4(int currentPage, bool advance)
{
    if ((unsigned int)(currentPage - 4) > 2)
    {
        fn_80206DFC(currentPage, advance);
    }
    else
    {
        int currentIndex = 0;
        for (int i = 0; i < 3; ++i)
        {
            if (currentPage == lbl_804E84A8[i])
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

        switch (lbl_804E84A8[nextIndex])
        {
        case 4:
            lbl_806E1838->Push((SceneList)36, SCREEN_NOTHING, true);
            break;
        case 5:
            lbl_806E1838->Push((SceneList)37, SCREEN_NOTHING, true);
            break;
        case 6:
            lbl_806E1838->Push((SceneList)38, SCREEN_NOTHING, true);
            break;
        }
    }
}

extern "C" void fn_80206DFC(int currentPage, bool advance)
{
    int currentIndex = 0;
    int pageCount = 0;
    const int* pages = 0;

    int roundType = lbl_806E0F90->mUnidentified8680 == 0x10
                      ? 0
                      : lbl_806E0F90->GetCurrentRoundType();

    if (roundType != 0)
    {
        switch (roundType)
        {
        case 1:
            pageCount = 2;
            pages = lbl_806E5838;
            break;
        case 2:
            pageCount = 3;
            pages = lbl_804E84B4;
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
            lbl_806E1838->Push((SceneList)32, SCREEN_NOTHING, true);
            break;
        case 2:
            lbl_806E1838->Push((SceneList)34, SCREEN_NOTHING, true);
            break;
        case 3:
            lbl_806E1838->Push((SceneList)35, SCREEN_NOTHING, true);
            break;
        }
    }
}

extern "C" void fn_80206F94()
{
    lbl_806E1838->Push((SceneList)36, SCREEN_NOTHING, true);
}

extern "C" void fn_80206FB4()
{
    int scene = -2;
    int roundType = lbl_806E0F90->mUnidentified8680 == 0x10
                      ? 0
                      : lbl_806E0F90->GetCurrentRoundType();

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

    lbl_806E1838->Push((SceneList)scene, SCREEN_NOTHING, true);
}

extern "C" void fn_80207B8C()
{
    fn_80207354(false);
}

extern "C" void fn_80207B94()
{
    lbl_806E18A8 = true;
}

extern "C" void fn_8020815C()
{
    FEMusic::StartStreamIfDifferent(9);
    fn_8010C5C0(lbl_806E0F90);
    fn_8010D2D0(lbl_806E0F90);
    lbl_806E1838->Push((SceneList)31, SCREEN_NOTHING, true);
    SaveLoad::StartSave(false);
}

extern "C" const char* fn_802081B8()
{
    return "waiting";
}

extern "C" void fn_80208380()
{
    lbl_806E1838->Push((SceneList)31, SCREEN_NOTHING, false);
}
