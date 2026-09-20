#ifndef GAME_RENDER_MEGABALLINDICATORS_H
#define GAME_RENDER_MEGABALLINDICATORS_H

#include "types.h"

class cGlobalPad;

struct MegaBallIndicatorTween
{
    MegaBallIndicatorTween()
        : mElapsedTime(0.0f)
        , mValue(1.0f)
        , mEndTime(0.0f)
        , mStartValue(0.0f)
        , mTransitionTime(0.0f)
        , mEndValue(0.0f)
        , mRepeatCount(0)
        , mActive(false)
    {
    }

    /* 0x00 */ float mElapsedTime;
    /* 0x04 */ float mValue;
    /* 0x08 */ float mEndTime;
    /* 0x0C */ float mStartValue;
    /* 0x10 */ float mTransitionTime;
    /* 0x14 */ float mEndValue;
    /* 0x18 */ int mRepeatCount;
    /* 0x1C */ bool mActive;
}; // size: 0x20

struct MegaBallIndicator
{
    MegaBallIndicator();

    bool IsActive() const
    {
        return mActive;
    }

    /* 0x00 */ float mX;
    /* 0x04 */ float mY;
    /* 0x08 */ float mWidth;
    /* 0x0C */ float mHeight;
    /* 0x10 */ float mScale;
    /* 0x14 */ float mOpacity;
    /* 0x18 */ float mAngle;
    /* 0x1C */ unsigned int mIndex;
    /* 0x20 */ unsigned int mTextureId;
    /* 0x24 */ unsigned int mTextureIndex;
    /* 0x28 */ bool mVisible;
    /* 0x29 */ bool mActive;
    /* 0x2C */ MegaBallIndicatorTween mScaleTween;
    /* 0x4C */ MegaBallIndicatorTween mOpacityTween;
    /* 0x6C */ MegaBallIndicatorTween mVisibilityTween;
}; // size: 0x8C

void ResetMegaBallIndicator(
    MegaBallIndicator* pIndicator, unsigned int textureIndex);
void SetMegaBallIndicatorTexture(
    MegaBallIndicator* pIndicator, unsigned int textureIndex);
bool UpdateMegaBallIndicatorTween(
    MegaBallIndicatorTween* pTween, float deltaTime);
void SetMegaBallIndicatorScaleTween(MegaBallIndicator* pIndicator,
    int repeatCount, float transitionTime, float endValue, float endTime,
    float startValue);
void SetMegaBallIndicatorOpacityTween(MegaBallIndicator* pIndicator,
    int repeatCount, float transitionTime, float endValue, float endTime,
    float startValue);
void StopMegaBallIndicatorOpacityTween(MegaBallIndicator* pIndicator);
void ResetMegaBallIndicators();
void ResetMegaBallTimer();
MegaBallIndicator* CreateMegaBallIndicator(float x, float y, float scale);
void ReleaseMegaBallIndicator(MegaBallIndicator* pIndicator);
MegaBallIndicator* GetMegaBallIndicator(unsigned int index);
MegaBallIndicator* GetMegaBallTargetIndicator(unsigned int index);
MegaBallIndicator* CreateMegaBallCatchIndicator(
    float x, float y, float scale, float angle);
MegaBallIndicator* GetMegaBallCatchIndicator(unsigned int index);
void SetMegaBallIndicatorTextures(
    unsigned int pointerTexture, unsigned int catchTexture);
void ActivateMegaBallPointer(
    bool magnetize, float x, float y, float scale);
void ResetMegaBallPointer();
float TestMegaBallIndicatorCollision(
    MegaBallIndicator* pIndicator, MegaBallIndicator* pTarget);
void DrawMegaBallIndicator(int x, int y, unsigned int textureId,
    float width, float height, float opacity, float angle);
void SetMegaBallController(cGlobalPad* pController);
void UpdateAndRenderMegaBallPointer(float deltaTime);
void RenderMegaBallIndicators();
void UpdateMegaBallIndicators(float deltaTime);
void SetMegaBallTimerCount(unsigned int count);
void SetMegaBallTimerStatus(unsigned int index, int status);
void RenderMegaBallTimer(float deltaTime);
void UpdateAndRenderMegaBallIndicators(float deltaTime);
void ReceiveMegaBallPointerUpdate(void* pMessage);

#endif // GAME_RENDER_MEGABALLINDICATORS_H
