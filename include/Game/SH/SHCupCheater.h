#ifndef _SHCUPCHEATER_H_
#define _SHCUPCHEATER_H_

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feSlideMenu.h"

class CupCheaterScene : public BaseSceneHandler
{
public:
    CupCheaterScene();
    virtual ~CupCheaterScene();
    virtual void Update(float dt);
    virtual void SceneCreated();

    void OnSelectGameplay();
    void OnSelectHomeWin();
    void OnSelectAwayWin();
    void OnSelectHomeOTWin();
    void OnSelectAwayOTWin();
    void fn_801E793C();
    void fn_801E7A94();
    void UpdateSlides();

    /* 0x1C */ FESlideMenu* m_SlideMenu;
    /* 0x20 */ int mSniper;
    /* 0x24 */ int mStriker;
    /* 0x28 */ unsigned short mSniperBuffer[10];
    /* 0x3C */ unsigned short mStrikerBuffer[10];
    /* 0x50 */ int mUnidentified50;
}; // size 0x54

#endif // _SHCUPCHEATER_H_
