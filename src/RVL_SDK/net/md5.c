#include <revolution/net/NETDigest.h>

#include <string.h>

#define F(x, y, z)        (((x) & (y)) | (~(x) & (z)))
#define G(x, y, z)        (((x) & (z)) | ((y) & ~(z)))
#define H(x, y, z)        ((x) ^ (y) ^ (z))
#define I(x, y, z)        ((y) ^ ((x) | ~(z)))
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
#define STEP(f, a, b, c, d, x, s, t) \
    ((a) = (b) + ROTATE_LEFT((t) + (x) + (a) + f((b), (c), (d)), (s)))
#define SWAP32(x) \
    (((x) >> 24) | (((x) >> 8) & 0xFF00) | (((x) << 8) & 0xFF0000) | ((x) << 24))
#define LOAD32(address, offset) __lwbrx((address), (offset))

static u8 sPadding = 0x80;

static u32 sRoundConstants[64] = {
    0xd76aa478,
    0xe8c7b756,
    0x242070db,
    0xc1bdceee,
    0xf57c0faf,
    0x4787c62a,
    0xa8304613,
    0xfd469501,
    0x698098d8,
    0x8b44f7af,
    0xffff5bb1,
    0x895cd7be,
    0x6b901122,
    0xfd987193,
    0xa679438e,
    0x49b40821,
    0xf61e2562,
    0xc040b340,
    0x265e5a51,
    0xe9b6c7aa,
    0xd62f105d,
    0x02441453,
    0xd8a1e681,
    0xe7d3fbc8,
    0x21e1cde6,
    0xc33707d6,
    0xf4d50d87,
    0x455a14ed,
    0xa9e3e905,
    0xfcefa3f8,
    0x676f02d9,
    0x8d2a4c8a,
    0xfffa3942,
    0x8771f681,
    0x6d9d6122,
    0xfde5380c,
    0xa4beea44,
    0x4bdecfa9,
    0xf6bb4b60,
    0xbebfbc70,
    0x289b7ec6,
    0xeaa127fa,
    0xd4ef3085,
    0x04881d05,
    0xd9d4d039,
    0xe6db99e5,
    0x1fa27cf8,
    0xc4ac5665,
    0xf4292244,
    0x432aff97,
    0xab9423a7,
    0xfc93a039,
    0x655b59c3,
    0x8f0ccc92,
    0xffeff47d,
    0x85845dd1,
    0x6fa87e4f,
    0xfe2ce6e0,
    0xa3014314,
    0x4e0811a1,
    0xf7537e82,
    0xbd3af235,
    0x2ad7d2bb,
    0xeb86d391,
};

static u32 sRoundIndices[48] = {
    1,
    6,
    11,
    0,
    5,
    10,
    15,
    4,
    9,
    14,
    3,
    8,
    13,
    2,
    7,
    12,
    5,
    8,
    11,
    14,
    1,
    4,
    7,
    10,
    13,
    0,
    3,
    6,
    9,
    12,
    15,
    2,
    0,
    7,
    14,
    5,
    12,
    3,
    10,
    1,
    8,
    15,
    6,
    13,
    4,
    11,
    2,
    9,
};

static void ProcessBlock(NETMD5Context* context);

static const NETHashInterface sMD5Interface = {
    NET_MD5_DIGEST_SIZE,
    64,
    sizeof(NETMD5Context),
    0,
    (NETHashInitFunc)NETMD5Init,
    (NETHashUpdateFunc)NETMD5Update,
    (NETHashGetDigestFunc)NETMD5GetDigest,
};

void NETMD5Init(NETMD5Context* context)
{
    context->a = 0x67452301;
    context->b = 0xefcdab89;
    context->c = 0x98badcfe;
    context->d = 0x10325476;
    context->length = 0;
}

