#include <revolution/ax.h>
#include <revolution/axfx.h>
#include <revolution/os.h>

#include <string.h>

static BOOL __AllocDelayLine(AXFX_DELAY_EXP_DPL2* fx);
static void __FreeDelayLine(AXFX_DELAY_EXP_DPL2* fx);
static BOOL __InitParams(AXFX_DELAY_EXP_DPL2* fx) DECOMP_DONT_INLINE;

u32 AXFXDelayExpGetMemSizeDpl2(const AXFX_DELAY_EXP_DPL2* fx) {
    u32 num = 0;
    num += fx->delay[0];
    num += fx->delay[1];
    num += fx->delay[2];
    num += fx->delay[3];

    return num * 128;
}

BOOL AXFXDelayExpInitDpl2(AXFX_DELAY_EXP_DPL2* fx) {
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();
    if (AXGetMode() != AX_OUTPUT_DPL2) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    fx->active = 1;

    for (i = 0; i < ARRAY_SIZE(fx->delay); i++) {
        if (fx->delay[i] == 0) {
            AXFXDelayExpShutdownDpl2(fx);
            OSRestoreInterrupts(enabled);
            return FALSE;
        }

        fx->length[i] = fx->delay[i] * 32;
    }

    if (!__AllocDelayLine(fx)) {
        AXFXDelayExpShutdownDpl2(fx);
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    if (!__InitParams(fx)) {
        AXFXDelayExpShutdownDpl2(fx);
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    fx->active |= 2;
    fx->active &= ~1;
    OSRestoreInterrupts(enabled);

    return TRUE;
}

BOOL AXFXDelayExpSettingsDpl2(AXFX_DELAY_EXP_DPL2* fx) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    fx->active |= 1;
    AXFXDelayExpShutdownDpl2(fx);

    if (!AXFXDelayExpInitDpl2(fx)) {
        AXFXDelayExpShutdownDpl2(fx);
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    fx->active |= 2;
    fx->active &= ~1;
    OSRestoreInterrupts(enabled);

    return TRUE;
}

void AXFXDelayExpShutdownDpl2(AXFX_DELAY_EXP_DPL2* fx) {
    BOOL enabled;

    enabled = OSDisableInterrupts();

    fx->active |= 1;
    __FreeDelayLine(fx);

    OSRestoreInterrupts(enabled);
}

void AXFXDelayExpCallbackDpl2(AXFX_BUFFERUPDATE_DPL2* update, AXFX_DELAY_EXP_DPL2* fx) {
    s32* lp;
    s32* rp;
    s32* lsp;
    s32* rsp;
    s32 lv, rv, lsv, rsv;
    int i;

    if (fx->active) {
        fx->active &= ~2;
        return;
    }

    lp = update->left;
    rp = update->right;
    lsp = update->left_surround;
    rsp = update->right_surround;

    for (i = 0; i < AX_SAMPLES_PER_FRAME; i++) {
        lv = fx->line[0][fx->curPos[0]];
        rv = fx->line[1][fx->curPos[1]];
        lsv = fx->line[2][fx->curPos[2]];
        rsv = fx->line[3][fx->curPos[3]];

        fx->line[0][fx->curPos[0]] = ((lv * fx->feedbackGain[0]) >> 7) + *lp;
        fx->line[1][fx->curPos[1]] = ((rv * fx->feedbackGain[1]) >> 7) + *rp;
        fx->line[2][fx->curPos[2]] = ((lsv * fx->feedbackGain[2]) >> 7) + *lsp;
        fx->line[3][fx->curPos[3]] = ((rsv * fx->feedbackGain[3]) >> 7) + *rsp;

        if (++fx->curPos[0] >= fx->length[0]) {
            fx->curPos[0] = 0;
        }

        if (++fx->curPos[1] >= fx->length[1]) {
            fx->curPos[1] = 0;
        }

        if (++fx->curPos[2] >= fx->length[2]) {
            fx->curPos[2] = 0;
        }

        if (++fx->curPos[3] >= fx->length[3]) {
            fx->curPos[3] = 0;
        }

        *lp++ = (lv * fx->outGain[0]) >> 7;
        *rp++ = (rv * fx->outGain[1]) >> 7;
        *lsp++ = (lsv * fx->outGain[2]) >> 7;
        *rsp++ = (rsv * fx->outGain[3]) >> 7;
    }
}

static BOOL __AllocDelayLine(AXFX_DELAY_EXP_DPL2* fx) {
    int i;

    for (i = 0; i < ARRAY_SIZE(fx->line); i++) {
        fx->line[i] = __AXFXAlloc(fx->length[i] * sizeof(s32));
        if (fx->line[i] == NULL) {
            return FALSE;
        }
    }

    return TRUE;
}

static void __FreeDelayLine(AXFX_DELAY_EXP_DPL2* fx) {
    int i;

    for (i = 0; i < ARRAY_SIZE(fx->line); i++) {
        if (fx->line[i] != NULL) {
            __AXFXFree(fx->line[i]);
            fx->line[i] = NULL;
        }
    }
}

static BOOL __InitParams(AXFX_DELAY_EXP_DPL2* fx) {
    int i;

    for (i = 0; i < ARRAY_SIZE(fx->line); i++) {
        if (fx->feedback[i] >= 100) {
            return FALSE;
        }

        if (fx->output[i] > 100) {
            return FALSE;
        }

        if (fx->line[i] == NULL) {
            return FALSE;
        }

        memset(fx->line[i], 0, fx->length[i] * sizeof(s32));
        fx->curPos[i] = 0;

        fx->feedbackGain[i] = 128.0f * fx->feedback[i] / 100.0f;
        fx->outGain[i] = 128.0f * fx->output[i] / 100.0f;
    }

    return TRUE;
}
