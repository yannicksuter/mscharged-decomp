#include "Game/NetworkRandom_803236CC.h"

#include "NL/nlTicker.h"

static const char lbl_804EB428[] = "bcdfghjklmnprstvwyz";
static const char lbl_806E6920[] = "aeiou";
static u32 lbl_806E20D0;

extern "C" u32 fn_803236CC()
{
    if (lbl_806E20D0 == 0)
    {
        lbl_806E20D0 = nlGetTicker() % 2147483647U;
    }
    lbl_806E20D0 = 48271U * (lbl_806E20D0 % 44488U)
                 - 3399U * (lbl_806E20D0 / 44488U);
    if (lbl_806E20D0 == 0)
    {
        lbl_806E20D0 += 2147483647U;
    }
    return lbl_806E20D0;
}

extern "C" void fn_8032376C(char* buffer, int size)
{
    int length = fn_803236CC() % size;
    if (length < 2)
    {
        length = 2;
    }

    bool consonant = true;
    int i;
    for (i = 0; i < length; ++i)
    {
        if (consonant)
        {
            buffer[i] = lbl_804EB428[fn_803236CC() % 19];
        }
        else
        {
            buffer[i] = lbl_806E6920[fn_803236CC() % 5];
        }
        consonant = !consonant;
    }
    buffer[i] = '\0';
}
