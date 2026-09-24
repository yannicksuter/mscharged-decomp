#include "Game/HBMManager.h"
#include "NL/nlFunction.inl"
#include "Game/SH/SHNavigation.h"
#include "Game/FE/fePopupMenu.h"

#include "Game/FE/FEAudio.h"
#include "Game/FE/feInput.h"
#include "NL/nlMemory.h"
#include "NL/nlBind.h"
#include "Game/GameSceneManager.h"
#include "Game/FE/feFinder.h"
#include "NL/nlString.h"
#include "Game/FE/feDPD.h"
#include "Game/GameInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/DB/SaveLoad.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/NetTournManager.h"
#include "Game/NetworkSession.h"
#include "Game/NetworkDraft.h"
#include "Game/FriendManager.h"
#include "Game/TrophyInfo.h"
#include "Game/FE/feHelpFuncs.h"
#include "Game/FE/feOnlineError.h"
#include "Game/FE/feOptionsSubMenus.h"
#include "Game/FE/feText.h"
#include "Game/FE/feScene.h"
#include "Game/FE/fePackage.h"
#include "NL/nlConfig.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalization.h"
#include "NL/plat/SocketNetwork.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/fePresentation.inl"
#include "NL/nlstring_tmpl.h"
#include "Game/DB/CharacterInfo.inl"
#include "Game/DB/GameProgress.inl"
#include "NL/nlLocalizationLookup.h"
#include "Game/FE/tlInstance.inl"
#include "Game/GameInfo.inl"
#include "Game/SH/SHNavigation.inl"
#include <cmath>

char* optionNames[3] = { "button_1", "button_2", "button_3" };
static const char* optionOKName = "button_OK";

struct PopupEntry
{
    int mMessageType;
    const char* mMessage;
    const char* mOptions[4];
    int mInitialHighlight;
};

