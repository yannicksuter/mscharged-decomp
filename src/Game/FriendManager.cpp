#include "NL/nlSingleton.inl"
#include <dwc/dwc_account.h>
#include "Game/Sys/debug.h"
#include <dwc/dwc_base64.h>
#include <dwc/dwc_common.h>
#include <dwc/dwc_friend.h>
#include "NL/nlstring_tmpl.h"

#include "Game/FriendManager.h"

#include "Game/GameSceneManager.h"
#include "Game/DB/SaveLoad.h"
#include "Game/GameInfo.h"
#include "Game/NetworkSession.h"
#include "Game/NetworkLobby.h"

#include <string.h>
#include "Game/UnidentifiedStaticStorage.h"

FriendManager* g_pFriendManagerInstance;
FriendManager* g_pFriendManager;

static const char* sFriendStatusNames[] = {
    "Offline",
    "Online",
    "Playing",
    "MatchAny",
    "MatchFriend",
    "MatchSCClient",
    "MatchSCServ",
};

static inline int GetStatusDataSize(u8 status)
{
    switch (status)
    {
    case EFriendStatus_Initial_NotAvailable:
    case EFriendStatus_Initial_Available:
        return 4;
    case EFriendStatus_ClientDecliningHost:
    case EFriendStatus_ClientReceivedInvitation:
        return 8;
    case EFriendStatus_HostInvitingPlayer:
        return sizeof(FriendStatusPayload);
    default:
        return 0;
    }
}

FriendManager::FriendManager()
{
    Reset(true);
}

void FriendManager::Reset(bool setInstance)
{
    if (setInstance)
    {
        g_pFriendManagerInstance = this;
    }

    mUpdateTime = 0.0f;
    mFriendListChanged = false;
    mHostInvitationIndex = -1;
    mFriendStatusIndex = -1;
    mReturnScene = -2;
    mPreviousRankedMode = 0;
    memset(mFriendCodeInput, 0, sizeof(mFriendCodeInput));

    mOwnStatus.mHeader.mMagic[0] = 'S';
    mOwnStatus.mHeader.mMagic[1] = 'C';
    mOwnStatus.mHeader.mMagic[2] = '2';
    mOwnStatus.mHeader.mStatus = EFriendStatus_Initial_NotAvailable;

    for (int i = 0; i < 64; ++i)
    {
        mFriendStatus[i].mHeader.mMagic[0] = 'S';
        mFriendStatus[i].mHeader.mMagic[1] = 'C';
        mFriendStatus[i].mHeader.mMagic[2] = '2';
        mFriendStatus[i].mHeader.mStatus = EFriendStatus_Initial_NotAvailable;
        mFriendStatusChanged[i] = false;
    }
}

void FriendManager::GetOwnFriendKeyString(u16* output)
{
    DWCUserData* userData = reinterpret_cast<DWCUserData*>(
        GameInfoManager::GetInstance()->GetSaveSlot(gNetworkSaveSlotIndex));
    u64 friendKey = DWC_CreateFriendKey(userData);
    GameInfoManager::GetInstance()->ValidateSaveSlot(gNetworkSaveSlotIndex);
    FormatFriendKey(friendKey, output);
}

bool FriendManager::AddFriendKey(unsigned long long friendKey, int* error)
{
    *error = -1;

    DWCUserData* userData = reinterpret_cast<DWCUserData*>(
        GameInfoManager::GetInstance()->GetSaveSlot(gNetworkSaveSlotIndex));
    u64 ownFriendKey = DWC_CreateFriendKey(userData);
    GameInfoManager::GetInstance()->ValidateSaveSlot(gNetworkSaveSlotIndex);

    if (friendKey == ownFriendKey)
    {
        *error = 0x66;
        return false;
    }

    userData = reinterpret_cast<DWCUserData*>(
        GameInfoManager::GetInstance()->GetSaveSlot(gNetworkSaveSlotIndex));
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
            GameInfoManager::GetInstance()->GetUnknown0x40(gNetworkSaveSlotIndex, i));
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
            GameInfoManager::GetInstance()->GetUnknown0x40(gNetworkSaveSlotIndex, i));
        if (!DWC_IsValidFriendData(friendData))
        {
            memcpy(friendData, &token, sizeof(token));
            u16* name = GameInfoManager::GetInstance()->GetSavedFriendName(gNetworkSaveSlotIndex, i);
            if (name[0] == 0)
            {
                FormatFriendKey(DWC_GetFriendKey(friendData), name);
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

static void BuddyFriendCallback(int index, void*)
{
    FriendManager* manager = g_pFriendManager;
    tDebugPrintManager::Print(DC_NETWORK, "Got friendship with friend [%d].]\n", index);
    manager->mFriendListChanged = true;
}

static void UpdateServersCallback(int error, BOOL isChanged, void*)
{
    if (error == 0)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Friends list synchronization successful (isChanged == %s)\n",
            isChanged ? "true" : "false");
        if (isChanged)
        {
            SaveLoad::StartSave(true);
        }
    }
}

