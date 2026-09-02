#include <dwc/dwc_account.h>
#include <dwc/dwc_base64.h>
#include <dwc/dwc_common.h>
#include <dwc/dwc_friend.h>

#include "Game/tu_801360A4.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/DB/SaveLoad.h"
#include "Game/GameInfo.h"
#include "Game/NetworkSession.h"

#include <string.h>

extern int lbl_806E20E0;
extern BaseGameSceneManager* lbl_806E1838;

extern "C"
{
    int fn_8004F594(int channel, const char* format, ...);
    u32 fn_80124238();
    void fn_8025BD7C(bool value);
    void fn_8025BDCC(unsigned long long friendKey, u16* output);
}

static const char* sFriendStatusNames[] = {
    "Offline",
    "Online",
    "Playing",
    "MatchAny",
    "MatchFriend",
    "MatchSCClient",
    "MatchSCServ",
};

static inline u32 GetStatusDataSize(u8 status)
{
    switch (status)
    {
    case 0:
    case 1:
        return 4;
    case 3:
    case 4:
        return 8;
    case 2:
        return sizeof(UnidentifiedFriendStatusPayload);
    default:
        return 0;
    }
}

static void BuddyFriendCallback_801364B4(int index, void*)
{
    UnidentifiedFriendManager_801360A4* manager = lbl_806E1194;
    fn_8004F594(0x10, "Got friendship with friend [%d].]\n", index);
    manager->mFriendListChanged = true;
}

static void UpdateServersCallback_801364FC(int error, BOOL isChanged, void*)
{
    if (error == 0)
    {
        fn_8004F594(0x10,
            "Friends list synchronization successful (isChanged == %s)\n",
            isChanged ? "true" : "false");
        if (isChanged)
        {
            SaveLoad::StartSave(true);
        }
    }
}

static void FriendStatusCallback_80136560(
    int index, u8 status, const char* statusString, void* param)
{
    lbl_806E1194->HandleFriendStatus_801365C4(
        index, status, statusString, param);
}

static void DeleteFriendCallback_80136584(
    int deletedIndex, int sourceIndex, void*)
{
    fn_8004F594(0x10,
        "Friend [%d] was deleted (equal friend[%d]).\n",
        deletedIndex,
        sourceIndex);
    SaveLoad::StartSave(true);
}

UnidentifiedFriendManager_801360A4::UnidentifiedFriendManager_801360A4()
{
    Reset(true);
}

void UnidentifiedFriendManager_801360A4::Reset(bool setCallbackInstance)
{
    if (setCallbackInstance)
    {
        lbl_806E1190 = this;
    }

    mUpdateTime = 0.0f;
    mFriendListChanged = false;
    mHostInvitationIndex = -1;
    mFriendStatusIndex = -1;
    mUnidentified00C = -2;
    mUnidentified010 = 0;
    memset(mUnidentified018, 0, sizeof(mUnidentified018));

    mOwnStatus.mMagic[0] = 'S';
    mOwnStatus.mMagic[1] = 'C';
    mOwnStatus.mMagic[2] = '2';
    mOwnStatus.mStatus = 0;

    for (int i = 0; i < 64; ++i)
    {
        mFriendStatus[i].mMagic[0] = 'S';
        mFriendStatus[i].mMagic[1] = 'C';
        mFriendStatus[i].mMagic[2] = '2';
        mFriendStatus[i].mStatus = 0;
        mFriendStatusChanged[i] = false;
    }
}

void UnidentifiedFriendManager_801360A4::GetOwnFriendKeyString_80136290(
    u16* output)
{
    DWCUserData* userData = reinterpret_cast<DWCUserData*>(
        GameInfoManager::GetInstance()->GetSaveSlot(lbl_806E20E0));
    u64 friendKey = DWC_CreateFriendKey(userData);
    GameInfoManager::GetInstance()->ValidateSaveSlot(lbl_806E20E0);
    fn_8025BDCC(friendKey, output);
}

