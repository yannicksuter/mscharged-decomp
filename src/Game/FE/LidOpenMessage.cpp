#include <revolution/gx/GXMisc_fwd.h>
#include <revolution/sc_fwd.h>
#include <revolution/gx/GXFrameBuf_fwd.h>
#include "NL/nlDLListContainer.inl"
#include <revolution/vi/vi_fwd.h>
#include "Game/FE/LidOpenMessage.h"

#include "Game/NetworkSession.h"
#include "Game/Sys/audio.h"
#include "Game/Task/ResetTask.h"
#include "Game/main.h"
#include "NL/gl/glPlat.h"
#include "NL/globalpad.h"
#include "NL/glx/glxSwap.h"
#include "NL/nlFileGC.h"
#include "NL/nlMemory.h"
#include "types.h"
#include "Game/RumbleActions.h"
#include "Game/Audio/RegistryPools.h"

struct DiscMessageImage
{
    int width;
    int height;
    unsigned long compressedSize;
    const unsigned char* compressedData;
};

#include "Game/FE/DiscMessageImages.inc"

static const DiscMessageImage sDiscMessageImages[7][5] = {
    {
        { 400, 200, sizeof(HEADER_FatalErrorImage_eng::g_ImageData), HEADER_FatalErrorImage_eng::g_ImageData },
        { 400, 200, sizeof(HEADER_NoDiskImage_eng::g_ImageData), HEADER_NoDiskImage_eng::g_ImageData },
        { 400, 200, sizeof(HEADER_WrongDiskImage_eng::g_ImageData), HEADER_WrongDiskImage_eng::g_ImageData },
        { 400, 200, sizeof(HEADER_RetryImage_eng::g_ImageData), HEADER_RetryImage_eng::g_ImageData },
        { 128, 32, sizeof(HEADER_Loading_eng::g_ImageData), HEADER_Loading_eng::g_ImageData },
    },
    {
        { 400, 200, sizeof(HEADER_FatalErrorImage_fre::g_ImageData), HEADER_FatalErrorImage_fre::g_ImageData },
        { 400, 200, sizeof(HEADER_NoDiskImage_fre::g_ImageData), HEADER_NoDiskImage_fre::g_ImageData },
        { 400, 200, sizeof(HEADER_WrongDiskImage_fre::g_ImageData), HEADER_WrongDiskImage_fre::g_ImageData },
        { 400, 200, sizeof(HEADER_RetryImage_fre::g_ImageData), HEADER_RetryImage_fre::g_ImageData },
        { 280, 32, sizeof(HEADER_Loading_fre::g_ImageData), HEADER_Loading_fre::g_ImageData },
    },
    {
        { 400, 200, sizeof(HEADER_FatalErrorImage_deu::g_ImageData), HEADER_FatalErrorImage_deu::g_ImageData },
        { 400, 200, sizeof(HEADER_NoDiskImage_deu::g_ImageData), HEADER_NoDiskImage_deu::g_ImageData },
        { 400, 200, sizeof(HEADER_WrongDiskImage_deu::g_ImageData), HEADER_WrongDiskImage_deu::g_ImageData },
        { 400, 200, sizeof(HEADER_RetryImage_deu::g_ImageData), HEADER_RetryImage_deu::g_ImageData },
        { 288, 32, sizeof(HEADER_Loading_deu::g_ImageData), HEADER_Loading_deu::g_ImageData },
    },
    {
        { 400, 200, sizeof(HEADER_FatalErrorImage_spa::g_ImageData), HEADER_FatalErrorImage_spa::g_ImageData },
        { 400, 200, sizeof(HEADER_NoDiskImage_spa::g_ImageData), HEADER_NoDiskImage_spa::g_ImageData },
        { 400, 200, sizeof(HEADER_WrongDiskImage_spa::g_ImageData), HEADER_WrongDiskImage_spa::g_ImageData },
        { 400, 200, sizeof(HEADER_RetryImage_spa::g_ImageData), HEADER_RetryImage_spa::g_ImageData },
        { 128, 32, sizeof(HEADER_Loading_spa::g_ImageData), HEADER_Loading_spa::g_ImageData },
    },
    {
        { 400, 200, sizeof(HEADER_FatalErrorImage_ita::g_ImageData), HEADER_FatalErrorImage_ita::g_ImageData },
        { 400, 200, sizeof(HEADER_NoDiskImage_ita::g_ImageData), HEADER_NoDiskImage_ita::g_ImageData },
        { 400, 200, sizeof(HEADER_WrongDiskImage_ita::g_ImageData), HEADER_WrongDiskImage_ita::g_ImageData },
        { 400, 200, sizeof(HEADER_RetryImage_ita::g_ImageData), HEADER_RetryImage_ita::g_ImageData },
        { 288, 32, sizeof(HEADER_Loading_ita::g_ImageData), HEADER_Loading_ita::g_ImageData },
    },
    {
        { 400, 200, sizeof(HEADER_FatalErrorImage_jpn::g_ImageData), HEADER_FatalErrorImage_jpn::g_ImageData },
        { 400, 200, sizeof(HEADER_NoDiskImage_jpn::g_ImageData), HEADER_NoDiskImage_jpn::g_ImageData },
        { 400, 200, sizeof(HEADER_WrongDiskImage_jpn::g_ImageData), HEADER_WrongDiskImage_jpn::g_ImageData },
        { 400, 200, sizeof(HEADER_RetryImage_jpn::g_ImageData), HEADER_RetryImage_jpn::g_ImageData },
        { 256, 48, sizeof(HEADER_Loading_jpn::g_ImageData), HEADER_Loading_jpn::g_ImageData },
    },
    {
        { 400, 200, sizeof(HEADER_FatalErrorImage_uke::g_ImageData), HEADER_FatalErrorImage_uke::g_ImageData },
        { 400, 200, sizeof(HEADER_NoDiskImage_uke::g_ImageData), HEADER_NoDiskImage_uke::g_ImageData },
        { 400, 200, sizeof(HEADER_WrongDiskImage_uke::g_ImageData), HEADER_WrongDiskImage_uke::g_ImageData },
        { 400, 200, sizeof(HEADER_RetryImage_uke::g_ImageData), HEADER_RetryImage_uke::g_ImageData },
        { 128, 32, sizeof(HEADER_Loading_uke::g_ImageData), HEADER_Loading_uke::g_ImageData },
    },
};

