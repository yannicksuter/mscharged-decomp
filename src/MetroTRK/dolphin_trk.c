#include <MetroTRK/dolphin_trk.h>
#include <MetroTRK/dolphin_trk_glue.h>
#include <MetroTRK/main_TRK.h>
#include <MetroTRK/mem_TRK.h>
#include <MetroTRK/mpc_7xx_603e.h>
#include <MetroTRK/targimpl.h>
#include <revolution/os/OSReset.h>
#include <revolution/os/__ppc_eabi_init.h>

extern u8 gTRKInterruptVectorTable[];
extern void TRK_flush_cache(u32 address, u32 length);

#define EXCEPTIONMASK_ADDR 0x80000044

static u32 lc_base;

static u32 TRK_ISR_OFFSETS[15] = {
    PPC_SystemReset,
    PPC_MachineCheck,
    PPC_DataStorage,
    PPC_InstructionStorage,
    PPC_ExternalInterrupt,
    PPC_Alignment,
    PPC_Program,
    PPC_FloatingPointUnavaiable,
    PPC_Decrementer,
    PPC_SystemCall,
    PPC_Trace,
    PPC_PerformanceMonitor,
    PPC_InstructionAddressBreakpoint,
    PPC_SystemManagementInterrupt,
    PPC_ThermalManagementInterrupt,
};

__declspec(section ".init") void __TRK_reset(void)
{
    OSResetSystem(FALSE, 0, FALSE);
}

void EnableMetroTRKInterrupts(void)
{
    EnableEXI2Interrupts();
}

u32 TRKTargetTranslate(u32 param_0)
{
    if (param_0 >= lc_base)
    {
        if ((param_0 < lc_base + 0x4000) && ((gTRKCPUState.Extended1.DBAT3U & 3) != 0))
        {
            return param_0;
        }
    }
    if ((0x7E000000 <= param_0) && (param_0 <= 0x80000000))
    {
        return param_0;
    }
    return (param_0 & 0x3FFFFFFF) | 0x80000000;
}

static void TRK_copy_vector(u32 offset)
{
    void* destPtr = (void*)TRKTargetTranslate(offset);
    TRK_memcpy(destPtr, (void*)(gTRKInterruptVectorTable + offset), 0x100);
    TRK_flush_cache((u32)destPtr, 0x100);
}

void __TRK_copy_vectors(void)
{
    u32 r3 = lc_base;
    u32* isrOffsetPtr;
    int i;
    u32 r29;

    if (r3 <= 0x44 && r3 + 0x4000 > 0x44 && gTRKCPUState.Extended1.DBAT3U & 3)
    {
        r3 = 0x44;
    }
    else
    {
        r3 = EXCEPTIONMASK_ADDR;
    }

    i = 0;
    r29 = *(u32*)r3;
    isrOffsetPtr = TRK_ISR_OFFSETS;

    do
    {
        if ((r29 & (1 << i)) && i != 4)
        {
            TRK_copy_vector(isrOffsetPtr[i]);
        }

        i++;
    } while (i <= 14);
}

DSError TRKInitializeTarget()
{
    gTRKState.isStopped = TRUE;
    gTRKState.msr = __TRK_get_MSR();
    lc_base = 0xE0000000;
    return kNoError;
}

// clang-format off
asm void InitMetroTRK(void)
{
    nofralloc
    addi r1, r1, -4
    stw r3, 0(r1)
    lis r3, gTRKCPUState@h
    ori r3, r3, gTRKCPUState@l
    stmw r0, ProcessorState_PPC.Default.GPR(r3)
    lwz r4, 0(r1)
    addi r1, r1, 4
    stw r1, ProcessorState_PPC.Default.GPR[1](r3)
    stw r4, ProcessorState_PPC.Default.GPR[3](r3)
    mflr r4
    stw r4, ProcessorState_PPC.Default.LR(r3)
    stw r4, ProcessorState_PPC.Default.PC(r3)
    mfcr r4
    stw r4, ProcessorState_PPC.Default.CR(r3)
    mfmsr r4
    ori r3, r4, (1 << (31 - 16))
    xori r3, r3, (1 << (31 - 16))
    mtmsr r3
    mtsrr1 r4
    bl TRKSaveExtended1Block
    lis r3, gTRKCPUState@h
    ori r3, r3, gTRKCPUState@l
    lmw r0, ProcessorState_PPC.Default.GPR(r3)
    li r0, 0
    mtspr 0x3f2, r0
    mtspr 0x3f5, r0
    lis r1, _db_stack_addr@h
    ori r1, r1, _db_stack_addr@l
    mr r3, r5
    bl InitMetroTRKCommTable
    cmpwi r3, 1
    bne initCommTableSuccess
    lwz r4, ProcessorState_PPC.Default.LR(r3)
    mtlr r4
    lmw r0, ProcessorState_PPC.Default.GPR(r3)
    blr
initCommTableSuccess:
    b TRK_main
    blr
}
// clang-format on

// clang-format off
asm void InitMetroTRK_BBA(void)
{
    nofralloc
    addi r1, r1, -4
    stw r3, 0(r1)
    lis r3, gTRKCPUState@h
    ori r3, r3, gTRKCPUState@l
    stmw r0, ProcessorState_PPC.Default.GPR(r3)
    lwz r4, 0(r1)
    addi r1, r1, 4
    stw r1, ProcessorState_PPC.Default.GPR[1](r3)
    stw r4, ProcessorState_PPC.Default.GPR[3](r3)
    mflr r4
    stw r4, ProcessorState_PPC.Default.LR(r3)
    stw r4, ProcessorState_PPC.Default.PC(r3)
    mfcr r4
    stw r4, ProcessorState_PPC.Default.CR(r3)
    mfmsr r4
    ori r3, r4, (1 << (31 - 16))
    mtmsr r3
    mtsrr1 r4
    bl TRKSaveExtended1Block
    lis r3, gTRKCPUState@h
    ori r3, r3, gTRKCPUState@l
    lmw r0, ProcessorState_PPC.Default.GPR(r3)
    li r0, 0
    mtspr 0x3f2, r0
    mtspr 0x3f5, r0
    lis r1, _db_stack_addr@h
    ori r1, r1, _db_stack_addr@l
    li r3, 2
    bl InitMetroTRKCommTable
    cmpwi r3, 1
    bne initCommTableSuccess
    lwz r4, ProcessorState_PPC.Default.LR(r3)
    mtlr r4
    lmw r0, ProcessorState_PPC.Default.GPR(r3)
    blr
initCommTableSuccess:
    b TRK_main
    blr
}
// clang-format on
