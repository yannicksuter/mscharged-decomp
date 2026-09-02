#ifndef _OVERLAYHANDLERHUD_H_
#define _OVERLAYHANDLERHUD_H_

#include "Game/FE/BaseOverlayHandler.h"

class HUDOverlay : public BaseOverlayHandler
{
public:
    HUDOverlay();
    virtual ~HUDOverlay();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void SetSlideIn();
    void SetSlideOut();
    void UpdateScore();
    void DisplayNewScore();
    void ResetScores();
    void SwapPowerUps(int homeAway);
};

#endif // _OVERLAYHANDLERHUD_H_
