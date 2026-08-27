#include "NL/gc/gcSwizzler.h"
#include "NL/nlColour.h"

u32 GCTextureSize(eGXTextureFormat format, int width, int height, int numLevels,
    unsigned long texhandle)
{
    unsigned long size = 0;

    for (;;)
    {
        int rowBytes;
        if ((int)format == 2)
        {
            s32 sign = (u32)width >> 31;
            rowBytes = (sign + width) >> 1;
        }
        else if ((int)format == 3)
        {
            rowBytes = width << 2;
        }
        else if ((int)format == 6 || (int)format == 8 || (int)format == 4)
        {
            rowBytes = width;
        }
        else
        {
            rowBytes = width << 1;
        }

        size += (unsigned long)(height * rowBytes);

        numLevels -= 1;
        if (numLevels == 0)
        {
            break;
        }

        width >>= 1;
        height >>= 1;
    }

    return size;
}

#pragma push
#pragma ppc_unroll_factor_limit 2
void GCSwizzle(void* pSwizzledData, const void* pLinearData, unsigned short width,
    unsigned short height, eGXTextureFormat format, bool bEndianSwap)
{
    int stride;

    if ((int)format == 2)
    {
        stride = width >> 1;
    }
    else if ((int)format == 3)
    {
        stride = width << 2;
    }
    else if ((int)format == 6 || (int)format == 8 || (int)format == 4)
    {
        stride = width;
    }
    else
    {
        stride = width << 1;
    }

    if (stride == (width << 2))
    {
        nlColour* pSrc = (nlColour*)pLinearData;
        nlColour* pTempSrc;
        unsigned char* pDest = (unsigned char*)pSwizzledData;
        int y, yy, x, i;

        for (y = 0; y < height; y += 4)
        {
            for (x = 0; x < width; x += 4)
            {
                pTempSrc = pSrc;
                i = 0;
                for (yy = 0; yy < 4; yy++)
                {
                    pDest[i + 0x00] = pTempSrc[0].c[3];
                    pDest[i + 0x01] = pTempSrc[0].c[0];
                    pDest[i + 0x20] = pTempSrc[0].c[1];
                    pDest[i + 0x21] = pTempSrc[0].c[2];

                    pDest[i + 0x02] = pTempSrc[1].c[3];
                    pDest[i + 0x03] = pTempSrc[1].c[0];
                    pDest[i + 0x22] = pTempSrc[1].c[1];
                    pDest[i + 0x23] = pTempSrc[1].c[2];

                    pDest[i + 0x04] = pTempSrc[2].c[3];
                    pDest[i + 0x05] = pTempSrc[2].c[0];
                    pDest[i + 0x24] = pTempSrc[2].c[1];
                    pDest[i + 0x25] = pTempSrc[2].c[2];

                    pDest[i + 0x06] = pTempSrc[3].c[3];
                    pDest[i + 0x07] = pTempSrc[3].c[0];
                    pDest[i + 0x26] = pTempSrc[3].c[1];
                    pDest[i + 0x27] = pTempSrc[3].c[2];

                    i += 8;
                    pTempSrc += width;
                }

                pSrc += 4;
                pDest += 64;
            }

            pSrc += width * 3;
        }
    }
    else if (stride == (width << 1))
    {
        unsigned char* pDest = (unsigned char*)pSwizzledData;
        const unsigned short* pSrc = (const unsigned short*)pLinearData;
        const unsigned short* pTempSrc;
        int y, yy, x, xx, i;

        for (y = 0; y < height; y += 4)
        {
            for (x = 0; x < width; x += 4)
            {
                pTempSrc = pSrc;
                for (yy = 0, i = 0; yy < 4; yy++, i += 4)
                {
                    for (xx = 0; xx < 4; xx++)
                    {
                        *(unsigned short*)(pDest + ((i + xx) << 1)) =
                            pTempSrc[xx];
                    }

                    pTempSrc += width;
                }

                pSrc += 4;
                pDest += 32;
            }

            pSrc += width * 3;
        }

        if (bEndianSwap)
        {
            int count = width * height;
            unsigned short* p = (unsigned short*)pSwizzledData;
            int swapIndex = 0;
            if (count > 0)
            {
                while (swapIndex < count)
                {
                    unsigned short value = p[swapIndex];
                    unsigned short swapped;

                    ((unsigned char*)&swapped)[0] = ((unsigned char*)&value)[1];
                    ((unsigned char*)&swapped)[1] = ((unsigned char*)&value)[0];

                    p[swapIndex] = swapped;
                    swapIndex++;
                }
            }
        }
    }
    else if (stride == (int)((unsigned int)width >> 1))
    {
    }
    else if (stride == width)
    {
        int y, x, yy;
        unsigned char* pDest = (unsigned char*)pSwizzledData;
        const unsigned char* pSrc = (const unsigned char*)pLinearData;
        const unsigned char* tempSrc;

        for (y = 0; y < height; y += 4)
        {
            for (x = 0; x < width; x += 8)
            {
                tempSrc = pSrc;

                for (yy = 0; yy < 4; yy++)
                {
                    pDest[0] = tempSrc[0];
                    pDest[1] = tempSrc[1];
                    pDest[2] = tempSrc[2];
                    pDest[3] = tempSrc[3];
                    pDest[4] = tempSrc[4];
                    pDest[5] = tempSrc[5];
                    pDest[6] = tempSrc[6];
                    pDest[7] = tempSrc[7];

                    tempSrc += width;
                    pDest += 8;
                }

                pSrc += 8;
            }

            pSrc += width * 3;
        }
    }
}
#pragma pop
