#ifndef UNCLASSIFIED_TU_80259B88_H
#define UNCLASSIFIED_TU_80259B88_H

#include "Game/BaseSceneHandler.h"
#include "unclassified/tu_80219248.h"
#include "unclassified/tu_8022EF84.h"
#include "unclassified/tu_8030616C.h"

class TLComponentInstance;

class TU80259B88Scene : public BaseSceneHandler
{
public:
    TU80259B88Scene();
    virtual ~TU80259B88Scene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void fn_80259C04();
    void fn_80259DB4(int index, void*);
    void fn_80259E94(int index, void*);
    void fn_80259F54(int index, void*);
    void fn_8025A1A8(int index, void*);
    void fn_8025A244(int index, void*);
    void fn_8025A2EC(int, void*);
    void fn_8025B204();
    void fn_8025BC24(int index, void*);

    /* 0x01C */ bool mUnidentified1C;
    /* 0x01D */ unsigned char mPadding1D[3];
    /* 0x020 */ int mUnidentified20;
    /* 0x024 */ TU8022EF84Component mNavigation;
    /* 0x0FC */ TU80219248Component mUnidentifiedFC;
    /* 0x1B0 */ TU80219248Component mUnidentified1B0;
    /* 0x264 */ TLComponentInstance* mUnidentified264;
    /* 0x268 */ TLComponentInstance* mUnidentified268;
    /* 0x26C */ unsigned short mUnidentified26C[11];
    /* 0x282 */ unsigned char mPadding282[2];
    /* 0x284 */ int mUnidentified284;
    /* 0x288 */ int mUnidentified288;
    /* 0x28C */ int mUnidentified28C[4];
    /* 0x29C */ bool mUnidentified29C;
    /* 0x29D */ bool mUnidentified29D;
    /* 0x29E */ bool mUnidentified29E;
    /* 0x29F */ unsigned char mPadding29F;
}; // size 0x2A0

class TU8025BE74Scene : public BaseSceneHandler
{
public:
    TU8025BE74Scene();
    virtual ~TU8025BE74Scene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void fn_8025C084();
    void fn_8025CFC0(int index, void* context);
    void fn_8025D04C(int index, void* context);
    void fn_8025D0C0(int index, void* context);

    /* 0x01C */ TU80219248Component mUnidentified1C;
    /* 0x0D0 */ TLComponentInstance* mUnidentifiedD0;
    /* 0x0D4 */ bool mUnidentifiedD4;
    /* 0x0D5 */ u8 mPaddingD5[0x301];
    /* 0x3D6 */ bool mUnidentified3D6;
    /* 0x3D7 */ u8 mPadding3D7;
    /* 0x3D8 */ UnidentifiedTimer_8030616C mUnidentified3D8;
    /* 0x3F4 */ int mUnidentified3F4;
}; // size 0x3F8

#endif // UNCLASSIFIED_TU_80259B88_H
