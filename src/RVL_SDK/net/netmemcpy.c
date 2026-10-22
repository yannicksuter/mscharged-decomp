#include <revolution/net/NETMem.h>

static inline void NETMemCpy_DstSize32Aligned(void* dst, const void* src, unsigned long size)
{
    unsigned long tmp0;
    unsigned long tmp1;
    unsigned long tmp2;
    unsigned long tmp3;
    unsigned long tmp4;
    unsigned long tmp5;
    unsigned long loopSize = size >> 5;
    unsigned long shift = (unsigned long)src & 3;

    switch (shift)
    {
    case 0:
        do
        {
            __dcbz(dst, 0);
            tmp0 = ((const unsigned long*)src)[0];
            tmp1 = ((const unsigned long*)src)[1];
            ((unsigned long*)dst)[0] = tmp0;
            ((unsigned long*)dst)[1] = tmp1;
            tmp0 = ((const unsigned long*)src)[2];
            tmp1 = ((const unsigned long*)src)[3];
            ((unsigned long*)dst)[2] = tmp0;
            ((unsigned long*)dst)[3] = tmp1;
            tmp0 = ((const unsigned long*)src)[4];
            tmp1 = ((const unsigned long*)src)[5];
            ((unsigned long*)dst)[4] = tmp0;
            ((unsigned long*)dst)[5] = tmp1;
            tmp0 = ((const unsigned long*)src)[6];
            tmp1 = ((const unsigned long*)src)[7];
            ((unsigned long*)dst)[6] = tmp0;
            ((unsigned long*)dst)[7] = tmp1;
            src = (const unsigned char*)src + 32;
            dst = (unsigned char*)dst + 32;
        } while (--loopSize);
        break;
    case 1:
        src = (const unsigned char*)src - 1;
        tmp2 = ((const unsigned long*)src)[0];
        tmp2 = tmp2 << 8;
        src = (const unsigned char*)src + 4;
        do
        {
            __dcbz(dst, 0);
            tmp0 = ((const unsigned long*)src)[0];
            tmp3 = tmp0 << 8;
            tmp2 = __rlwimi(tmp2, tmp0, 8, 24, 31);
            tmp1 = ((const unsigned long*)src)[1];
            ((unsigned long*)dst)[0] = tmp2;
            tmp4 = tmp1 << 8;
            tmp3 = __rlwimi(tmp3, tmp1, 8, 24, 31);
            tmp0 = ((const unsigned long*)src)[2];
            ((unsigned long*)dst)[1] = tmp3;
            tmp5 = tmp0 << 8;
            tmp4 = __rlwimi(tmp4, tmp0, 8, 24, 31);
            tmp1 = ((const unsigned long*)src)[3];
            ((unsigned long*)dst)[2] = tmp4;
            tmp2 = tmp1 << 8;
            tmp5 = __rlwimi(tmp5, tmp1, 8, 24, 31);
            tmp0 = ((const unsigned long*)src)[4];
            ((unsigned long*)dst)[3] = tmp5;
            tmp3 = tmp0 << 8;
            tmp2 = __rlwimi(tmp2, tmp0, 8, 24, 31);
            tmp1 = ((const unsigned long*)src)[5];
            ((unsigned long*)dst)[4] = tmp2;
            tmp4 = tmp1 << 8;
            tmp3 = __rlwimi(tmp3, tmp1, 8, 24, 31);
            tmp0 = ((const unsigned long*)src)[6];
            ((unsigned long*)dst)[5] = tmp3;
            tmp5 = tmp0 << 8;
            tmp4 = __rlwimi(tmp4, tmp0, 8, 24, 31);
            tmp1 = ((const unsigned long*)src)[7];
            ((unsigned long*)dst)[6] = tmp4;
            tmp2 = tmp1 << 8;
            tmp5 = __rlwimi(tmp5, tmp1, 8, 24, 31);
            src = (const unsigned char*)src + 32;
            ((unsigned long*)dst)[7] = tmp5;
            dst = (unsigned char*)dst + 32;
        } while (--loopSize);
        break;
    case 2:
        src = (const unsigned char*)src - 2;
        tmp2 = ((const unsigned long*)src)[0];
        tmp2 = tmp2 << 16;
        src = (const unsigned char*)src + 4;
        do
        {
            __dcbz(dst, 0);
            tmp0 = ((const unsigned long*)src)[0];
            tmp3 = tmp0 << 16;
            tmp2 = __rlwimi(tmp2, tmp0, 16, 16, 31);
            tmp1 = ((const unsigned long*)src)[1];
            ((unsigned long*)dst)[0] = tmp2;
            tmp4 = tmp1 << 16;
            tmp3 = __rlwimi(tmp3, tmp1, 16, 16, 31);
            tmp0 = ((const unsigned long*)src)[2];
            ((unsigned long*)dst)[1] = tmp3;
            tmp5 = tmp0 << 16;
            tmp4 = __rlwimi(tmp4, tmp0, 16, 16, 31);
            tmp1 = ((const unsigned long*)src)[3];
            ((unsigned long*)dst)[2] = tmp4;
            tmp2 = tmp1 << 16;
            tmp5 = __rlwimi(tmp5, tmp1, 16, 16, 31);
            tmp0 = ((const unsigned long*)src)[4];
            ((unsigned long*)dst)[3] = tmp5;
            tmp3 = tmp0 << 16;
            tmp2 = __rlwimi(tmp2, tmp0, 16, 16, 31);
            tmp1 = ((const unsigned long*)src)[5];
            ((unsigned long*)dst)[4] = tmp2;
            tmp4 = tmp1 << 16;
            tmp3 = __rlwimi(tmp3, tmp1, 16, 16, 31);
            tmp0 = ((const unsigned long*)src)[6];
            ((unsigned long*)dst)[5] = tmp3;
            tmp5 = tmp0 << 16;
            tmp4 = __rlwimi(tmp4, tmp0, 16, 16, 31);
            tmp1 = ((const unsigned long*)src)[7];
            ((unsigned long*)dst)[6] = tmp4;
            tmp2 = tmp1 << 16;
            tmp5 = __rlwimi(tmp5, tmp1, 16, 16, 31);
            src = (const unsigned char*)src + 32;
            ((unsigned long*)dst)[7] = tmp5;
            dst = (unsigned char*)dst + 32;
        } while (--loopSize);
        break;
    case 3:
        src = (const unsigned char*)src - 3;
        tmp2 = ((const unsigned long*)src)[0];
        tmp2 = tmp2 << 24;
        src = (const unsigned char*)src + 4;
        do
        {
            __dcbz(dst, 0);
            tmp0 = ((const unsigned long*)src)[0];
            tmp3 = tmp0 << 24;
            tmp2 = __rlwimi(tmp2, tmp0, 24, 8, 31);
            tmp1 = ((const unsigned long*)src)[1];
            ((unsigned long*)dst)[0] = tmp2;
            tmp4 = tmp1 << 24;
            tmp3 = __rlwimi(tmp3, tmp1, 24, 8, 31);
            tmp0 = ((const unsigned long*)src)[2];
            ((unsigned long*)dst)[1] = tmp3;
            tmp5 = tmp0 << 24;
            tmp4 = __rlwimi(tmp4, tmp0, 24, 8, 31);
            tmp1 = ((const unsigned long*)src)[3];
            ((unsigned long*)dst)[2] = tmp4;
            tmp2 = tmp1 << 24;
            tmp5 = __rlwimi(tmp5, tmp1, 24, 8, 31);
            tmp0 = ((const unsigned long*)src)[4];
            ((unsigned long*)dst)[3] = tmp5;
            tmp3 = tmp0 << 24;
            tmp2 = __rlwimi(tmp2, tmp0, 24, 8, 31);
            tmp1 = ((const unsigned long*)src)[5];
            ((unsigned long*)dst)[4] = tmp2;
            tmp4 = tmp1 << 24;
            tmp3 = __rlwimi(tmp3, tmp1, 24, 8, 31);
            tmp0 = ((const unsigned long*)src)[6];
            ((unsigned long*)dst)[5] = tmp3;
            tmp5 = tmp0 << 24;
            tmp4 = __rlwimi(tmp4, tmp0, 24, 8, 31);
            tmp1 = ((const unsigned long*)src)[7];
            ((unsigned long*)dst)[6] = tmp4;
            tmp2 = tmp1 << 24;
            tmp5 = __rlwimi(tmp5, tmp1, 24, 8, 31);
            src = (const unsigned char*)src + 32;
            ((unsigned long*)dst)[7] = tmp5;
            dst = (unsigned char*)dst + 32;
        } while (--loopSize);
        break;
    }
}

