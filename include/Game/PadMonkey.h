#ifndef GAME_PAD_MONKEY_H
#define GAME_PAD_MONKEY_H

#include "NL/platpad.h"
#include "types.h"

extern "C" int fn_802C06C8(int buttonIndex);

class PadMonkey : public PadBackend
{
public:
    PadMonkey(int padIndex);

    virtual int GetButtonMask(int buttonIndex);

    void SetButtonChance(int button, float pct);

    /* 0x1C */ u8 mUnidentified01C[0x18];
    /* 0x34 */ float m_unk_0x34;
    /* 0x38 */ u8 mUnidentified038[0xC4];
}; // size 0xFC

#endif // GAME_PAD_MONKEY_H
