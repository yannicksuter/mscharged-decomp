#ifndef REVOLUTION_OS_HARDWARE_H
#define REVOLUTION_OS_HARDWARE_H

#include <revolution/types.h>

volatile u32 PI_HW_REGS[] AT_ADDRESS(0xCC003000);

typedef enum PIHwReg
{
    PI_INTSR
} PIHwReg;

#define PI_INTSR_RSWST (1 << 16)

#endif // REVOLUTION_OS_HARDWARE_H
