#ifndef UNCLASSIFIED_TU_802141FC_H
#define UNCLASSIFIED_TU_802141FC_H

#include "Game/SH/SHSceneBase.h"

class TU802141FCScene : public UnidentifiedSHSceneBase
{
public:
    TU802141FCScene();
    virtual ~TU802141FCScene();
    virtual void Update(float dt);
    virtual void SceneCreated();
    virtual void SHSceneVirtual2C(unsigned int transition);
    virtual void SHSceneVirtual30();

    /* 0x5D4 */ bool mUnidentified5D4;
    /* 0x5D5 */ bool mUnidentified5D5;
}; // size 0x5D8

#endif // UNCLASSIFIED_TU_802141FC_H
