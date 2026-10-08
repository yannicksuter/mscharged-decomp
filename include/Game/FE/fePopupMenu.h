#ifndef GAME_FE_FE_POPUP_MENU_H
#define GAME_FE_FE_POPUP_MENU_H

#include "Game/BaseSceneHandler.h"
#include "NL/nlBasicString.h"
#include "NL/nlFunction.h"
#include "NL/nlFunction.inl"
#include "Game/FE/fePointerButton.h"
#include "types.h"
#include "NL/nlColour.h"

class TLComponentInstance;
class TLTextInstance;

enum ePopupMenu
{
    INVALID_TYPE = -1,
    POPUP_LEAVE_CUP_SAVE = 0,
    POPUP_LEAVE_CUP_SAVE_OR_RESTART = 1,
    POPUP_LEAVE_CUP_QUIT = 2,
    POPUP_LEAVE_CUP_QUIT_OR_RESTART = 3,
    POPUP_RESTART_CUP = 4,
    POPUP_FLOWER_LOCKED = 5,
    POPUP_STAR_LOCKED = 6,
    POPUP_BOWSER_LOCKED = 7,
    POPUP_SUPER_LOCKED = 8,
    POPUP_FORFEIT = 9,
    POPUP_QUIT = 10,
    POPUP_STRIKERS_101_QUITTING = 11,
    POPUP_NO_SIDES = 12,
    POPUP_MUST_CHOOSE_SIDES = 13,
    POPUP_NO_HUMAN = 14,
    POPUP_CONTINUE_OR_NEW_CUP = 15,
    POPUP_CONFIRM_NEW_CUP = 16,
    POPUP_CONFIRM_CUP_TEAM = 17,
    POPUP_NEW_TOURNAMENT = 18,
    POPUP_FILL_OUT_ALL_ENTRIES = 19,
    POPUP_CONFIRM_OPTIONS_CHANGES = 20,
    POPUP_END_CUSTOM_SINGLE_PLAYER = 21,
    POPUP_FORFEIT_NO_TEAM = 22,
    POPUP_NEW_CUP_CONFIRM = 23,
    POPUP_CHANGE_AUDIO_SETTINGS = 24,
    POPUP_CUP_WIN = 25,
    POPUP_CUP_LOSE = 26,
    POPUP_CUP_DNF = 27,
    POPUP_CUP_OTHER_WIN = 28,
    POPUP_FIRE_QUAL_RULES = 29,
    POPUP_CRYSTAL_QUAL_RULES = 30,
    POPUP_STRIKER_QUAL_RULES = 31,
    POPUP_FIRE_ELIM_RULES = 32,
    POPUP_CRYSTAL_ELIM_RULES = 33,
    POPUP_STRIKER_ELIM_RULES = 34,
    POPUP_FIRE_FINAL_RULES = 35,
    POPUP_CRYSTAL_FINAL_RULES = 36,
    POPUP_STRIKER_FINAL_RULES = 37,
    POPUP_RTSC_STANDINGS_HELP = 38,
    POPUP_CUP_AWARDS_BRICK = 39,
    POPUP_CUP_AWARDS_STRIKER = 40,
    POPUP_WIN_FIRECUP_BOOT_WALL_REWARDS = 41,
    POPUP_WIN_CRYSTALCUP_BOOT_WALL_REWARDS = 42,
    POPUP_WIN_STRIKERCUP_BOOT_WALL_REWARDS = 43,
    POPUP_WIN_FIRECUP_MAINCUP_REWARDS = 44,
    POPUP_WIN_CRYSTALCUP_MAINCUP_REWARDS = 45,
    POPUP_WIN_STRIKERCUP_MAINCUP_REWARDS = 46,
    POPUP_CHALLENGE_UNLOCK_CAPTAIN = 47,
    POPUP_CHALLENGE_SUCCEEDED = 48,
    POPUP_CHALLENGE_FAILED = 49,
    POPUP_TUTORIAL_UNLOCK = 50,
    POPUP_TUTORIAL_SUCCEEDED = 51,
    POPUP_TUTORIAL_FAILED = 52,
    POPUP_SERIES_OVER = 53,
    POPUP_NO_GAME_RESULTS = 54,
    POPUP_GAME_OVER = 55,
    POPUP_STRIKER_CUP_COMPLETE = 56,
    POPUP_CUP_DIFFICULTY = 57,
    POPUP_ONLINE_MENU_HELP = 58,
    POPUP_CHOOSE_SIDES_HELP = 59,
    POPUP_CHOOSE_SIDES_NUNCHUCK = 60,
    POPUP_NO_SAVE_FILE = 61,
    POPUP_SAVE_INFO = 62,
    POPUP_NO_SAVE_SPACE = 63,
    POPUP_NO_SAVE_SPACE_ONLINE = 64,
    POPUP_NO_SAVE_INODES = 65,
    POPUP_NO_SAVE_INODES_ONLINE = 66,
    POPUP_NAND_CORRUPT = 67,
    POPUP_NAND_ERROR = 68,
    POPUP_SAVE_FILE_ERROR = 69,
    POPUP_ONLINE_SAVE_DATA_REQUIRED = 70,
    POPUP_ONLINE_NO_COPY = 71,
    POPUP_UNLOCK_FLOWERCUP = 72,
    POPUP_UNLOCK_STARCUP = 73,
    POPUP_UNLOCK_BOWSERCUP = 74,
    POPUP_UNLOCK_SUPERCUPS = 75,
    POPUP_UNLOCK_POWERUPSET = 76,
    POPUP_UNLOCK_KONGASTAD = 77,
    POPUP_UNLOCK_YOSHISTAD = 78,
    POPUP_UNLOCK_FORBIDDENSTAD = 79,
    POPUP_UNLOCK_SUPERSTAD = 80,
    POPUP_UNLOCK_LEGENDDIFF = 81,
    POPUP_UNLOCK_SUPERTEAM = 82,
    POPUP_UNLOCK_CHEAT_GOALIE = 83,
    POPUP_UNLOCK_CHEAT_INFINITE = 84,
    POPUP_UNLOCK_CHEAT_TILT = 85,
    POPUP_UNLOCK_CHEAT_ALLSTS = 86,
    POPUP_UNLOCK_CREDITS = 87,
    POPUP_NETWORK_CREATE_FAILED = 88,
    POPUP_NETWORK_JOIN_FAILED = 89,
    POPUP_NETWORK_MATCHMAKING_ERROR = 90,
    POPUP_ONLINE_ERROR_CONNECT_FRIEND = 91,
    POPUP_NETWORK_NO_LOCAL_IP = 92,
    POPUP_NETWORK_FAILED_TO_CONNECT = 93,
    POPUP_NETWORK_NO_GAMES_TO_JOIN = 94,
    POPUP_NETWORK_JOIN_REJECTED = 95,
    POPUP_NETWORK_CONNECTION_LOST = 96,
    POPUP_NETWORK_CLIENTS_FAILED_CONNECT = 97,
    POPUP_NETWORK_START_FAILED = 98,
    POPUP_NETWORK_SYNC_ERROR = 99,
    POPUP_NETWORK_OVERFLOW = 100,
    POPUP_NETWORK_INVALID_FRIEND_CODE = 101,
    POPUP_NETWORK_OWN_FRIEND_CODE = 102,
    POPUP_NETWORK_DUPLICATE_FRIEND = 103,
    POPUP_NETWORK_FRIEND_LIST_FULL = 104,
    POPUP_ONLINE_REGION_NOT_CONFIRMED = 105,
    POPUP_NETWORK_DELETE_FRIEND = 106,
    POPUP_NETWORK_CONNECT_WIFI_ERROR = 107,
    POPUP_NETWORK_CONNECT_WIFI_ERRORSPECIFIC = 108,
    POPUP_NETWORK_LOGINDWC_AUTHERR = 109,
    POPUP_NETWORK_LOGINDWC_AUTHERRSPECIFIC = 110,
    POPUP_NETWORK_LOGINGETSTATS_ERROR = 111,
    POPUP_NETWORK_LOGINBADNAME = 112,
    POPUP_NETWORK_NO_FRIENDS = 113,
    POPUP_NETWORK_CONNECTION_REJECTED = 114,
    POPUP_ONLINE_PLAYER_BUSY_INVITATION = 115,
    POPUP_NETWORK_ERROR_NO_CONNECTION = 116,
    POPUP_NETWORK_ERROR_SERVICE_DOWN = 117,
    POPUP_NETWORK_ERROR_DISCONTINUED = 118,
    POPUP_NETWORK_ERROR_NO_SPACE = 119,
    POPUP_NETWORK_ERROR_UNABLE_TO_CONNECT = 120,
    POPUP_NETWORK_ERROR_NO_RESPONSE = 121,
    POPUP_NETWORK_ERROR_DISCONNECTED = 122,
    POPUP_NETWORK_ERROR_COMMUNICATION = 123,
    POPUP_NETWORK_NAND_CORRUPT = 124,
    POPUP_LOCKED_FIRE_CUP = 125,
    POPUP_LOCKED_STRIKER_CUP = 126,
    POPUP_LOCKED_CRYSTAL_CUP = 127,
    POPUP_LOCKED_FIRE_BRICK_WALL = 128,
    POPUP_LOCKED_FIRE_GOLDEN_BOOT = 129,
    POPUP_LOCKED_STRIKER_BRICK_WALL = 130,
    POPUP_LOCKED_STRIKER_GOLDEN_BOOT = 131,
    POPUP_LOCKED_CRYSTAL_BRICK_WALL = 132,
    POPUP_LOCKED_CRYSTAL_GOLDEN_BOOT = 133,
    POPUP_PROFILE_NO_MIIS = 134,
    POPUP_PROFILE_DATABASE_CORRUPT = 135,
    POPUP_PROFILE_MII_NOT_FOUND = 136,
    POPUP_PROFILE_UNLINK_WARNING = 137,
    POPUP_PROFILE_MII_DELETION_WARNING = 138,
    POPUP_PROFILE_NO_SLOTS_LEFT = 139,
    POPUP_DELETED_MII = 140,
    POPUP_LOW_BATTERY = 141,
    POPUP_E3_THANKS_FOR_PLAYING = 142,
};

