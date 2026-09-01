#ifndef GAME_SH_SH_CHOOSE_SIDES_H
#define GAME_SH_SH_CHOOSE_SIDES_H

#include "Game/BaseGameSceneManager.h"
#include "NL/nlColour.h"
#include "unclassified/tu_80219248.h"
#include "unclassified/tu_8022EF84.h"

class TLComponentInstance;
class TLImageInstance;

class SHChooseSides2 : public BaseSceneHandler
{
public:
    enum eCSContext
    {
        FRIENDLY = 0,
        CUP = 1,
        SUPERCUP = 2,
        TOURNAMENT = 3,
        PAUSE = 4,
    };

    SHChooseSides2(eCSContext context, ScreenMovement movement);
    virtual ~SHChooseSides2();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void fn_8021CBD0();
    void BindChooseSideInstances();
    void fn_8021DC28(int index, void* context);
    void fn_8021DCFC(int index, void* context);
    void fn_8021DDAC(int index, void* context);
    void fn_8021DFCC(int index, void* context);
    void fn_8021E098(int index, void* context);
    void fn_8021E170(int index, void* context);
    void fn_8021E1E0(int index, void* context);
    void Proceed();
    void fn_8021E64C(int index, void* context);
    void fn_8021E6E8(int index, void* context);
    void fn_8021E76C(int index, void* context);
    void fn_8021E910(int index);
    void fn_8021EB18();
    void fn_8021ED64(TLImageInstance* image, int sidekick, int team);
    bool fn_8021EED8(bool playSound);

    /* 0x01C */ bool mUnidentified1C;
    /* 0x01D */ bool mUnidentified1D;
    /* 0x01E */ bool mUnidentified1E;
    /* 0x01F */ bool mUnidentified1F;
    /* 0x020 */ TU80219248Component mControllerComponents[2];
    /* 0x188 */ TU80219248Component mHomeAwayComponent;
    /* 0x23C */ TU80219248Component mHelpComponent;
    /* 0x2F0 */ TU8022EF84Component mUnidentified2F0;
    /* 0x3C8 */ ScreenMovement mMovement;
    /* 0x3CC */ TLComponentInstance* mSideGroups[2];
    /* 0x3D4 */ TLComponentInstance* mHomeAwayBox;
    /* 0x3D8 */ TLComponentInstance* mHelpButton;
    /* 0x3DC */ eCSContext mContext;
    /* 0x3E0 */ int mPlayingSides[4];
    /* 0x3F0 */ nlColour mUnidentified3F0[2];
    /* 0x3F8 */ int mControllerCounts[4];
    /* 0x408 */ int mUnidentified408;
    /* 0x40C */ int mState;
}; // size 0x410

#endif // GAME_SH_SH_CHOOSE_SIDES_H
