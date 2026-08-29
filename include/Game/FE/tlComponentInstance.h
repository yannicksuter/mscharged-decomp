#ifndef _TLCOMPONENTINSTANCE_H_
#define _TLCOMPONENTINSTANCE_H_

#include "Game/FE/tlComponent.h"
#include "Game/FE/tlInstance.h"

class TLComponentInstance : public TLInstance
{
public:
    void Update(float dt);
    void SetActiveSlide(const char* name, bool arg2, bool arg3);
    void SetActiveSlide(unsigned long hash, bool arg2, bool arg3);
    void SetActiveSlide(TLSlide* slide, bool arg2, bool arg3);
    TLSlide* GetActiveSlide();
};

#endif // _TLCOMPONENTINSTANCE_H_
