#include <revolution/vf/pf_filelock.h>

static int VFipf_sys_set = 0;

void VFiPF_InitLockFile(void) {
    VFipf_sys_set = 0;
}

s32 VFiPF_UnLockFile(struct PF_FILE* p_file) {
    return -1;
}
