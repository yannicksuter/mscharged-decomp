#ifndef UNCLASSIFIED_TU_8026B10C_H
#define UNCLASSIFIED_TU_8026B10C_H

#include "Game/BaseSceneHandler.h"
#include "Game/NetworkMessages.h"
#include "NL/nlColour.h"
#include "unclassified/tu_80219248.h"
#include "unclassified/tu_8030616C.h"

class TLComponentInstance;
class TLTextInstance;

struct TU8026B10CEntry
{
    TU8026B10CEntry()
        : mUnidentified00(-1)
        , mUnidentified04(false)
    {
    }

    int mUnidentified00;
    bool mUnidentified04;
}; // size 0x8

class TU8026B10CScene : public BaseSceneHandler
{
public:
    TU8026B10CScene();
    virtual ~TU8026B10CScene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void fn_8026C308(UnidentifiedTimer_8030616C* timer);
    void fn_8026C9D4();
    void fn_8026D4D4(int index, void* context);
    void fn_8026D614(int index, void* context);
    void fn_8026D6E0(int index, void* context);
    void fn_8026D85C(int index, void* context);
    void fn_8026D984(int index, void* context);
    void fn_8026DA2C(int index, void* context);
    void fn_8026DAE0(int index, void* context);
    void fn_8026DB5C(int index, void* context);
    void fn_8026DEC0();
    int fn_8026E1AC(int pad);
    void fn_8026E338(NetworkMessageType25_8050B778* message);
    void fn_8026E6C4(int newSide, int oldSide, int index);
    void fn_8026EC64();

    /* 0x01C */ bool mUnidentified01C;
    /* 0x01D */ u8 mPadding01D[3];
    /* 0x020 */ NetMessageDraft mUnidentified020;
    /* 0x434 */ bool mUnidentified434;
    /* 0x435 */ bool mUnidentified435;
    /* 0x436 */ bool mUnidentified436;
    /* 0x437 */ u8 mPadding437;
    /* 0x438 */ TU80219248Component mUnidentified438[2];
    /* 0x5A0 */ TU80219248Component mUnidentified5A0;
    /* 0x654 */ TLComponentInstance* mUnidentified654[2];
    /* 0x65C */ TLComponentInstance* mUnidentified65C;
    /* 0x660 */ TLTextInstance* mUnidentified660;
    /* 0x664 */ int mUnidentified664[4];
    /* 0x674 */ nlColour mUnidentified674[4];
    /* 0x684 */ TU8026B10CEntry mUnidentified684[4];
    /* 0x6A4 */ int mUnidentified6A4[4];
    /* 0x6B4 */ int mUnidentified6B4;
    /* 0x6B8 */ int mUnidentified6B8[2][3];
    /* 0x6D0 */ unsigned short mUnidentified6D0[4][14];
    /* 0x740 */ unsigned short mUnidentified740[8];
    /* 0x750 */ UnidentifiedTimer_8030616C mUnidentified750;
    /* 0x76C */ bool mUnidentified76C;
    /* 0x76D */ u8 mPadding76D[3];
    /* 0x770 */ int mUnidentified770;
}; // size 0x774

#endif // UNCLASSIFIED_TU_8026B10C_H
