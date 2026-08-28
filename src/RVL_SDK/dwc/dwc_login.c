#include <dwc/dwc_main.h>

#include <dwc/dwc_error.h>
#include <dwc/dwc_report.h>
#include <revolution/types.h>
#include <string.h>

typedef struct DWCLoginControlView
{
    void* _00;
    s32 _04;
    u32 _08;
    u32 _0C;
    u32 _10;
    DWCLoginCallback _14;
    void* _18;
    void* _1C;
    u8 _20[0x10];
    u32 _30;
    u8 _34[0x234];
} DWCLoginControlView;

typedef struct DWCLoginIdView
{
    u32 _00[3];
} DWCLoginIdView;

typedef struct DWCUserDataView
{
    u32 _00;
    DWCLoginIdView _04;
    DWCLoginIdView _10;
} DWCUserDataView;

static DWCLoginControlView* sLoginControl;

u64 fn_8049D8E8(DWCLoginIdView* arg0);
u32 fn_8049D8F8(DWCLoginIdView* arg0);
BOOL fn_8049F704(void);
void fn_8049EF74(void);
BOOL fn_8048C8D0(void);

void fn_8048C214(DWCLoginControlView* arg0, DWCUserDataView* arg1, void* arg2, u32 arg3,
    u32 arg4, u32 arg5, DWCLoginCallback arg6, void* arg7)
{
    DWC_Printf(0x20, "Login Init\n");

    sLoginControl = arg0;
    memset(sLoginControl, 0, sizeof(DWCLoginControlView));
    sLoginControl->_00 = arg2;
    sLoginControl->_04 = 0;
    sLoginControl->_08 = arg3;
    sLoginControl->_0C = arg4;
    sLoginControl->_10 = arg5;
    sLoginControl->_14 = arg6;
    sLoginControl->_18 = arg7;
    sLoginControl->_1C = arg1;

    DWC_Printf(0x20, "******************************************\n");
    DWC_Printf(0x20, "  pseudo    UserID   : %016llx\n", fn_8049D8E8(&arg1->_04));
    DWC_Printf(0x20, "  pseudo    PlayerID : %08x\n", fn_8049D8F8(&arg1->_04));
    DWC_Printf(0x20, "  authentic UserID   : %016llx\n", fn_8049D8E8(&arg1->_10));
    DWC_Printf(0x20, "  authentic PlayerID : %08x\n", fn_8049D8F8(&arg1->_10));
    DWC_Printf(0x20, "******************************************\n");
}

BOOL fn_8048C368(void)
{
    if (fn_8048C8D0())
    {
        sLoginControl->_04 = 1;
        sLoginControl->_30 = 0;
        return TRUE;
    }
    return FALSE;
}

void* fn_8048C530(void)
{
    if (sLoginControl != NULL)
    {
        return sLoginControl->_1C;
    }
    return NULL;
}

void fn_8048C54C(int arg0, int arg1)
{
    if (sLoginControl != NULL)
    {
        if (arg0 != 0)
        {
            DWCi_SetError(arg0, arg1);
            if (sLoginControl->_14 != NULL)
            {
                sLoginControl->_14(arg0, 0, sLoginControl->_18);
            }
            if (sLoginControl != NULL)
            {
                sLoginControl->_04 = 0;
                sLoginControl->_30 = 0;
            }
        }
    }
}

void fn_8048C5D0(void)
{
    if (!fn_8049F704())
    {
        fn_8049EF74();
    }
    sLoginControl = NULL;
}

int fn_8048C604(int arg0)
{
    int error;
    int errorCode;

    if (arg0 == 0)
    {
        return 0;
    }

    DWC_Printf(2, "Login, GP error %d\n", arg0);
    switch (arg0)
    {
    case 1:
        error = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_ALLOC;
        break;
    case 2:
        error = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_PARAM;
        break;
    case 3:
        error = 6;
        errorCode = DWC_ECODE_TYPE_NETWORK;
        break;
    case 4:
        error = 6;
        errorCode = DWC_ECODE_TYPE_SERVER;
        break;
    }

    if (sLoginControl != NULL && error != 0)
    {
        DWCi_SetError(error, DWC_ECODE_SEQ_LOGIN + DWC_ECODE_GS_GP + errorCode);
        if (sLoginControl->_14 != NULL)
        {
            sLoginControl->_14(error, 0, sLoginControl->_18);
        }
        if (sLoginControl != NULL)
        {
            sLoginControl->_04 = 0;
            sLoginControl->_30 = 0;
        }
    }
    return arg0;
}

BOOL fn_8048CF50(void)
{
    if (sLoginControl != NULL && sLoginControl->_04 == 5)
    {
        return TRUE;
    }
    return FALSE;
}