static int lastImageWidth;
static int lastImageHeight;
static int lastImageSize;
static const unsigned char* lastImageData;
static int OpenCloseRefCount;
static bool ResetWasPaused;
static bool sAudioPausedForDiscMessage;
static bool CanGetResetPauseState = true;


static int GetDiscMessageLanguageIndex()
{
    int lang;

    switch (GetRegion())
    {
    case GAME_REGION_US:
#if defined(VERSION_R4QE01)
        switch ((u8)SCGetLanguage())
        {
        case 3:
            lang = 1;
            break;
        case 4:
            lang = 3;
            break;
        default:
            lang = 0;
            break;
        }
#else
        lang = 0;
#endif
        break;
    case GAME_REGION_EU:
        switch ((u8)SCGetLanguage())
        {
        case 2:
            lang = 2;
            break;
        case 3:
            lang = 1;
            break;
        case 4:
            lang = 3;
            break;
        case 5:
            lang = 4;
            break;
        default:
            lang = 6;
            break;
        }
        break;
    case GAME_REGION_JAPAN:
        lang = 5;
        break;
    default:
        lang = g_Language;
        break;
    }

    if (lang > 6)
    {
        lang = 0;
    }
    return lang;
}

static void UncompressLidMessage(const unsigned char* compressed, int compressedSize, unsigned char* uncompressed, int)
{
    int index = 0;
    int i = 0;

    while (i < (compressedSize - 1))
    {
        if (compressed[i] == 0)
        {
            int k = 0;
            while (k < compressed[i + 1])
            {
                uncompressed[index] = 0;
                index++;
                k++;
            }
            i++;
        }
        else
        {
            uncompressed[index] = compressed[i];
            index++;
        }
        i++;
    }

    uncompressed[index] = compressed[compressedSize - 1];
}

