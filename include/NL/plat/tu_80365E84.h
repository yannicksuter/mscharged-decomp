#ifndef NL_PLAT_TU_80365E84_H
#define NL_PLAT_TU_80365E84_H

#include "Game/Event.h"

class Class_80365E84 : public UnidentifiedStaticEvent<void(int), 5>
{
public:
    Class_80365E84()
        : UnidentifiedStaticEvent<void(int), 5>("SwappablePadChanged", -1)
    {
    }

    virtual ~Class_80365E84() { }
}; // size 0xA4

extern Class_80365E84 lbl_8058936C;
extern bool lbl_806DFA48;
extern bool lbl_806DFA49;
extern bool lbl_806DFA4A;

#endif // NL_PLAT_TU_80365E84_H
