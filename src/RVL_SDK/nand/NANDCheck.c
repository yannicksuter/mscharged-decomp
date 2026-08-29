#include <revolution/nand.h>
#include <revolution/os.h>

const char* USER_DIR_LIST[] = {"/meta",
                               "/ticket",
                               "/title/00010000",
                               "/title/00010001",
                               "/title/00010003",
                               "/title/00010004",
                               "/title/00010005",
                               "/title/00010006",
                               "/title/00010007",
                               "/shared2/title",
                               NULL};

void nandUserAreaCallback(s32 result, void* arg);

static s32 nandCalcUsage(s32* blockCountOut, s32* fileCountOut,
                         const char** dirList) {
    s32 result = -117;

    *blockCountOut = 0;
    *fileCountOut = 0;

    for (; *dirList != NULL; dirList++) {
        s32 blockCount = 0;
        s32 fileCount = 0;

        result = ISFS_GetUsage(*dirList, &blockCount, &fileCount);
        if (result == IPC_RESULT_OK) {
            *blockCountOut += blockCount;
            *fileCountOut += fileCount;
        } else if (result == IPC_RESULT_NOEXISTS) {
            result = IPC_RESULT_OK;
        } else {
            break;
        }
    }

    return result;
}

static s32 nandCalcUserUsage(s32* blockCountOut, s32* fileCountOut) {
    return nandCalcUsage(blockCountOut, fileCountOut, USER_DIR_LIST);
}

static u32 nandCheck(u32 neededBlocks, u32 neededFiles, u32 homeDirBlocks,
                     u32 homeDirFiles, u32 userBlocks, u32 userFiles) {
    u32 answer = 0;

    if (homeDirBlocks + neededBlocks > 0x400) {
        answer |= NAND_CHECK_TOO_MANY_APP_BLOCKS;
    }

    if (homeDirFiles + neededFiles > 0x21) {
        answer |= NAND_CHECK_TOO_MANY_APP_FILES;
    }

    if (userBlocks + neededBlocks > 0x4400) {
        answer |= NAND_CHECK_TOO_MANY_USER_BLOCKS;
    }

    if (userFiles + neededFiles > 0xFA0) {
        answer |= NAND_CHECK_TOO_MANY_USER_FILES;
    }

    return answer;
}

s32 NANDCheck(u32 neededBlocks, u32 neededFiles, u32* answer) {
    s32 result;

    s32 homeDirBlocks = -1;
    s32 homeDirFiles = -1;
    s32 userBlocks = -1;
    s32 userFiles = -1;

    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    result = ISFS_GetUsage(nandGetHomeDir(), &homeDirBlocks, &homeDirFiles);
    if (result != IPC_RESULT_OK) {
        return nandConvertErrorCode(result);
    }

    result = nandCalcUserUsage(&userBlocks, &userFiles);
    if (result != IPC_RESULT_OK) {
        return nandConvertErrorCode(result);
    }

    *answer = nandCheck(neededBlocks, neededFiles, homeDirBlocks, homeDirFiles,
                        userBlocks, userFiles);

    return NAND_RESULT_OK;
}

s32 NANDCheckAsync(u32 neededBlocks, u32 neededFiles, u32* answer,
                   NANDAsyncCallback callback, NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = callback;
    block->reqBlocks = neededBlocks;
    block->reqInodes = neededFiles;
    block->answer = answer;
    block->userBlocks = 0;
    block->userInodes = 0;
    block->workBlocks = 0;
    block->workInodes = 0;
    block->dir = USER_DIR_LIST;
    block->state = 20;

    return nandConvertErrorCode(
        ISFS_GetUsageAsync(nandGetHomeDir(), (s32*)&block->homeBlocks,
                           (s32*)&block->homeInodes, nandUserAreaCallback,
                           block));
}

void nandUserAreaCallback(s32 result, void* arg) {
    NANDCommandBlock* block = (NANDCommandBlock*)arg;

    if (result == IPC_RESULT_OK || result == IPC_RESULT_NOEXISTS) {
        const char* dir = *block->dir;

        if (result == IPC_RESULT_OK) {
            block->userBlocks += block->workBlocks;
            block->userInodes += block->workInodes;
        }

        if (dir != NULL) {
            block->dir++;
            result = ISFS_GetUsageAsync(
                dir, (s32*)&block->workBlocks, (s32*)&block->workInodes,
                nandUserAreaCallback, arg);

            if (result != IPC_RESULT_OK) {
                block->callback(nandConvertErrorCode(result), block);
            }
        } else if (block->state == 20) {
            *block->answer =
                nandCheck(block->reqBlocks, block->reqInodes,
                          block->homeBlocks, block->homeInodes,
                          block->userBlocks, block->userInodes);
            block->callback(nandConvertErrorCode(IPC_RESULT_OK), block);
        } else if (block->state == 21) {
            u32 userBlocks = block->userBlocks > 0x4400
                                 ? 0
                                 : 0x4400 - block->userBlocks;
            u32 userInodes = block->userInodes > 0xFA0
                                 ? 0
                                 : 0xFA0 - block->userInodes;
            u32 homeBlocks = block->homeBlocks > 0x400
                                 ? 0
                                 : 0x400 - block->homeBlocks;
            u32 homeInodes = block->homeInodes > 0x21
                                 ? 0
                                 : 0x21 - block->homeInodes;

            *(u32*)block->bytes =
                userBlocks < homeBlocks ? userBlocks : homeBlocks;
            *(u32*)block->inodes =
                userInodes < homeInodes ? userInodes : homeInodes;
            block->callback(nandConvertErrorCode(IPC_RESULT_OK), block);
        } else {
            OSReport("Illegal status is detected at %s()", __FUNCTION__);
            block->callback(nandConvertErrorCode(-117), block);
        }
    } else {
        block->callback(nandConvertErrorCode(result), block);
    }
}
