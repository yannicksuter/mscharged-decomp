#ifndef REVOLUTION_VI_H
#define REVOLUTION_VI_H

#include <revolution/types.h>

volatile u16 VI_HW_REGS[0x3B] AT_ADDRESS(0xCC002000);

typedef enum VIHwReg
{
    VI_VTR,
    VI_DCR
} VIHwReg;

#endif // REVOLUTION_VI_H