struct Popup
{
    /* 0x00 */ BasicString<unsigned short, Detail::TempStringAllocator>* pMessage;
    /* 0x04 */ BasicString<unsigned short, Detail::TempStringAllocator>* pOptionLabels[3];
    /* 0x10 */ int numOptions;
}; // size 0x14

class FEPopupMenu : public BaseSceneHandler
{
public:
    FEPopupMenu();
    virtual ~FEPopupMenu();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void SetPositions();
    void CentrePopup(float totalHeight, float topOfMessageBox);
    void SetMessageAndOptionsVisible(bool visible);
    void InitializePointerButtons();
    void OnOptionPointerEnter(unsigned int index, void* context);
    void OnOptionPointerLeave(unsigned int index, void* context);
    void OnOptionPointerPress(unsigned int index, void* context);
    ePopupMenu GetType() const { return mType; }

    // Never called. R4QE01 keeps the implicit nlColour copy-assignment that
    // SceneCreated calls out of line in the position of an already-synthesized
    // member: at its call slot in SceneCreated's header drain, right behind
    // TLInstance::SetVisible (0x801CA618). Under GC/3.0a5 -sym on an implicit
    // member drains there only when some body parsed before SceneCreated
    // already assigned an nlColour; no header this unit includes does, and the
    // linker kept no trace of the body that did. This inline reproduces that
    // parse-time synthesis and emits nothing itself. The original setter's
    // spelling, signature and callers are unknown; the name describes the body.
    void SetHighlightedOptionColour(const nlColour& colour)
    {
        mHighlightedOptionColour = colour;
    }

