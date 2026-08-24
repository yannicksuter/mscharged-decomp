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
void PPCMtmmcr0(u32 value);
void PPCMtmmcr1(u32 value);
void PPCMtpmc1(u32 value);
void PPCMtpmc2(u32 value);
void PPCMtpmc3(u32 value);
void PPCMtpmc4(u32 value);
u32 PPCMfhid2(void);
void PPCMthid2(u32 value);
u32 PPCMfwpar(void);
void PPCMtwpar(u32 value);
void PPCDisableSpeculation(void);
void PPCSetFpNonIEEEMode(void);
void PPCMthid4(u32 value);

#ifdef __cplusplus
}
#endif

#define MSR_POW (1 << (31 - 13))
#define MSR_ILE (1 << (31 - 15))
#define MSR_EE (1 << (31 - 16))
#define MSR_PR (1 << (31 - 17))
#define MSR_FP (1 << (31 - 18))
#define MSR_ME (1 << (31 - 19))
#define MSR_FE0 (1 << (31 - 20))
#define MSR_SE (1 << (31 - 21))
#define MSR_BE (1 << (31 - 22))
#define MSR_FE1 (1 << (31 - 23))
#define MSR_IP (1 << (31 - 25))
#define MSR_IR (1 << (31 - 26))
#define MSR_DR (1 << (31 - 27))
#define MSR_PM (1 << (31 - 29))
#define MSR_RI (1 << (31 - 30))
#define MSR_LE (1 << (31 - 31))
#define HID0_EMCP (1 << (31 - 0))
#define HID0_DBP (1 << (31 - 1))
#define HID0_EBA (1 << (31 - 2))
#define HID0_EBD (1 << (31 - 3))
#define HID0_BCLK (1 << (31 - 4))
#define HID0_ECLK (1 << (31 - 6))
#define HID0_PAR (1 << (31 - 7))
#define HID0_DOZE (1 << (31 - 8))
#define HID0_NAP (1 << (31 - 9))
#define HID0_SLEEP (1 << (31 - 10))
#define HID0_DPM (1 << (31 - 11))
#define HID0_NHR (1 << (31 - 15))
#define HID0_ICE (1 << (31 - 16))
#define HID0_DCE (1 << (31 - 17))
#define HID0_ILOCK (1 << (31 - 18))
#define HID0_DLOCK (1 << (31 - 19))
#define HID0_ICFI (1 << (31 - 20))
#define HID0_DCFI (1 << (31 - 21))
#define HID0_SPD (1 << (31 - 22))
#define HID0_IFEM (1 << (31 - 23))
#define HID0_SGE (1 << (31 - 24))
#define HID0_DCFA (1 << (31 - 25))
#define HID0_BTIC (1 << (31 - 26))
#define HID0_ABE (1 << (31 - 28))
#define HID0_BHT (1 << (31 - 29))
#define HID0_NOOPTI (1 << (31 - 31))
#define HID1_PC0 (1 << (31 - 0))
#define HID1_PC1 (1 << (31 - 1))
#define HID1_PC2 (1 << (31 - 2))
#define HID1_PC3 (1 << (31 - 3))
#define HID1_PC4 (1 << (31 - 4))
#define L2CR_L2E (1 << (31 - 0))
#define L2CR_L2CE (1 << (31 - 1))
#define L2CR_L2DO (1 << (31 - 9))
#define L2CR_L2I (1 << (31 - 10))
#define L2CR_L2WT (1 << (31 - 12))
#define L2CR_L2TS (1 << (31 - 13))
#define L2CR_L2IP (1 << (31 - 31))
#define FPSCR_FX (1 << (31 - 0))
#define FPSCR_FEX (1 << (31 - 1))
#define FPSCR_VX (1 << (31 - 2))
#define FPSCR_OX (1 << (31 - 3))
#define FPSCR_UX (1 << (31 - 4))
#define FPSCR_ZX (1 << (31 - 5))
#define FPSCR_XX (1 << (31 - 6))
#define FPSCR_VXSNAN (1 << (31 - 7))
#define FPSCR_VXISI (1 << (31 - 8))
#define FPSCR_VXIDI (1 << (31 - 9))
#define FPSCR_VXZDZ (1 << (31 - 10))
#define FPSCR_VXIMZ (1 << (31 - 11))
#define FPSCR_VXVC (1 << (31 - 12))
#define FPSCR_FR (1 << (31 - 13))
#define FPSCR_FI (1 << (31 - 14))
#define FPSCR_FPRF (0x1f << (31 - 19))
#define FPSCR_UNK20 (1 << (31 - 20))
#define FPSCR_VXSOFT (1 << (31 - 21))
#define FPSCR_VXSQRT (1 << (31 - 22))
#define FPSCR_VXCVI (1 << (31 - 23))
#define FPSCR_VE (1 << (31 - 24))
#define FPSCR_OE (1 << (31 - 25))
#define FPSCR_UE (1 << (31 - 26))
#define FPSCR_ZE (1 << (31 - 27))
#define FPSCR_XE (1 << (31 - 28))
#define FPSCR_NI (1 << (31 - 29))
#define FPSCR_RN (0x3 << (31 - 31))
#define HID2_WPE (1 << (31 - 1))
#define HID2_PSE (1 << (31 - 2))
#define HID2_LCE (1 << (31 - 3))
#define HID2_DMAQL                                                             \
    ((1 << (31 - 4)) | (1 << (31 - 5)) | (1 << (31 - 6)) | (1 << (31 - 7)))
#define HID2_DCHERR (1 << (31 - 8))
#define HID2_DNCERR (1 << (31 - 9))
#define HID2_DCMERR (1 << (31 - 10))
#define HID2_DQOERR (1 << (31 - 11))
#define HID2_DCHEE (1 << (31 - 12))
#define HID2_DNCEE (1 << (31 - 13))
#define HID2_DCMEE (1 << (31 - 14))
#define HID2_DQOEE (1 << (31 - 15))
#define WPAR_GB_ADDR (0x07FFFFFF << (31 - 26))
#define WPAR_BNE (1 << (31 - 31))
#define HID4_H4A (1 << (31 - 0))
#define HID4_L2FM (0x3 << (31 - 2))
#define HID4_BPD (0x3 << (31 - 4))
#define HID4_BCO (1 << (31 - 5))
#define HID4_SBE (1 << (31 - 6))
#define HID4_PS1_CTL (1 << (31 - 7))
#define HID4_DBP (1 << (31 - 9))
#define HID4_L2MUM (1 << (31 - 10))
#define HID4_L2_CCFI (1 << (31 - 11))
#define HID4_PSS2_CTL (1 << (31 - 12))

#endif  // REVOLUTION_BASE_H
