#ifndef GAME_SH_SH_CHOOSE_SIDEKICKS_H
#define GAME_SH_SH_CHOOSE_SIDEKICKS_H

#include "Game/SH/SHChooseCaptains.h"
#include "Game/DB/StatsTracker.h"

class AsyncImage;
class TLComponentInstance;
class FETextureResource;

class ChooseSidekicksSceneV2 : public BaseSceneHandler
{
public:
    ChooseSidekicksSceneV2(ChooseCaptainsSceneV2::SceneType sceneType, ScreenMovement movement);
    virtual ~ChooseSidekicksSceneV2();
    virtual void Update(float dt);
    virtual void SceneCreated();

    int GetSide(unsigned long pad);
    eTeamID GetTeam(int side) const
    {
        eTeamID team = (eTeamID)mUnidentified40[side];
        return team;
    }
    void fn_802299C4(int value);
    void fn_8022AB68(int index, void* context);
    void fn_8022ACEC(int index, void* context);
    void fn_8022AEA4(int index, void* context);
    void fn_8022AF84(int index, void* context);
    void fn_8022B234(int index, void* context);
    void fn_8022B400(int index, void* context);
    void fn_8022B494(int index, void* context);
    void fn_8022B52C(int index, void* context);
    void fn_8022B600(int index, void* context);
    void fn_8022B6E8(int index, void* context);
    void fn_8022B7C0(int index, void* context);
    void fn_8022B99C(int index, void* context);
    void fn_8022BA74(int index, void* context);
    void fn_8022BDE4(int index, void* context);
    void fn_8022B8B4(int index, void* context);
    void fn_8022BE94(int index, void* context);
    void fn_8022BF20(int index, void* context);
    void fn_8022BFEC();
    void fn_8022C720();
    void fn_8022DB70();
    void fn_8022D5F8();
    void fn_8022D8A4();
    void fn_8022DA2C();
    void fn_8022DC68();
    void fn_8022DDB0();
    void fn_8022E258(int firstCaptain, int secondCaptain);
    void fn_8022EACC();
    bool fn_8022EBB8();
    void fn_8022ED20();
    void ShowDisconnectedError();
    void ReleaseController(int index);

    /* 0x001C */ bool mUnidentified1C;
    /* 0x001D */ u8 mPadding1D[3];
    /* 0x0020 */ int mUnidentified20[2];
    /* 0x0028 */ int mUnidentified28[2];
    /* 0x0030 */ int mUnidentified30[2];
    /* 0x0038 */ ScreenMovement mMovement;
    /* 0x003C */ ChooseCaptainsSceneV2::SceneType mSceneType;
    /* 0x0040 */ int mUnidentified40[2];
    /* 0x0048 */ bool mUnidentified48[2];
    /* 0x004A */ bool mUnidentified4A;
    /* 0x004B */ bool mUnidentified4B;
    /* 0x004C */ bool mUnidentified4C;
    /* 0x004D */ bool mUnidentified4D;
    /* 0x004E */ u8 mPadding4E[2];
    /* 0x0050 */ TU801DA134Component mUnidentified50[2];
    /* 0x00A0 */ bool mUnidentifiedA0[2][3];
    /* 0x00A6 */ u8 mPaddingA6[2];
    /* 0x00A8 */ FETextureResource* mUnidentifiedA8[8][2];
    /* 0x00E8 */ FEPointerButton mUnidentifiedE8[8];
    /* 0x0688 */ FEPointerButton mUnidentified688[2][3];
    /* 0x0AC0 */ FEPointerButton mUnidentifiedAC0[2][3];
    /* 0x0EF8 */ FEPointerButton mUnidentifiedEF8[2];
    /* 0x1060 */ FEPointerButton mUnidentified1060[2];
    /* 0x11C8 */ FEPointerButton mUnidentified11C8;
    /* 0x127C */ FEBackButton mBackButton;
    /* 0x1354 */ TU801DCD9CComponent mCaptainComponents[2];
    /* 0x18AC */ TLComponentInstance* mUnidentified18AC[8];
    /* 0x18CC */ TLComponentInstance* mUnidentified18CC[2][3];
    /* 0x18E4 */ TLComponentInstance* mUnidentified18E4[2][3];
    /* 0x18FC */ TLComponentInstance* mUnidentified18FC[2];
    /* 0x1904 */ TLComponentInstance* mUnidentified1904[2];
    /* 0x190C */ TLInstance* mUnidentified190C[2];
    /* 0x1914 */ TLComponentInstance* mUnidentified1914;
    /* 0x1918 */ TLComponentInstance* mUnidentified1918;
    /* 0x191C */ u8 mUnidentified191C[0x20];
    /* 0x193C */ int mUnidentified193C;
    /* 0x1940 */ unsigned short mTimerText[8];
    /* 0x1950 */ bool mUnidentified1950;
    /* 0x1951 */ u8 mPadding1951[3];
    /* 0x1954 */ int mUnidentified1954;
    /* 0x1958 */ AsyncImage* mUnidentified1958[2][8];
    /* 0x1998 */ AsyncImage* mUnidentified1998[2][8];
    /* 0x19D8 */ bool mUnidentified19D8[2][8];
    /* 0x19E8 */ bool mUnidentified19E8[2][8];
    /* 0x19F8 */ bool mUnidentified19F8;
    /* 0x19F9 */ u8 mPadding19F9[3];
}; // size 0x19FC

#endif // GAME_SH_SH_CHOOSE_SIDEKICKS_H