    void Create(ePopupMenu type)
    {
        Create(type, Function<FnVoidVoid>(Nothing));
    }

    void Create(ePopupMenu type, Function<FnVoidVoid> option1)
    {
        Create(type, option1, Function<FnVoidVoid>(Nothing));
    }

    void Create(ePopupMenu type, FnVoidVoid* callback)
    {
        Create(type, Function<FnVoidVoid>(callback));
    }

    void Create(ePopupMenu type, Function<FnVoidVoid> option1, Function<FnVoidVoid> option2)
    {
        Create(type, option1, option2, Function<FnVoidVoid>(Nothing));
    }

    void Create(
        ePopupMenu type,
        Function<FnVoidVoid> option1,
        Function<FnVoidVoid> option2,
        Function<FnVoidVoid> option3)
    {
        Create(type, option1, option2, option3, Function<FnVoidVoid>(Nothing));
    }

    void Create(
        ePopupMenu type,
        Function<FnVoidVoid> option1,
        Function<FnVoidVoid> option2,
        Function<FnVoidVoid> option3,
        Function<FnVoidVoid> option4);

    static void Nothing() { }

    /* 0x01C */ unsigned short mMessageBuffer[1024];
    /* 0x81C */ unsigned short mOptionBuffers[4][48];
    /* 0x99C */ bool mMenuDisplayed;
    /* 0x99D */ bool mMessageAndOptionsShown;
    /* 0x99E */ bool mMenuCreated;
    /* 0x99F */ bool mRunCallBack;
    /* 0x9A0 */ bool mRunBackCallback;
    /* 0x9A1 */ bool mAllPointersActive;
    /* 0x9A2 */ bool mOptionPressed;
    /* 0x9A3 */ bool mHBMWasBlocked;
    /* 0x9A4 */ int mHighlightedOption;
    /* 0x9A8 */ int mShowLongButton;
    /* 0x9AC */ float mAcceptDelayTime;
    /* 0x9B0 */ Popup mPopup;
    /* 0x9C4 */ TLComponentInstance* mOptionInstances[3];
    /* 0x9D0 */ TLTextInstance* mOptionTextInstances[3];
    /* 0x9DC */ FEPointerButton mControllerComponents[3];
    /* 0xBF8 */ unsigned int mPointerHoverCounts[4];
    /* 0xC08 */ int mUpdateCount;
    /* 0xC0C */ int mControlInput;
    /* 0xC10 */ Function<FnVoidVoid> callBacks[3];
    /* 0xC28 */ Function<FnVoidVoid> mBackCallback;
    /* 0xC30 */ nlColour mHighlightedOptionColour;
    /* 0xC34 */ unsigned char m_padC34[0x0C];
    /* 0xC40 */ ePopupMenu mType;
    /* 0xC44 */ bool mPlayIntroAnimation;
    /* 0xC45 */ bool mWideMessageBox;
    /* 0xC46 */ unsigned char m_padC46[2];
    /* 0xC48 */ TLComponentInstance* mHighlightInstance;
    /* 0xC4C */ bool m_padC4C;
    /* 0xC4D */ unsigned char m_padC4D[3];
    /* 0xC50 */ float mBackgroundTargetScaleX;
    /* 0xC54 */ float mBackgroundTargetScaleY;
    /* 0xC58 */ bool mBackgroundScaleDone;

private:
    void UpdateBackgroundScale(float fDeltaT);
}; // size 0xC5C

#endif // GAME_FE_FE_POPUP_MENU_H