bool UnidentifiedFriendManager_801360A4::AddFriendKey_801362FC(
    unsigned long long friendKey, int* error)
{
    *error = -1;

    DWCUserData* userData = reinterpret_cast<DWCUserData*>(
        GameInfoManager::GetInstance()->GetSaveSlot(lbl_806E20E0));
    u64 ownFriendKey = DWC_CreateFriendKey(userData);
    GameInfoManager::GetInstance()->ValidateSaveSlot(lbl_806E20E0);

    if (friendKey == ownFriendKey)
    {
        *error = 0x66;
        return false;
    }

    userData = reinterpret_cast<DWCUserData*>(
        GameInfoManager::GetInstance()->GetSaveSlot(lbl_806E20E0));
    if (!DWC_CheckFriendKey(userData, friendKey))
    {
        *error = 0x65;
        return false;
    }

    DWCFriendData token;
    DWC_CreateFriendKeyToken(&token, friendKey);

    for (int i = 0; i < 64; ++i)
    {
        DWCFriendData* friendData = reinterpret_cast<DWCFriendData*>(
            GameInfoManager::GetInstance()->GetUnknown0x40(lbl_806E20E0, i));
        if (DWC_IsEqualFriendData(&token, friendData)
            || token.gs_profile_id.id == friendData->gs_profile_id.id)
        {
            *error = 0x67;
            return false;
        }
    }

    bool added = false;
    for (int i = 0; i < 64; ++i)
    {
        DWCFriendData* friendData = reinterpret_cast<DWCFriendData*>(
            GameInfoManager::GetInstance()->GetUnknown0x40(lbl_806E20E0, i));
        if (!DWC_IsValidFriendData(friendData))
        {
            memcpy(friendData, &token, sizeof(token));
            u16* name = static_cast<u16*>(
                GameInfoManager::GetInstance()->GetUnknown0x340(
                    lbl_806E20E0, i));
            if (name[0] == 0)
            {
                fn_8025BDCC(DWC_GetFriendKey(friendData), name);
            }
            added = true;
            break;
        }
    }

    if (!added)
    {
        *error = 0x68;
        return false;
    }

    mFriendListChanged = true;
    return true;
}

void UnidentifiedFriendManager_801360A4::HandleFriendStatus_801365C4(
    int index, u8 status, const char* statusString, void*)
{
    GameInfoManager* gameInfo = GameInfoManager::GetInstance();
    DWCFriendData* friendData = reinterpret_cast<DWCFriendData*>(
        gameInfo->GetUnknown0x40(lbl_806E20E0, index));
    if (!DWC_IsValidFriendData(friendData))
    {
        return;
    }

    fn_8004F594(0x10, "friend[%.2d] type %d Friend:%s status %s (%s).\n", index, DWC_GetFriendDataType(friendData), DWC_IsBuddyFriendData(friendData) ? "Yes" : "No", sFriendStatusNames[status], statusString);

    UnidentifiedFriendStatusPayload previous = mFriendStatus[index];
    UnidentifiedFriendStatusPayload& current = mFriendStatus[index];

    u32 encodedLength = 0;
    if (statusString != 0)
    {
        encodedLength = strlen(statusString);
    }
    u32 decodedLength = DWC_Base64Decode(statusString, encodedLength, reinterpret_cast<char*>(&current), sizeof(current));

    bool valid = decodedLength >= 4
              && current.mMagic[0] == 'S'
              && current.mMagic[1] == 'C'
              && current.mMagic[2] == '2'
              && current.mStatus <= 4
              && decodedLength == GetStatusDataSize(current.mStatus);
    if (!valid)
    {
        current.mMagic[0] = 'S';
        current.mMagic[1] = 'C';
        current.mMagic[2] = '2';
        current.mStatus = 0;
    }

    bool unchanged = current.mStatus == previous.mStatus
                  && current.mMagic[0] == previous.mMagic[0]
                  && current.mMagic[1] == previous.mMagic[1]
                  && current.mMagic[2] == previous.mMagic[2];
    if (unchanged)
    {
        if (current.mStatus <= 1)
        {
            unchanged = true;
        }
        else if (current.mStatus == 3 || current.mStatus == 4)
        {
            unchanged = current.mProfileId == previous.mProfileId;
        }
        else
        {
            unchanged = current.mProfileId == previous.mProfileId
                     && current.mUnidentified34 == previous.mUnidentified34
                     && current.mUnidentified30 == previous.mUnidentified30
                     && current.mPowerupSettings.m_unk0
                            == previous.mPowerupSettings.m_unk0
                     && current.mPowerupSettings.m_unk4
                            == previous.mPowerupSettings.m_unk4
                     && current.mPowerupSettings.m_unk8
                            == previous.mPowerupSettings.m_unk8
                     && current.mGameplaySettings.m_unk04
                            == previous.mGameplaySettings.m_unk04
                     && current.mGameplaySettings.m_unk10
                            == previous.mGameplaySettings.m_unk10;
            if (unchanged)
            {
                if (current.mGameplaySettings.m_unk04 == 0)
                {
                    unchanged = current.mGameplaySettings.GameTime
                             == previous.mGameplaySettings.GameTime;
                }
                else
                {
                    unchanged = current.mGameplaySettings.m_unk0C
                             == previous.mGameplaySettings.m_unk0C;
                }
            }
        }
    }

    if (unchanged)
    {
        return;
    }

    switch (current.mStatus)
    {
    case 0:
        fn_8004F594(0x10,
            "FriendStatusChanged FriendPID %d EFriendStatus_Initial_NotAvailable\n",
            friendData->gs_profile_id.id);
        break;
    case 1:
        fn_8004F594(0x10,
            "FriendStatusChanged FriendPID %d EFriendStatus_Initial_Available\n",
            friendData->gs_profile_id.id);
        break;
    case 2:
        fn_8004F594(0x10,
            "FriendStatusChanged FriendPID %d EFriendStatus_HostInvitingPlayer forPID %d\n",
            friendData->gs_profile_id.id,
            current.mProfileId);
        break;
    case 3:
        fn_8004F594(0x10,
            "FriendStatusChanged FriendPID %d EFriendStatus_ClientDecliningHost forPID %d\n",
            friendData->gs_profile_id.id,
            current.mProfileId);
        break;
    case 4:
        fn_8004F594(0x10,
            "FriendStatusChanged FriendPID %d EFriendStatus_ClientReceivedInvitation forPID %d\n",
            friendData->gs_profile_id.id,
            current.mProfileId);
        break;
    default:
        fn_8004F594(0x10,
            "FriendStatusChanged FriendPID %d Invalid Status %d\n",
            friendData->gs_profile_id.id,
            current.mStatus);
        break;
    }

    mFriendStatusChanged[index] = true;
}

