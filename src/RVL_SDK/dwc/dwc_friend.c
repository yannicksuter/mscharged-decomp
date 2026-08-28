#include <dwc/dwc_friend.h>

#include <dwc/dwc_base64.h>
#include <dwc/dwc_main.h>
#include <dwc/dwc_report.h>
#include <gamespy/GP/gp.h>
#include <string.h>

typedef struct DWCiFriendControl
{
    DWCState state;
    GPConnection* connection;
    u32 _08;
    u32 _0C;
    u64 _10;
    int friendListLen;
    DWCFriendData* friendList;
    u8 _20;
    u8 _21;
    u8 _22;
    u8 _23;
    u32 _24;
    u32 _28;
    void* _2C;
    void* _30;
    void* _34;
    DWCFriendStatusCallback friendStatusCallback;
    void* friendStatusParam;
    void* _40;
    void* _44;
    DWCBuddyFriendCallback buddyFriendCallback;
    void* buddyFriendParam;
    void* _50;
    void* _54;
    void* _58;
    void* _5C;
} DWCiFriendControl;

static DWCiFriendControl* sFriendControl;

void* fn_8048C530(void);
BOOL fn_8048CF50(void);
int fn_8049E4CC(void* userData, const DWCFriendData* friendData);
BOOL fn_804A9EE4(GPConnection* connection, int profileId);
GPResult fn_804A9F5C(GPConnection* connection, int profileId);
GPResult fn_8048AFCC(GPEnum status, const char* statusString,
    const char* locationString);

BOOL DWC_SetOwnStatusData(const char* statusData, u32 size)
{
    char encodedStatus[256];
    int encodedSize;

    if (sFriendControl == NULL || fn_8048CF50() == FALSE)
    {
        return FALSE;
    }

    encodedSize = DWC_Base64Encode(statusData, size, encodedStatus, 255);
    if (encodedSize == -1)
    {
        return FALSE;
    }

    encodedStatus[encodedSize] = '\0';
    return fn_8048AFCC(-1, NULL, encodedStatus) == GP_NO_ERROR;
}

void DWC_DeleteBuddyFriendData(DWCFriendData* friendData)
{
    int profileId;

    if (sFriendControl != NULL && fn_8048CF50() != FALSE && fn_8048C530() != NULL)
    {
        profileId = fn_8049E4CC(fn_8048C530(), friendData);
        if (profileId != 0 && profileId != -1 && fn_804A9EE4(sFriendControl->connection, profileId))
        {
            fn_804A9F5C(sFriendControl->connection, profileId);
            DWC_Printf(4, "DWC_DeleteBuddyFriendData : Deleted buddy.\n");
            goto clear_data;
        }
    }

    DWC_Printf(4, "DWC_DeleteBuddyFriendData : Only clear data.\n");

clear_data:
    memset(friendData, 0, 12);
}

BOOL DWC_SetBuddyFriendCallback(DWCBuddyFriendCallback callback, void* param)
{
    if (sFriendControl == NULL)
    {
        return FALSE;
    }

    sFriendControl->buddyFriendCallback = callback;
    sFriendControl->buddyFriendParam = param;
    return TRUE;
}

BOOL DWC_SetFriendStatusCallback(DWCFriendStatusCallback callback,
    void* param)
{
    if (sFriendControl == NULL)
    {
        return FALSE;
    }

    sFriendControl->friendStatusCallback = callback;
    if (param != NULL)
    {
        sFriendControl->friendStatusParam = param;
    }
    return TRUE;
}

void DWCi_FriendInit(DWCiFriendControl* control, GPConnection* connection,
    void* _2C, DWCFriendData* friendList,
    int friendListLen)
{
    sFriendControl = control;
    sFriendControl->state = DWC_STATE_INIT;
    sFriendControl->connection = connection;
    sFriendControl->_08 = 0;
    sFriendControl->_10 = 0;
    sFriendControl->friendListLen = friendListLen;
    sFriendControl->friendList = friendList;
    sFriendControl->_20 = 0;
    sFriendControl->_21 = 0;
    sFriendControl->_22 = 0;
    sFriendControl->_23 = 0;
    sFriendControl->_24 = 0;
    sFriendControl->_28 = 0;
    sFriendControl->_2C = _2C;
    sFriendControl->_30 = NULL;
    sFriendControl->_34 = NULL;
    sFriendControl->friendStatusCallback = NULL;
    sFriendControl->friendStatusParam = NULL;
    sFriendControl->_40 = NULL;
    sFriendControl->_44 = NULL;
    sFriendControl->buddyFriendCallback = NULL;
    sFriendControl->buddyFriendParam = NULL;
    sFriendControl->_50 = NULL;
    sFriendControl->_54 = NULL;
    sFriendControl->_58 = NULL;
    sFriendControl->_5C = NULL;
}
