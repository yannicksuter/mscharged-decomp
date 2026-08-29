#include <nitro/math/crc.h>
#include <nitro/math/dgt.h>

#include <string.h>

#define ROTL32(value, shift) \
    (((value) << (shift)) | ((value) >> (32 - (shift))))

void MATH_CalcSHA1(void* digest, const void* data, u32 dataLength)
{
    u8 tail[128];
    u32 words[80];
    u32 h0 = 0x67452301;
    u32 h1 = 0xEFCDAB89;
    u32 h2 = 0x98BADCFE;
    u32 h3 = 0x10325476;
    u32 h4 = 0xC3D2E1F0;
    u32 remainder = dataLength & 0x3F;
    u32 tailSize;
    u32 remaining;
    u32 i;
    const u8* input = data;
    u32* output = digest;

    if (remainder != 0)
    {
        memcpy(tail, input + dataLength - remainder, remainder);
    }
    tail[remainder++] = 0x80;

    if (remainder <= 56)
    {
        memset(tail + remainder, 0, 56 - remainder);
        tailSize = 64;
        i = 56;
    }
    else
    {
        memset(tail + remainder, 0, 120 - remainder);
        tailSize = 128;
        i = 120;
    }

    tail[i++] = 0;
    tail[i++] = 0;
    tail[i++] = 0;
    tail[i++] = 0;
    tail[i++] = dataLength >> 29;
    tail[i++] = dataLength >> 21;
    tail[i++] = dataLength >> 13;
    tail[i] = dataLength << 3;

    remaining = (dataLength + 8U) | 0x3F;
    remaining++;

    while (remaining != 0)
    {
        const u8* block;
        u32 a;
        u32 b;
        u32 c;
        u32 d;
        u32 e;

        if (remaining == tailSize)
        {
            block = tail;
        }
        else if (remaining == 64)
        {
            block = tail + 64;
        }
        else
        {
            block = input;
            input += 64;
        }

        memcpy(words, block, 64);
        for (i = 16; i < 80; i++)
        {
            words[i] = ROTL32(words[i - 3] ^ words[i - 8] ^ words[i - 14] ^ words[i - 16],
                1);
        }

        a = h0;
        b = h1;
        c = h2;
        d = h3;
        e = h4;

        for (i = 0; i < 20; i++)
        {
            u32 next = ROTL32(a, 5) + ((b & c) | (~b & d)) + e + words[i] + 0x5A827999;
            e = d;
            d = c;
            c = ROTL32(b, 30);
            b = a;
            a = next;
        }
        for (; i < 40; i++)
        {
            u32 next = ROTL32(a, 5) + (b ^ c ^ d) + e + words[i] + 0x6ED9EBA1;
            e = d;
            d = c;
            c = ROTL32(b, 30);
            b = a;
            a = next;
        }
        for (; i < 60; i++)
        {
            u32 next = ROTL32(a, 5) + ((b & c) | (b & d) | (c & d)) + e + words[i] + 0x8F1BBCDC;
            e = d;
            d = c;
            c = ROTL32(b, 30);
            b = a;
            a = next;
        }
        for (; i < 80; i++)
        {
            u32 next = ROTL32(a, 5) + (b ^ c ^ d) + e + words[i] + 0xCA62C1D6;
            e = d;
            d = c;
            c = ROTL32(b, 30);
            b = a;
            a = next;
        }

        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
        h4 += e;
        remaining -= 64;
    }

    output[0] = h0;
    output[1] = h1;
    output[2] = h2;
    output[3] = h3;
    output[4] = h4;
}

void MATHi_CRC8InitTable(MATHCRC8Table* table, u8 poly)
{
    u32 r, i, j;
    u8* t = table->table;

    for (i = 0; i < 256; i++)
    {
        r = i;
        for (j = 0; j < 8; j++)
        {
            if (r & 0x80)
            {
                r = (r << 1) ^ poly;
            }
            else
            {
                r <<= 1;
            }
        }
        t[i] = (u8)r;
    }
}

void MATHi_CRC32InitTableRev(MATHCRC32Table* table, u32 poly)
{
    u32 r, i, j;
    u32* t = table->table;

    for (i = 0; i < 256; i++)
    {
        r = i;
        for (j = 0; j < 8; j++)
        {
            if (r & 1)
            {
                r = (r >> 1) ^ poly;
            }
            else
            {
                r >>= 1;
            }
        }
        t[i] = r;
    }
}

void MATHi_CRC8Update(const MATHCRC8Table* table, MATHCRC8Context* context,
    const void* input, u32 length)
{
    u32 r, i;
    const u8* t = table->table;
    u8* data = (u8*)input;

    r = *context;
    for (i = 0; i < length; i++)
    {
        r = t[(r ^ *data) & 0xff];
        data++;
    }
    *context = (MATHCRC8Context)r;
}

void MATHi_CRC32UpdateRev(const MATHCRC32Table* table,
    MATHCRC32Context* context, const void* input,
    u32 length)
{
    u32 r, i;
    const u32* t = table->table;
    u8* data = (u8*)input;

    r = *context;
    for (i = 0; i < length; i++)
    {
        r = (r >> 8) ^ t[(r ^ *data) & 0xff];
        data++;
    }
    *context = r;
}

u8 MATH_CalcCRC8(const MATHCRC8Table* table, const void* data,
    u32 dataLength)
{
    MATHCRC8Context context = 0;
    MATHi_CRC8Update(table, &context, data, dataLength);
    return context;
}

u32 MATH_CalcCRC32(const MATHCRC32Table* table, const void* data,
    u32 dataLength)
{
    MATHCRC32Context context = (MATHCRC32Context)(~0);
    MATHi_CRC32UpdateRev(table, &context, data, dataLength);
    return ~context;
}
