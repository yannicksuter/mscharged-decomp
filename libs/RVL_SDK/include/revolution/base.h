#ifndef REVOLUTION_BASE_H
#define REVOLUTION_BASE_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

u32 PPCMfmsr(void);
void PPCMtmsr(u32 value);
u32 PPCMfhid0(void);
void PPCMthid0(u32 value);
u32 PPCMfl2cr(void);
void PPCMtl2cr(u32 value);
void PPCMtdec(u32 value);
void PPCSync(void);
void PPCHalt(void);
u32 PPCMffpscr(void);
void PPCMtfpscr(u32 value);
u32 PPCMfhid2(void);
void PPCMthid2(u32 value);

#ifdef __cplusplus
}
#endif

#define MSR_EE (1 << (31 - 16))
#define MSR_FP (1 << (31 - 18))
#define MSR_FE0 (1 << (31 - 20))
#define MSR_FE1 (1 << (31 - 23))
#define MSR_IR (1 << (31 - 26))
#define MSR_DR (1 << (31 - 27))

#define HID0_ICE (1 << (31 - 16))
#define HID0_DCE (1 << (31 - 17))

#define L2CR_L2E (1 << (31 - 0))
#define L2CR_L2I (1 << (31 - 10))
#define L2CR_L2IP (1 << (31 - 31))

#define FPSCR_FEX (1 << (31 - 1))
#define FPSCR_VX (1 << (31 - 2))
#define FPSCR_FR (1 << (31 - 13))
#define FPSCR_FPRF (0x1F << (31 - 19))
#define FPSCR_UNK20 (1 << (31 - 20))
#define FPSCR_VE (1 << (31 - 24))
#define FPSCR_OE (1 << (31 - 25))
#define FPSCR_UE (1 << (31 - 26))
#define FPSCR_ZE (1 << (31 - 27))
#define FPSCR_XE (1 << (31 - 28))
#define FPSCR_NI (1 << (31 - 29))
#define FPSCR_RN (3 << (31 - 31))

#define HID2_DCHERR (1 << (31 - 8))
#define HID2_DNCERR (1 << (31 - 9))
#define HID2_DCMERR (1 << (31 - 10))
#define HID2_DQOERR (1 << (31 - 11))

#endif  // REVOLUTION_BASE_H