UnidentifiedFriendStatusPayload*
UnidentifiedFriendManager_801360A4::GetFriendStatusPayload_80136A88(int index)
{
    if (index >= 0 && static_cast<unsigned int>(index) < 64)
    {
        return &mFriendStatus[index];
    }
    return 0;
}

bool UnidentifiedFriendManager_801360A4::FindHostInvitation_80136AB0()
{
    for (int i = 0; i < 64; ++i)
    {
        if (!mFriendStatusChanged[i])
        {
            continue;
        }

        DWCFriendData* friendData = reinterpret_cast<DWCFriendData*>(
            GameInfoManager::GetInstance()->GetUnknown0x40(lbl_806E20E0, i));
        UnidentifiedFriendStatusPayload& status = mFriendStatus[i];
        if (!DWC_IsValidFriendData(friendData) || status.mStatus != 2)
        {
            continue;
        }

        DWCUserData* userData = reinterpret_cast<DWCUserData*>(
            GameInfoManager::GetInstance()->GetSaveSlot(lbl_806E20E0));
        if (userData->gs_profile_id != status.mProfileId)
        {
            continue;
        }

        const u16* name = static_cast<const u16*>(
            GameInfoManager::GetInstance()->GetUnknown0x340(
                lbl_806E20E0, i));
        unsigned int length = 0;
        if (name != 0)
        {
            while (*name++ != 0)
            {
                ++length;
            }
        }
        if (length > 11 || status.mUnidentified30 != fn_80124238())
        {
            continue;
        }

        mHostInvitationIndex = i;
        mFriendStatusChanged[i] = false;
        return true;
    }
    return false;
}

bool UnidentifiedFriendManager_801360A4::ValidateHostInvitation_80136BC0()
{
    int index = mHostInvitationIndex;
    if (index >= 0 && index < 64)
    {
        DWCFriendData* friendData = reinterpret_cast<DWCFriendData*>(
            GameInfoManager::GetInstance()->GetUnknown0x40(
                lbl_806E20E0, index));
        if (DWC_IsValidFriendData(friendData)
            && mFriendStatus[index].mStatus == 2)
        {
            DWCUserData* userData = reinterpret_cast<DWCUserData*>(
                GameInfoManager::GetInstance()->GetSaveSlot(lbl_806E20E0));
            if (userData->gs_profile_id == mFriendStatus[index].mProfileId)
            {
                return true;
            }
        }
    }

    mHostInvitationIndex = -1;
    return false;
}

