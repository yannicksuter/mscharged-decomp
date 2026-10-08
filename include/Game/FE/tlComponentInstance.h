#ifndef _TLCOMPONENTINSTANCE_H_
#define _TLCOMPONENTINSTANCE_H_

#include "Game/FE/tlComponent.h"
#include "Game/FE/tlInstance.h"

class TLComponentInstance : public TLInstance
{
public:
    TLComponentInstance(FELibObject* component)
        : TLInstance(component)
    {
        m_type = TLAT_COMPONENT;
    }

    void Update(float dt);
    void SetActiveSlide(const char* name, bool forceRestart, bool preserveTime);
    void SetActiveSlide(unsigned long hash, bool forceRestart, bool preserveTime);
    void SetActiveSlide(TLSlide* slide, bool forceRestart, bool preserveTime);
    TLSlide* GetActiveSlide();
};


#endif // _TLCOMPONENTINSTANCE_H_
