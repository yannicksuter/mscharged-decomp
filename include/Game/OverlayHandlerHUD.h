#ifndef _OVERLAYHANDLERHUD_H_
#define _OVERLAYHANDLERHUD_H_

#include "Game/FE/BaseOverlayHandler.h"

class TLTextInstance;

struct UnidentifiedHUD_801E99F0
{
    void fn_801E99F0();

    /* 0x00 */ int mScore[2];
    /* 0x08 */ int mNewScore[2];
    /* 0x10 */ unsigned short mScoreBuffer[2][32];
    /* 0x90 */ TLTextInstance* m_pTextInstanceScore[2][2];
    /* 0xA0 */ float mScoreUpdateDelay[2];
    /* 0xA8 */ bool mStartScoreAnimation[2];
};

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

    /* 0x028 */ unsigned char mUnidentified028[0xFC];
    /* 0x124 */ int mNumFlareCycles[2][2];
    /* 0x134 */ unsigned char mUnidentified134[0x74];
    /* 0x1A8 */ UnidentifiedHUD_801E99F0 mUnidentified1A8;
};

#endif // _OVERLAYHANDLERHUD_H_