bool UnidentifiedFriendManager_801360A4::HasFriendDeclined_80136C68()
{
    int index = mFriendStatusIndex;
    if (index < 0 || index >= 64)
    {
        return false;
    }

    DWCFriendData* friendData = reinterpret_cast<DWCFriendData*>(
        GameInfoManager::GetInstance()->GetUnknown0x40(lbl_806E20E0, index));
    if (!DWC_IsValidFriendData(friendData))
    {
        return false;
    }
    if (DWC_GetFriendStatus(friendData, 0) == 0)
    {
        return true;
    }

    if (mFriendStatus[index].mStatus != 3)
    {
        return false;
    }
    DWCUserData* userData = reinterpret_cast<DWCUserData*>(
        GameInfoManager::GetInstance()->GetSaveSlot(lbl_806E20E0));
    return userData->gs_profile_id == mFriendStatus[index].mProfileId;
}

int UnidentifiedFriendManager_801360A4::GetFriendInvitationResponse_80136D30()
{
    int index = mFriendStatusIndex;
    if (index < 0 || index >= 64)
    {
        return 0;
    }

    DWCFriendData* friendData = reinterpret_cast<DWCFriendData*>(
        GameInfoManager::GetInstance()->GetUnknown0x40(lbl_806E20E0, index));
    if (!DWC_IsValidFriendData(friendData)
        || DWC_GetFriendStatus(friendData, 0) == 0
        || mFriendStatus[index].mStatus != 4)
    {
        return 0;
    }

    DWCUserData* userData = reinterpret_cast<DWCUserData*>(
        GameInfoManager::GetInstance()->GetSaveSlot(lbl_806E20E0));
    if (userData->gs_profile_id == mFriendStatus[index].mProfileId)
    {
        return 1;
    }
    return 2;
}

void UnidentifiedFriendManager_801360A4::Update(float dt)
{
    mUpdateTime += dt;
    if (!mFriendListChanged || lbl_806E20D8->OnlineVirtual10() != 2)
    {
        return;
    }

    bool matchmaking = false;
    NetworkLobby_80133634* lobby = lbl_806E10EC->fn_801216F0();
    if (lobby != 0 && (lobby->mMatchmakingThreadRunning || lobby->mState != 0))
    {
        matchmaking = true;
    }
    bool invitationScene = lbl_806E1838->IsOnStack(static_cast<SceneList>(0x2F));
    if (matchmaking && !invitationScene)
    {
        return;
    }

    if (mFriendListChanged)
    {
        mFriendListChanged = false;
        DWC_SetBuddyFriendCallback(BuddyFriendCallback_801364B4, 0);
        DWC_SetFriendStatusCallback(FriendStatusCallback_80136560, 0);
        DWC_UpdateServersAsync(0,
            UpdateServersCallback_801364FC,
            0,
            FriendStatusCallback_80136560,
            0,
            DeleteFriendCallback_80136584,
            0);
    }
}

void UnidentifiedFriendManager_801360A4::SynchronizeFriends_80136F30()
{
    DWCFriendStatusCallback statusCallback = FriendStatusCallback_80136560;
    mFriendListChanged = false;
    DWC_SetBuddyFriendCallback(BuddyFriendCallback_801364B4, 0);
    DWC_SetFriendStatusCallback(statusCallback, 0);
    DWC_UpdateServersAsync(0,
        UpdateServersCallback_801364FC,
        0,
        statusCallback,
        0,
        DeleteFriendCallback_80136584,
        0);
}

void UnidentifiedFriendManager_801360A4::SetOwnStatusInitial_80136FA4(
    int available)
{
    mFriendStatusIndex = -1;
    int currentStatus = mOwnStatus.mStatus;
    int status = available != 0;
    if (currentStatus != status)
    {
        mOwnStatus.mStatus = status;
        fn_8004F594(0x10, "SetOwnStatusInitial %d\n", status);
        DWC_SetOwnStatusData(reinterpret_cast<const char*>(&mOwnStatus),
            GetStatusDataSize(mOwnStatus.mStatus));
    }
    else
    {
        fn_8004F594(0x10,
            "SetOwnStatusInitial did not change from last %d not calling DWC_SetOwnStatusData\n",
            status);
    }
}

void UnidentifiedFriendManager_801360A4::SetOwnStatusDecline_80137068(int index)
{
    mFriendStatusIndex = -1;
    GameInfoManager* gameInfo = GameInfoManager::GetInstance();
    DWCFriendData* friendData = reinterpret_cast<DWCFriendData*>(
        gameInfo->GetUnknown0x40(lbl_806E20E0, index));
    mOwnStatus.mStatus = 3;
    mOwnStatus.mProfileId = friendData->gs_profile_id.id;
    fn_8004F594(0x10, "SetOwnStatusDecline forPID %d\n", mOwnStatus.mProfileId);
    DWC_SetOwnStatusData(reinterpret_cast<const char*>(&mOwnStatus),
        GetStatusDataSize(mOwnStatus.mStatus));
}

