#ifndef UNCLASSIFIED_TU_80264E98_H
#define UNCLASSIFIED_TU_80264E98_H

#include "Game/BaseSceneHandler.h"
#include "unclassified/tu_80219248.h"
#include "unclassified/tu_8022EF84.h"

struct TU802384AC;

class TU80264E98Scene : public BaseSceneHandler
{
public:
    TU80264E98Scene();
    virtual ~TU80264E98Scene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();
    void fn_802654AC();
    void fn_80265670();
    void fn_80266014();
    void fn_80266B68();
    void fn_80266EA0(int index, void* context);
    void fn_80266F64(int index, void* context);
    void fn_80266FEC(int, void* context);
    void fn_802670D4();

    /* 0x01C */ int mUnidentified1C;
    /* 0x020 */ int mUnidentified20[4];
    /* 0x030 */ bool mUnidentified30;
    /* 0x031 */ unsigned char mPadding31[3];
    /* 0x034 */ int mUnidentified34;
    /* 0x038 */ int mUnidentified38;
    /* 0x03C */ bool mUnidentified3C;
    /* 0x03D */ unsigned char mPadding3D[3];
    /* 0x040 */ int mUnidentified40[100];
    /* 0x1D0 */ unsigned short mUnidentified1D0[24];
    /* 0x200 */ TU80219248Component mUnidentified200[10];
    /* 0x908 */ TU8022EF84Component mNavigation;
    /* 0x9E0 */ TU802384AC* mUnidentified9E0;
    /* 0x9E4 */ TLComponentInstance* mUnidentified9E4[10];
}; // size 0xA0C

#endif // UNCLASSIFIED_TU_80264E98_H
