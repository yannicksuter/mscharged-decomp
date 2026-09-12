#ifndef GAME_PACKED_DET_INPUT_H
#define GAME_PACKED_DET_INPUT_H

#include <string.h>

#include "types.h"

class DetInput;

struct PackedDetInput
{
    PackedDetInput()
    {
        memset(this, 0, sizeof(PackedDetInput));
    }

    /* 0x00 */ u16 mButtonBitfield;
    /* 0x02 */ u8 mTriggers;
    /* 0x03 */ s8 mRemoteAccel[3];
    /* 0x06 */ s8 mFreeStyleAccel[3];
    /* 0x09 */ u8 mDPDNumTargets;
    /* 0x0A */ s8 mDPDCoord[2];
    /* 0x0C */ s8 mAnalogAxes[4];
}; // size: 0x10

void PackDetInput(PackedDetInput* record, const DetInput* input);
void UnpackDetInput(const PackedDetInput* record, DetInput* input);

#endif // GAME_PACKED_DET_INPUT_H
