#ifndef UNCLASSIFIED_TU_80203D70_H
#define UNCLASSIFIED_TU_80203D70_H

#include "Game/BaseSceneHandler.h"
#include "unclassified/tu_80219248.h"
#include "unclassified/tu_8022EF84.h"

class TLComponentInstance;
class TLImageInstance;
class TLTextInstance;

class TU80203D70Scene : public BaseSceneHandler
{
public:
    TU80203D70Scene();
    virtual ~TU80203D70Scene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void fn_80204A48(TLImageInstance* image, int captain);
    void fn_80204B20(TLTextInstance* text);
    void fn_80204ECC();
    void fn_80205AD4();
    void fn_80205D00();
    void fn_80206268();
    void fn_802066C8(int index, void* context);
    void fn_8020688C(int index, void* context);
    void fn_802069F0(int index, void* context);

    /* 0x01C */ u16 mDescriptorBuffer[256];
    /* 0x21C */ bool mUnidentified21C;
    /* 0x21D */ bool mUnidentified21D;
    /* 0x21E */ bool mUnidentified21E;
    /* 0x21F */ bool mUnidentified21F;
    /* 0x220 */ int mButtonStates[4];
    /* 0x230 */ int mUnidentified230;
    /* 0x234 */ TU8022EF84Component mNavigationComponent;
    /* 0x30C */ TU80219248Component mButtonComponent0;
    /* 0x3C0 */ TU80219248Component mButtonComponent1;
    /* 0x474 */ TU80219248Component mButtonComponent2;
    /* 0x528 */ TU80219248Component mButtonComponent3;
    /* 0x5DC */ TLComponentInstance* mUnidentified5DC;
    /* 0x5E0 */ TLComponentInstance* mUnidentified5E0;
    /* 0x5E4 */ TLComponentInstance* mUnidentified5E4;
    /* 0x5E8 */ TLComponentInstance* mUnidentified5E8;
    /* 0x5EC */ int mUnidentified5EC;
}; // size 0x5F0

#endif // UNCLASSIFIED_TU_80203D70_H
