#ifndef UNCLASSIFIED_TU_80214DF8_H
#define UNCLASSIFIED_TU_80214DF8_H

#include "Game/SH/SHSceneBase.h"
#include "unclassified/tu_8022EF84.h"

class TLComponentInstance;

class TU80214DF8Scene : public BaseSceneHandler
{
public:
    TU80214DF8Scene();
    virtual ~TU80214DF8Scene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void fn_80216048();
    bool fn_80216170(int index, int value);
    void fn_80216808(int index, void* context);
    void fn_80216820(int index, void* context);
    void fn_80216838(int index, void* context);
    void fn_802168A0();
    void fn_80216A90(int index, void* context);

    /* 0x001C */ TLComponentInstance* mUnidentified1C;
    /* 0x0020 */ int mUnidentified20[4];
    /* 0x0030 */ bool mUnidentified30;
    /* 0x0031 */ u8 mUnidentified31[3];
    /* 0x0034 */ int mUnidentified34;
    /* 0x0038 */ int mUnidentified38;
    /* 0x003C */ u8 mUnidentified3C[0x1020];
    /* 0x105C */ int mUnidentified105C;
    /* 0x1060 */ u8 mUnidentified1060[4];
    /* 0x1064 */ TU80219248Component mUnidentified1064[10];
    /* 0x176C */ UnidentifiedScrollWidget mUnidentified176C;
    /* 0x1920 */ TU8022EF84Component mUnidentified1920;
    /* 0x19F8 */ TLComponentInstance* mUnidentified19F8;
    /* 0x19FC */ int mUnidentified19FC;
    /* 0x1A00 */ TLComponentInstance* mUnidentified1A00[10];
    /* 0x1A28 */ TLComponentInstance* mUnidentified1A28;
    /* 0x1A2C */ u8 mUnidentified1A2C[0x18];
    /* 0x1A44 */ bool mUnidentified1A44;
}; // size 0x1A48

#endif // UNCLASSIFIED_TU_80214DF8_H
