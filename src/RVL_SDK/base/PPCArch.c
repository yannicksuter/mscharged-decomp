#include <revolution/base.h>
#include <revolution/os.h>

// clang-format off
asm u32 PPCMfmsr(void) {
    nofralloc
    mfmsr r3
    blr
}
// clang-format on

// clang-format off
asm void PPCMtmsr(register u32 val) {
    nofralloc
    mtmsr val
    blr
}
// clang-format on

// clang-format off
asm u32 PPCMfhid0(void) {
    nofralloc
    mfhid0 r3
    blr
}
// clang-format on

// clang-format off
asm void PPCMthid0(register u32 val) {
    nofralloc
    mthid0 val
    blr
}
// clang-format on

// clang-format off
asm u32 PPCMfl2cr(void) {
    nofralloc
    mfl2cr r3
    blr
}
// clang-format on

// clang-format off
asm void PPCMtl2cr(register u32 val) {
    nofralloc
    mtl2cr val
    blr
}
// clang-format on

// clang-format off
asm void PPCMtdec(register u32 val) {
    nofralloc
    mtdec val
    blr
}
// clang-format on

// clang-format off
asm void PPCSync(void) {
    nofralloc
    sc
    blr
}
// clang-format on

// clang-format off
asm void PPCHalt(void) {
    nofralloc
    sync
// clang-format on

loop:
    nop
    li r3, 0
    nop
    b loop
}

// clang-format off
asm void PPCMtmmcr0(register u32 val) {
    nofralloc
    mtmmcr0 val
    blr
}
// clang-format on

// clang-format off
asm void PPCMtmmcr1(register u32 val) {
    nofralloc
    mtmmcr1 val
    blr
}
// clang-format on

// clang-format off
asm void PPCMtpmc1(register u32 val) {
    nofralloc
    mtpmc1 val
    blr
}
// clang-format on

// clang-format off
asm void PPCMtpmc2(register u32 val) {
    nofralloc
    mtpmc2 val
    blr
}
// clang-format on

// clang-format off
asm void PPCMtpmc3(register u32 val) {
    nofralloc
    mtpmc3 val
    blr
}
// clang-format on

// clang-format off
asm void PPCMtpmc4(register u32 val){
    nofralloc
    mtpmc4 val
    blr
}
// clang-format on

// clang-format off
u32 PPCMffpscr(void) {
    register u64 fpscr;

    // clang-format off
    asm {
        mffs f31
        stfd f31, fpscr
    }
    // clang-format on

    return fpscr;
}

void PPCMtfpscr(register u32 val) {
    register struct {
        f32 tmp;
        f32 data;
    } fpscr;

    // clang-format off
    asm {
        li r4, 0
        stw val, fpscr.data
        stw r4, fpscr.tmp
        lfd f31, fpscr.tmp
        mtfs f31
    }
    // clang-format on
}

// clang-format off
asm u32 PPCMfhid2(void) {
    nofralloc
    mfspr r3, 0x398
    blr
}
// clang-format on

// clang-format off
asm void PPCMthid2(register u32 val) {
    nofralloc
    mtspr 0x398, val
    blr
}
// clang-format on

// clang-format off
asm u32 PPCMfwpar(void) {
    nofralloc
    sync
    mfwpar r3
    blr
}
// clang-format on

// clang-format off
asm void PPCMtwpar(register u32 val) {
    nofralloc
    mtwpar r3
    blr
}
// clang-format on

void PPCDisableSpeculation(void) {
    PPCMthid0(PPCMfhid0() | HID0_SPD);
}

// clang-format off
asm void PPCSetFpNonIEEEMode(void) {
    nofralloc
    mtfsb1 29
    blr
}
// clang-format on

void PPCMthid4(register u32 val) {
    if (val & HID4_H4A) {
        // clang-format off
        asm {
            mtspr 0x3F3, val
        }
        // clang-format on
    } else {
        OSReport("H4A should not be cleared because of Broadway errata.\n");
        val |= HID4_H4A;
        // clang-format off
        asm {
            mtspr 0x3F3, val
        }
        // clang-format on
    }
}