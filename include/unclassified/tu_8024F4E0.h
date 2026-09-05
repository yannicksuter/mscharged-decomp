#ifndef UNCLASSIFIED_TU_8024F4E0_H
#define UNCLASSIFIED_TU_8024F4E0_H

#include "Game/BaseSceneHandler.h"
#include "unclassified/tu_80219248.h"
#include "unclassified/tu_8022EF84.h"

class TLComponentInstance;

class TU80250754Scene : public BaseSceneHandler
{
public:
    TU80250754Scene();
    virtual ~TU80250754Scene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void fn_8024F570();
    void fn_8024FDD4(int index, void* context);
    void fn_8024FE6C(int index, void* context);
    void fn_8024FEEC(int index, void* context);
    void fn_8025005C(int index, void* context);
    void fn_80250100(int index, void* context);
    void fn_8025018C();
    void fn_802505E8();
    void fn_80250718();
    void fn_80251328(int index, void* context);

    /* 0x001C */ int mUnidentified001C;
    /* 0x0020 */ bool mUnidentified0020;
    /* 0x0021 */ unsigned char mPadding0021[3];
    /* 0x0024 */ int mUnidentified0024;
    /* 0x0028 */ unsigned short mUnidentified0028[12][2];
    /* 0x0058 */ TU80219248Component mUnidentified0058[12];
    /* 0x08C8 */ TU80219248Component mUnidentified08C8[12];
    /* 0x1138 */ TU8022EF84Component mUnidentified1138;
    /* 0x1210 */ TLComponentInstance* mUnidentified1210[12];
    /* 0x1240 */ TLComponentInstance* mUnidentified1240[12];
    /* 0x1270 */ TLComponentInstance* mUnidentified1270;
    /* 0x1274 */ bool mUnidentified1274;
    /* 0x1275 */ unsigned char mPadding1275[3];
    /* 0x1278 */ int mUnidentified1278;
}; // size 0x127C

#endif // UNCLASSIFIED_TU_8024F4E0_H
