#ifndef GAME_SANIM_DECODE_H
#define GAME_SANIM_DECODE_H

#include "NL/nlMath.h"

struct PackedScale;

void SAnimInitGQR();
extern "C" void SAnimDecodeRot16(nlQuaternion* result, const void* packed);
extern "C" void SAnimDecodeRot12(nlQuaternion* result, const void* packed);
extern "C" void SAnimDecodeRot8(nlQuaternion* result, const void* packed);
extern "C" void SAnimDecodeScale(nlVector3* result, const PackedScale* packed);
extern "C" void SAnimDecodeWeight(float* result, const unsigned char* packed);
extern "C" void SAnimDecodeMorphWeight(float* result, const unsigned char* packed);

#endif // GAME_SANIM_DECODE_H
