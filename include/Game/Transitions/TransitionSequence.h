#ifndef GAME_TRANSITIONS_TRANSITION_SEQUENCE_H
#define GAME_TRANSITIONS_TRANSITION_SEQUENCE_H

#include "Game/Sys/simpleparser.h"
#include "Game/Transitions/ScreenTransitionManager.h"

struct TransitionSounds
{
    /* 0x00 */ char* soundStr;
    /* 0x04 */ bool stopAtEnd;
}; // total size: 0x8

class TransitionSequence : public ScreenTransition
{
public:
    TransitionSequence();
    virtual ~TransitionSequence();
    virtual void Update(float dt);
    virtual void Render(GLView* glView);
    virtual bool IsFinished();
    virtual float Time() const;
    virtual float CutTime() const;
    virtual void Reset();
    virtual void Cancel();
    virtual float GetTransitionLength();
    virtual void DoSanityCheck();

    void Initialize(SimpleParser* parser);

    /* 0x04 */ ScreenTransition** m_pTransitions;
    /* 0x08 */ unsigned short m_nNumTransitions;
    /* 0x0C */ char* m_pPlaying;
    /* 0x10 */ float* m_pEarly;
    /* 0x14 */ TransitionSounds* m_pSound;
    /* 0x18 */ float m_Time;
    /* 0x1C */ float m_CutAt;
}; // total size: 0x20

#endif // GAME_TRANSITIONS_TRANSITION_SEQUENCE_H
