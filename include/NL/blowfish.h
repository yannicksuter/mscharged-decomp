// blowfish.h     interface file for blowfish.cpp
// _THE BLOWFISH ENCRYPTION ALGORITHM_
// by Bruce Schneier
// Revised code--3/20/94
// Converted to C++ class 5/96, Jim Conger

#ifndef NL_BLOWFISH_H
#define NL_BLOWFISH_H

#define MAXKEYBYTES 56
#define NPASS       16

#define DWORD unsigned long
#define WORD  unsigned short
#define BYTE  unsigned char

class CBlowFish
{
private:
    DWORD* PArray;
    DWORD (*SBoxes)
    [256];
    void Blowfish_encipher(DWORD* xl, DWORD* xr);
    void Blowfish_decipher(DWORD* xl, DWORD* xr);

public:
    CBlowFish();
    ~CBlowFish();
    void Initialize(BYTE key[], int keybytes);
    DWORD GetOutputLength(DWORD lInputLong);
    DWORD Encode(BYTE* pInput, BYTE* pOutput, DWORD lSize);
    void Decode(BYTE* pInput, BYTE* pOutput, DWORD lSize);
};

// R4QE01 retains the donor's DCBA field order on PowerPC.
union aword
{
    DWORD dword;
    BYTE byte[4];
    struct
    {
        unsigned int byte3 : 8;
        unsigned int byte2 : 8;
        unsigned int byte1 : 8;
        unsigned int byte0 : 8;
    } w;
};

#endif // NL_BLOWFISH_H
