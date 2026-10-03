#ifndef _OVERLAYHANDLERHUD_H_
#define _OVERLAYHANDLERHUD_H_

#include "Game/FE/BaseOverlayHandler.h"
#include "NL/nlColour.h"

class AsyncImage;
class FETextureResource;
class TLComponentInstance;
class TLImageInstance;
class TLTextInstance;

struct HUDCaptainMeter
{
    HUDCaptainMeter()
    {
        for (int i = 0; i < 2; i++)
        {
            m_pMeter[i] = 0;
            mMeterFullScale[i] = 0.0f;
            mMeterShown[i] = false;
        }
    }

    void Update(float fDeltaT);
    void Init(FEPresentation* presentation);

    /* 0x00 */ TLComponentInstance* m_pMeter[2];
    /* 0x08 */ TLComponentInstance* m_pPowerBarContainer[2];
    /* 0x10 */ TLImageInstance* m_pMeterFill[2];
    /* 0x18 */ float mMeterFullScale[2];
    /* 0x20 */ TLComponentInstance* m_pPowerUpPad[2];
    /* 0x28 */ bool mMeterShown[2];
};

struct HUDPowerUpTextures
{
    void LoadHUDTextures(FEPresentation* presentation);

    /* 0x00 */ FETextureResource* m_pStar;
    /* 0x04 */ FETextureResource* m_pMega;
    /* 0x08 */ FETextureResource* m_pShellGreen;
    /* 0x0C */ FETextureResource* m_pShellRed;
    /* 0x10 */ FETextureResource* m_pBanana;
    /* 0x14 */ FETextureResource* m_pMushroom;
    /* 0x18 */ FETextureResource* m_pShellBlue;
    /* 0x1C */ FETextureResource* m_pBobomb;
    /* 0x20 */ FETextureResource* m_pShellSpike;
    /* 0x24 */ FETextureResource* m_pChomp;
    /* 0x28 */ FETextureResource* m_pCaptainAbility[12];
};

struct HUDPowerUpDisplay
{
    HUDPowerUpDisplay()
    {
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                mNumFlareCycles[i][j] = -1;
                m_pImagePowerUps[0][i][j] = 0;
                m_pImagePowerUps[1][i][j] = 0;
                m_pImageFlares[i][j] = 0;
                m_pComponentFlares[i][j] = 0;
                m_pPowerupTextComponents[i][j] = 0;
                m_pPowerUpComponents[i][j] = 0;
            }
            mHasPowerUps[i] = false;
            mHasTwoPowerUps[i] = false;
        }
        m_pPowerUpTextures = 0;
    }

    void DisplayPowerUps(float fDeltaT);
    void Init(FEPresentation* presentation, HUDPowerUpTextures* textures);

    /* 0x00 */ TLImageInstance* m_pImagePowerUps[2][2][2];
    /* 0x20 */ TLImageInstance* m_pImageFlares[2][2];
    /* 0x30 */ TLComponentInstance* m_pComponentFlares[2][2];
    /* 0x40 */ TLComponentInstance* m_pPowerupTextComponents[2][2];
    /* 0x50 */ TLComponentInstance* m_pPowerUpPads[2][2];
    /* 0x60 */ TLComponentInstance* m_pPowerUpComponents[2][2];
    /* 0x70 */ TLComponentInstance* m_pBlinkers[2];
    /* 0x78 */ int mNumFlareCycles[2][2];
    /* 0x88 */ bool mHasPowerUps[2];
    /* 0x8A */ bool mHasTwoPowerUps[2];
    /* 0x8C */ HUDPowerUpTextures* m_pPowerUpTextures;
};

struct HUDClock
{
    HUDClock()
    {
        mSeconds = -1;
        mMinutes = -1;
        mTenths = -1;
        mClockColourChanged = false;
        mOvertimeSFXPlayed = false;
    }

    void Init(FEPresentation* presentation);
    void Update(float fDeltaT);

    /* 0x00 */ unsigned long mSeconds;
    /* 0x04 */ unsigned long mMinutes;
    /* 0x08 */ unsigned long mTenths;
    /* 0x0C */ TLTextInstance* m_pTextInstanceClock[2];
    /* 0x14 */ unsigned short mClockBuffer[32];
    /* 0x54 */ bool mClockColourChanged;
    /* 0x55 */ bool mOvertimeSFXPlayed;
    /* 0x56 */ nlColour mOriginalClockColour;
    /* 0x5C */ TLComponentInstance* mSuddenDeath[2];
    /* 0x64 */ TLTextInstance* m_pTextInstanceGameType;
};

struct HUDScoreDisplay
{
    HUDScoreDisplay()
    {
        for (int i = 0; i < 2; i++)
        {
            mScore[i] = 0;
            mNewScore[i] = 0;
            mScoreBuffer[i][0] = 0;
            m_pTextInstanceScore[0][i] = 0;
            m_pTextInstanceScore[1][i] = 0;
            mScoreUpdateDelay[i] = 0.0f;
            mStartScoreAnimation[i] = false;
        }
    }

    void ResetScores();
    void Init(FEPresentation* presentation);
    void Update(float fDeltaT);

    /* 0x00 */ int mScore[2];
    /* 0x08 */ int mNewScore[2];
    /* 0x10 */ unsigned short mScoreBuffer[2][32];
    /* 0x90 */ TLTextInstance* m_pTextInstanceScore[2][2];
    /* 0xA0 */ float mScoreUpdateDelay[2];
    /* 0xA8 */ bool mStartScoreAnimation[2];
    /* 0xAC */ FEPresentation* mPresentation;
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
    void SetTeamIcons();

    /* 0x028 */ HUDCaptainMeter mCaptainMeter;
    /* 0x054 */ HUDPowerUpTextures mPowerUpTextures;
    /* 0x0AC */ HUDPowerUpDisplay mPowerUpDisplay;
    /* 0x13C */ unsigned char mUnidentified13C[4];
    /* 0x140 */ HUDClock mClock;
    /* 0x1A8 */ HUDScoreDisplay mScoreDisplay;
    /* 0x258 */ AsyncImage* mAsyncImage[2];
};

#endif // _OVERLAYHANDLERHUD_H_
