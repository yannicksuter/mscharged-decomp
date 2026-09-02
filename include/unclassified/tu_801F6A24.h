#ifndef UNCLASSIFIED_TU_801F6A24_H
#define UNCLASSIFIED_TU_801F6A24_H

#include "Game/FE/BaseOverlayHandler.h"

class cFielder;
class nlVector3;
class TLComponentInstance;

class TU801F6A24Overlay : public BaseOverlayHandler
{
public:
    TU801F6A24Overlay();
    virtual ~TU801F6A24Overlay();
    virtual void Update(float dt);
    virtual void SceneCreated();

    void fn_801F6D94(float fParam1);
    void fn_801F6E18(float fParam1);
    void fn_801F6E2C(cFielder* pParam1);
    void fn_801F6E8C(const nlVector3& vParam1);

    /* 0x28 */ TLComponentInstance* mUnidentified28;
    /* 0x2C */ TLComponentInstance* mUnidentified2C;
    /* 0x30 */ cFielder* mUnidentified30;
    /* 0x34 */ bool mUnidentified34;
    /* 0x35 */ bool mUnidentified35;
    /* 0x36 */ bool mUnidentified36;
}; // size 0x38

#endif // UNCLASSIFIED_TU_801F6A24_H
