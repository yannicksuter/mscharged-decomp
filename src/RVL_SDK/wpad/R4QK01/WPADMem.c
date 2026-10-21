#include <revolution/os.h>
#include <revolution/wpad.h>

static inline s32 __wpadCommandResult(BOOL success)
{
    if (success)
        return WPAD_ERR_OK;
    return WPAD_ERR_COMMUNICATION_ERROR;
}

void WPADiClearMemBlock(WPADChannel chan)
{
    WPADCB* p_wpd = _wpdcb[chan];

    p_wpd->memBlock.at_0x00 = 0;
    p_wpd->memBlock.at_0x04 = 0;
    p_wpd->memBlock.at_0x08 = 0;
    p_wpd->memBlock.at_0x0c = 0;
    p_wpd->memBlock.callback = NULL;
}

WPADResult WPADWriteExtReg(WPADChannel chan, const void* data, u16 length,
                          u32 address, WPADCallback callback)
{
    WPADCB* p_wpd = _wpdcb[chan];
    BOOL success;
    BOOL intrStatus;
    BOOL handshakeFinished;
    WPADResult status = WPAD_ERR_OK;

    intrStatus = OSDisableInterrupts();
    status = p_wpd->status;
    handshakeFinished = p_wpd->handshakeFinished;
    OSRestoreInterrupts(intrStatus);

    if (status == WPAD_ERR_NO_CONTROLLER)
        goto end;

    if (!handshakeFinished)
    {
        status = WPAD_ERR_COMMUNICATION_ERROR;
        goto end;
    }

    success = WPADiSendWriteData(&p_wpd->stdCmdQueue, data, length,
                                address | (WPAD_EXT_REG_EXTENSION << 16) | (1 << 26),
                                callback);
    status = __wpadCommandResult(success);

end:
    if (status != WPAD_ERR_OK && callback)
        callback(chan, status);

    return status;
}
