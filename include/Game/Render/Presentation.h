#ifndef GAME_RENDER_PRESENTATION_H
#define GAME_RENDER_PRESENTATION_H

#include "Game/InterpreterCore.h"
#include "Game/EventDataTypes.h"
#include "Game/NetworkMessage.h"
#include "types.h"

class cPlayer;
struct GoalScoredData;
struct GoalieSaveData;

// The in-game presentation director. It drives the presentation byte code
// (art/Scripts/presentation.byte_code) and mirrors skip/bypass state across the
// network, so it is both an InterpreterCore and a NetworkMessageReceiver.
//
// The class identity and established method names come from the Super Mario
// Strikers Game/Render/Presentation donor. Charged adds networking and other
// target-specific presentation state.
class Presentation : public InterpreterCore, public NetworkMessageReceiver
{
public:
    Presentation();
    virtual ~Presentation();
    virtual void DoFunctionCall(unsigned int function);
    virtual int ProcessMessage(NetworkMessage* message);

    void UpdateAllowedToSkip();
    bool DetectSkipPress();
    void Finish();
    void Update(float deltaTime);
    void fn_80285E1C();
    void Call(const char* functionName, const char* nisFilter);
    void PlayHighlights();
    void RegisterEventListeners();
    void OnGoalScored(GoalScoredData* data);
    void OnGoalieSave(GoalieSaveData* data);
    void OnKickoff();
    void OnSuddenDeath();
    void OnCharacterDirectionEnd();
    void OnMegaStrikeIntro(cPlayer* player);
    void OnMegaStrikeEnd(MegaStrikeEndData* data);
    void HandleMegaStrikeResult(MegaStrikeEndData* data);
    void WaitForNisLoaded();
    void PlayNis();
    void LoadNis(const char* name, int variant);
    void PlayOverlay(const char* name, float delay, float length);
    void ReceiveNisLoaded(u32 machineBits);
    void SendSkipNis();
    void PlayGoalEffects(const char* effects);
    void UpdateBallGlow(int level);
    bool ClearSkipVotes();
    void StopOverlay();
    void UpdateAndRenderLetterBox();
    void Reset();

    /* 0x02C */ u32 mUnidentified02C;
    /* 0x030 */ char mCurrentFunction[64];
    /* 0x070 */ char mInterruptWipe[64];
    /* 0x0B0 */ bool mByPassWasSkipped;
    /* 0x0B1 */ bool mSkipPressed;
    /* 0x0B2 */ bool mInsideByPass;
    /* 0x0B3 */ bool mByPassing;
    /* 0x0B4 */ float mWaitingForCharacterDirectionSince;
    /* 0x0B8 */ float mTimeInFunction;
    /* 0x0BC */ float mDisplayLetterBox;
    /* 0x0C0 */ float mLetterBoxDuration;
    /* 0x0C4 */ bool mLetterBoxEnabled;
    /* 0x0C8 */ float mOverlayDelay;
    /* 0x0CC */ float mOverlayDisplayLength;
    /* 0x0D0 */ bool mOverlayDisplayed;
    /* 0x0D4 */ int mOverlayToDisplay;
    /* 0x0D8 */ float mBallGlowLevel;
    /* 0x0DC */ char mQueuedFunction[64];
    /* 0x11C */ char mQueuedFilter[32];
    /* 0x13C */ bool mIsAllowedToSkip[4];
    /* 0x140 */ u8 mNisLoadedBits;
    /* 0x141 */ bool mUnidentified141;
    /* 0x142 */ bool mUnidentified142;
    /* 0x143 */ bool mUnidentified143;
    /* 0x144 */ u32 mRandomSeed;
    /* 0x148 */ int mHighlightsLeft;
    /* 0x14C */ int mByPassNumber;
    /* 0x150 */ int mSkipPastByPass;
    /* 0x154 */ u8 mUnidentified154[0x2];
    /* 0x156 */ bool mUnidentified156;
    /* 0x157 */ bool mUnidentified157;
    /* 0x158 */ bool mUnidentified158;
    /* 0x159 */ bool mUnidentified159;
    /* 0x15A */ u8 mUnidentified15A[0x2];
    /* 0x15C */ float mUnidentified15C;
    /* 0x160 */ bool mUnidentified160;
    /* 0x161 */ bool mUnidentified161;
    /* 0x162 */ bool mUnidentified162;
    /* 0x163 */ bool mUnidentified163;
    /* 0x164 */ bool mUnidentified164;
    /* 0x165 */ u8 mUnidentified165[0x3];
    /* 0x168 */ MegaStrikeEndData mMegaStrikeResult;
}; // total size: 0x174

Presentation* GetPresentation();
u32* fn_80287B2C(Presentation* state);
bool fn_80287B34(Presentation* state);
bool IsIdleAndNoShotInProgress(Presentation* presentation);
void fn_80285714(Presentation* state, u32 from, u32 to);
bool fn_80287AB0(Presentation* state);

extern "C" bool DuringEndOfGamePresentation(
    Presentation* presentation);
extern "C" void RestoreWorldRendering(
    Presentation* presentation);

#endif // GAME_RENDER_PRESENTATION_H
