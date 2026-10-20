#include <revolution/os.h>
#include <revolution/dvd.h>
#include <revolution/sc.h>

static u8 CheckBuffer[32] ALIGN(32);

static volatile BOOL lowDone = TRUE;
static volatile u32 lowIntType = 0;

static void lowCallback(u32 intType)
{
    lowIntType = intType;
    lowDone = TRUE;
}

void __DVDShowDeviceErrorMessage(void);

BOOL __DVDCheckDevice(void)
{
    u32 checkCode = 0x460A0000;
    u32 outOfRangeError = 0xFFFFFFFF;
    u32 reportKeyError = 0xFFFFFFFF;
    OSIOSRev iosRev;

    if (OSGetPhysicalMem2Size() == 0x08000000)
    {
        return TRUE;
    }

    __OSGetIOSRev(&iosRev);

    if (iosRev.idLo < 30 || iosRev.idLo >= 254)
    {
        return TRUE;
    }

    if (OS_DEVICE_CHECK_CODE == 129)
    {
        checkCode = 0x7ED40000;
    }

    lowDone = FALSE;
    DVDLowUnencryptedRead((void*)CheckBuffer, 32, checkCode, lowCallback);

    while (!lowDone)
    {
    }

    switch (lowIntType)
    {
    case 2:
        break;
    case 1:
        goto invalid;
    default:
        goto fatal;
    }

    lowDone = FALSE;
    DVDLowRequestError(lowCallback);

    while (!lowDone)
    {
    }

    outOfRangeError = DVDLowGetImmBufferReg();

    switch (lowIntType)
    {
    case 1:
    {
        if ((DVDLowGetImmBufferReg() & 0xFF000000) != 0)
        {
            goto recover;
        }

        switch (DVDLowGetImmBufferReg() & 0xFFFFFF)
        {
        case 0x52100:
            break;
        default:
            goto invalid;
        }

        break;
    }

    default:
        goto fatal;
    }

    lowDone = FALSE;
    DVDLowReportKey((struct DVDVideoReportKey*)CheckBuffer, 0x40000, 0, lowCallback);

    while (!lowDone)
    {
    }

    switch (lowIntType)
    {
    case 2:
        break;
    case 1:
        goto invalid;
    default:
        goto fatal;
    }

    lowDone = FALSE;
    DVDLowRequestError(lowCallback);

    while (!lowDone)
    {
    }

    reportKeyError = DVDLowGetImmBufferReg();
    switch (lowIntType)
    {
    case 1:
        if ((DVDLowGetImmBufferReg() & 0xFF000000) != 0)
        {
            goto recover;
        }

        switch (DVDLowGetImmBufferReg() & 0xFFFFFF)
        {
        case 0x53100:
        case 0x52000:
            break;
        default:
            goto invalid;
        }

        break;

    default:
        goto fatal;
    }

    return TRUE;

invalid:
    __DVDShowDeviceErrorMessage();
    return FALSE;

recover:
    return FALSE;

fatal:
    __DVDShowFatalMessage();
    return FALSE;
}

const char* const __DVDDeviceErrorMessage[] = {
    "\n\n\n\x83\x47\x83\x89\x81\x5B\x83\x52\x81\x5B\x83\x68\x82\x4F\x82\x4F\x82\x50\x81\x42\n"
    "\x95\x73\x96\xBE\x82\xC8\x83\x66\x83\x6F\x83\x43\x83\x58\x82\xAA\x8C\xA9\x82\xC2\x82\xA9\x82\xE8\x82\xDC\x82\xB5\x82\xBD\x81\x42",

    "\n\n\nError #001,\n"
    "unauthorized device has been detected.",

    "\n\n\nFehler #001:\n"
    "Es wurde eine unzul\xE4ssige Komponente\n"
    "entdeckt.",

    "\n\n\nErreur 001:\n"
    "un dispositif non autoris\xE9 a \xE9t\xE9 d\xE9tect\xE9.",

    "\n\n\nError 001:\n"
    "Se ha detectado un dispositivo no\n"
    "autorizado.",

    "\n\n\nErrore #001:\n"
    "rilevato un dispositivo non autorizzato.",

    "\n\n\nFout #001:\n"
    "ongeoorloofd onderdeel gevonden."
};

void __DVDShowDeviceErrorMessage(void)
{
    const char* message;
    const char* const* messageList;
    GXColor bg = { 0, 0, 0, 0 };
    GXColor fg = { 255, 255, 255, 0 };

    if (SCGetLanguage() == 0)
    {
        OSSetFontEncode(1);
    }
    else
    {
        OSSetFontEncode(0);
    }

    messageList = __DVDDeviceErrorMessage;

    if (SCGetLanguage() > 6)
    {
        message = messageList[1];
    }
    else
    {
        message = messageList[SCGetLanguage()];
    }

    OSFatal(fg, bg, message);
}
