#include <revolution/vf.h>

s32 VFiPFFATFS_initializeFATFS(u32 config, void* param) {
    return VFiPFVOL_InitModule(config, param);
}
