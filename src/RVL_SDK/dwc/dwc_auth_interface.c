#include <dwc/dwc_auth_interface.h>

#include <dwc/dwc_report.h>
#include <revolution/nand.h>
#include <revolution/os/OSTime.h>
#include <string.h>

typedef struct DWCAuthResultView
{
    s32 _000;
    char _004[0x12D];
    char _131[0x7F];
    u64 _1B0;
    s32 _1B8;
    u8 _1BC[4];
    OSTime _1C0;
    u8 _1C8[8];
} DWCAuthResultView;

typedef struct DWCAuthWorkView
{
    u8 _0000[0x55C4];
    s32 _55C4;
    BOOL _55C8;
    s32 _55CC;
    u8 _55D0[0x14C];
    DWCAllocFunc _571C;
    DWCFreeFunc _5720;
    u8 _5724[0xC];
} DWCAuthWorkView;

static DWCAuthResultView s_auth_result;
static u8 s_svl_result[0x174];
static DWC_AuthServer s_authserver;
static DWCAuthWorkView* s_auth_work;
static s32 s_auth_state;

void DWCi_Auth_InitInterface(DWC_AuthServer arg0)
{
    memset(&s_auth_result, 0, sizeof(s_auth_result));
    memset(s_svl_result, 0, sizeof(s_svl_result));
    s_auth_work = NULL;
    s_auth_state = 0;
    s_authserver = arg0;
    s_auth_result._1B8 = 0;
}

DWC_AuthServer fn_8049EDC0(void)
{
    return s_authserver;
}

BOOL DWCi_Auth_IsFinished(void)
{
    if (s_auth_state == 0x19)
    {
        s_auth_state = 0x1A;
        return TRUE;
    }

    if (s_auth_state == 0 || s_auth_state == 0x1A)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL DWCi_Auth_IsSucceeded(void)
{
    return s_auth_result._000 == 1;
}

int DWCi_Auth_GetErrorCode(void)
{
    return s_auth_result._000;
}

void fn_8049F764(char* arg0, char* arg1)
{
    strcpy(arg0, s_auth_result._004);
    strcpy(arg1, s_auth_result._131);
}

u64 DWCi_Auth_GetConsoleUserId(void)
{
    return s_auth_result._1B0;
}

s32 DWCi_Auth_GetIngamesnCheckResult(void)
{
    return s_auth_result._1B8;
}

BOOL fn_8049F7D4(OSTime* arg0)
{
    if (s_auth_result._1C0 == 0)
    {
        return FALSE;
    }

    *arg0 = s_auth_result._1C0 / OS_TIME_SPEED;
    return TRUE;
}

void* nhttp_alloc(u32 size, int align)
{
    return s_auth_work->_571C(DWC_ALLOCTYPE_NHTTP, size);
}

void nhttp_free(void* buffer)
{
    s_auth_work->_5720(DWC_ALLOCTYPE_NHTTP, buffer, 0);
}

void nand_callback(s32 result, NANDCommandBlock* block)
{
    s_auth_work->_55C8 = TRUE;
    s_auth_work->_55CC = result;
}

void DWCi_Auth_EndProcess(void)
{
    DWC_Printf(0x1000000, "DWCi_Auth_EndProcess()\n");
    s_auth_work->_5720(DWC_ALLOCTYPE_AUTH, s_auth_work, 0);
    s_auth_work = NULL;
    s_auth_state = 0x19;
}

void DWCi_Auth_HandleNandResult(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    s_auth_work->_55C8 = FALSE;

    if (arg1 != 0x1B && s_auth_work->_55CC == NAND_RESULT_BUSY && s_auth_work->_55C4 < 5)
    {
        s_auth_work->_55C4++;
        s_auth_state = arg1;
        return;
    }

    s_auth_work->_55C4 = 0;

    if (s_auth_work->_55CC == NAND_RESULT_OK)
    {
        s_auth_state = arg0;
        return;
    }

    if (arg2 != 0x1B && s_auth_work->_55CC == NAND_RESULT_NOEXISTS)
    {
        s_auth_state = arg2;
        return;
    }

    if (arg3 != 0x1B && s_auth_work->_55CC == NAND_RESULT_ACCESS)
    {
        s_auth_state = arg3;
        return;
    }

    DWC_Printf(0x1000000, " NAND access failed.[%d]\n", s_auth_work->_55CC);
    s_auth_result._000 = s_auth_work->_55CC == NAND_RESULT_CORRUPT ? -0x7149 : -0x7148;
    DWCi_Auth_EndProcess();
}