void NETMD5Update(NETMD5Context* context, const void* input, u32 length)
{
    u32 index = (u32)context->length;
    u32 fill;
    s32 blocks;
    const u8* data = input;

    context->length += length;
    index &= 0x3F;
    fill = 64 - index;

    if (fill > length)
    {
        if (length != 0)
        {
            memcpy(&context->buffer8[index], data, length);
        }
        return;
    }

    memcpy(&context->buffer8[index], data, fill);
    ProcessBlock(context);
    data += fill;
    length -= fill;

    blocks = length >> 6;
    while (blocks > 0)
    {
        memcpy(context->buffer8, data, 64);
        data += 64;
        ProcessBlock(context);
        blocks--;
    }

    length &= 0x3F;
    if (length != 0)
    {
        memcpy(context->buffer8, data, length);
    }
}

void NETMD5GetDigest(NETMD5Context* context, void* digest)
{
    u64 length = context->length << 3;
    u32 high = (u32)(length >> 32);
    u32 low = (u32)length;
    u32 index;
    u32 fill;

    NETMD5Update(context, &sPadding, 1);

    index = (u32)context->length & 0x3F;
    fill = 64 - index;
    if (fill < 8)
    {
        memset(&context->buffer8[index], 0, fill);
        ProcessBlock(context);
        index = 0;
        fill = 64;
    }
    if (fill > 8)
    {
        memset(&context->buffer8[index], 0, fill - 8);
    }

    context->buffer32[14] = SWAP32(low);
    context->buffer32[15] = SWAP32(high);
    ProcessBlock(context);

    __stwbrx(context->state[0], digest, 0);
    __stwbrx(context->state[1], (u8*)digest + 4, 0);
    __stwbrx(context->state[2], (u8*)digest + 8, 0);
    __stwbrx(context->state[3], (u8*)digest + 12, 0);
    memset(context, 0, sizeof(*context));
}

const NETHashInterface* NETGetMD5Interface(void)
{
    return &sMD5Interface;
}

static void ProcessBlock(NETMD5Context* context)
{
    u32 a = context->a;
    u32* buffer = context->buffer32;
    u32 b = context->b;
    u32 c = context->c;
    u32 d = context->d;
    u32* data = buffer;
    u32* constants = sRoundConstants;
    u32* indices;
    u32 i;

    for (i = 0; i < 4; i++)
    {
        STEP(F, a, b, c, d, LOAD32(data, 0), 7, constants[0]);
        data++;
        STEP(F, d, a, b, c, LOAD32(data, 0), 12, constants[1]);
        data++;
        STEP(F, c, d, a, b, LOAD32(data, 0), 17, constants[2]);
        data++;
        STEP(F, b, c, d, a, LOAD32(data, 0), 22, constants[3]);
        data++;
        constants += 4;
    }

    indices = sRoundIndices;
    for (i = 0; i < 4; i++)
    {
        STEP(G, a, b, c, d, LOAD32(buffer, indices[0] * 4), 5, constants[0]);
        STEP(G, d, a, b, c, LOAD32(buffer, indices[1] * 4), 9, constants[1]);
        STEP(G, c, d, a, b, LOAD32(buffer, indices[2] * 4), 14, constants[2]);
        STEP(G, b, c, d, a, LOAD32(buffer, indices[3] * 4), 20, constants[3]);
        indices += 4;
        constants += 4;
    }

    for (i = 0; i < 4; i++)
    {
        STEP(H, a, b, c, d, LOAD32(buffer, indices[0] * 4), 4, constants[0]);
        STEP(H, d, a, b, c, LOAD32(buffer, indices[1] * 4), 11, constants[1]);
        STEP(H, c, d, a, b, LOAD32(buffer, indices[2] * 4), 16, constants[2]);
        STEP(H, b, c, d, a, LOAD32(buffer, indices[3] * 4), 23, constants[3]);
        indices += 4;
        constants += 4;
    }

    for (i = 0; i < 4; i++)
    {
        STEP(I, a, b, c, d, LOAD32(buffer, indices[0] * 4), 6, constants[0]);
        STEP(I, d, a, b, c, LOAD32(buffer, indices[1] * 4), 10, constants[1]);
        STEP(I, c, d, a, b, LOAD32(buffer, indices[2] * 4), 15, constants[2]);
        STEP(I, b, c, d, a, LOAD32(buffer, indices[3] * 4), 21, constants[3]);
        indices += 4;
        constants += 4;
    }

    context->a += a;
    context->b += b;
    context->c += c;
    context->d += d;
}
