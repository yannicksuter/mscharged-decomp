#include <dwc/dwc_friend.h>

#include <dwc/dwc_base64.h>
#include <dwc/dwc_error.h>
#include <dwc/dwc_main.h>
#include <dwc/dwc_report.h>
#include <gamespy/GP/gp.h>
#include <revolution/os/OSTime.h>
#include <string.h>

typedef struct DWCFriendControl
{
    DWCFriendState state;
    GPConnection* pGpObj;
    u32 gpProcessCount;
    u64 lastGpProcess;
    int friendListLen;
    DWCFriendData* friendList;
    u8 buddyUpdateIdx;
    u8 friendListChanged;
    u8 buddyUpdateState;
    u8 svUpdateComplete;
    u32 persCallbackLevel;
    int profileID;
    const u16* playerName;
    DWCUpdateServersCallback updateCallback;
    void* updateParam;
    DWCFriendStatusCallback statusCallback;
    void* statusParam;
    DWCDeleteFriendListCallback deleteCallback;
    void* deleteParam;
    DWCBuddyFriendCallback buddyCallback;
    void* buddyParam;
    void* persLoginCallback;
    void* persLoginParam;
    void* saveCallback;
    void* loadCallback;
} DWCFriendControl;

static DWCFriendControl* stpFriendCnt;

static void DWCi_CloseFriendProcess(void);

void* fn_8048C530(void);
BOOL fn_8048CF50(void);
GPResult fn_8048AFCC(GPEnum status, const char* statusString,
    const char* locationString);
void DWCi_GPGetInfoCallback_RecvBuddyRequest(GPConnection* connection,
    GPGetInfoResponseArg* arg, void* param);
void DWCi_GPGetInfoCallback_RecvAuthMessage(GPConnection* connection,
    GPGetInfoResponseArg* arg,
    void* param);
int DWCi_GetFriendListIndex(int profileID);

BOOL DWC_SetOwnStatusData(const char* statusData, u32 size)
{
    char encodedStatus[256];
    int encodedSize;

    if (stpFriendCnt == NULL || fn_8048CF50() == FALSE)
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

    if (stpFriendCnt != NULL && fn_8048CF50() != FALSE && fn_8048C530() != NULL)
    {
        profileId = DWC_GetGsProfileId(fn_8048C530(), friendData);
        if (profileId != 0 && profileId != -1 && gpIsBuddy(stpFriendCnt->pGpObj, profileId))
        {
            gpDeleteBuddy(stpFriendCnt->pGpObj, profileId);
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
    if (stpFriendCnt == NULL)
    {
        return FALSE;
    }

    stpFriendCnt->buddyCallback = callback;
    stpFriendCnt->buddyParam = param;
    return TRUE;
}

BOOL DWC_SetFriendStatusCallback(DWCFriendStatusCallback callback,
    void* param)
{
    if (stpFriendCnt == NULL)
    {
        return FALSE;
    }

    stpFriendCnt->statusCallback = callback;
    if (param != NULL)
    {
        stpFriendCnt->statusParam = param;
    }
    return TRUE;
}

void DWCi_FriendInit(DWCFriendControl* friendcnt, GPConnection* pGpObj,
    const u16* playerName, DWCFriendData* friendList,
    int friendListLen)
{
    stpFriendCnt = friendcnt;
    stpFriendCnt->state = DWC_FRIEND_STATE_INIT;
    stpFriendCnt->pGpObj = pGpObj;
    stpFriendCnt->gpProcessCount = 0;
    stpFriendCnt->lastGpProcess = 0;
    stpFriendCnt->friendListLen = friendListLen;
    stpFriendCnt->friendList = friendList;
    stpFriendCnt->buddyUpdateIdx = 0;
    stpFriendCnt->friendListChanged = 0;
    stpFriendCnt->buddyUpdateState = DWC_BUDDY_UPDATE_STATE_WAIT;
    stpFriendCnt->svUpdateComplete = 0;
    stpFriendCnt->persCallbackLevel = 0;
    stpFriendCnt->profileID = 0;
    stpFriendCnt->playerName = playerName;
    stpFriendCnt->updateCallback = NULL;
    stpFriendCnt->updateParam = NULL;
    stpFriendCnt->statusCallback = NULL;
    stpFriendCnt->statusParam = NULL;
    stpFriendCnt->deleteCallback = NULL;
    stpFriendCnt->deleteParam = NULL;
    stpFriendCnt->buddyCallback = NULL;
    stpFriendCnt->buddyParam = NULL;
    stpFriendCnt->persLoginCallback = NULL;
    stpFriendCnt->persLoginParam = NULL;
    stpFriendCnt->saveCallback = NULL;
    stpFriendCnt->loadCallback = NULL;
}

void DWCi_UpdateServersAsync(const char* authToken,
    const char* partnerChallenge, DWCUpdateServersCallback updateCallback,
    void* updateParam, DWCFriendStatusCallback statusCallback,
    void* statusParam, DWCDeleteFriendListCallback deleteCallback,
    void* deleteParam)
{
    stpFriendCnt->updateCallback = updateCallback;
    stpFriendCnt->updateParam = updateParam;
    stpFriendCnt->statusCallback = statusCallback;
    stpFriendCnt->statusParam = statusParam;
    stpFriendCnt->deleteCallback = deleteCallback;
    stpFriendCnt->deleteParam = deleteParam;

    stpFriendCnt->friendListChanged = 0;
    stpFriendCnt->buddyUpdateState = DWC_BUDDY_UPDATE_STATE_WAIT;
    stpFriendCnt->svUpdateComplete = 0;
    stpFriendCnt->buddyUpdateIdx = 0;

    stpFriendCnt->state = DWC_FRIEND_STATE_PERS_LOGIN;

    if (stpFriendCnt->friendList == NULL)
    {
        stpFriendCnt->svUpdateComplete++;
    }

    stpFriendCnt->svUpdateComplete++;
}

void DWCi_StopFriendProcess(int error, int errorCode)
{
    if (stpFriendCnt == NULL || error == DWC_ERROR_NONE)
    {
        return;
    }

    DWCi_SetError(error, errorCode);

    if (stpFriendCnt->state != DWC_FRIEND_STATE_INIT
        && stpFriendCnt->state != DWC_FRIEND_STATE_LOGON)
    {
        stpFriendCnt->updateCallback(
            error, stpFriendCnt->friendListChanged, stpFriendCnt->updateParam);
    }

    DWCi_CloseFriendProcess();
}

static void DWCi_CloseFriendProcess(void)
{
    if (stpFriendCnt == NULL)
    {
        return;
    }

    stpFriendCnt->state = DWC_FRIEND_STATE_INIT;
    stpFriendCnt->buddyUpdateState = DWC_BUDDY_UPDATE_STATE_WAIT;
    stpFriendCnt->svUpdateComplete = 0;
}

void DWCi_GPRecvBuddyRequestCallback(GPConnection* connection,
    GPRecvBuddyRequestArg* arg, void* param)
{
#pragma unused(param)

    DWC_Printf(DWC_REPORTFLAG_UPDATE_SV,
        "Received buddy request from %u\n", arg->profile);

    if (!stpFriendCnt->friendList)
    {
        return;
    }

    DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "Begin to search gpInfo.\n");
    gpGetInfo(connection, arg->profile, GP_DONT_CHECK_CACHE,
        GP_NON_BLOCKING, (GPCallback)DWCi_GPGetInfoCallback_RecvBuddyRequest,
        NULL);
}

BOOL DWCi_GPRecvBuddyAuthCallback(GPConnection* connection,
    GPRecvBuddyMessageArg* arg, void* param)
{
#pragma unused(param)

    DWC_Printf(DWC_REPORTFLAG_UPDATE_SV,
        "Received buddy authenticated message from %u.\n", arg->profile);
    DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "Begin to search gpInfo.\n");

    gpGetInfo(connection, arg->profile, GP_DONT_CHECK_CACHE, GP_NON_BLOCKING,
        (GPCallback)DWCi_GPGetInfoCallback_RecvAuthMessage, NULL);

    return TRUE;
}

