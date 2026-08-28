#include <dwc/dwc_main.h>

#include <dwc/dwc_error.h>
#include <dwc/dwc_report.h>
#include <gamespy/common/gsPlatformSocket.h>
#include <gamespy/gt2/gt2.h>

typedef struct DWCMainControlView
{
    GT2Socket _00;
    u8 _04[0x10];
    int _14;
    int _18;
    u8 _1C[8];
    DWCState state;
    u8 _28[4];
    u8 myAid;
    u8 _2D[0x63];
    DWCConnectionClosedCallback connectionClosedCallback;
    void* connectionClosedParam;
    u8 _98[0x2DD];
    u8 matchType;
    u8 _376[0x2CA];
    u8 aidList[0x20];
    u32 aidBitmap;
    u8 _664[0x8AC];
} DWCMainControlView;

typedef struct DWCConnectionDataView
{
    u8 _00;
    u8 aid;
} DWCConnectionDataView;

static GT2Connection lbl_806C98A0[32];
static DWCMainControlView* sMainControl;
static s32 lbl_806E2EE4;

int fn_804929A8(void);
int fn_804929C4(void);
int fn_80492ABC(void);
int fn_80492AE8(void);
BOOL fn_8048E638(GT2Result result);
GT2Bool fn_80491578(GT2Socket socket, unsigned int ip, unsigned short port,
    GT2Byte* message, int len);
void fn_804916EC(GT2Socket socket, GT2Connection connection, unsigned int ip,
    unsigned short port, int latency, GT2Byte* message, int len);
BOOL DWC_isValidAid(u8 aid);
static void fn_8048F4F8(GT2Socket socket);

BOOL DWC_SetConnectionClosedCallback(DWCConnectionClosedCallback callback,
    void* param)
{
    if (sMainControl == NULL)
    {
        return FALSE;
    }
    sMainControl->connectionClosedCallback = callback;
    sMainControl->connectionClosedParam = param;
    return TRUE;
}

int DWC_GetNumConnectionHost(void)
{
    if (sMainControl == NULL)
    {
        return 0;
    }
    if (sMainControl->matchType != 2)
    {
        if (sMainControl->matchType != 3)
        {
            return fn_804929A8() + 1;
        }
    }
    return fn_804929C4() + 1;
}

u8 DWC_GetMyAID(void)
{
    if (sMainControl != NULL)
    {
        return sMainControl->myAid;
    }
    return 0;
}

int DWC_GetAIDList(u8** aidList)
{
    if (sMainControl == NULL)
    {
        return 0;
    }
    *aidList = sMainControl->aidList;
    if (sMainControl->matchType != 2)
    {
        if (sMainControl->matchType != 3)
        {
            return fn_80492ABC();
        }
    }
    return fn_80492AE8();
}

BOOL DWC_IsValidAID(u8 aid)
{
    if (sMainControl == NULL)
    {
        return FALSE;
    }
    if ((sMainControl->aidBitmap & (1 << aid)) == 0)
    {
        return FALSE;
    }
    return DWC_isValidAid(aid);
}

DWCState DWC_GetState(void)
{
    if (sMainControl != NULL)
    {
        return sMainControl->state;
    }
    return DWC_STATE_INIT;
}

GT2Result fn_8048E0C4(void)
{
    u16 port;
    GT2Result result;

    if (sMainControl->_00 != NULL)
    {
        DWC_Printf(8, "gt2Socket is already made.\n");
        return GT2Success;
    }

    port = fn_8048A3BC(0x4000) + 0xC000;
    DWC_Printf(0x40, "--- Private port = %d ---\n", port);
    result = gt2CreateSocket(&sMainControl->_00,
        gt2AddressToString(0, port, NULL),
        sMainControl->_14,
        sMainControl->_18,
        fn_8048F4F8);
    if (fn_8048E638(result))
    {
        return result;
    }
    gt2Listen(sMainControl->_00, fn_804916EC);
    gt2SetUnrecognizedMessageCallback(sMainControl->_00, fn_80491578);
    return result;
}

GT2Connection DWCi_GetGT2Connection(u8 aid)
{
    int i;

    if (sMainControl == NULL)
    {
        return NULL;
    }
    for (i = 0; i < 32; i++)
    {
        if (lbl_806C98A0[i] != NULL && ((DWCConnectionDataView*)gt2GetConnectionData(lbl_806C98A0[i]))->aid == aid)
        {
            return lbl_806C98A0[i];
        }
    }
    return NULL;
}

u8 DWCi_GetConnectionAid(GT2Connection connection)
{
    return ((DWCConnectionDataView*)gt2GetConnectionData(connection))->aid;
}

BOOL DWC_isValidAid(u8 aid)
{
    int i;

    for (i = 0; i < 32; i++)
    {
        if (lbl_806C98A0[i] != NULL && ((DWCConnectionDataView*)gt2GetConnectionData(lbl_806C98A0[i]))->aid == aid)
        {
            return TRUE;
        }
    }
    return FALSE;
}

static void fn_8048F4F8(GT2Socket socket)
{
    lbl_806E2EE4 = GOAGetLastError(gt2GetSocketSOCKET(socket));
    DWC_Printf(2, "Socket fatal error! (%d)\n", lbl_806E2EE4);
    DWCi_SetError(DWC_ERROR_FATAL,
        DWC_ECODE_SEQ_ETC + DWC_ECODE_GS_GT2 + DWC_ECODE_TYPE_SO_SOCKET);
    sMainControl->_00 = 0;
}
