#include "NL/platpad.h"

int gNextPadClassID;

void PadBackend::Update(float deltaTime)
{
    float x;
    float y;

    x = this->AnalogLeftX();
    y = this->AnalogLeftY();
    m_polarAnalogLeft.r = nlSqrt((x * x) + (y * y), 1);

    if ((0.f != x) || (0.f != y))
    {
        m_polarAnalogLeft.a = (u16)(10430.378f * nlATan2f(y, x));
    }

    x = this->AnalogRightX();
    y = this->AnalogRightY();
    m_polarAnalogRight.r = nlSqrt((x * x) + (y * y), 1);
    if ((0.f != x) || (0.f != y))
    {
        m_polarAnalogRight.a = (u16)(10430.378f * nlATan2f(y, x));
    }
}

int GetPadButtonIndex(int button)
{
    switch (button)
    {
    case 0x00000001:
        return 0;
    case 0x00000002:
        return 1;
    case 0x00000004:
        return 2;
    case 0x00000008:
        return 3;
    case 0x00000010:
        return 4;
    case 0x00000020:
        return 5;
    case 0x00000040:
        return 6;
    case 0x00000080:
        return 7;
    case 0x00000100:
        return 8;
    case 0x00000200:
        return 9;
    case 0x00000400:
        return 10;
    case 0x00000800:
        return 11;
    case 0x00001000:
        return 12;
    case 0x00002000:
        return 13;
    case 0x00004000:
        return 14;
    case 0x00008000:
        return 15;
    case 0x00010000:
        return 16;
    case 0x00020000:
        return 17;
    case 0x00040000:
        return 18;
    case 0x00080000:
        return 19;
    case 0x00100000:
        return 20;
    case 0x00200000:
        return 21;
    case 0x00400000:
        return 22;
    case 0x00800000:
        return 23;
    case 0x01000000:
        return 24;
    case 0x02000000:
        return 25;
    case 0x04000000:
        return 26;
    case 0x08000000:
        return 27;
    case 0x10000000:
        return 28;
    case 0x20000000:
        return 29;
    case 0x40000000:
        return 30;
    case (int)0x80000000:
        return 31;
    default:
        return 0;
    }
}

int GetPadButtonMask(int buttonIndex)
{
    return 1 << buttonIndex;
}
