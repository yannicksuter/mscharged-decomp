#ifndef UNCLASSIFIED_TU_8022EF84_H
#define UNCLASSIFIED_TU_8022EF84_H

#include "Game/FE/feLibObject.h"
#include "unclassified/tu_80219248.h"

class TLComponentInstance;

class TU8022EF84Component : public TU80219248Component
{
public:
    TU8022EF84Component();
    virtual ~TU8022EF84Component();

    virtual void fn_80301BA8(int index, void* context);
    virtual void fn_80301CE8(int index, void* context);
    virtual void fn_80301C28(int index, void* context);
    virtual void fn_80301C68(int index, void* context);
    virtual void fn_80301D28(int index, void* context);
    virtual void fn_801E6CB4(int value);
    virtual void fn_8022F194(TLComponentInstance* instance);
    virtual bool fn_8022F2E0(TU80300104Event event, float fDeltaT);
    virtual void fn_801D2BE8(bool value) { mUnidentifiedCF = value; }
    virtual void fn_801D2BE0(bool value) { mUnidentifiedD0 = value; }

    /* 0x0B4 */ int mUnidentifiedB4;
    /* 0x0B8 */ float mUnidentifiedB8;
    /* 0x0BC */ feVector3 mUnidentifiedBC;
    /* 0x0C8 */ TLComponentInstance* mUnidentifiedC8;
    /* 0x0CC */ bool mUnidentifiedCC;
    /* 0x0CD */ bool mUnidentifiedCD;
    /* 0x0CE */ bool mUnidentifiedCE;
    /* 0x0CF */ bool mUnidentifiedCF;
    /* 0x0D0 */ bool mUnidentifiedD0;
    /* 0x0D1 */ bool mUnidentifiedD1;
    /* 0x0D2 */ bool mUnidentifiedD2[4];
}; // size 0xD8

#endif // UNCLASSIFIED_TU_8022EF84_H
