#ifndef GAME_SANIM_DECODE_8030EC30_H
#define GAME_SANIM_DECODE_8030EC30_H

#include "NL/nlMath.h"

struct PackedScale;

void SAnimInitGQR();
extern "C" void fn_8030EC4C(nlQuaternion* result, const void* packed);
extern "C" void fn_8030EC60(nlQuaternion* result, const void* packed);
extern "C" void fn_8030ECC8(nlQuaternion* result, const void* packed);
extern "C" void fn_8030ECDC(nlVector3* result, const PackedScale* packed);
extern "C" void fn_8030ED48(float* result, const unsigned char* packed);
extern "C" void fn_8030ED7C(float* result, const unsigned char* packed);

#endif // GAME_SANIM_DECODE_8030EC30_H
