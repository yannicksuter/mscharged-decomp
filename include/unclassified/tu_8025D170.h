#ifndef UNCLASSIFIED_TU_8025D170_H
#define UNCLASSIFIED_TU_8025D170_H

#include "Game/BaseSceneHandler.h"

class TU8025D170Scene : public BaseSceneHandler
{
public:
    TU8025D170Scene();
    virtual ~TU8025D170Scene();
    virtual void Update(float dt);
    virtual void SceneCreated();

    /* 0x01C */ bool mUnidentified01C;
    /* 0x01D */ bool mUnidentified01D;
    /* 0x01E */ unsigned char mUnidentified01E[0xE5C - 0x1E];
}; // size 0xE5C

#endif // UNCLASSIFIED_TU_8025D170_H
