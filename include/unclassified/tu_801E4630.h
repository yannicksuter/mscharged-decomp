#ifndef UNCLASSIFIED_TU_801E4630_H
#define UNCLASSIFIED_TU_801E4630_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/fePointerButton.h"

class TLComponentInstance;

class TU801E4630Scene : public BaseSceneHandler
{
public:
    TU801E4630Scene();
    virtual ~TU801E4630Scene();
    virtual void Update(float deltaTime);
    virtual void SceneCreated();

    void fn_801E6504(int index, void* context);
    void fn_801E6578(int index, void* context);

    /* 0x01C */ u8 mUnidentified01C[0x99];
    /* 0x0B5 */ bool mUnidentifiedB5;
    /* 0x0B6 */ u8 mUnidentifiedB6[0x5A];
    /* 0x110 */ FEPointerButton mUnidentified110;
    /* 0x1C4 */ u8 mUnidentified1C4[0x288];
    /* 0x44C */ TLComponentInstance* mUnidentified44C;
    /* 0x450 */ u8 mUnidentified450[4];
}; // size 0x454

#endif // UNCLASSIFIED_TU_801E4630_H
