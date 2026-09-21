#ifndef GAME_FE_FEDPD_H
#define GAME_FE_FEDPD_H

#include "NL/nlColour.h"
#include "NL/nlMath.h"
#include "NL/nlTask.h"

class TLComponentInstance;

extern TLComponentInstance* gFEPointerInstances[4];
extern nlVector2 gFEPointerPositions[4];
extern bool gFEPointerEnabled[4];
extern bool gDisableRemotePointer;

inline TLComponentInstance* GetPointerInstance(unsigned int index)
{
    return gFEPointerInstances[index];
}

class FEDPDTask : public nlTask
{
public:
    FEDPDTask();
    ~FEDPDTask();

    virtual void Run(float dt);
    virtual const char* GetName() { return "FE DPD"; }
};

void SetPointerInstance(unsigned int index, TLComponentInstance* component);
nlVector2 GetPointerPosition(int pad, u8* valid);
nlVector2 GetPointerPosition(int pad, u16* angle, u8* valid);
void SetPointerColour(int index, nlColour colour);
bool IsFreeStylePad(int index);

#endif // GAME_FE_FEDPD_H
