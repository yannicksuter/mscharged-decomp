#include <revolution/net/NETMem.h>

// clang-format off
asm void* NETMemSet(register void* dst, register int value, register u32 size)
{
    cmpwi r5, 0x0
    mr r9, r3
    beqlr
    cmplwi r5, 0x40
    rlwimi r4, r4, 8, 16, 23
    rlwimi r4, r4, 16, 0, 15
    blt Tail
    clrlwi. r8, r3, 27
    beq Blocks
    subfic r8, r8, 0x20
    mr r7, r3
    srwi. r6, r8, 2
    clrlwi r0, r8, 30
    beq HeadBytes
    mtctr r6
HeadWordLoop:
    stw r4, 0x0(r7)
    addi r7, r7, 0x4
    bdnz HeadWordLoop
HeadBytes:
    cmpwi r0, 0x0
    beq HeadDone
    mtctr r0
HeadByteLoop:
    stb r4, 0x0(r7)
    addi r7, r7, 0x1
    bdnz HeadByteLoop
HeadDone:
    add r3, r3, r8
    subf r5, r8, r5
Blocks:
    cmpwi r4, 0x0
    mr r6, r3
    clrrwi r7, r5, 5
    srwi r0, r5, 5
    bne FillBlocks
    mtctr r0
ZeroBlockLoop:
    dcbz r0, r6
    addi r6, r6, 0x20
    bdnz ZeroBlockLoop
    b BlocksDone
FillBlocks:
    mtctr r0
FillBlockLoop:
    dcbz r0, r6
    stw r4, 0x0(r6)
    stw r4, 0x4(r6)
    stw r4, 0x8(r6)
    stw r4, 0xc(r6)
    stw r4, 0x10(r6)
    stw r4, 0x14(r6)
    stw r4, 0x18(r6)
    stw r4, 0x1c(r6)
    addi r6, r6, 0x20
    bdnz FillBlockLoop
BlocksDone:
    add r3, r3, r7
    subf r5, r7, r5
Tail:
    srwi. r6, r5, 2
    clrlwi r0, r5, 30
    beq TailBytes
    mtctr r6
TailWordLoop:
    stw r4, 0x0(r3)
    addi r3, r3, 0x4
    bdnz TailWordLoop
TailBytes:
    cmpwi r0, 0x0
    beq Done
    mtctr r0
TailByteLoop:
    stb r4, 0x0(r3)
    addi r3, r3, 0x1
    bdnz TailByteLoop
Done:
    mr r3, r9
    blr
}
// clang-format on
