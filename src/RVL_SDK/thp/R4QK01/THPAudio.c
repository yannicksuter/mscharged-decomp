#include <revolution/thp.h>

u32 THPAudioDecode(s16* audioBuffer, u8* audioFrame, s32 flag) {
    THPAudioRecordHeader* header;
    THPAudioDecodeInfo decInfoL, decInfoR;
    u8 *left, *right;
    s16 *decLeftPtr, *decRightPtr;
    s16 lYn1, lYn2, rYn1, rYn2;
    s32 i;
    s32 step;
    s32 sample;
    s64 ynL, ynR;
    BOOL mono;

    if (audioBuffer == NULL || audioFrame == NULL) {
        return 0;
    }

    header = (THPAudioRecordHeader*)audioFrame;
    left = audioFrame + sizeof(THPAudioRecordHeader);
    right = left + header->offsetNextChannel;

    if (flag & 1) {
        decRightPtr = audioBuffer;
        decLeftPtr = audioBuffer + header->sampleSize;
        step = 1;
    } else {
        decRightPtr = audioBuffer;
        decLeftPtr = audioBuffer + 1;
        step = 2;
    }

    mono = FALSE;
    if (flag & 2) {
        mono = TRUE;
    }

    if (header->offsetNextChannel == 0) {
        __THPAudioInitialize(&decInfoL, left);
        lYn1 = header->lYn1;
        lYn2 = header->lYn2;

        for (i = 0; i < header->sampleSize; i++) {
            sample = __THPAudioGetNewSample(&decInfoL);
            ynL = header->lCoef[decInfoL.predictor][1] * lYn2;
            ynL += header->lCoef[decInfoL.predictor][0] * lYn1;
            ynL += (sample << decInfoL.scale) << 11;
            ynL += 0x400;
            ynL >>= 11;

            if (ynL > 32767) {
                ynL = 32767;
            } else if (ynL < -32768) {
                ynL = -32768;
            }

            lYn2 = lYn1;
            lYn1 = (s16)ynL;

            if (mono) {
                ynL <<= 1;
                ynL *= 0x5A82;
                ynL += 0x4000;
                ynL >>= 15;
                if (ynL > 32767) {
                    ynL = 32767;
                } else if (ynL < -32768) {
                    ynL = -32768;
                }
            }

            *decLeftPtr = (s16)ynL;
            decLeftPtr += step;
            *decRightPtr = (s16)ynL;
            decRightPtr += step;
        }
    } else {
        __THPAudioInitialize(&decInfoL, left);
        __THPAudioInitialize(&decInfoR, right);
        lYn1 = header->lYn1;
        lYn2 = header->lYn2;
        rYn1 = header->rYn1;
        rYn2 = header->rYn2;

        for (i = 0; i < header->sampleSize; i++) {
            sample = __THPAudioGetNewSample(&decInfoL);
            ynL = header->lCoef[decInfoL.predictor][1] * lYn2;
            ynL += header->lCoef[decInfoL.predictor][0] * lYn1;
            ynL += (sample << decInfoL.scale) << 11;
            ynL += 0x400;
            ynL >>= 11;

            if (ynL > 32767) {
                ynL = 32767;
            } else if (ynL < -32768) {
                ynL = -32768;
            }

            lYn2 = lYn1;
            lYn1 = (s16)ynL;

            sample = __THPAudioGetNewSample(&decInfoR);
            ynR = header->rCoef[decInfoR.predictor][1] * rYn2;
            ynR += header->rCoef[decInfoR.predictor][0] * rYn1;
            ynR += (sample << decInfoR.scale) << 11;
            ynR += 0x400;
            ynR >>= 11;

            if (ynR > 32767) {
                ynR = 32767;
            } else if (ynR < -32768) {
                ynR = -32768;
            }

            rYn2 = rYn1;
            rYn1 = (s16)ynR;

            if (mono) {
                ynL += ynR;
                ynL *= 0x5A82;
                ynL += 0x4000;
                ynL >>= 15;
                if (ynL > 32767) {
                    ynL = 32767;
                } else if (ynL < -32768) {
                    ynL = -32768;
                }
                ynR = ynL;
            }

            *decLeftPtr = (s16)ynL;
            decLeftPtr += step;
            *decRightPtr = (s16)ynR;
            decRightPtr += step;
        }
    }

    return header->sampleSize;
}

static s32 __THPAudioGetNewSample(THPAudioDecodeInfo* info) {
    s32 sample;

    if (!(info->offsetNibbles & 0x0f)) {
        info->predictor = (u8)((*(info->encodeData) & 0x70) >> 4);
        info->scale = (u8)((*(info->encodeData) & 0xF));
        info->encodeData++;
        info->offsetNibbles += 2;
    }

    if (info->offsetNibbles & 0x1) {
        sample = (s32)((*(info->encodeData) & 0xF) << 28) >> 28;
        info->encodeData++;
    } else {
        sample = (s32)((*(info->encodeData) & 0xF0) << 24) >> 28;
    }

    info->offsetNibbles++;
    return sample;
}

static void __THPAudioInitialize(THPAudioDecodeInfo* info, u8* ptr) {
    info->encodeData = ptr;
    info->offsetNibbles = 2;
    info->predictor = (u8)((*(info->encodeData) & 0x70) >> 4);
    info->scale = (u8)((*(info->encodeData) & 0xF));
    info->encodeData++;
}
