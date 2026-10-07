#ifndef RVL_SDK_NAND_BANNER_H
#define RVL_SDK_NAND_BANNER_H

#include <stddef.h>

#define NAND_BANNER_TITLE_MAX 32
#define NAND_BANNER_ICON_MAX_FRAME 8
#define NAND_BANNER_TEXTURE_SIZE 0x6000
#define NAND_BANNER_ICON_SIZE 0x1200
#define NAND_BANNER_SIZE(frames) \
    (offsetof(NANDBanner, iconTexture) + (NAND_BANNER_ICON_SIZE * (frames)))
#define NAND_BANNER_SET_ICON_SPEED(banner, frame, speed) \
    (banner)->iconSpeed = ((banner)->iconSpeed & ~(3 << ((frame) * 2))) | \
        (((speed) & 3) << ((frame) * 2))

typedef struct NANDBanner
{
    unsigned long magic;
    unsigned long flags;
    unsigned short iconSpeed;
    unsigned char reserved[0x16];
    wchar_t title[NAND_BANNER_TITLE_MAX];
    wchar_t subtitle[NAND_BANNER_TITLE_MAX];
    unsigned char bannerTexture[NAND_BANNER_TEXTURE_SIZE];
    unsigned char iconTexture[NAND_BANNER_ICON_MAX_FRAME][NAND_BANNER_ICON_SIZE];
} NANDBanner;

#endif // RVL_SDK_NAND_BANNER_H