void UnidentifiedFriendManager_801360A4::
    SetOwnStatusReceivedInvitation_80137118(int index)
{
    mFriendStatusIndex = -1;
    GameInfoManager* gameInfo = GameInfoManager::GetInstance();
    DWCFriendData* friendData = reinterpret_cast<DWCFriendData*>(
        gameInfo->GetUnknown0x40(lbl_806E20E0, index));
    mOwnStatus.mStatus = 4;
    mOwnStatus.mProfileId = friendData->gs_profile_id.id;
    fn_8004F594(0x10, "SetOwnStatusReceivedInvitation forPID %d\n", mOwnStatus.mProfileId);
    DWC_SetOwnStatusData(reinterpret_cast<const char*>(&mOwnStatus),
        GetStatusDataSize(mOwnStatus.mStatus));
}

void UnidentifiedFriendManager_801360A4::SetOwnStatusHostInviting_801371C8(
    int index, const GameplaySettings* gameplaySettings,
    const PowerupSettings* powerupSettings, u8 value)
{
    mFriendStatusIndex = index;
    GameInfoManager* gameInfo = GameInfoManager::GetInstance();
    DWCFriendData* friendData = reinterpret_cast<DWCFriendData*>(
        gameInfo->GetUnknown0x40(lbl_806E20E0, index));
    mOwnStatus.mStatus = 2;
    mOwnStatus.mProfileId = friendData->gs_profile_id.id;
    mOwnStatus.mGameplaySettings = *gameplaySettings;
    mOwnStatus.mPowerupSettings = *powerupSettings;
    mOwnStatus.mUnidentified34 = value;
    mOwnStatus.mUnidentified30 = fn_80124238();
    fn_8004F594(0x10, "SetOwnStatusHostInvitingPlayer forPID %d\n", mOwnStatus.mProfileId);
    DWC_SetOwnStatusData(reinterpret_cast<const char*>(&mOwnStatus),
        GetStatusDataSize(mOwnStatus.mStatus));
}

void UnidentifiedFriendManager_801360A4::DeleteFriend_8013730C(int index)
{
    DWCFriendData* friendData = reinterpret_cast<DWCFriendData*>(
        GameInfoManager::GetInstance()->GetUnknown0x40(lbl_806E20E0, index));
    u16* name = static_cast<u16*>(
        GameInfoManager::GetInstance()->GetUnknown0x340(lbl_806E20E0, index));
    name[0] = 0;
    u8* status = static_cast<u8*>(
        GameInfoManager::GetInstance()->GetUnknown0xA40(lbl_806E20E0, index));
    *status = 0;
    DWC_DeleteBuddyFriendData(friendData);
    SaveLoad::StartSave(true);
    mFriendListChanged = true;
}

int UnidentifiedFriendManager_801360A4::CountFriends_801373A8()
{
    int count = 0;
    for (int i = 0; i < 64; ++i)
    {
        DWCFriendData* friendData = reinterpret_cast<DWCFriendData*>(
            GameInfoManager::GetInstance()->GetUnknown0x40(lbl_806E20E0, i));
        if (DWC_IsValidFriendData(friendData))
        {
            ++count;
        }
    }
    return count;
}

int UnidentifiedFriendManager_801360A4::CountBuddies_8013740C()
{
    int count = 0;
    for (int i = 0; i < 64; ++i)
    {
        DWCFriendData* friendData = reinterpret_cast<DWCFriendData*>(
            GameInfoManager::GetInstance()->GetUnknown0x40(lbl_806E20E0, i));
        if (DWC_IsValidFriendData(friendData)
            && DWC_GetFriendDataType(friendData) == DWC_FRIENDDATA_GS_PROFILE_ID)
        {
            bool isBuddy = DWC_IsBuddyFriendData(friendData);
            if (isBuddy)
            {
                ++count;
            }
        }
    }
    return count;
}

void UnidentifiedFriendManager_801360A4::SetOwnStatusAvailable_801374A4()
{
    if (mUnidentified010 == 1)
    {
        fn_8025BD7C(true);
    }
    lbl_806E1194->SetOwnStatusInitial_80136FA4(true);
}
