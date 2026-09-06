#ifndef GAME_RENDER_TU_8027AE14_H
#define GAME_RENDER_TU_8027AE14_H

#include "NL/nlMath.h"
#include "unclassified/tu_80188884.h"

class UnidentifiedObject_8027AE14 : public UnidentifiedObject_80188884
{
public:
    UnidentifiedObject_8027AE14(const nlVector3& param1);
    virtual ~UnidentifiedObject_8027AE14();
    virtual void UnidentifiedVirtual0C(float param1);

    /* 0x10 */ nlVector3 mUnidentified010;
    /* 0x1C */ float mUnidentified01C;
    /* 0x20 */ bool mUnidentified020;
}; // size: 0x24

#endif // GAME_RENDER_TU_8027AE14_H