static inline void NETMemCpy_SimpleFwd(void* dst, const void* src, unsigned long size)
{
    unsigned char* dst_u8;
    const unsigned char* src_u8;
    unsigned long numWords;
    unsigned long remainBytes;
    unsigned long tmp;

    src_u8 = (const unsigned char*)src;
    dst_u8 = (unsigned char*)dst;
    numWords = size >> 2;
    remainBytes = size & 3;
    if (numWords != 0)
    {
        do
        {
            tmp = ((const unsigned long*)src_u8)[0];
            src_u8 += 4;
            ((unsigned long*)dst_u8)[0] = tmp;
            dst_u8 += 4;
        } while (--numWords);
    }
    if (remainBytes != 0)
    {
        do
        {
            tmp = *src_u8;
            src_u8 += 1;
            *dst_u8 = (unsigned char)tmp;
            dst_u8 += 1;
        } while (--remainBytes);
    }
}

static inline void NETMemCpy_SimpleRev(void* dst, const void* src, unsigned long size)
{
    unsigned char* dst_u8;
    const unsigned char* src_u8;
    unsigned long numWords;
    unsigned long remainBytes;
    unsigned long tmp;

    src_u8 = (const unsigned char*)src + size;
    dst_u8 = (unsigned char*)dst + size;
    numWords = size >> 2;
    remainBytes = size & 3;
    if (remainBytes != 0)
    {
        do
        {
            src_u8 -= 1;
            tmp = *src_u8;
            dst_u8 -= 1;
            *dst_u8 = (unsigned char)tmp;
        } while (--remainBytes);
    }
    if (numWords != 0)
    {
        do
        {
            src_u8 -= 4;
            tmp = ((const unsigned long*)src_u8)[0];
            dst_u8 -= 4;
            ((unsigned long*)dst_u8)[0] = tmp;
        } while (--numWords);
    }
}

