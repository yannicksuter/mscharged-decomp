#include <revolution/os.h>

static void* SaveEnd;
static void* SaveStart;

extern void __OSBootDol(u32 bootDol, u32 resetCode, char** argv);

void __OSReboot(u32 resetCode, u32 bootDol) {
    char* argvToPass;

    OSDisableInterrupts();
    OSSetArenaLo((void*)0x81280000);
    OSSetArenaHi((void*)0x812F0000);
    argvToPass = NULL;
    __OSBootDol(bootDol, resetCode | 0x80000000, &argvToPass);
}

void OSGetSaveRegion(void** start, void** end) {
    *start = SaveStart;
    *end = SaveEnd;
}
