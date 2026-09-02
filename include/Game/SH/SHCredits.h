#ifndef GAME_SH_SHCREDITS_H
#define GAME_SH_SHCREDITS_H

#include "Game/SH/SHMoviePlayer.h"
#include "Game/Sys/simpleparser.h"
#include "NL/nlMemory.h"

class TLComponentInstance;
class TLTextInstance;

class CreditScene : public MoviePlayerScene
{
public:
    struct CreditParser
    {
        CreditParser()
            : mFileSize(0)
            , mFileData(0)
        {
        }

        ~CreditParser()
        {
            if (mFileData != 0)
            {
                nlFree(mFileData);
                mFileData = 0;
            }
        }

        /* 0x000 */ unsigned long mFileSize;
        /* 0x004 */ char* mFileData;
        /* 0x008 */ SimpleParser mParser;
    }; // size 0x520

    CreditScene();
    virtual ~CreditScene();
    virtual void SceneCreated();
    virtual void Update(float fDeltaT);
    virtual void MoviePlayerVirtual3C();

    void SetupForPhase();
    void SetupForCredits();
    void UpdateForCopyrightMessage(float fDeltaT);
    void UpdateForCredits(float fDeltaT);
    void UpdateForNintendoLogo(float fDeltaT);
    TLComponentInstance* GetWhiteFadeComponent();

    static SceneList mNextScene;

    /* 0x0B4 */ TLTextInstance* m_pTextLines[20];
    /* 0x104 */ bool mLineOnScreen[20];
    /* 0x118 */ bool mAreCreditsOver;
    /* 0x119 */ bool mFinalMessageDisplayed;
    /* 0x11A */ bool mFadeStarted;
    /* 0x11B */ bool mCenteredLine[20];
    /* 0x130 */ float mTimeElapsed;
    /* 0x134 */ int mPhase;
    /* 0x138 */ CreditParser mCreditParser;
    /* 0x658 */ unsigned short mStrings[20][64];
}; // size 0x1058

#endif // GAME_SH_SHCREDITS_H