static const PopupEntry PopupEntries[] = {
    { 0, "POPUP_END_CUP_MESSAGE", { "POPUP_END_CUP_SAVE", "POPUP_CANCEL", 0, 0 }, 1 },
    { 0, "POPUP_END_CUP_MESSAGE", { "POPUP_END_CUP_SAVE", "CUP_BACKPOPUP_RESTART", "POPUP_CANCEL", 0 }, 1 },
    { 0, "POPUP_END_CUP_MESSAGE", { "PAUSE_QUIT", "POPUP_CANCEL", 0, 0 }, 1 },
    { 0, "POPUP_END_CUP_MESSAGE", { "PAUSE_QUIT", "CUP_BACKPOPUP_RESTART", "POPUP_CANCEL", 0 }, 1 },
    { 1, "POPUP_RESTART_CUP_MESSAGE", { "POPUP_YES", "POPUP_NO", 0, 0 }, 1 },
    { 2, "POPUP_FLOWER_LOCKED_MESSAGE", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 2, "POPUP_STAR_LOCKED_MESSAGE", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 2, "POPUP_BOWSER_LOCKED_MESSAGE", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 2, "POPUP_SUPER_LOCKED_MESSAGE", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_FORFEIT_MESSAGE", { "POPUP_YES", "POPUP_NO", 0, 0 }, 1 },
    { 1, "POPUP_QUIT_MESSAGE", { "POPUP_YES", "POPUP_NO", 0, 0 }, 1 },
    { 1, "STRIKERS_101_QUITTING", { "POPUP_YES", "POPUP_NO", 0, 0 }, 1 },
    { 2, "POPUP_NO_SIDES_MESSAGE", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 2, "POPUP_MUST_CHOOSE_SIDES_MESSAGE", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 2, "POPUP_NO_HUMAN_MESSAGE", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 1, "POPUP_NEW_CUP_MESSAGE", { "POPUP_CONTINUE_CUP", "POPUP_NEW_CUP", "POPUP_CANCEL", 0 }, 0 },
    { 1, "POPUP_CONFIRM_NEW_CUP", { "POPUP_YES", "POPUP_NO", 0, 0 }, 1 },
    { 1, "POPUP_CONFIRM_CUP_TEAM", { "POPUP_YES", "POPUP_NO", 0, 0 }, 0 },
    { 1, "POPUP_NEW_CUP_MESSAGE", { "POPUP_NEW_TOURN_OPTION1", "POPUP_NEW_TOURN_OPTION2", 0, 0 }, 0 },
    { 2, "POPUP_FILLOUTALLENTRIES_MESSAGE", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 1, "OPTIONS_POPUP_CONFIRM_CHANGES", { "POPUP_YES", "POPUP_NO", 0, 0 }, 1 },
    { 1, "POPUP_END_CUSTOM_SINGLE_PLAYER", { "POPUP_YES", "POPUP_NO", 0, 0 }, 1 },
    { 0, "FORFEIT_NO_TEAM", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 1, "POPUP_NEWCUP_CONFIRM", { "POPUP_YES", "POPUP_NO", 0, 0 }, 1 },
    { 0, "CHANGE_AUDIO_SETTINGS", { 0, 0, 0, 0 }, 0 },
    { 0, "POPUP_CUP_WIN_MESSAGE", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_CUP_LOSE_MESSAGE", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_CUP_DNF_MESSAGE", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_CUP_OTHER_WIN_MESSAGE", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_FIRE_QUAL_RULES", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_CRYSTAL_QUAL_RULES", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_STRIKER_QUAL_RULES", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_FIRE_ELIM_RULES", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_CRYSTAL_ELIM_RULES", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_STRIKER_ELIM_RULES", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_FIRE_FINAL_RULES", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_CRYSTAL_FINAL_RULES", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_STRIKER_FINAL_RULES", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "RTSC_STANDINGS_HELP", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_CUP_AWARDS_BRICK", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_CUP_AWARDS_STRIKER", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_WIN_FIRECUP_BOOT_WALL_REWARDS", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_WIN_CRYSTALCUP_BOOT_WALL_REWARDS", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_WIN_STRIKERCUP_BOOT_WALL_REWARDS", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_WIN_FIRECUP_MAINCUP_REWARDS", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_WIN_CRYSTALCUP_MAINCUP_REWARDS", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_WIN_STRIKERCUP_MAINCUP_REWARDS", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_CHALLENGE_UNLOCK_MARIO", { "POPUP_SELECT_NEW_CHALLENGE", "POPUP_RESTART_CHALLENGE", "POPUP_MAIN_MENU", 0 }, 0 },
    { 0, "POPUP_CHALLENGE_SUCCEEDED", { "POPUP_SELECT_NEW_CHALLENGE", "POPUP_RESTART_CHALLENGE", "POPUP_MAIN_MENU", 0 }, 0 },
    { 0, "POPUP_CHALLENGE_FAILED", { "POPUP_SELECT_NEW_CHALLENGE", "POPUP_RESTART_CHALLENGE", "POPUP_MAIN_MENU", 0 }, 0 },
    { 0, "POPUP_CHALLENGE_UNLOCK_CLASSROOM", { "POPUP_SELECT_NEW_TUTORIAL", "POPUP_RESTART_TUTORIAL", "POPUP_MAIN_MENU", 0 }, 0 },
    { 0, "POPUP_TUTORIAL_SUCCEEDED", { "POPUP_SELECT_NEW_TUTORIAL", "POPUP_RESTART_TUTORIAL", "POPUP_MAIN_MENU", 0 }, 0 },
    { 0, "POPUP_TUTORIAL_FAILED", { "POPUP_SELECT_NEW_TUTORIAL", "POPUP_RESTART_TUTORIAL", "POPUP_MAIN_MENU", 0 }, 0 },
    { 0, "POPUP_SERIES_OVER", { "POPUP_REPLAY_SERIES", "POPUP_CHOOSE_NEW_TEAMS", "POPUP_MAIN_MENU", 0 }, 0 },
    { 2, "POPUP_NO_GAME_RESULTS", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_GAME_OVER", { "POPUP_RESTART_CUP", 0, 0, 0 }, 0 },
    { 0, "STRIKER_CUP_COMPLETE", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 1, "POPUP_CUP_DIFFICULTY", { "POPUP_NORMAL_DIFFICULTY", "POPUP_EXTREME_DIFFICULTY", 0, 0 }, 0 },
    { 0, "POPUP_ONLINE_MENU_HELP", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_CHOOSE_SIDES_HELP", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_CHOOSE_SIDES_NUNCHUCK", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 1, "POPUP_NO_SAVE_FILE", { "POPUP_CREATE_NEW_FILE", "POPUP_CONTINUE_NO_SAVE", 0, 0 }, 0 },
    { 0, "POPUP_SAVE_INFO", { "POPUP_CREATE_NEW_FILE", "POPUP_CONTINUE_NO_SAVE", 0, 0 }, 0 },
    { 1, "POPUP_NO_SAVE_SPACE", { "POPUP_CONTINUE_NO_SAVE", "POPUP_MANAGE_MEMORY", 0, 0 }, 0 },
    { 1, "POPUP_NO_SAVE_SPACE_ONLINE", { "POPUP_MAIN_MENU", "POPUP_MANAGE_MEMORY", 0, 0 }, 0 },
    { 1, "POPUP_NO_SAVE_INODES", { "POPUP_CONTINUE_NO_SAVE", "POPUP_MANAGE_MEMORY", 0, 0 }, 0 },
    { 1, "POPUP_NO_SAVE_INODES", { "POPUP_MAIN_MENU", "POPUP_MANAGE_MEMORY", 0, 0 }, 0 },
    { 2, "POPUP_NAND_CORRUPT", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 2, "POPUP_NAND_ERROR", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 2, "POPUP_SAVE_FILE_ERROR", { "POPUP_DELETE_FILE", 0, 0, 0 }, 0 },
    { 0, "POPUP_ONLINE_SAVE_DATA_REQUIRED", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_ONLINE_NO_COPY", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 3, "POPUP_UNLOCK_FLOWERCUP", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 3, "POPUP_UNLOCK_STARCUP", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 3, "POPUP_UNLOCK_BOWSERCUP", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 3, "POPUP_UNLOCK_SUPERCUPS", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 3, "POPUP_UNLOCK_POWERUPSET", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 3, "POPUP_UNLOCK_KONGASTAD", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 3, "POPUP_UNLOCK_YOSHISTAD", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 3, "POPUP_UNLOCK_FORBIDDENSTAD", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 3, "POPUP_UNLOCK_SUPERSTAD", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 3, "POPUP_UNLOCK_LEGENDDIFF", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 3, "POPUP_UNLOCK_SUPERTEAM", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 3, "POPUP_UNLOCK_CHEAT_GOALIE", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 3, "POPUP_UNLOCK_CHEAT_INFINITE", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 3, "POPUP_UNLOCK_CHEAT_TILT", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 3, "POPUP_UNLOCK_CHEAT_ALLSTS", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 3, "POPUP_UNLOCK_CREDITS", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_CREATE_FAILED", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_JOIN_FAILED", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_MATCHMAKING_ERROR", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "ONLINE_ERROR_CONNECT_FRIEND", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_NO_LOCAL_IP", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_FAILED_TO_CONNECT", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_NO_GAMES_TO_JOIN", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_JOIN_REJECTED", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_CONNECTION_LOST", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_CLIENTS_FAILED_CONNECT", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_START_FAILED", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_SYNC_ERROR", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_OVERFLOW", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_INVALID_FRIEND_CODE", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_OWN_FRIEND_CODE", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_DUPLICATE_FRIEND", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_FRIEND_LIST_FULL", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "ONLINE_REGION_NOT_CONFIRMED", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 1, "POPUP_NETWORK_DELETE_FRIEND", { "POPUP_YES", "POPUP_NO", 0, 0 }, 1 },
    { 0, "POPUP_NETWORK_CONNECT_WIFI_ERROR", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_CONNECT_WIFI_ERRORSPECIFIC", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_LOGINDWC_AUTHERR", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_LOGINDWC_AUTHERRSPECIFIC", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_LOGINGETSTATS_ERROR", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_LOGINBADNAME", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_NO_FRIENDS", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_CONNECTION_REJECTED", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_ONLINE_PLAYER_BUSY_INVITATION", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_ERROR_NO_CONNECTION", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_ERROR_SERVICE_DOWN", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_ERROR_DISCONTINUED", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_ERROR_NO_SPACE", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_ERROR_UNABLE_TO_CONNECT", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_ERROR_NO_RESPONSE", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_ERROR_DISCONNECTED", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_ERROR_COMMUNICATION", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_NETWORK_NAND_CORRUPT", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_LOCKED_FIRE_CUP", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_LOCKED_STRIKER_CUP", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_LOCKED_CRYSTAL_CUP", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_LOCKED_FIRE_BRICK_WALL", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_LOCKED_FIRE_GOLDEN_BOOT", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_LOCKED_STRIKER_BRICK_WALL", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_LOCKED_STRIKER_GOLDEN_BOOT", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_LOCKED_CRYSTAL_BRICK_WALL", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_LOCKED_CRYSTAL_GOLDEN_BOOT", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_PROFILE_NO_MIIS", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_PROFILE_DATABASE_CORRUPT", { "POPUP_MANAGE_MEMORY", 0, 0, 0 }, 0 },
    { 0, "POPUP_PROFILE_MII_NOT_FOUND", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 1, "POPUP_PROFILE_UNLINK_WARNING", { "POPUP_YES", "POPUP_NO", 0, 0 }, 1 },
    { 0, "POPUP_PROFILE_MII_DELETION_WARNING", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_PROFILE_NO_SLOTS_LEFT", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_DELETED_MII", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_LOW_BATTERY", { "POPUP_OK", 0, 0, 0 }, 0 },
    { 0, "POPUP_E3_THANKS_FOR_PLAYING", { "POPUP_OK", 0, 0, 0 }, 0 },
};

FEPopupMenu::FEPopupMenu()
    : mMenuDisplayed(false)
    , mUnidentified99D(false)
    , mMenuCreated(false)
    , mRunCallBack(false)
    , mUnknownA1F(false)
    , mUnidentified9A1(false)
    , mUnidentified9A2(false)
    , mUnidentified9A3(false)
    , mHighlightedOption(0)
    , mUnidentified9A8(0)
    , mAcceptDelayTime(0.0f)
    , mUnidentifiedC08(0)
    , mUnidentifiedC0C(8)
    , mUnknownA64()
    , mType(INVALID_TYPE)
    , mUnknownAA4(true)
    , mUnknownAA5(false)
    , mUnidentifiedC4C(false)
    , mUnidentifiedC50(0.0f)
    , mUnidentifiedC54(0.0f)
    , mUnidentifiedC58(false)
{
    mPopup.numOptions = 0;
    mPopup.pMessage = 0;
    mPopup.pOptionLabels[0] = 0;
    mPopup.pOptionLabels[1] = 0;
    mPopup.pOptionLabels[2] = 0;

    mControllerComponents[0].mContext = 0;
    mControllerComponents[0].mIgnoreInputLock = true;
    mOptionInstances[0] = 0;
    mUnidentified9D0[0] = 0;

    mControllerComponents[1].mContext = (void*)1;
    mControllerComponents[1].mIgnoreInputLock = true;
    mOptionInstances[1] = 0;
    mUnidentified9D0[1] = 0;

    mControllerComponents[2].mContext = (void*)2;
    mControllerComponents[2].mIgnoreInputLock = true;
    mOptionInstances[2] = 0;
    mUnidentified9D0[2] = 0;

    for (int i = 0; i < 4; ++i)
    {
        mUnidentifiedBF8[i] = 0;
    }

    g_pFEInput->PushExclusiveInputLock(this, 10);
    FEAudio::EnableSounds(false);
}

FEPopupMenu::~FEPopupMenu()
{
    for (int optionIndex = 0; optionIndex < mPopup.numOptions; ++optionIndex)
    {
        delete mPopup.pOptionLabels[optionIndex];
    }

    if (mPopup.pMessage != 0)
    {
        delete mPopup.pMessage;
    }

    g_pFEInput->PopExclusiveInputLock(this);
    FEAudio::EnableSounds(true);

    if (!mUnidentified9A3)
    {
        gpHBMManager->mBlocked = false;
    }

    SHNavigation* scene = GetNavigationScene();
    if (scene != 0)
    {
        scene->RestoreButtonVisibility();
    }

    if (mRunCallBack == true)
    {
        Function<FnVoidVoid>& callback = callBacks[mHighlightedOption];
        callback();
    }
    else if (mUnknownA1F != false)
    {
        mUnknownA64();
    }
}

void FEPopupMenu::SceneCreated()
{
    FEPresentation* presentation = mPresentation;
    TLTextInstance* pText = FEFinder<TLTextInstance, 3>::Find<FEPresentation>(
        presentation, "Slide1", "Layer", "Message");
    pText->SetString(mPopup.pMessage->begin());
    if (mUnknownAA5)
    {
        nlVector2 boxSize = pText->fn_801CA5E8();
        boxSize.e[0] = 650.0f;
        pText->fn_801CA5F0(boxSize);
    }

    TLComponentInstance* pOK = FEFinder<TLComponentInstance, 4>::Find<FEPresentation>(
        presentation, "Slide1", "Layer", optionOKName);
    pOK->SetVisible(false);

    for (int optionIndex = 0; optionIndex < mPopup.numOptions; ++optionIndex)
    {
        mOptionInstances[optionIndex] = FEFinder<TLComponentInstance, 4>::Find<FEPresentation>(
            presentation, "Slide1", "Layer", optionNames[optionIndex]);
        if (nlStrNCmp(PopupEntries[mType].mOptions[optionIndex], "POPUP_OK", 10) == 0)
        {
            mOptionInstances[optionIndex]->SetVisible(false);
            mOptionInstances[optionIndex] = pOK;
            pOK->SetVisible(true);
        }
        mUnidentified9D0[optionIndex] = FEFinder<TLTextInstance, 3>::Find<TLComponentInstance>(
            mOptionInstances[optionIndex], "off", "Group", "btn_text");
        mUnidentified9D0[optionIndex]->SetString(mPopup.pOptionLabels[optionIndex]->begin());
        pText = FEFinder<TLTextInstance, 3>::Find<TLComponentInstance>(
            mOptionInstances[optionIndex], "over", "Group", "btn_text");
        pText->SetString(mPopup.pOptionLabels[optionIndex]->begin());
        pText = FEFinder<TLTextInstance, 3>::Find<TLComponentInstance>(
            mOptionInstances[optionIndex], "down", "Group", "btn_text");
        pText->SetString(mPopup.pOptionLabels[optionIndex]->begin());

        if (optionIndex == 0)
            mHighlightedOptionColour = mUnidentified9D0[optionIndex]->GetAssetColour();
    }

    for (int hiddenOptionIndex = mPopup.numOptions; hiddenOptionIndex < 3; ++hiddenOptionIndex)
    {
        FEFinder<TLComponentInstance, 4>::Find<FEPresentation>(
            presentation, "Slide1", "Layer", optionNames[hiddenOptionIndex])->SetVisible(false);
    }
    if (mUnidentified9A8 == 0)
    {
        FEFinder<TLComponentInstance, 4>::Find<FEPresentation>(
            presentation, "Slide1", "Layer", "button_LONG")->SetVisible(false);
    }
    mUnidentifiedC48 = FEFinder<TLComponentInstance, 4>::FindOrDefault<TLSlide>(
        presentation->GetActiveSlide(), "Layer", "HIGHLIGHT");
    mUnidentifiedC48->SetVisible(false);
    FEAudio::EnableSounds(true);
    fn_801CA644();
    FEAudio::EnableSounds(false);

    TLInstance* bars = FEFinder<TLInstance, 4>::FindOrDefault<TLSlide>(
        presentation->GetActiveSlide(), "Layer", "widescreenbars");
    bars->SetVisible(!GameInfoManager::Instance()->GetUserInfo().IsWidescreen());

    for (int index = 0; index < 4; ++index)
        GetPointerInstance(index)->SetActiveSlide("waiting", true, false);
    SHNavigation* scene = GetNavigationScene();
    if (scene != 0)
        scene->HideButtons();
    fn_801C83AC(true);
}

void FEPopupMenu::Update(float fDeltaT)
{
    if (!mMenuCreated)
        return;
    if (!mMenuDisplayed)
    {
        SetPositions();
        if (!mUnknownAA4)
            mPresentation->m_fadeDuration = 999.9f;
    }
    if (mUnidentifiedC08 < 2)
        ++mUnidentifiedC08;
    else if (mUnidentifiedC08 == 2)
    {
        fn_801C8494();
        ++mUnidentifiedC08;
    }
    BaseSceneHandler::Update(fDeltaT);

    UpdateBackgroundScale(fDeltaT);
    if (mPopup.numOptions <= 0)
        return;
    TLSlide* pSlide = mPresentation->GetActiveSlide();
    if (pSlide->GetCurrentTime() < 1.0)
        return;
    if (!mUnidentified99D)
    {
        fn_801C83AC(true);
        mUnidentified99D = true;
    }
    if (pSlide->GetCurrentTime() < 1.5)
        return;

    for (int index = 0; index < 4; ++index)
    {
        unsigned char valid = true;
        FEPointerEvent event;
        event.mIndex = index;
        event.mPosition = GetPointerPosition(index, &valid);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)index, 30, true, 0);
        TLComponentInstance* pPointer = GetPointerInstance(index);
        if (!mUnidentified9A1 && index != gFEControllerIndex)
            pPointer->SetActiveSlide("waiting", true, false);
        else
        {
            pPointer->SetActiveSlide("cursor", true, false);
            for (int optionIndex = 0; optionIndex < mPopup.numOptions; ++optionIndex)
                mControllerComponents[optionIndex].HandlePointerEvent(&event);
            if (mUnidentified9A2)
                break;
        }
    }
}

void FEPopupMenu::UpdateBackgroundScale(float fDeltaT)
{
    if (mMenuDisplayed && !mUnidentifiedC58)
    {
        TLImageInstance* pImage = FEFinder<TLImageInstance, 2>::FindOrDefault<TLSlide>(
            mPresentation->GetActiveSlide(), "Layer", "blackbox");
        const feVector3 scale = pImage->GetAssetScale();
        if (scale.e[0] >= mUnidentifiedC50 && scale.e[1] >= mUnidentifiedC54)
            mUnidentifiedC58 = true;
        else
            pImage->SetAssetScale(scale.e[0] + mUnidentifiedC50 * fDeltaT / 0.75,
                scale.e[1] + mUnidentifiedC54 * fDeltaT / 0.75, scale.e[2]);
    }
}

void FEPopupMenu::Create(ePopupMenu type, Function<FnVoidVoid> option1,
    Function<FnVoidVoid> option2, Function<FnVoidVoid> option3, Function<FnVoidVoid> option4)
{
    typedef BasicString<unsigned short, Detail::TempStringAllocator> WStr;
    if (mMenuCreated)
        return;
    const PopupEntry* popupEntry = &PopupEntries[type];

    switch (type)
    {
    case 23:
    {
        WStr cupName(g_pLocalization->GetString(GetLOCModeName(GameInfoManager::Instance()->fn_801CA658())));
        WStr message(g_pLocalization->GetString(popupEntry->mMessage));
        mPopup.pMessage = new (8, false) WStr(Format<WStr>(message, cupName));
        mPopup.pOptionLabels[0] = new (8, false) WStr(g_pLocalization->GetString(popupEntry->mOptions[0]));
        mPopup.pOptionLabels[1] = new (8, false) WStr(g_pLocalization->GetString(popupEntry->mOptions[1]));
        mPopup.pOptionLabels[2] = 0;
        mPopup.numOptions = 2;
        break;
    }
    case 25:
    case 26:
    case 27:
    {
        WStr message(g_pLocalization->GetString(popupEntry->mMessage));
        WStr cupName;
        CupInterface* manager;
        if (g_pNetworkSessionBase->GetSessionMode() != 0)
            manager = NetTournManager::Instance();
        else
            manager = CupManager::Instance();
        int mode = manager->GetCurrentMode();
        if (mode == 0)
            cupName = g_pLocalization->GetString("FIRE_CUP");
        else if (mode == 1)
            cupName = g_pLocalization->GetString("CRYSTAL_CUP");
        else if (mode == 3)
            cupName = g_pLocalization->GetString(gCupPersonaStringIDs[manager->GetCupPersona()]);
        else
            cupName = g_pLocalization->GetString("STRIKER_CUP");
        mPopup.pMessage = new (8, false) WStr(Format<WStr>(message, cupName));
        mPopup.numOptions = 1;
        mPopup.pOptionLabels[0] = new (8, false) WStr(g_pLocalization->GetString(popupEntry->mOptions[0]));
        mPopup.pOptionLabels[1] = 0;
        mPopup.pOptionLabels[2] = 0;
        break;
    }
    case 28:
    {
        WStr message(g_pLocalization->GetString(popupEntry->mMessage));
        WStr cupName;
        CupInterface* manager = NetTournManager::Instance();
        manager->GetCurrentMode();
        cupName = g_pLocalization->GetString(gCupPersonaStringIDs[manager->GetCupPersona()]);
        int winningMachine = NetTournManager::Instance()->fn_801CA9D0();
        NetworkDraftTeam* team = NetworkDraft::Instance()->FindDraftTeamByPeerIndex(winningMachine);
        mPopup.pMessage = new (8, false) WStr(Format<WStr>(message, team->mPlayers[0].mName, cupName));
        mPopup.numOptions = 1;
        mPopup.pOptionLabels[0] = new (8, false) WStr(g_pLocalization->GetString(popupEntry->mOptions[0]));
        mPopup.pOptionLabels[1] = 0;
        mPopup.pOptionLabels[2] = 0;
        break;
    }
    case 108:
    {
        WStr message(g_pLocalization->GetString(popupEntry->mMessage));
        int error = SocketNetworkGetLastError();
        if (error < 0)
            error = -error;
        mPopup.pMessage = new (8, false) WStr(Format<WStr>(message, error));
        mPopup.numOptions = 1;
        mPopup.pOptionLabels[0] = new (8, false) WStr(g_pLocalization->GetString(popupEntry->mOptions[0]));
        mPopup.pOptionLabels[1] = 0;
        mPopup.pOptionLabels[2] = 0;
        break;
    }
    case 110:
    {
        WStr message(g_pLocalization->GetString(popupEntry->mMessage));
        int error = g_pNetworkSession->fn_801CA9D8();
        if (error < 0)
            error = -error;
        mPopup.pMessage = new (8, false) WStr(Format<WStr>(message, error));
        mPopup.numOptions = 1;
        mPopup.pOptionLabels[0] = new (8, false) WStr(g_pLocalization->GetString(popupEntry->mOptions[0]));
        mPopup.pOptionLabels[1] = 0;
        mPopup.pOptionLabels[2] = 0;
        break;
    }
    case 116:
    case 117:
    case 118:
    case 119:
    case 120:
    case 121:
    case 122:
    case 123:
    case 124:
    {
        SHNavigation* scene = GetNavigationScene();
        if (scene != 0)
            scene->fn_801CA9E0(false);
        WStr message(g_pLocalization->GetString(popupEntry->mMessage));
        mPopup.pMessage = new (8, false) WStr(Format<WStr>(message, gOnlineErrorCode));
        mPopup.numOptions = 1;
        mPopup.pOptionLabels[0] = new (8, false) WStr(g_pLocalization->GetString(popupEntry->mOptions[0]));
        mPopup.pOptionLabels[1] = 0;
        mPopup.pOptionLabels[2] = 0;
        break;
    }
    case 63:
    {
        int blockSize = 0x20000;
        int blocks = (int)std::ceil((float)SaveLoad::GetSaveBlockSize() / (float)blockSize);
        char blockText[4];
        char fileText[4];
        unsigned short wideBlocks[8];
        unsigned short wideFiles[8];
        nlSNPrintf(blockText, sizeof(blockText), "%d", blocks);
        nlSNPrintf(fileText, sizeof(fileText), "%d", 2);
        nlStrToWcs(blockText, wideBlocks, 8);
        nlStrToWcs(fileText, wideFiles, 8);
        const unsigned short* message = g_pLocalization->GetString(popupEntry->mMessage);
        mPopup.pMessage = new (8, false) WStr(Format<WStr>(message, wideFiles, wideBlocks));
        mPopup.numOptions = 2;
        mPopup.pOptionLabels[0] = new (8, false) WStr(g_pLocalization->GetString(popupEntry->mOptions[0]));
        mPopup.pOptionLabels[1] = new (8, false) WStr(g_pLocalization->GetString(popupEntry->mOptions[1]));
        mPopup.pOptionLabels[2] = 0;
        break;
    }
    case 20:
    {
        WStr message(g_pLocalization->GetString(popupEntry->mMessage));
        WStr optionsName;
        int menu = -1;
        if (fn_801CA660() != 0)
            menu = ((OptionsSubMenu*)fn_801CA660()->GetScene((SceneList)13))->fn_801CAA10();
        else if (GetOverlayManager()->GetScene((SceneList)82) != 0)
            menu = 1;
        else if (GetOverlayManager()->GetScene((SceneList)83) != 0)
            menu = 2;
        switch (menu)
        {
        case 1: optionsName = g_pLocalization->GetString("AUDIO_OPTIONS"); break;
        case 2: optionsName = g_pLocalization->GetString("VISUAL_OPTIONS"); break;
        case 3: optionsName = g_pLocalization->GetString("GAMEPLAY_OPTIONS"); break;
        case 4: optionsName = g_pLocalization->GetString("CHEATS"); break;
        }
        mPopup.pMessage = new (8, false) WStr(Format<WStr>(message, optionsName));
        if (menu == 3)
            mUnknownAA5 = true;
        mPopup.numOptions = 2;
        mPopup.pOptionLabels[0] = new (8, false) WStr(g_pLocalization->GetString(popupEntry->mOptions[0]));
        mPopup.pOptionLabels[1] = new (8, false) WStr(g_pLocalization->GetString(popupEntry->mOptions[1]));
        mPopup.pOptionLabels[2] = 0;
        break;
    }
    case 39:
    {
        int value = 0;
        CupManager::Instance()->fn_8010DE2C(&value);
        char number[8];
        unsigned short wideNumber[8];
        nlSNPrintf(number, sizeof(number), "%d", value);
        nlStrToWcs(number, wideNumber, 8);
        const unsigned short* message = g_pLocalization->GetString(popupEntry->mMessage);
        mPopup.pMessage = new (8, false) WStr(Format<WStr>(message, wideNumber));
        mPopup.numOptions = 1;
        mPopup.pOptionLabels[0] = new (8, false) WStr(g_pLocalization->GetString(popupEntry->mOptions[0]));
        mPopup.pOptionLabels[1] = 0;
        mPopup.pOptionLabels[2] = 0;
        break;
    }
    case 40:
    {
        int value = 0;
        CupManager::Instance()->fn_8010D9C4(&value);
        char number[8];
        unsigned short wideNumber[8];
        nlSNPrintf(number, sizeof(number), "%d", value);
        nlStrToWcs(number, wideNumber, 8);
        const unsigned short* message = g_pLocalization->GetString(popupEntry->mMessage);
        mPopup.pMessage = new (8, false) WStr(Format<WStr>(message, wideNumber));
        mPopup.numOptions = 1;
        mPopup.pOptionLabels[0] = new (8, false) WStr(g_pLocalization->GetString(popupEntry->mOptions[0]));
        mPopup.pOptionLabels[1] = 0;
        mPopup.pOptionLabels[2] = 0;
        break;
    }
    case 47:
    {
        int captain = fn_801CA670()->fn_801CAA18();
        const char* captainName = GetCharacterInfo(GetCharacterIndexFromCaptain(captain)).GetName();
        char key[64];
        nlSNPrintf(key, sizeof(key), "POPUP_CHALLENGE_UNLOCK_%s", captainName);
        const unsigned short* message = g_pLocalization->GetString(key);
        mPopup.pMessage = new (8, false) WStr(message);
        mPopup.numOptions = 3;
        mPopup.pOptionLabels[0] = new (8, false) WStr(g_pLocalization->GetString(popupEntry->mOptions[0]));
        mPopup.pOptionLabels[1] = new (8, false) WStr(g_pLocalization->GetString(popupEntry->mOptions[1]));
        mPopup.pOptionLabels[2] = new (8, false) WStr(g_pLocalization->GetString(popupEntry->mOptions[2]));
        break;
    }
    case 18:
        break;
    default:
        switch (type)
        {
        case 96:
        case 99:
        case 100:
            GetFriendManager()->SetOwnStatusInitial(0);
            break;
        }
        switch (type)
        {
        case 90:
        case 91:
        case 96:
        case 114:
        {
            SHNavigation* scene = GetNavigationScene();
            if (scene != 0)
                scene->fn_801CA9E0(false);
            break;
        }
        }
        mPopup.pMessage = popupEntry->mMessage ? new (8, false) WStr(g_pLocalization->GetString(popupEntry->mMessage)) : 0;
        mPopup.numOptions = 0;
        for (int optionIndex = 0; optionIndex < 3; ++optionIndex)
        {
            if (popupEntry->mOptions[optionIndex] != 0)
            {
                mPopup.pOptionLabels[optionIndex] = new (8, false) WStr(g_pLocalization->GetString(popupEntry->mOptions[optionIndex]));
                ++mPopup.numOptions;
            }
            else
                mPopup.pOptionLabels[optionIndex] = 0;
        }
        break;
    }
    callBacks[0] = option1;
    callBacks[1] = option2;
    callBacks[2] = option3;
    mUnknownA64 = option4;
    mMenuCreated = true;
    mType = type;
    mHighlightedOption = popupEntry->mInitialHighlight;
}

void FEPopupMenu::CentrePopup(float totalHeight, float topOfMessageBox)
{
    float halfHeight = totalHeight;
    halfHeight *= 0.5f;
    float verticalOffset = halfHeight - topOfMessageBox;
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    TLTextInstance* pText = FEFinder<TLTextInstance, 3>::Find(
        presentation, "Slide1", "Layer", "Message");
    feVector3 position = pText->GetAssetPosition();
    pText->SetAssetPosition(position.e[0], position.e[1] + verticalOffset, position.e[2]);
    for (int optionIndex = 0; optionIndex < mPopup.numOptions; ++optionIndex)
    {
        position = mOptionInstances[optionIndex]->GetAssetPosition();
        mOptionInstances[optionIndex]->SetAssetPosition(position.e[0], position.e[1] + verticalOffset, position.e[2]);
    }
}

void FEPopupMenu::SetPositions()
{
    float topOfMessage;
    const nlFont* pFont;
    feVector3 optionPosition;
    float prevOptionHeight = 0.0f;
    float totalHeight = 0.0f;
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    TLTextInstance* pText = FEFinder<TLTextInstance, 3>::Find(
        presentation, nlStringLowerHash("Slide1"), nlStringLowerHash("Layer"), nlStringLowerHash("Message"), 0, 0, 0);
    const feVector3 messagePosition = pText->GetAssetPosition();
    pFont = ((const FEText*)pText->m_component)->m_pFeFontResource->GetFontReference();
    nlTextBox::StringDrawInfo drawInfo = pText->m_DrawInfo;
    feVector3 messageScale = pText->GetAssetScale();
    float messageHeight = messageScale.e[1] * (float)(drawInfo.RowCount * pFont->m_Metrics.Height);
    totalHeight += messageHeight;
    topOfMessage = messagePosition.e[1] + messageHeight / 2.0f;

    if (messageHeight == 0.0)
    {
        nlColour hiddenTextColour = pText->GetAssetColour();
        hiddenTextColour.c[3] = 0;
        pText->SetAssetColour(hiddenTextColour);
        for (int hiddenOptionIndex = 0; hiddenOptionIndex < mPopup.numOptions; ++hiddenOptionIndex)
        {
            pText = mUnidentified9D0[hiddenOptionIndex];
            hiddenTextColour = pText->GetAssetColour();
            hiddenTextColour.c[3] = 0;
            pText->SetAssetColour(hiddenTextColour);
        }
        glDiscardFrame(1);
        return;
    }
    nlColour messageColour = pText->GetAssetColour();
    messageColour.c[3] = 255;
    pText->SetAssetColour(messageColour);

    float firstOptionSpacing = GetConfigFloat(Config::Global(), "popup_first_option_spacing", 75.0f);
    float otherOptionSpacing = GetConfigFloat(Config::Global(), "popup_other_option_spacing", 12.5f);
    for (int optionIndex = 0; optionIndex < mPopup.numOptions; ++optionIndex)
    {
        pText = mUnidentified9D0[optionIndex];
        nlColour optionColour = pText->GetAssetColour();
        optionColour.c[3] = 255;
        pText->SetAssetColour(optionColour);
        float optionHeight = 56.7f;
        totalHeight += optionHeight;
        float optionY;
        if (optionIndex == 0)
        {
            totalHeight += firstOptionSpacing;
            optionY = messagePosition.e[1] - messageHeight / 2.0f - optionHeight / 2.0f - firstOptionSpacing;
        }
        else
        {
            totalHeight += otherOptionSpacing;
            optionY = optionPosition.e[1] - prevOptionHeight / 2.0f - optionHeight / 2.0f - otherOptionSpacing;
        }
        prevOptionHeight = optionHeight;
        optionPosition = mOptionInstances[optionIndex]->GetAssetPosition();
        optionPosition.e[1] = optionY;
        mOptionInstances[optionIndex]->SetAssetPosition(optionPosition.e[0], optionPosition.e[1], optionPosition.e[2]);
    }
    CentrePopup(totalHeight, topOfMessage);

    TLImageInstance* pImage = FEFinder<TLImageInstance, 2>::FindOrDefault<TLSlide>(
        presentation->GetActiveSlide(), "Layer", "blackbox");
    pText = FEFinder<TLTextInstance, 3>::Find(
        mPresentation, "Slide1", "Layer", "Message");
    nlVector2 size = fn_801CC48C(pText);
    float width = size.e[0] > 630.0f ? size.e[0] : 630.0f;
    mUnidentifiedC50 = (40.0f + width) / 120.0f;
    mUnidentifiedC54 = (40.0f + (totalHeight + 15.0f * mPopup.numOptions)) / 100.0f;
    pImage->SetAssetScale(0.0f, 0.0f, 1.0f);
    feVector3 position = pImage->GetAssetPosition();
    pImage->SetAssetPosition(position.e[0], position.e[1], position.e[2]);
    fn_801C83AC(false);
    mMenuDisplayed = true;
}

void FEPopupMenu::fn_801C83AC(bool visible)
{
    unsigned char alpha = visible ? 255 : 0;
    TLTextInstance* pText = FEFinder<TLTextInstance, 3>::Find(
        mPresentation, "Slide1", "Layer", "Message");
    nlColour colour = pText->GetColour();
    colour.c[3] = alpha;
    pText->SetAssetColour(colour);

    for (int optionIndex = 0; optionIndex < mPopup.numOptions; ++optionIndex)
    {
        mOptionInstances[optionIndex]->m_bVisible = visible;
    }
}

void FEPopupMenu::fn_801C8494()
{
    typedef Detail::MemFunImpl<void, void (FEPopupMenu::*)(unsigned int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, FEPopupMenu*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback enterCallback(PointerBinding(
        MemFun(&FEPopupMenu::fn_801C87E0), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback leaveCallback(PointerBinding(
        MemFun(&FEPopupMenu::fn_801C88B4), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback pressCallback(PointerBinding(
        MemFun(&FEPopupMenu::fn_801C8960), this, Placeholder<0>(), Placeholder<1>()));

    for (int optionIndex = 0; optionIndex < mPopup.numOptions; ++optionIndex)
    {
        mControllerComponents[optionIndex].SetInstanceBounds(
            mOptionInstances[optionIndex], true, 0.0f, 0.0f, 0.75f, 0.6f);
        mControllerComponents[optionIndex].SetPointerEnterCallback(enterCallback);
        mControllerComponents[optionIndex].SetPointerLeaveCallback(leaveCallback);
        mControllerComponents[optionIndex].SetPointerPressCallback(pressCallback);
    }
}

void FEPopupMenu::fn_801C87E0(unsigned int index, void* context)
{
    int optionIndex = (int)context;
    ++mUnidentifiedBF8[index];
    if (!mControllerComponents[optionIndex].HasOtherPointerState(1, index))
    {
        mOptionInstances[optionIndex]->SetActiveSlide("over", true, false);
        FEAudio::EnableSounds(true);
        FEAudio::PlayAnimAudioEvent(0xDE912775, 0, 0, true);
        FEAudio::EnableSounds(false);
    }
    mControllerComponents[optionIndex].SetPointerState(1, index);
}

void FEPopupMenu::fn_801C88B4(unsigned int index, void* context)
{
    int optionIndex = (int)context;
    --mUnidentifiedBF8[index];
    if (!mControllerComponents[optionIndex].HasOtherPointerState(1, index))
    {
        mOptionInstances[optionIndex]->SetActiveSlide("off", true, false);
    }
    mControllerComponents[optionIndex].SetPointerState(0, index);
}

void FEPopupMenu::fn_801C8960(unsigned int index, void* context)
{
    FEAudio::EnableSounds(true);
    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, true);
    FEAudio::EnableSounds(false);

    BaseGameSceneManager* manager = nlSingleton<GameSceneManager>::s_pInstance;
    if (manager != 0)
    {
        manager->Pop();
    }
    else
    {
        manager = g_pOverlayManager;
        if (manager != 0)
        {
            manager->Pop();
        }
    }

    mRunCallBack = true;
    mHighlightedOption = (int)context;
    mUnidentified9A2 = true;
    mUnidentified9A3 = gpHBMManager->mBlocked;
    gpHBMManager->mBlocked = true;
}
