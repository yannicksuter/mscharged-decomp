#include "NL/plat/WiiPad.h"
#include "NL/nlMath.h"

#include <math.h>

void ClampWiiStick(signed char* px, signed char* py)
{
    if (*px < 15 && *px > -15)
        *px = 0;
    else if (*px > 0)
        *px -= 15;
    else
        *px += 15;

    if (*py < 15 && *py > -15)
        *py = 0;
    else if (*py > 0)
        *py -= 15;
    else
        *py += 15;

    int squared = *px * *px + *py * *py;
    if (squared > 56 * 56)
    {
        float scale = 56.0f / nlSqrt((float)squared, true);
        *px = (signed char)(*px * scale);
        *py = (signed char)(*py * scale);
    }
}

int GetWiiButtonIndex(int button)
{
    switch (button)
    {
    case 0x8000: return 0;
    case 0x1000: return 1;
    case 0x0800: return 2;
    case 0x0400: return 3;
    case 0x0200: return 4;
    case 0x0100: return 5;
    case 0x0010: return 6;
    case 0x0008: return 7;
    case 0x0004: return 8;
    case 0x0002: return 9;
    case 0x0001: return 10;
    case 0x2000: return 11;
    case 0x4000: return 12;
    default: return button;
    }
}

int GetWiiButtonMask(int buttonIndex)
{
    switch (buttonIndex)
    {
    case 0: return 0x8000;
    case 1: return 0x1000;
    case 2: return 0x0800;
    case 3: return 0x0400;
    case 4: return 0x0200;
    case 5: return 0x0100;
    case 6: return 0x0010;
    case 7: return 0x0008;
    case 8: return 0x0004;
    case 9: return 0x0002;
    case 10: return 0x0001;
    case 11: return 0x2000;
    case 12: return 0x4000;
    default: return buttonIndex;
    }
}

unsigned short MapWiiStickToDPad(float normalizedX, float normalizedY,
    unsigned short left, unsigned short right, unsigned short down, unsigned short up)
{
    unsigned short button = 0;
    if (fabsf(normalizedX) >= 0.6f || fabsf(normalizedY) >= 0.6f)
    {
        normalizedX = fabsf(normalizedX) >= 0.6f ? normalizedX : 0.0f;
        normalizedY = fabsf(normalizedY) >= 0.6f ? normalizedY : 0.0f;
        float angle = nlATan2f(normalizedY, normalizedX);
        unsigned short angleU16 = (unsigned short)(int)(angle * 10430.378f);
        float degrees = (float)angleU16 * 0.005493164f;
        int roundedDeg = (int)degrees;
        roundedDeg = (roundedDeg / 45) * 45;
        switch (roundedDeg)
        {
        case 0: button = right; break;
        case 45: button = right | up; break;
        case 90: button = up; break;
        case 135: button = left | up; break;
        case 180: button = left; break;
        case 225: button = left | down; break;
        case 270: button = down; break;
        case 315: button = right | down; break;
        }
    }
    return button;
}
