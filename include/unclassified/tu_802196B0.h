#ifndef UNCLASSIFIED_TU_802196B0_H
#define UNCLASSIFIED_TU_802196B0_H

#include "NL/nlColour.h"
#include "NL/nlMath.h"
#include "NL/nlTask.h"

class UnidentifiedTask_802196B0 : public nlTask
{
public:
    UnidentifiedTask_802196B0();
    ~UnidentifiedTask_802196B0();

    virtual void Run(float dt);
    virtual const char* GetName() { return "FE DPD"; }
};

extern "C"
{
    nlVector2 fn_802197FC(int pad, u8* valid);
    nlVector2 fn_80219824(int pad, u16* angle, u8* valid);
    void fn_80219E08(int index, nlColour colour);
    bool fn_80219E0C(int index);
}

#endif // UNCLASSIFIED_TU_802196B0_H
