#ifndef NL_NLMATH_INL
#define NL_NLMATH_INL

/**
 * Offset/Address/Size: 0x9540 | 0x800D1C3C | size: 0x10
 */
inline void nlVector3::Set(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

/**
 * Offset/Address/Size: 0x9550 | 0x800D1C4C | size: 0x34
 */
extern "C" inline nlVector3* fn_800D1C4C(
    nlVector3* result, const nlVector3* first, const nlVector3* second)
{
    nlVec3Sub(*result, *first, *second);
    return result;
}

/**
 * Offset/Address/Size: 0x9584 | 0x800D1C80 | size: 0x4C
 */
extern "C" inline float fn_800D1C80(
    const nlVector2* first, const nlVector2* second)
{
    nlVector2 delta = {
        first->x - second->x,
        first->y - second->y,
    };
    return nlVec2Length(delta);
}

/**
 * Offset/Address/Size: 0x95D0 | 0x800D1CCC | size: 0x38
 */
extern "C" inline unsigned short fn_800D1CCC(float y, float x)
{
    return (unsigned short)(int)(10430.378f * nlATan2f(y, x));
}

/**
 * Offset/Address/Size: 0x9608 | 0x800D1D04 | size: 0xC
 */
extern "C" inline short fn_800D1D04(
    unsigned short first, unsigned short second)
{
    return nlAngleDiff(first, second);
}

/**
 * Offset/Address/Size: 0x9614 | 0x800D1D10 | size: 0x14
 */
extern "C" inline unsigned short fn_800D1D10(short angle)
{
    return angle < 0 ? -angle : angle;
}

/**
 * Offset/Address/Size: 0x9628 | 0x800D1D24 | size: 0x10
 */
extern "C" inline int fn_800D1D24(int value)
{
    return value < 0 ? -value : value;
}

#endif // NL_NLMATH_INL
