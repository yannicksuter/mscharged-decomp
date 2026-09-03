#include <revolution/nand.h>
#include <revolution/vf/vf_struct.h>
#include <revolution/vf/d_common.h>
#include <revolution/vf/d_vf_sys.h>
#include <revolution/vf/nand_drv.h>

s32 VF_nand_sleep_msec;
s32 VF_nand_retry_max;

static struct {
    s32 (*create)(const char*, u8, u8);
    s32 (*open)(const char*, struct NANDFileInfo*, u8);
    s32 (*createDir)(const char*, u8, u8);
    s32 (*delete)(const char*);
} l_nandFunc[26];

static inline void _SleepAfewMiliSec(void) {
    OSSleepTicks(
        (s64)VF_nand_sleep_msec * ((*(u32*)0x800000F8 / 4) / 1000));
}

s32 VFi_NandClose(struct NANDFileInfo* info) {
    s32 challenge;
    s32 error;

    challenge = VF_nand_retry_max;
    error = 0;
    while (challenge-- > 0) {
        error = NANDClose(info);
        if (error != NAND_RESULT_BUSY) {
            return error;
        } else {
            _SleepAfewMiliSec();
        }
    }
    return error;
}

s32 VFi_NandOpen(const char* path, struct NANDFileInfo* info, u8 accType) {
    s32 challenge;
    s32 error;

    challenge = VF_nand_retry_max;
    error = 0;
    while (challenge-- > 0) {
        error = NANDOpen(path, info, accType);
        if (error != NAND_RESULT_BUSY) {
            return error;
        } else {
            _SleepAfewMiliSec();
        }
    }
    return error;
}

s32 VFi_NandPrivateOpen(
    const char* path, struct NANDFileInfo* info, u8 accType) {
    s32 challenge;
    s32 error;

    challenge = VF_nand_retry_max;
    error = 0;
    while (challenge-- > 0) {
        error = NANDPrivateOpen(path, info, accType);
        if (error != NAND_RESULT_BUSY) {
            return error;
        } else {
            _SleepAfewMiliSec();
        }
    }
    return error;
}

s32 VFi_NandFlushNANDFromHandleIdx(s32 i_handleIdx, int i_setLastDeviceError) {
    s32 NANDError;
    struct VF_HANDLE_DRIVE* drive_p;
    struct VF_HANDLE_TYPE* handle_p;
    const char* sys_name_p;
    struct NANDFileInfo* fileInfo_p;

    drive_p = (struct VF_HANDLE_DRIVE*)VFSysGetDriveP(i_handleIdx);
    handle_p = (struct VF_HANDLE_TYPE*)VFSysGetHandleP(i_handleIdx);

    if (handle_p != NULL && handle_p->device_p != NULL && handle_p->device_p->sync_mode == 1) {
        return 0;
    }

    if (drive_p != NULL) {
        sys_name_p = (const char*)drive_p->pf_filename;
        fileInfo_p = drive_p->file_p;
        NANDError = VFi_NandClose(fileInfo_p);

        if (NANDError < 0) {
            if (i_setLastDeviceError != 0) {
                dCommon_setLastDeviceErrorToDisk2(i_handleIdx, NANDError);
            }
            return NANDError;
        }

        if ((u32)i_handleIdx < 26) {
            s32 (*tmpOpen)(const char*, struct NANDFileInfo*, u8) = l_nandFunc[i_handleIdx].open;
            if (tmpOpen != NULL) {
                NANDError = tmpOpen(sys_name_p, fileInfo_p, 3);
            } else {
                NANDError = VFi_NandOpen(sys_name_p, fileInfo_p, 3);
            }
        } else if (i_handleIdx == 0xFFFFFFF6) {
            NANDError = VFi_NandOpen(sys_name_p, fileInfo_p, 3);
        } else {
            NANDError = VFi_NandPrivateOpen(sys_name_p, fileInfo_p, 3);
        }

        if (NANDError < 0) {
            if (i_setLastDeviceError != 0) {
                dCommon_setLastDeviceErrorToDisk2(i_handleIdx, NANDError);
            }
            return NANDError;
        }
    }
    return 0;
}
