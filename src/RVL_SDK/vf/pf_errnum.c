#include <revolution/vf.h>

int VFipf2_errnum() {
    s32 err;
    err = VFiPFAPI_convertError(VFiPFVOL_errnum());
    return err;
}
