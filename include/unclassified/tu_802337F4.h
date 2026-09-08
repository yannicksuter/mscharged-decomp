#ifndef UNCLASSIFIED_TU_802337F4_H
#define UNCLASSIFIED_TU_802337F4_H

#include "Game/SH/SHSceneBase.h"
#include "Game/FE/feBackButton.h"

class TU802337F4Scene : public UnidentifiedSHSceneBase
{
public:
    TU802337F4Scene();
    virtual ~TU802337F4Scene();
    virtual void Update(float dt);
    virtual void SceneCreated();
    virtual void SHSceneVirtual2C(unsigned int transition);
    virtual void SHSceneVirtual30();
    virtual void SHSceneVirtual34();

    static void fn_80233948();
    static void fn_80233980();
    static void fn_802346F0();

    /* 0x5D4 */ FEBackButton mUnidentified5D4;
    /* 0x6AC */ TLComponentInstance* mUnidentified6AC;
    /* 0x6B0 */ bool mUnidentified6B0;
    /* 0x6B1 */ bool mUnidentified6B1;
}; // size 0x6B4

#endif // UNCLASSIFIED_TU_802337F4_H