static void FriendStatusCallback(
    int index, u8 status, const char* statusString, void* param)
{
    g_pFriendManager->HandleFriendStatus(
        index, status, statusString, param);
}

static void DeleteFriendCallback(
    int deletedIndex, int sourceIndex, void*)
{
    tDebugPrintManager::Print(DC_NETWORK,
        "Friend [%d] was deleted (equal friend[%d]).\n",
        deletedIndex,
        sourceIndex);
    SaveLoad::StartSave(true);
}

static inline bool IsValidFriendStatusHeader(const FriendStatusHeader& header)
{
    if (header.mMagic[0] == 'S'
        && header.mMagic[1] == 'C'
        && header.mMagic[2] == '2'
        && header.mStatus <= EFriendStatus_ClientReceivedInvitation)
    {
        return true;
    }
    return false;
}

static inline bool HaveSameFriendStatusHeader(
    FriendStatusHeader& current, const FriendStatusHeader& previous)
{
    if (current.mStatus == previous.mStatus
        && current.mMagic[0] == previous.mMagic[0]
        && current.mMagic[1] == previous.mMagic[1]
        && current.mMagic[2] == previous.mMagic[2])
    {
        return true;
    }
    return false;
}

static inline bool HaveSameInvitationSettings(
    const FriendStatusPayload& current, const FriendStatusPayload& previous)
{
    if (current.mStadium == previous.mStadium
        && current.mNetworkVersion == previous.mNetworkVersion
        && current.mPowerupSettings.mCustomPowerups
            == previous.mPowerupSettings.mCustomPowerups
        && current.mPowerupSettings.mEnvironmentCheat
            == previous.mPowerupSettings.mEnvironmentCheat
        && current.mPowerupSettings.mPlayerCheat
            == previous.mPowerupSettings.mPlayerCheat
        && current.mGameplaySettings.GameLimitType
            == previous.mGameplaySettings.GameLimitType
        && current.mGameplaySettings.NumGames
            == previous.mGameplaySettings.NumGames
        && (current.mGameplaySettings.GameLimitType == 0
            ? current.mGameplaySettings.GameTime
                == previous.mGameplaySettings.GameTime
            : current.mGameplaySettings.GoalLimit
                == previous.mGameplaySettings.GoalLimit))
    {
        return true;
    }
    return false;
}

static inline bool HaveSameFriendStatus(
    FriendStatusPayload& current, const FriendStatusPayload& previous)
{
    if (HaveSameFriendStatusHeader(current.mHeader, previous.mHeader))
    {
        if (current.mHeader.mStatus <= EFriendStatus_Initial_Available)
        {
            return true;
        }
        if (current.mHeader.mStatus == EFriendStatus_ClientDecliningHost
            || current.mHeader.mStatus == EFriendStatus_ClientReceivedInvitation)
        {
            return current.mProfileId == previous.mProfileId;
        }
        if (current.mProfileId == previous.mProfileId)
        {
            return HaveSameInvitationSettings(current, previous);
        }
        return false;
    }
    return false;
}

