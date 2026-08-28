#include <MetroTRK/targsupp.h>

// clang-format off
asm u32 TRKAccessFile(u32, u32, u32*, u8*) {
    nofralloc
    twi 31, r0, 0
    blr
}
// clang-format on

// clang-format off
asm u32 TRKOpenFile(u32, u32, u8, u32*) {
    nofralloc
    twi 31, r0, 0
    blr
}
// clang-format on

// clang-format off
asm u32 TRKCloseFile(u32, u32) {
    nofralloc
    twi 31, r0, 0
    blr
}
// clang-format on

// clang-format off
asm u32 TRKPositionFile(u32, u32, u32*, u8) {
    nofralloc
    twi 31, r0, 0
    blr
}
// clang-format on