static void DisplayMessage(int imagewidth, int imageheight, const unsigned char* datastart, int datasize, unsigned long color, bool alignBottom)
{
    unsigned int centerVertically;
    unsigned char* expanded;
    int yStart;
    int yCenter;
    int yBottom;
    int xEnd;
    int rowBytes;
    unsigned char* row;
    void* fb;
    int pass;
    int expandedSize;
    int xStart;
    int yEnd;
    int y;
    int x;
    int i;
    int j;

    GXDrawDone();
    VIWaitForRetrace();
    VISetBlack(1);
    VIFlush();
    VIWaitForRetrace();
    VIWaitForRetrace();

    lastImageWidth = imagewidth;
    lastImageHeight = imageheight;
    lastImageData = datastart;
    lastImageSize = datasize;

    expandedSize = (imagewidth * imageheight) / 8;
    expanded = (unsigned char*)nlMalloc((unsigned long)expandedSize, 0x20, 1);
    UncompressLidMessage(datastart, datasize, expanded, expandedSize);

    GXPokeColorUpdate(1);
    GXPokeBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_SET);

    xStart = (0x280 - imagewidth) / 2;
    centerVertically = (alignBottom == 0);
    pass = 0;
    yCenter = (0x1C0 - imageheight) / 2;
    xEnd = xStart + imagewidth;
    yBottom = 0x1A0 - imageheight;
    rowBytes = imagewidth / 8;

    while (pass < 4)
    {
        if ((pass == 0) || (pass == 2))
        {
            fb = glxGetDisplayedBuffer();
        }
        else
        {
            fb = glxGetBackBuffer();
        }

        glx_ClearXFB(fb);

        yStart = yBottom;
        if (centerVertically != 0)
        {
            yStart = yCenter;
        }

        yEnd = yStart + imageheight;
        i = 0;
        while (i < 0x1C0)
        {
            j = 0;
            while (j < 0x280)
            {
                GXPokeARGB((unsigned short)j, (unsigned short)i, 0);
                j++;
            }
            i++;
        }

        y = yStart;
        while (y < yEnd)
        {
            x = xStart;
            int rowOffset = rowBytes * (y - yStart);
            row = expanded + rowOffset;

            while (x < xEnd)
            {
                int pixel = x - xStart;
                if ((row[pixel / 8] & (1 << (pixel % 8))) != 0)
                {
                    GXPokeARGB((unsigned short)x, (unsigned short)y, color);
                }
                x++;
            }
            y++;
        }

        GXCopyDisp(fb, 1);
        GXDrawDone();
        GXFlush();
        pass++;
    }

    GXPokeColorUpdate(0);
    delete[] expanded;

    VIWaitForRetrace();
    VIWaitForRetrace();
    VISetBlack(0);
    VISetNextFrameBuffer(fb);
    VIFlush();
    VIWaitForRetrace();
}

