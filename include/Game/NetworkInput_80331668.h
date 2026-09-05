#ifndef GAME_NETWORK_INPUT_80331668_H
#define GAME_NETWORK_INPUT_80331668_H

#include "types.h"

class DetInput;

struct InputRouterRecord
{
    /* 0x00 */ u16 mUnidentified00;
    /* 0x02 */ u8 mUnidentified02;
    /* 0x03 */ s8 mUnidentified03[3];
    /* 0x06 */ s8 mUnidentified06[3];
    /* 0x09 */ u8 mUnidentified09;
    /* 0x0A */ s8 mUnidentified0A[2];
    /* 0x0C */ s8 mUnidentified0C[4];
}; // size: 0x10

extern "C" void fn_80331668(InputRouterRecord* record, const DetInput* input);
extern "C" void fn_803317E0(const InputRouterRecord* record, DetInput* input);

#endif // GAME_NETWORK_INPUT_80331668_H
