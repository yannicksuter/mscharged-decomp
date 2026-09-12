#ifndef GAME_RENDER_NUMBER_DISPLAY_H
#define GAME_RENDER_NUMBER_DISPLAY_H

#include "NL/nlMath.h"
#include "types.h"

class DrawableObject;

class NumberDisplay
{
public:
    NumberDisplay();
    ~NumberDisplay();

    void OnGetReadyForKickoff();
    void Update(float deltaTime);
    void Render();
    void RenderScores();
    void RenderGlyph(
        int modelIndex, float scale, float opacity, const nlVector2& position);
    void SetScores(int firstScore, int secondScore);
    void Reset();
    void BeginScoreUpdate();
    void EndScoreUpdate()
    {
        if (!mHoldUntilKickoff)
        {
            mExpandedHoldTimer = 0.0f;
            mExpanded = false;
        }
    }
    void IncrementGoalCount();
    void ResetGoalCount();
    void ShowScores();
    void ShowAccumulatedScore();

    /* 0x00 */ DrawableObject** mModels;
    /* 0x04 */ bool mVisible;
    /* 0x05 */ bool mHoldUntilKickoff;
    /* 0x06 */ u8 mPadding006[0x02];
    /* 0x08 */ int mLeftScore;
    /* 0x0C */ int mRightScore;
    /* 0x10 */ bool mExpanded;
    /* 0x11 */ u8 mPadding011[0x03];
    /* 0x14 */ float mExpansion;
    /* 0x18 */ float mScoreUpdateTimer;
    /* 0x1C */ float mExpandedHoldTimer;
    /* 0x20 */ int mGoalCount;
    /* 0x24 */ bool mShowGoalCount;
    /* 0x25 */ bool mShowAccumulatedScore;
    /* 0x26 */ u8 mPadding026[0x02];
}; // total size: 0x28

extern NumberDisplay* gpNumberDisplay;

#endif // GAME_RENDER_NUMBER_DISPLAY_H
