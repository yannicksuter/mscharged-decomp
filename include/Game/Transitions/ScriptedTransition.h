#ifndef GAME_TRANSITIONS_SCRIPTED_TRANSITION_H
#define GAME_TRANSITIONS_SCRIPTED_TRANSITION_H

#include "Game/Sys/simpleparser.h"
#include "Game/Transitions/ScreenTransitionManager.h"

enum eTimeLine
{
    TIME_LINEAR = 0,
    TIME_ACCEL = 1,
    TIME_DECEL = 2,
    NUM_TIME_LINES = 3,
};

class glPoly2;

class TransitionModifierInterface
{
public:
    TransitionModifierInterface() { }
    virtual ~TransitionModifierInterface() { }
    virtual void InitializeFromParser(SimpleParser* parser) = 0;
    virtual void ApplyModifier(glPoly2& poly, float time) = 0;
    virtual void Cleanup() { }
    virtual bool UnidentifiedVirtual18() { return false; }
};

class ScriptedScreenTransition : public ScreenTransition
{
public:
    ScriptedScreenTransition();
    virtual ~ScriptedScreenTransition();
    virtual bool IsFinished() { return m_fCurrentTime > m_fLength; }
    virtual void Reset() { m_fCurrentTime = 0.0f; }
    virtual float Time() const { return m_fCurrentTime / m_fLength; }
    virtual float GetTransitionLength() { return m_fLength; }
    virtual void Update(float dt);
    virtual void Render(GLView* view);
    virtual void Cancel();
    virtual bool UnidentifiedVirtual30();

    void InitializeFromParser(SimpleParser* parser);
    TransitionModifierInterface* GetModifierFromName(char* pName);

    /* 0x04 */ TransitionModifierInterface** m_pModifiers;
    /* 0x08 */ int m_nModifiers;
    /* 0x0C */ float m_fLength;
    /* 0x10 */ float m_fCurrentTime;
    /* 0x14 */ unsigned long m_nTexture;
    /* 0x18 */ eTimeLine m_eTimeLine;
}; // total size: 0x1C

#endif // GAME_TRANSITIONS_SCRIPTED_TRANSITION_H
