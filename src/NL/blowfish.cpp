// blowfish.cpp   C++ class implementation of the BLOWFISH encryption algorithm
// _THE BLOWFISH ENCRYPTION ALGORITHM_
// by Bruce Schneier
// Revised code--3/20/94
// Converted to C++ class 5/96, Jim Conger

#include "NL/blowfish.h"
#include "NL/nlMemory.h"

#include "NL/blowfish.h2"

#define S(x, i)        (SBoxes[i][x.w.byte##i])
#define bf_F(x)        (((S(x, 0) + S(x, 1)) ^ S(x, 2)) + S(x, 3))
#define ROUND(a, b, n) (a.dword ^= bf_F(b) ^ PArray[n])

CBlowFish::CBlowFish()
{
    PArray = new (8, false) DWORD[18];
    SBoxes = new (8, false) DWORD[4][256];
}

void CBlowFish::Blowfish_encipher(DWORD* xl, DWORD* xr)
{
    union aword Xl, Xr;

    Xl.dword = *xl;
    Xr.dword = *xr;

    Xl.dword ^= PArray[0];
    ROUND(Xr, Xl, 1);
    ROUND(Xl, Xr, 2);
    ROUND(Xr, Xl, 3);
    ROUND(Xl, Xr, 4);
    ROUND(Xr, Xl, 5);
    ROUND(Xl, Xr, 6);
    ROUND(Xr, Xl, 7);
    ROUND(Xl, Xr, 8);
    ROUND(Xr, Xl, 9);
    ROUND(Xl, Xr, 10);
    ROUND(Xr, Xl, 11);
    ROUND(Xl, Xr, 12);
    ROUND(Xr, Xl, 13);
    ROUND(Xl, Xr, 14);
    ROUND(Xr, Xl, 15);
    ROUND(Xl, Xr, 16);
    Xr.dword ^= PArray[17];

    *xr = Xl.dword;
    *xl = Xr.dword;
}

void CBlowFish::Initialize(BYTE key[], int keybytes)
{
    int i, j;
    DWORD data, datal, datar;
    union aword temp;

    for (i = 0; i < 18; i++)
        PArray[i] = bf_P[i];

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 256; j++)
            SBoxes[i][j] = bf_S[i][j];
    }

    j = 0;
    for (i = 0; i < NPASS + 2; ++i)
    {
        temp.dword = 0;
        temp.w.byte0 = key[j];
        temp.w.byte1 = key[(j + 1) % keybytes];
        temp.w.byte2 = key[(j + 2) % keybytes];
        temp.w.byte3 = key[(j + 3) % keybytes];
        data = temp.dword;
        PArray[i] ^= data;
        j = (j + 4) % keybytes;
    }

    datal = 0;
    datar = 0;

    for (i = 0; i < NPASS + 2; i += 2)
    {
        Blowfish_encipher(&datal, &datar);
        PArray[i] = datal;
        PArray[i + 1] = datar;
    }

    for (i = 0; i < 4; ++i)
    {
        for (j = 0; j < 256; j += 2)
        {
            Blowfish_encipher(&datal, &datar);
            SBoxes[i][j] = datal;
            SBoxes[i][j + 1] = datar;
        }
    }
}

DWORD CBlowFish::GetOutputLength(DWORD lInputLong)
{
    DWORD lVal;

    lVal = lInputLong % 8;
    if (lVal != 0)
        return lInputLong + 8 - lVal;
    else
        return lInputLong;
}

DWORD CBlowFish::Encode(BYTE* pInput, BYTE* pOutput, DWORD lSize)
{
    DWORD lCount, lOutSize, lGoodBytes;
    BYTE *pi, *po;
    int i, j;
    int SameDest = (pInput == pOutput ? 1 : 0);

    lOutSize = GetOutputLength(lSize);
    for (lCount = 0; lCount < lOutSize; lCount += 8)
    {
        if (SameDest)
        {
            if (lCount < lSize - 7)
            {
                Blowfish_encipher((DWORD*)pInput, (DWORD*)(pInput + 4));
            }
            else
            {
                po = pInput + lSize;
                j = (int)(lOutSize - lSize);
                for (i = 0; i < j; i++)
                    *po++ = 0;
                Blowfish_encipher((DWORD*)pInput, (DWORD*)(pInput + 4));
            }
            pInput += 8;
        }
        else
        {
            if (lCount < lSize - 7)
            {
                pi = pInput;
                po = pOutput;
                for (i = 0; i < 8; i++)
                    *po++ = *pi++;
                Blowfish_encipher((DWORD*)pOutput, (DWORD*)(pOutput + 4));
            }
            else
            {
                lGoodBytes = lSize - lCount;
                po = pOutput;
                for (i = 0; i < (int)lGoodBytes; i++)
                    *po++ = *pInput++;
                for (j = i; j < 8; j++)
                    *po++ = 0;
                Blowfish_encipher((DWORD*)pOutput, (DWORD*)(pOutput + 4));
            }
            pInput += 8;
            pOutput += 8;
        }
    }
    return lOutSize;
}
