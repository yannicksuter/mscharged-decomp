#ifndef GAME_NETWORK_SESSION_H
#define GAME_NETWORK_SESSION_H

#include "types.h"

class UnidentifiedNetworkSession
{
public:
    /* 0x0000 */ u8 mUnidentified000[0x242C];

    virtual void UnidentifiedVirtual00();
    virtual void UnidentifiedVirtual04();
    virtual void UnidentifiedVirtual08();
    virtual void UnidentifiedVirtual0C();
    virtual void UnidentifiedVirtual10();
    virtual void UnidentifiedVirtual14();
    virtual void UnidentifiedVirtual18();
    virtual void UnidentifiedVirtual1C();
    virtual void UnidentifiedVirtual20();
    virtual void Update();
    virtual void UnidentifiedVirtual28();
    virtual void UnidentifiedVirtual2C();
    virtual void UnidentifiedVirtual30();
    virtual void UnidentifiedVirtual34();
    virtual void UnidentifiedVirtual38();
    virtual void UnidentifiedVirtual3C();
    virtual void UnidentifiedVirtual40();
    virtual void Send(s8 player, void* buffer, int size, bool reliable);
    virtual void UnidentifiedVirtual48();
}; // size: 0x2430

extern UnidentifiedNetworkSession* lbl_806E20D8;

extern "C" int fn_80338BF0(UnidentifiedNetworkSession* session);
extern "C" s8 fn_80338C20(UnidentifiedNetworkSession* session);

#endif // GAME_NETWORK_SESSION_H
