#ifndef _OVERLAYHANDLERHUD_H_
#define _OVERLAYHANDLERHUD_H_

#include "Game/FE/BaseOverlayHandler.h"
#include "NL/nlColour.h"

class AsyncImage;
class FETextureResource;
class TLComponentInstance;
class TLImageInstance;
class TLTextInstance;

struct UnidentifiedHUD_801EB108
{
    UnidentifiedHUD_801EB108()
    {
        for (int i = 0; i < 2; i++)
        {
            mUnidentified00[i] = 0;
            mUnidentified18[i] = 0.0f;
            mUnidentified28[i] = false;
        }
    }

    void fn_801EAF50(float fDeltaT);
    void fn_801EB108(FEPresentation* presentation);

    /* 0x00 */ TLComponentInstance* mUnidentified00[2];
    /* 0x08 */ TLComponentInstance* mUnidentified08[2];
    /* 0x10 */ TLImageInstance* mUnidentified10[2];
    /* 0x18 */ float mUnidentified18[2];
    /* 0x20 */ TLComponentInstance* mUnidentified20[2];
    /* 0x28 */ bool mUnidentified28[2];
};

struct UnidentifiedHUD_801E8CD0
{
    void fn_801E8CD0(FEPresentation* presentation);

    /* 0x00 */ FETextureResource* m_pStar;
    /* 0x04 */ FETextureResource* mUnidentified04;
    /* 0x08 */ FETextureResource* m_pShellGreen;
    /* 0x0C */ FETextureResource* m_pShellRed;
    /* 0x10 */ FETextureResource* m_pBanana;
    /* 0x14 */ FETextureResource* m_pMushroom;
    /* 0x18 */ FETextureResource* m_pShellBlue;
    /* 0x1C */ FETextureResource* m_pBobomb;
    /* 0x20 */ FETextureResource* m_pShellSpike;
    /* 0x24 */ FETextureResource* m_pChomp;
    /* 0x28 */ FETextureResource* mUnidentified28[12];
};

struct UnidentifiedHUD_801E9198
{
    UnidentifiedHUD_801E9198()
    {
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                mNumFlareCycles[i][j] = -1;
                m_pImagePowerUps[0][i][j] = 0;
                m_pImagePowerUps[1][i][j] = 0;
                mUnidentified20[i][j] = 0;
                m_pComponentFlares[i][j] = 0;
                m_pPowerupTextComponents[i][j] = 0;
                mUnidentified60[i][j] = 0;
            }
            mUnidentified88[i] = false;
            mUnidentified8A[i] = false;
        }
        mUnidentified8C = 0;
    }

    void fn_801E9198(float fDeltaT);
    void fn_801EB5CC(FEPresentation* presentation, UnidentifiedHUD_801E8CD0* textures);

    /* 0x00 */ TLImageInstance* m_pImagePowerUps[2][2][2];
    /* 0x20 */ TLImageInstance* mUnidentified20[2][2];
    /* 0x30 */ TLComponentInstance* m_pComponentFlares[2][2];
    /* 0x40 */ TLComponentInstance* m_pPowerupTextComponents[2][2];
    /* 0x50 */ TLComponentInstance* mUnidentified50[2][2];
    /* 0x60 */ TLComponentInstance* mUnidentified60[2][2];
    /* 0x70 */ TLComponentInstance* mUnidentified70[2];
    /* 0x78 */ int mNumFlareCycles[2][2];
    /* 0x88 */ bool mUnidentified88[2];
    /* 0x8A */ bool mUnidentified8A[2];
    /* 0x8C */ UnidentifiedHUD_801E8CD0* mUnidentified8C;
};

struct UnidentifiedHUD_801E9AF0
{
    UnidentifiedHUD_801E9AF0()
    {
        mSeconds = -1;
        mMinutes = -1;
        mTenths = -1;
        mClockColourChanged = false;
        mOvertimeSFXPlayed = false;
    }

    void fn_801E9AF0(FEPresentation* presentation);
    void fn_801E9FEC(float fDeltaT);

    /* 0x00 */ unsigned long mSeconds;
    /* 0x04 */ unsigned long mMinutes;
    /* 0x08 */ unsigned long mTenths;
    /* 0x0C */ TLTextInstance* m_pTextInstanceClock[2];
    /* 0x14 */ unsigned short mClockBuffer[32];
    /* 0x54 */ bool mClockColourChanged;
    /* 0x55 */ bool mOvertimeSFXPlayed;
    /* 0x56 */ nlColour mOriginalClockColour;
    /* 0x5C */ TLComponentInstance* mSuddenDeath[2];
    /* 0x64 */ TLTextInstance* mUnidentified64;
};

struct UnidentifiedHUD_801E99F0
{
    UnidentifiedHUD_801E99F0()
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

    void fn_801E99F0();
    void fn_801EA808(FEPresentation* presentation);
    void fn_801EAD2C(float fDeltaT);

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

    /* 0x028 */ UnidentifiedHUD_801EB108 mUnidentified028;
    /* 0x054 */ UnidentifiedHUD_801E8CD0 mUnidentified054;
    /* 0x0AC */ UnidentifiedHUD_801E9198 mUnidentified0AC;
    /* 0x13C */ unsigned char mUnidentified13C[4];
    /* 0x140 */ UnidentifiedHUD_801E9AF0 mUnidentified140;
    /* 0x1A8 */ UnidentifiedHUD_801E99F0 mUnidentified1A8;
    /* 0x258 */ AsyncImage* mAsyncImage[2];
};

#endif // _OVERLAYHANDLERHUD_H_
