#ifndef GAME_EXCITEMENT_SYSTEM_H
#define GAME_EXCITEMENT_SYSTEM_H

#include "Game/InterpreterCore.h"

struct UnidentifiedEventData07;
struct UnidentifiedEventData_80066748;
struct UnidentifiedEventData_80067110;

class ExcitementSystem : public InterpreterCore
{
public:
    static ExcitementSystem& fn_80196644();

    void fn_801967DC();
    void fn_80196924();
    void fn_80196D30(UnidentifiedEventData_80066748*);
    void fn_80196D64(UnidentifiedEventData_80067110*);
    void fn_80196D8C(UnidentifiedEventData07*);
    virtual void DoFunctionCall(unsigned int);

    /* 0x028 */ float mUnidentified028;
    /* 0x02C */ u16 mUnidentified02C;
    /* 0x02E */ u16 mUnidentified02E;
    /* 0x030 */ u8 mUnidentified030[130];
    /* 0x0B2 */ u8 mUnidentified0B2[178];
    /* 0x164 */ u8 mUnidentified164[4];
    /* 0x168 */ void* mByteCode;

private:
    ExcitementSystem();
}; // total size: 0x16C

#endif // GAME_EXCITEMENT_SYSTEM_H