void FriendManager::HandleFriendStatus(
    int index, u8 status, const char* statusString, void*)
{
    GameInfoManager* gameInfo = GameInfoManager::GetInstance();
    DWCFriendData* friendData = reinterpret_cast<DWCFriendData*>(
        gameInfo->GetUnknown0x40(gNetworkSaveSlotIndex, index));
    if (!DWC_IsValidFriendData(friendData))
    {
        return;
    }

    const char* statusName = sFriendStatusNames[status];
    int friendType = DWC_GetFriendDataType(friendData);
    bool isBuddy = DWC_IsBuddyFriendData(friendData);
    tDebugPrintManager::Print(DC_NETWORK,
        "friend[%.2d] type %d Friend:%s status %s (%s).\n",
        index, friendType, isBuddy ? "Yes" : "No", statusName, statusString);

    FriendStatusPayload* current = &mFriendStatus[index];
    FriendStatusPayload previous = *current;

    u32 encodedLength = nlStrLen<char>(statusString);
    int decodedLength = DWC_Base64Decode(statusString, encodedLength,
        reinterpret_cast<char*>(current), sizeof(*current));

    bool valid = false;
    if (decodedLength >= sizeof(FriendStatusHeader)
        && IsValidFriendStatusHeader(current->mHeader)
        && decodedLength == GetStatusDataSize(current->mHeader.mStatus))
    {
        valid = true;
    }
    if (!valid)
    {
        current->mHeader.mMagic[0] = 'S';
        current->mHeader.mMagic[1] = 'C';
        current->mHeader.mMagic[2] = '2';
        current->mHeader.mStatus = EFriendStatus_Initial_NotAvailable;
    }

    if (HaveSameFriendStatus(*current, previous))
    {
        return;
    }

    switch (current->mHeader.mStatus)
    {
    case EFriendStatus_Initial_Available:
        tDebugPrintManager::Print(DC_NETWORK,
            "FriendStatusChanged FriendPID %d EFriendStatus_Initial_Available\n",
            friendData->gs_profile_id.id);
        break;
    case EFriendStatus_Initial_NotAvailable:
        tDebugPrintManager::Print(DC_NETWORK,
            "FriendStatusChanged FriendPID %d EFriendStatus_Initial_NotAvailable\n",
            friendData->gs_profile_id.id);
        break;
    case EFriendStatus_HostInvitingPlayer:
        tDebugPrintManager::Print(DC_NETWORK,
            "FriendStatusChanged FriendPID %d EFriendStatus_HostInvitingPlayer forPID %d\n",
            friendData->gs_profile_id.id,
            current->mProfileId);
        break;
    case EFriendStatus_ClientDecliningHost:
        tDebugPrintManager::Print(DC_NETWORK,
            "FriendStatusChanged FriendPID %d EFriendStatus_ClientDecliningHost forPID %d\n",
            friendData->gs_profile_id.id,
            current->mProfileId);
        break;
    case EFriendStatus_ClientReceivedInvitation:
        tDebugPrintManager::Print(DC_NETWORK,
            "FriendStatusChanged FriendPID %d EFriendStatus_ClientReceivedInvitation forPID %d\n",
            friendData->gs_profile_id.id,
            current->mProfileId);
        break;
    default:
        tDebugPrintManager::Print(DC_NETWORK,
            "FriendStatusChanged FriendPID %d Invalid Status %d\n",
            friendData->gs_profile_id.id,
            current->mHeader.mStatus);
        break;
    }

    mFriendStatusChanged[index] = true;
}

FriendStatusPayload* FriendManager::GetFriendStatusPayload(int index)
{
    if (index >= 0 && static_cast<unsigned int>(index) < 64)
    {
        return &mFriendStatus[index];
    }
    return 0;
}

bool FriendManager::FindHostInvitation()
{
    for (int i = 0; i < 64; ++i)
    {
        if (!mFriendStatusChanged[i])
        {
            continue;
        }

        DWCFriendData* friendData = reinterpret_cast<DWCFriendData*>(
            GameInfoManager::GetInstance()->GetUnknown0x40(gNetworkSaveSlotIndex, i));
        if (!DWC_IsValidFriendData(friendData) || (int)mFriendStatus[i].mHeader.mStatus != EFriendStatus_HostInvitingPlayer)
        {
            continue;
        }

        DWCUserData* userData = reinterpret_cast<DWCUserData*>(
            GameInfoManager::GetInstance()->GetSaveSlot(gNetworkSaveSlotIndex));
        if (userData->gs_profile_id != mFriendStatus[i].mProfileId)
        {
            continue;
        }

        const u16* name = GameInfoManager::GetInstance()->GetSavedFriendName(gNetworkSaveSlotIndex, i);
        unsigned int length = 0;
        if (name != 0)
        {
            while (*name != 0)
            {
                ++name;
                ++length;
            }
        }
        if (length > 11 || mFriendStatus[i].mNetworkVersion != GetNetworkVersionWord())
        {
            continue;
        }

        mHostInvitationIndex = i;
        mFriendStatusChanged[i] = false;
        return true;
    }
    return false;
}

bool FriendManager::ValidateHostInvitation()
{
    int index = mHostInvitationIndex;
    if (index >= 0 && index < 64)
    {
        DWCFriendData* friendData = reinterpret_cast<DWCFriendData*>(
            GameInfoManager::GetInstance()->GetUnknown0x40(
                gNetworkSaveSlotIndex, index));
        if (DWC_IsValidFriendData(friendData)
            && (int)mFriendStatus[index].mHeader.mStatus == EFriendStatus_HostInvitingPlayer)
        {
            DWCUserData* userData = reinterpret_cast<DWCUserData*>(
                GameInfoManager::GetInstance()->GetSaveSlot(gNetworkSaveSlotIndex));
            if (userData->gs_profile_id == mFriendStatus[index].mProfileId)
            {
                return true;
            }
        }
    }

    mHostInvitationIndex = -1;
    return false;
}

