#include <revolution/os.h>
#include <revolution/sp.h>

void SPInitSoundTable(SPSoundTable* table, u32 aramBase) {
    int i;
    SPSoundEntry* sound;
    SPADPCM* adpcm;
    u32 aramBase4, aramBase8, aramBase16;

    aramBase -= 0x80000000;
    aramBase4 = aramBase << 1;
    aramBase8 = aramBase;
    aramBase16 = aramBase >> 1;

    sound = &table->sound[0];
    adpcm = (SPADPCM*)&table->sound[table->entries];

    for (i = 0; i < table->entries; i++) {
        switch (sound->type) {
        case 0:
            sound->loopAddr = aramBase4 + sound->currentAddr;
            sound->loopEndAddr = 0;
            sound->endAddr = aramBase4 + sound->endAddr;
            sound->currentAddr = aramBase4 + sound->currentAddr;
            sound->adpcm = adpcm;
            adpcm++;
            break;
        case 1:
            sound->loopAddr = aramBase4 + sound->loopAddr;
            sound->loopEndAddr = aramBase4 + sound->loopEndAddr;
            sound->endAddr = aramBase4 + sound->endAddr;
            sound->currentAddr = aramBase4 + sound->currentAddr;
            sound->adpcm = adpcm;
            adpcm++;
            break;
        case 2:
            sound->loopAddr = aramBase16 + sound->currentAddr;
            sound->loopEndAddr = 0;
            sound->endAddr = aramBase16 + sound->endAddr;
            sound->currentAddr = aramBase16 + sound->currentAddr;
            break;
        case 3:
            sound->loopAddr = aramBase16 + sound->loopAddr;
            sound->loopEndAddr = aramBase16 + sound->loopEndAddr;
            sound->endAddr = aramBase16 + sound->endAddr;
            sound->currentAddr = aramBase16 + sound->currentAddr;
            break;
        case 4:
            sound->loopAddr = aramBase8 + sound->currentAddr;
            sound->loopEndAddr = 0;
            sound->endAddr = aramBase8 + sound->endAddr;
            sound->currentAddr = aramBase8 + sound->currentAddr;
            break;
        case 5:
            sound->loopAddr = aramBase8 + sound->loopAddr;
            sound->loopEndAddr = aramBase8 + sound->loopEndAddr;
            sound->endAddr = aramBase8 + sound->endAddr;
            sound->currentAddr = aramBase8 + sound->currentAddr;
            break;
        }
        sound++;
    }
}

SPSoundEntry* SPGetSoundEntry(SPSoundTable* table, u32 index) {
    if (table->entries > index) {
        return &table->sound[index];
    }

    return NULL;
}

