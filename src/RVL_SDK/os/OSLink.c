#include <revolution/os/OSLink.h>

void __OSModuleInit(void) {
    *(int*)0x800030CC = 0;
    *(int*)0x800030C8 = 0;
    *(int*)0x800030D0 = 0;
}
