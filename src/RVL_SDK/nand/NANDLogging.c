#include <revolution/nand.h>
#include <revolution/nand/NANDLogging.h>
#include <revolution/os.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static s32 s_fd = -255;
static int s_stage;
static char s_message[256] __attribute__((aligned(64)));
static NANDLoggingCallback s_callback;

static void asyncRoutine(s32 result, void* context);
static void prepareLine(char line[256], int n, const char* message);

static BOOL reserveFileDescriptor(void) {
    BOOL enabled = FALSE;
    BOOL busy = FALSE;

    enabled = OSDisableInterrupts();

    if (s_fd == -255) {
        s_fd = -254;
        busy = FALSE;
    } else if (s_fd == -254) {
        busy = TRUE;
    } else if (s_fd >= 0) {
        busy = TRUE;
    } else {
        busy = TRUE;
    }

    OSRestoreInterrupts(enabled);
    return busy ? FALSE : TRUE;
}

BOOL NANDLoggingAddMessageAsync(NANDLoggingCallback callback,
                               const char* format, ...) {
    va_list args;
    s32 result = -117;

    if (!reserveFileDescriptor()) {
        return FALSE;
    }

    va_start(args, format);
    vsnprintf(s_message, 256, format, args);
    va_end(args);

    s_callback = callback;
    s_stage = 1;
    result = ISFS_OpenAsync("/shared2/test2/nanderr.log", IPC_OPEN_RW,
                            asyncRoutine, NULL);

    if (result == IPC_RESULT_OK) {
        return TRUE;
    } else {
        return FALSE;
    }
}

static void callbackRoutine(BOOL result) {
    if (s_callback) {
        s_callback(result);
    }
}

static void asyncRoutine(s32 result, void* context) {
    s32 ret = -117;
    static char s_rBuf[256] __attribute__((aligned(64)));
    static char s_wBuf[256] __attribute__((aligned(64)));

    ++s_stage;

    if (s_stage == 2) {
        if (result >= 0) {
            s_fd = result;
            ret = ISFS_SeekAsync(s_fd, 0, IPC_SEEK_BEG, asyncRoutine, NULL);
            if (ret != IPC_RESULT_OK) {
                callbackRoutine(FALSE);
            }
        } else {
            callbackRoutine(FALSE);
        }
    } else if (s_stage == 3) {
        if (result == 0) {
            ret = ISFS_ReadAsync(s_fd, s_rBuf, 256, asyncRoutine, NULL);
            if (ret != IPC_RESULT_OK) {
                callbackRoutine(FALSE);
            }
        } else {
            callbackRoutine(FALSE);
        }
    } else if (s_stage == 4) {
        if (result == 256) {
            ret = ISFS_SeekAsync(s_fd, 0, IPC_SEEK_BEG, asyncRoutine, NULL);
            if (ret != IPC_RESULT_OK) {
                callbackRoutine(FALSE);
            }
        } else {
            callbackRoutine(FALSE);
        }
    } else if (s_stage == 5) {
        if (result == 0) {
            int n = 0;
            s_rBuf[255] = '\0';
            n = atoi(s_rBuf);
            prepareLine(s_wBuf, n, s_message);
            ret = ISFS_WriteAsync(s_fd, s_wBuf, 256, asyncRoutine, NULL);
            if (ret != IPC_RESULT_OK) {
                callbackRoutine(FALSE);
            }
        } else {
            callbackRoutine(FALSE);
        }
    } else if (s_stage == 6) {
        if (result == 256) {
            int n = atoi(s_rBuf);
            ret = ISFS_SeekAsync(s_fd, n * 256, IPC_SEEK_BEG,
                                asyncRoutine, NULL);
            if (ret != IPC_RESULT_OK) {
                callbackRoutine(FALSE);
            }
        } else {
            callbackRoutine(FALSE);
        }
    } else if (s_stage == 7) {
        int n = atoi(s_rBuf);
        if (result == n * 256) {
            ret = ISFS_WriteAsync(s_fd, s_wBuf, 256, asyncRoutine, NULL);
            if (ret != IPC_RESULT_OK) {
                callbackRoutine(FALSE);
            }
        } else {
            callbackRoutine(FALSE);
        }
    } else if (s_stage == 8) {
        if (result == 256) {
            ret = ISFS_CloseAsync(s_fd, asyncRoutine, NULL);
            if (ret != IPC_RESULT_OK) {
                callbackRoutine(FALSE);
            }
        } else {
            callbackRoutine(FALSE);
        }
    } else if (s_stage == 9) {
        if (result == IPC_RESULT_OK) {
            s_fd = -255;
            callbackRoutine(TRUE);
        } else {
            callbackRoutine(FALSE);
        }
    }
}

static void prepareLine(char line[256], int n, const char* message) {
    char titleID[64];
    int end = 0;
    OSCalendarTime calendar;

    memset(line, ' ', 254);
    OSTicksToCalendarTime(OSGetTime(), &calendar);
    strncpy(titleID, nandGetHomeDir() + 7, 17);
    titleID[8] = '-';
    titleID[17] = '\0';
    end = snprintf(line, 256, "%d %04d/%02d/%02d %02d:%02d:%02d %s %s",
                   n % 63 + 1, calendar.year, calendar.month + 1,
                   calendar.mday, calendar.hour, calendar.min, calendar.sec,
                   titleID, message);

    if (end < 256) {
        line[end] = ' ';
    }
    line[254] = '\r';
    line[255] = '\n';
}
