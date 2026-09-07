#ifndef UNCLASSIFIED_TU_80261DF0_H
#define UNCLASSIFIED_TU_80261DF0_H

#include "Game/BaseSceneHandler.h"
#include "unclassified/tu_80219248.h"

class TLComponentInstance;

class TU80261DF0Scene : public BaseSceneHandler
{
public:
    TU80261DF0Scene();
    virtual ~TU80261DF0Scene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();
    void fn_802619A8();
    void fn_80262614(int index, void* context);
    void fn_802626D8(int index, void* context);
    void fn_80262784(int index, void* context);
    void fn_80262844();

    /* 0x01C */ TU80219248Component mUnidentified1C[4];
    /* 0x2EC */ TLComponentInstance* mUnidentified2EC[4];
    /* 0x2FC */ unsigned short mUnidentified2FC[0x80];
    /* 0x3FC */ int mUnidentified3FC[4];
    /* 0x40C */ int mUnidentified40C;
    /* 0x410 */ int mUnidentified410;
    /* 0x414 */ int mUnidentified414;
}; // size 0x418

extern "C" void fn_80261CE0();

#endif // UNCLASSIFIED_TU_80261DF0_H
