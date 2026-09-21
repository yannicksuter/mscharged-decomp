#include "Game/SAnimDecode.h"

#include "Game/SAnim.h"

// clang-format off
void SAnimInitGQR()
{
    register unsigned int value = 0x0F070F07;
    asm {
        mtspr 918, value
    }
    value = 0x07060706;
    asm {
        mtspr 919, value
    }
}
// clang-format on

// clang-format off
extern "C" void SAnimDecodeRot16(register nlQuaternion* result, register const void* packed)
{
    asm {
        psq_l f0, 0(packed), 0, 6
        psq_st f0, 0(result), 0, 0
        psq_l f0, 4(packed), 0, 6
        psq_st f0, 8(result), 0, 0
    }
}
// clang-format on

// clang-format off
extern "C" void SAnimDecodeRot12(register nlQuaternion* result, const void* packed)
{
    const unsigned char* bytes = (const unsigned char*)packed;
    unsigned char unpacked[8];
    unpacked[0] = bytes[0];
    unpacked[3] = unpacked[1] = bytes[1];
    unpacked[1] &= 0xF0;
    unpacked[2] = bytes[2];
    unpacked[3] <<= 4;
    unpacked[4] = bytes[3];
    unpacked[7] = unpacked[5] = bytes[4];
    unpacked[5] &= 0xF0;
    unpacked[6] = bytes[5];
    unpacked[7] <<= 4;
    register unsigned char* values = unpacked;
    asm {
        psq_l f0, 0(values), 0, 6
        psq_st f0, 0(result), 0, 0
        psq_l f0, 4(values), 0, 6
        psq_st f0, 8(result), 0, 0
    }
}
// clang-format on

// clang-format off
extern "C" void SAnimDecodeRot8(register nlQuaternion* result, register const void* packed)
{
    asm {
        psq_l f0, 0(packed), 0, 7
        psq_st f0, 0(result), 0, 0
        psq_l f0, 2(packed), 0, 7
        psq_st f0, 8(result), 0, 0
    }
}
// clang-format on

extern "C" void SAnimDecodeScale(nlVector3* result, const PackedScale* packed)
{
    result->x = 0.00048828125f * packed->x;
    result->y = 0.00048828125f * packed->y;
    result->z = 0.00048828125f * packed->z;
}

extern "C" void SAnimDecodeWeight(float* result, const unsigned char* packed)
{
    *result = *packed / 255.0f;
}

extern "C" void SAnimDecodeMorphWeight(float* result, const unsigned char* packed)
{
    *result = *packed / 255.0f;
}