void SPPrepareSound(SPSoundEntry* sound, AXVPB* axvpb, u32 sampleRate) {
    BOOL old;
    u32 srcBits;
    u32 loopAddr, endAddr, currentAddr;
    u16* p;
    u16* p1;

    srcBits = 0x10000 * ((f32)sampleRate / 32000);

    switch (sound->type) {
    case 0:
        loopAddr = sound->loopAddr;
        endAddr = sound->endAddr;
        currentAddr = sound->currentAddr;

        p = (u16*)&axvpb->pb.addr;
        p1 = (u16*)sound->adpcm;

        old = OSDisableInterrupts();

        *p++ = 0;
        *p++ = 0;
        *p++ = (u16)(loopAddr >> 0x10);
        *p++ = (u16)(loopAddr & 0xFFFF);
        *p++ = (u16)(endAddr >> 0x10);
        *p++ = (u16)(endAddr & 0xFFFF);
        *p++ = (u16)(currentAddr >> 0x10);
        *p++ = (u16)(currentAddr & 0xFFFF);
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = (u16)(srcBits >> 0x10);
        *p++ = (u16)(srcBits & 0xFFFF);
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;

        axvpb->sync |= 0x18400;
        OSRestoreInterrupts(old);
        break;
    case 1:
        loopAddr = sound->loopAddr;
        endAddr = sound->loopEndAddr;
        currentAddr = sound->currentAddr;

        p = (u16*)&axvpb->pb.addr;
        p1 = (u16*)sound->adpcm;

        old = OSDisableInterrupts();

        *p++ = 1;
        *p++ = 0;
        *p++ = (u16)(loopAddr >> 0x10);
        *p++ = (u16)(loopAddr & 0xFFFF);
        *p++ = (u16)(endAddr >> 0x10);
        *p++ = (u16)(endAddr & 0xFFFF);
        *p++ = (u16)(currentAddr >> 0x10);
        *p++ = (u16)(currentAddr & 0xFFFF);
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = (u16)(srcBits >> 0x10);
        *p++ = (u16)(srcBits & 0xFFFF);
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = *p1++;
        *p++ = *p1++;
        *p++ = *p1++;

        axvpb->sync |= 0x58400;
        OSRestoreInterrupts(old);
        break;
    case 2:
        loopAddr = sound->loopAddr;
        endAddr = sound->endAddr;
        currentAddr = sound->currentAddr;

        p = (u16*)&axvpb->pb.addr;

        old = OSDisableInterrupts();

        *p++ = 0;
        *p++ = 10;
        *p++ = (u16)(loopAddr >> 0x10);
        *p++ = (u16)(loopAddr & 0xFFFF);
        *p++ = (u16)(endAddr >> 0x10);
        *p++ = (u16)(endAddr & 0xFFFF);
        *p++ = (u16)(currentAddr >> 0x10);
        *p++ = (u16)(currentAddr & 0xFFFF);
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0x800;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = (u16)(srcBits >> 0x10);
        *p++ = (u16)(srcBits & 0xFFFF);
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;

        axvpb->sync |= 0x18400;
        OSRestoreInterrupts(old);
        break;
    case 3:
        loopAddr = sound->loopAddr;
        endAddr = sound->loopEndAddr;
        currentAddr = sound->currentAddr;

        p = (u16*)&axvpb->pb.addr;

        old = OSDisableInterrupts();

        *p++ = 1;
        *p++ = 10;
        *p++ = (u16)(loopAddr >> 0x10);
        *p++ = (u16)(loopAddr & 0xFFFF);
        *p++ = (u16)(endAddr >> 0x10);
        *p++ = (u16)(endAddr & 0xFFFF);
        *p++ = (u16)(currentAddr >> 0x10);
        *p++ = (u16)(currentAddr & 0xFFFF);
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0x800;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = (u16)(srcBits >> 0x10);
        *p++ = (u16)(srcBits & 0xFFFF);
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;

        axvpb->sync |= 0x18400;
        OSRestoreInterrupts(old);
        break;
    case 4:
        loopAddr = sound->loopAddr;
        endAddr = sound->endAddr;
        currentAddr = sound->currentAddr;

        p = (u16*)&axvpb->pb.addr;

        old = OSDisableInterrupts();

        *p++ = 0;
        *p++ = 0x19;
        *p++ = (u16)(loopAddr >> 0x10);
        *p++ = (u16)(loopAddr & 0xFFFF);
        *p++ = (u16)(endAddr >> 0x10);
        *p++ = (u16)(endAddr & 0xFFFF);
        *p++ = (u16)(currentAddr >> 0x10);
        *p++ = (u16)(currentAddr & 0xFFFF);
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0x100;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = (u16)(srcBits >> 0x10);
        *p++ = (u16)(srcBits & 0xFFFF);
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;

        axvpb->sync |= 0x18400;
        OSRestoreInterrupts(old);
        break;
    case 5:
        loopAddr = sound->loopAddr;
        endAddr = sound->loopEndAddr;
        currentAddr = sound->currentAddr;

        p = (u16*)&axvpb->pb.addr;

        old = OSDisableInterrupts();

        *p++ = 1;
        *p++ = 0x19;
        *p++ = (u16)(loopAddr >> 0x10);
        *p++ = (u16)(loopAddr & 0xFFFF);
        *p++ = (u16)(endAddr >> 0x10);
        *p++ = (u16)(endAddr & 0xFFFF);
        *p++ = (u16)(currentAddr >> 0x10);
        *p++ = (u16)(currentAddr & 0xFFFF);
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0x100;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = (u16)(srcBits >> 0x10);
        *p++ = (u16)(srcBits & 0xFFFF);
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;

        axvpb->sync |= 0x18400;
        OSRestoreInterrupts(old);
        break;
    }
}

void SPPrepareEnd(SPSoundEntry* sound, AXVPB* axvpb) {
    BOOL old;

    old = OSDisableInterrupts();

    axvpb->pb.addr.loopFlag = 0;
    axvpb->pb.addr.endAddressHi = sound->endAddr >> 0x10;
    axvpb->pb.addr.endAddressLo = sound->endAddr & 0xFFFF;

    axvpb->sync |= 0xA000;
    OSRestoreInterrupts(old);
}
