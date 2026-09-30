#ifndef _SHCUPCHEATER_H_
#define _SHCUPCHEATER_H_

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feSlideMenu.h"

class CupCheaterScene : public BaseSceneHandler
{
public:
    CupCheaterScene();
    virtual ~CupCheaterScene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void OnSelectGameplay();
    void OnSelectHomeWin();
    void OnSelectAwayWin();
    void OnSelectHomeOTWin();
    void OnSelectAwayOTWin();
    void OnSelectScore();
    void ProcessPostGame();
    void UpdateSlides();

    /* 0x1C */ FESlideMenu* m_SlideMenu;
    /* 0x20 */ int mHomeScore;
    /* 0x24 */ int mAwayScore;
    /* 0x28 */ unsigned short mHomeScoreBuffer[10];
    /* 0x3C */ unsigned short mAwayScoreBuffer[10];
    /* 0x50 */ int mUserSide;
}; // size 0x54

#endif // _SHCUPCHEATER_H_
