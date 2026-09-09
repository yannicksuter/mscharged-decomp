#ifndef GAME_SH_SH_CHOOSE_CAPTAINS_H
#define GAME_SH_SH_CHOOSE_CAPTAINS_H

#include "Game/BaseGameSceneManager.h"
#include "Game/FE/feBackButton.h"
#include "Game/FE/feCaptainComponent.h"
#include "Game/FE/fePointerButton.h"

class TLComponentInstance;
class TLInstance;

class ChooseCaptainsSceneV2 : public BaseSceneHandler
{
public:
    enum SceneType
    {
        ST_DOMINATION = 0,
        ST_STRIKER_CUP = 1,
    };

    ChooseCaptainsSceneV2(SceneType sceneType, ScreenMovement movement);
    virtual ~ChooseCaptainsSceneV2();
    virtual void Update(float dt);
    virtual void SceneCreated();

    int GetSide(unsigned long pad);
    void fn_80223B98(int value);
    void fn_80224CE4(int index, void* context);
    void fn_80224D30(int index, unsigned long which);
    void fn_80224F78(int index, void* context);
    void fn_80225484(int index, void* context);
    void fn_80225674(int index, void* context);
    void fn_802257CC(int index, void* context);
    void fn_802258B8(int index, void* context);
    void fn_802259AC(int index, void* context);
    void fn_80225AA0(int index, void* context);
    void fn_80225BD8(int index, void* context);
    void fn_80227BCC(int which);
    void fn_80228140();
    void fn_80225EE4(int index, void* context);
    void fn_8022559C(int index, void* context);
    void fn_80225DA8(int index, void* context);
    void fn_80225E58(int index, void* context);

    /* 0x001C */ bool mUnidentified1C;
    /* 0x001D */ u8 mPadding1D[3];
    /* 0x0020 */ SceneType mSceneType;
    /* 0x0024 */ ScreenMovement mMovement;
    /* 0x0028 */ int mUnidentified28[2];
    /* 0x0030 */ int mUnidentified30[2];
    /* 0x0038 */ bool mUnidentified38[2];
    /* 0x003A */ bool mUnidentified3A[2];
    /* 0x003C */ bool mUnidentified3C[2];
    /* 0x003E */ u8 mPadding3E[2];
    /* 0x0040 */ int mUnidentified40[2];
    /* 0x0048 */ bool mUnidentified48[2];
    /* 0x004A */ bool mUnidentified4A;
    /* 0x004B */ bool mUnidentified4B;
    /* 0x004C */ bool mUnidentified4C;
    /* 0x004D */ bool mUnidentified4D;
    /* 0x004E */ bool mUnidentified4E;
    /* 0x004F */ u8 mPadding4F;
    /* 0x0050 */ int mUnidentified50;
    /* 0x0054 */ int mUnidentified54;
    /* 0x0058 */ u8 mUnidentified58[0x58];
    /* 0x00B0 */ FEPointerButton mCaptainButtons[12];
    /* 0x0920 */ FEPointerButton mUnidentified920[2];
    /* 0x0A88 */ FEPointerButton mUnidentifiedA88[2];
    /* 0x0BF0 */ FEPointerButton mUnidentifiedBF0;
    /* 0x0CA4 */ FEBackButton mBackButton;
    /* 0x0D7C */ TU801DCD9CComponent mCaptainComponents[2];
    /* 0x12D4 */ TLComponentInstance* mUnidentified12D4[12];
    /* 0x1304 */ TLComponentInstance* mUnidentified1304[2];
    /* 0x130C */ TLComponentInstance* mUnidentified130C[2];
    /* 0x1314 */ TLComponentInstance* mUnidentified1314;
    /* 0x1318 */ int mUnidentified1318;
    /* 0x131C */ int mUnidentified131C;
    /* 0x1320 */ TLComponentInstance* mUnidentified1320;
    /* 0x1324 */ TLComponentInstance* mUnidentified1324;
    /* 0x1328 */ TLComponentInstance* mUnidentified1328;
    /* 0x132C */ TLComponentInstance* mUnidentified132C[2];
    /* 0x1334 */ TLComponentInstance* mUnidentified1334;
    /* 0x1338 */ TLInstance* mUnidentified1338;
    /* 0x133C */ u8 mUnidentified133C[0x2C];
    /* 0x1368 */ int mUnidentified1368;
    /* 0x136C */ unsigned short mTimerText[8];
    /* 0x137C */ bool mUnidentified137C;
    /* 0x137D */ u8 mPadding137D[3];
    /* 0x1380 */ int mUnidentified1380;
}; // size 0x1384

bool fn_80223D08(int pad);

#endif // GAME_SH_SH_CHOOSE_CAPTAINS_H
