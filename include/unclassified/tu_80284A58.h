#ifndef UNCLASSIFIED_TU_80284A58_H
#define UNCLASSIFIED_TU_80284A58_H

#include "Game/InterpreterCore.h"
#include "Game/NetworkMessage.h"
#include "types.h"

// The in-game presentation director. It drives the presentation byte code
// (art/Scripts/presentation.byte_code) and mirrors skip/bypass state across the
// network, so it is both an InterpreterCore and a NetworkMessageReceiver.
//
// The retail image carries no symbol table, so the class keeps the neutral
// UnidentifiedPresentationState identity its consumers already use. Field and
// method names come from the Super Mario Strikers predecessor's
// Game/Render/Presentation.cpp where the behaviour matches exactly.
class UnidentifiedPresentationState : public InterpreterCore,
                                      public NetworkMessageReceiver
{
public:
    UnidentifiedPresentationState();
    virtual ~UnidentifiedPresentationState();
    virtual void DoFunctionCall(unsigned int function);
    virtual int ProcessMessage(NetworkMessage* message);

    void UpdateAllowedToSkip();
    bool DetectSkipPress();
    void Finish();
    void Update(float deltaTime);
    void Call(const char* functionName, const char* nisFilter);
    void OnGoalScored(void* data);
    void OnGoalieSave(void* data);
    void OnSuddenDeath(void* data);
    void OnCharacterDirectionEnd(void* data);
    void OnMegaStrikeIntro(void* data);
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
    /* 0x165 */ u8 mUnidentified165[0xF];
}; // total size: 0x174

UnidentifiedPresentationState* GetPresentation();
u32* fn_80287B2C(UnidentifiedPresentationState* state);
bool IsIdleAndNoShotInProgress(UnidentifiedPresentationState* presentation);
void fn_80285714(UnidentifiedPresentationState* state, u32 from, u32 to);
bool fn_80287AB0(UnidentifiedPresentationState* state);

extern "C" bool DuringEndOfGamePresentation(
    UnidentifiedPresentationState* presentation);
extern "C" void RestoreWorldRendering(
    UnidentifiedPresentationState* presentation);

#endif // UNCLASSIFIED_TU_80284A58_H
