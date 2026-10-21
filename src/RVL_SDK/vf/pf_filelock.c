#include <revolution/vf/pf_filelock.h>

#if defined(VERSION_R4QK01)
#include <revolution/vf/pf_system.h>
#else
static int VFipf_sys_set = 0;
#endif

void VFiPF_InitLockFile(void) {
#if defined(VERSION_R4QK01)
    VFipf_sys_set.flock_count = 0;
#else
    VFipf_sys_set = 0;
#endif
}

s32 VFiPF_UnLockFile(struct PF_FILE* p_file) {
    return -1;
}