void* NETMemCpy(void* dst, const void* src, unsigned long size)
{
    void* originalDst = dst;
    unsigned long headSize;
    unsigned long accBlkSize;

    if (dst == src)
    {
        return dst;
    }

    if ((unsigned char*)dst > (const unsigned char*)src && (unsigned char*)dst < (const unsigned char*)src + size)
    {
        NETMemCpy_SimpleRev(dst, src, size);
    }
    else
    {
        if (((unsigned char*)dst <= (const unsigned char*)src - 0x20 || (unsigned char*)dst >= (const unsigned char*)src) && size >= 0x40)
        {
            headSize = (unsigned long)dst & 0x1F;
            if (headSize != 0)
            {
                headSize = 0x20 - headSize;
                NETMemCpy_SimpleFwd(dst, src, headSize);
                dst = (unsigned char*)dst + headSize;
                src = (const unsigned char*)src + headSize;
                size -= headSize;
            }

            accBlkSize = size & ~0x1F;
            NETMemCpy_DstSize32Aligned(dst, src, accBlkSize);
            dst = (unsigned char*)dst + accBlkSize;
            src = (const unsigned char*)src + accBlkSize;
            size -= accBlkSize;
        }

        NETMemCpy_SimpleFwd(dst, src, size);
    }
    return originalDst;
}
