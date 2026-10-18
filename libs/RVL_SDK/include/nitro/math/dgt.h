#pragma once

#include <revolution/types.h>

void MATH_CalcSHA1(void* digest, const void* data, u32 dataLength);

#if defined(VERSION_R4QK01)
void MATH_CalcMD5(void* digest, const void* data, u32 dataLength);
#endif
