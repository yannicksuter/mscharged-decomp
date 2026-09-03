#ifndef UNCLASSIFIED_TU_802126F8_H
#define UNCLASSIFIED_TU_802126F8_H

#include "Game/SH/SHSceneBase.h"
#include "unclassified/tu_80219248.h"
#include "unclassified/tu_8022EF84.h"

class TLComponentInstance;

class TU802126F8Scene : public BaseSceneHandler
{
public:
    TU802126F8Scene(int mode);
    virtual ~TU802126F8Scene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void fn_8021292C();
    void fn_802129EC();
    void fn_80212F2C(int index, void* context);
    void fn_80212FF8(int index, void* context);
    void fn_80213014(int index, void* context);
    bool fn_80213D48(int index, int teamIndex);
    void fn_802140C4(int index, void* context);
    void fn_80214168(int index, void* context);

    /* 0x01C */ TLComponentInstance* mUnidentified1C[4];
    /* 0x02C */ int mUnidentified2C;
    /* 0x030 */ unsigned char mUnidentified30[0x28];
    /* 0x058 */ int mMode;
    /* 0x05C */ unsigned char mUnidentified5C[0xA0];
    /* 0x0FC */ bool mUnidentifiedFC;
    /* 0x0FD */ bool mUnidentifiedFD;
    /* 0x0FE */ bool mUnidentifiedFE;
    /* 0x0FF */ bool mUnidentifiedFF;
    /* 0x100 */ int mUnidentified100[4];
    /* 0x110 */ int mUnidentified110;
    /* 0x114 */ UnidentifiedScrollWidget mUnidentified114;
    /* 0x2C8 */ TU8022EF84Component mUnidentified2C8;
    /* 0x3A0 */ TLComponentInstance* mUnidentified3A0;
    /* 0x3A4 */ TU80219248Component mUnidentified3A4;
    /* 0x458 */ TU80219248Component mUnidentified458[4];
    /* 0x728 */ UnidentifiedTextFader mUnidentified728;
    /* 0x768 */ TLComponentInstance* mUnidentified768;
    /* 0x76C */ TLComponentInstance* mUnidentified76C;
    /* 0x770 */ int mUnidentified770;
}; // size 0x774

#endif // UNCLASSIFIED_TU_802126F8_H