void DisplayDVDMessageSebring(int arg)
{
    if (CanGetResetPauseState)
    {
        ResetWasPaused = ResetTask::s_resetPaused;
        CanGetResetPauseState = false;
    }

    if (ResetTask::s_ResetState != RS_STARTRESET && g_pAudioSystem != 0)
    {
        if (arg == 4 || arg == -1 || arg == 11 || arg == 6)
        {
            if (!sAudioPausedForDiscMessage)
            {
                PauseAllAudio();
            }
            sAudioPausedForDiscMessage = true;
        }

        if (arg == 1)
        {
            int lang = GetDiscMessageLanguageIndex();
            DisplayMessage(sDiscMessageImages[lang][4].width, sDiscMessageImages[lang][4].height, sDiscMessageImages[lang][4].compressedData, sDiscMessageImages[lang][4].compressedSize, (unsigned long)-1, false);
            return;
        }
    }

    ResetTask::s_resetPaused = false;
    OpenCloseRefCount++;

    int lang = GetDiscMessageLanguageIndex();
    switch (arg + 1)
    {
    case 5:
        DisplayMessage(sDiscMessageImages[lang][0].width, sDiscMessageImages[lang][0].height, sDiscMessageImages[lang][0].compressedData, sDiscMessageImages[lang][0].compressedSize, (unsigned long)-1, false);
        break;
    case 0:
        DisplayMessage(sDiscMessageImages[lang][1].width, sDiscMessageImages[lang][1].height, sDiscMessageImages[lang][1].compressedData, sDiscMessageImages[lang][1].compressedSize, (unsigned long)-1, false);
        break;
    case 12:
        DisplayMessage(sDiscMessageImages[lang][2].width, sDiscMessageImages[lang][2].height, sDiscMessageImages[lang][2].compressedData, sDiscMessageImages[lang][2].compressedSize, (unsigned long)-1, false);
        break;
    case 7:
        DisplayMessage(sDiscMessageImages[lang][3].width, sDiscMessageImages[lang][3].height, sDiscMessageImages[lang][3].compressedData, sDiscMessageImages[lang][3].compressedSize, (unsigned long)-1, false);
        break;
    }

    for (int i = 0; i < 4; i++)
    {
        cGlobalPad* pad = g_pPadManager->GetPad(i);
        if (pad != 0)
        {
            StopRumbleAction(pad);
        }
    }

    if (g_pNetworkSession != 0)
    {
        g_pNetworkSession->DisconnectOnlineMatch();
    }
    ResetTask::s_checkCardRemoved = true;
}

void DVDAllClearSebring(int nStatus)
{
    DisplayMessage(lastImageWidth, lastImageHeight, lastImageData, lastImageSize, 0, false);

    while (nlAsyncReadsPending(0))
    {
        nlServiceFileSystem();
    }

    OpenCloseRefCount--;
    ResetTask::s_resetPaused = ResetWasPaused;
    CanGetResetPauseState = true;
    ResetTask::s_checkCardRemoved = false;
    ResumeAllAudio();
    sAudioPausedForDiscMessage = false;
}

#if defined(VERSION_R4QP01_REV1) || defined(VERSION_R4QP01_REV2)
static inline int GetLoadingMessageLanguageIndex()
{
    int lang;

    switch (GetRegion())
    {
    case GAME_REGION_US:
        lang = 0;
        break;
    case GAME_REGION_EU:
        switch ((u8)SCGetLanguage())
        {
        case 2:
            lang = 2;
            break;
        case 3:
            lang = 1;
            break;
        case 4:
            lang = 3;
            break;
        case 5:
            lang = 4;
            break;
        default:
            lang = 6;
            break;
        }
        break;
    case GAME_REGION_JAPAN:
        lang = 5;
        break;
    default:
        lang = g_Language;
        break;
    }

    if (lang > 6)
    {
        lang = 0;
    }
    return lang;
}
#endif

void DisplayLoadingMessageFast()
{
#if defined(VERSION_R4QP01_REV1) || defined(VERSION_R4QP01_REV2)
    int lang = GetLoadingMessageLanguageIndex();
#else
    int lang = GetDiscMessageLanguageIndex();
#endif
    DisplayMessage(sDiscMessageImages[lang][4].width, sDiscMessageImages[lang][4].height, sDiscMessageImages[lang][4].compressedData, sDiscMessageImages[lang][4].compressedSize, (unsigned long)-1, false);
}