bool FriendManager::HasFriendDeclined()
{
    int index = mFriendStatusIndex;
    if (index >= 0 && index < 64)
    {
        DWCFriendData* friendData = reinterpret_cast<DWCFriendData*>(
            GameInfoManager::GetInstance()->GetUnknown0x40(gNetworkSaveSlotIndex, index));
        if (DWC_IsValidFriendData(friendData))
        {
            if (DWC_GetFriendStatus(friendData, 0) == 0)
            {
                return true;
            }
            if ((int)mFriendStatus[index].mHeader.mStatus == EFriendStatus_ClientDecliningHost)
            {
                DWCUserData* userData = reinterpret_cast<DWCUserData*>(
                    GameInfoManager::GetInstance()->GetSaveSlot(gNetworkSaveSlotIndex));
                if (userData->gs_profile_id == mFriendStatus[index].mProfileId)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

int FriendManager::GetFriendInvitationResponse()
{
    int index = mFriendStatusIndex;
    if (index >= 0 && index < 64)
    {
        DWCFriendData* friendData = reinterpret_cast<DWCFriendData*>(
            GameInfoManager::GetInstance()->GetUnknown0x40(gNetworkSaveSlotIndex, index));
        if (DWC_IsValidFriendData(friendData))
        {
            if (DWC_GetFriendStatus(friendData, 0) == 0)
            {
                return 0;
            }
            if ((int)mFriendStatus[index].mHeader.mStatus == EFriendStatus_ClientReceivedInvitation)
            {
                DWCUserData* userData = reinterpret_cast<DWCUserData*>(
                    GameInfoManager::GetInstance()->GetSaveSlot(gNetworkSaveSlotIndex));
                int response = 2;
                if (userData->gs_profile_id == mFriendStatus[index].mProfileId)
                {
                    response = 1;
                }
                return response;
            }
        }
    }
    return 0;
}

void FriendManager::Update(float dt)
{
    mUpdateTime += dt;
    if (!mFriendListChanged)
    {
        return;
    }
    if (g_pNetworkSessionBase->GetSessionState() != 2)
    {
        return;
    }

    bool matchmaking = false;
    NetworkLobby* lobby = g_pNetworkSession->GetOnlineLobby();
    if (lobby != 0 && lobby->IsMatchmaking())
    {
        matchmaking = true;
    }
    bool invitationScene = false;
    if (GameSceneManager::Instance()->IsOnStack(static_cast<SceneList>(0x2F)))
    {
        invitationScene = true;
    }
    if (matchmaking && !invitationScene)
    {
        return;
    }

    if (mFriendListChanged)
    {
        mFriendListChanged = false;
        DWC_SetBuddyFriendCallback(BuddyFriendCallback, 0);
        DWC_SetFriendStatusCallback(FriendStatusCallback, 0);
        DWC_UpdateServersAsync(0,
            UpdateServersCallback,
            0,
            FriendStatusCallback,
            0,
            DeleteFriendCallback,
            0);
    }
}

void FriendManager::SynchronizeFriends()
{
    mFriendListChanged = false;
    DWC_SetBuddyFriendCallback(BuddyFriendCallback, 0);
    DWC_SetFriendStatusCallback(FriendStatusCallback, 0);
    if (!DWC_UpdateServersAsync(0,
            UpdateServersCallback,
            0,
            FriendStatusCallback,
            0,
            DeleteFriendCallback,
            0))
    {
        // Synchronization failed to start
        return;
    }
}

void FriendManager::SetOwnStatusInitial(int available)
{
    mFriendStatusIndex = -1;
    int currentStatus = mOwnStatus.mHeader.mStatus;
    EFriendStatus status = available ? EFriendStatus_Initial_Available
                                    : EFriendStatus_Initial_NotAvailable;
    if (currentStatus != status)
    {
        mOwnStatus.mHeader.mStatus = status;
        tDebugPrintManager::Print(DC_NETWORK, "SetOwnStatusInitial %d\n", status);
        DWC_SetOwnStatusData(reinterpret_cast<const char*>(&mOwnStatus),
            GetStatusDataSize(mOwnStatus.mHeader.mStatus));
    }
    else
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "SetOwnStatusInitial did not change from last %d not calling DWC_SetOwnStatusData\n",
            status);
    }
}

void FriendManager::SetOwnStatusDecline(int index)
{
    mFriendStatusIndex = -1;
    GameInfoManager* gameInfo = GameInfoManager::GetInstance();
    DWCFriendData* friendData = reinterpret_cast<DWCFriendData*>(
        gameInfo->GetUnknown0x40(gNetworkSaveSlotIndex, index));
    mOwnStatus.mHeader.mStatus = EFriendStatus_ClientDecliningHost;
    mOwnStatus.mProfileId = friendData->gs_profile_id.id;
    tDebugPrintManager::Print(DC_NETWORK, "SetOwnStatusDecline forPID %d\n", mOwnStatus.mProfileId);
    DWC_SetOwnStatusData(reinterpret_cast<const char*>(&mOwnStatus),
        GetStatusDataSize(mOwnStatus.mHeader.mStatus));
}

void FriendManager::SetOwnStatusReceivedInvitation(int index)
{
    mFriendStatusIndex = -1;
    GameInfoManager* gameInfo = GameInfoManager::GetInstance();
    DWCFriendData* friendData = reinterpret_cast<DWCFriendData*>(
        gameInfo->GetUnknown0x40(gNetworkSaveSlotIndex, index));
    mOwnStatus.mHeader.mStatus = EFriendStatus_ClientReceivedInvitation;
    mOwnStatus.mProfileId = friendData->gs_profile_id.id;
    tDebugPrintManager::Print(DC_NETWORK, "SetOwnStatusReceivedInvitation forPID %d\n", mOwnStatus.mProfileId);
    DWC_SetOwnStatusData(reinterpret_cast<const char*>(&mOwnStatus),
        GetStatusDataSize(mOwnStatus.mHeader.mStatus));
}

void FriendManager::SetOwnStatusHostInvitingPlayer(
    int index, const GameplaySettings* gameplaySettings,
    const CheatSettings* cheatSettings, u8 stadium)
{
    mFriendStatusIndex = index;
    GameInfoManager* gameInfo = GameInfoManager::GetInstance();
    DWCFriendData* friendData = reinterpret_cast<DWCFriendData*>(
        gameInfo->GetUnknown0x40(gNetworkSaveSlotIndex, index));
    mOwnStatus.mHeader.mStatus = EFriendStatus_HostInvitingPlayer;
    mOwnStatus.mProfileId = friendData->gs_profile_id.id;
    mOwnStatus.mGameplaySettings = *gameplaySettings;
    mOwnStatus.mPowerupSettings = *cheatSettings;
    mOwnStatus.mStadium = stadium;
    mOwnStatus.mNetworkVersion = GetNetworkVersionWord();
    tDebugPrintManager::Print(DC_NETWORK, "SetOwnStatusHostInvitingPlayer forPID %d\n", mOwnStatus.mProfileId);
    DWC_SetOwnStatusData(reinterpret_cast<const char*>(&mOwnStatus),
        GetStatusDataSize(mOwnStatus.mHeader.mStatus));
}

void FriendManager::DeleteFriend(int index)
{
    DWCFriendData* friendData = reinterpret_cast<DWCFriendData*>(
        GameInfoManager::GetInstance()->GetUnknown0x40(gNetworkSaveSlotIndex, index));
    u16* name = GameInfoManager::GetInstance()->GetSavedFriendName(gNetworkSaveSlotIndex, index);
    name[0] = 0;
    u8* status = static_cast<u8*>(
        GameInfoManager::GetInstance()->GetUnknown0xA40(gNetworkSaveSlotIndex, index));
    *status = 0;
    DWC_DeleteBuddyFriendData(friendData);
    SaveLoad::StartSave(true);
    mFriendListChanged = true;
}

int FriendManager::CountFriends()
{
    int count = 0;
    for (int i = 0; i < 64; ++i)
    {
        DWCFriendData* friendData = reinterpret_cast<DWCFriendData*>(
            GameInfoManager::GetInstance()->GetUnknown0x40(gNetworkSaveSlotIndex, i));
        if (DWC_IsValidFriendData(friendData))
        {
            ++count;
        }
    }
    return count;
}

int FriendManager::CountBuddies()
{
    int count = 0;
    for (int i = 0; i < 64; ++i)
    {
        DWCFriendData* friendData = reinterpret_cast<DWCFriendData*>(
            GameInfoManager::GetInstance()->GetUnknown0x40(gNetworkSaveSlotIndex, i));
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

void FriendManager::SetOwnStatusAvailable()
{
    if (mPreviousRankedMode == 1)
    {
        SetOnlineRankedMatch(true);
    }
    g_pFriendManager->SetOwnStatusInitial(true);
}