void DWCi_GPRecvBuddyStatusCallback(GPConnection* connection,
    GPRecvBuddyStatusArg* arg, void* param)
{
#pragma unused(param)
    int friendIdx;
    GPBuddyStatus status;

    DWC_Printf(DWC_REPORTFLAG_UPDATE_SV,
        "RECV update friend status. p:%d\n", arg->profile);

    if (stpFriendCnt->statusCallback)
    {
        friendIdx = DWCi_GetFriendListIndex(arg->profile);
        if (friendIdx == -1)
        {
            return;
        }

        gpGetBuddyStatus(connection, arg->index, &status);

        stpFriendCnt->statusCallback(friendIdx, (u8)status.status,
            status.locationString, stpFriendCnt->statusParam);
    }
}

int DWCi_GetProfileIDFromList(int index)
{
    int profileID;

    if (stpFriendCnt->friendList == NULL)
    {
        return 0;
    }

    profileID = DWC_GetGsProfileId(fn_8048C530(),
        &stpFriendCnt->friendList[index]);

    if (!profileID || profileID == -1)
    {
        return 0;
    }
    else
    {
        return profileID;
    }
}

int DWCi_GetFriendListIndex(int profileID)
{
    int i;

    if (!stpFriendCnt || !profileID)
    {
        return -1;
    }

    for (i = 0; i < stpFriendCnt->friendListLen; i++)
    {
        if (DWCi_GetProfileIDFromList(i) == profileID)
        {
            return i;
        }
    }

    return -1;
}

void DWCi_ShutdownFriend(void)
{
    stpFriendCnt = NULL;
}

void DWCi_InitGPProcessCount(void)
{
    if (stpFriendCnt)
    {
        stpFriendCnt->gpProcessCount = 0;
        stpFriendCnt->lastGpProcess = OSGetTime();
    }
}

BOOL DWCi_GetFriendBuddyStatus(const DWCFriendData* friendData,
    GPBuddyStatus* status)
{
    u8 ret;
    int buddyIdx = 0;
    int profileid;

    if (!stpFriendCnt || !fn_8048CF50())
    {
        return FALSE;
    }

    profileid = DWC_GetGsProfileId(fn_8048C530(), friendData);
    if (profileid > 0
        && gpGetBuddyIndex(stpFriendCnt->pGpObj, profileid, &buddyIdx))
    {
        ret = FALSE;
    }
    else if (profileid <= 0 || buddyIdx == -1)
    {
        ret = FALSE;
    }
    else
    {
        if (gpGetBuddyStatus(stpFriendCnt->pGpObj, buddyIdx, status))
        {
            ret = FALSE;
        }
        else
        {
            ret = TRUE;
        }
    }
    return ret;
    return ret;
}
