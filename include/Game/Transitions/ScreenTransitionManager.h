#ifndef GAME_TRANSITIONS_SCREEN_TRANSITION_MANAGER_H
#define GAME_TRANSITIONS_SCREEN_TRANSITION_MANAGER_H

#include "NL/gl/glView.h"
#include "NL/nlAVLTree.h"
#include "NL/nlBasicString.h"
#include "NL/nlSingleton.h"
#include "NL/nlString.h"
#include "NL/nlVector.h"

class DefaultAllocator;

class ScreenTransition
{
public:
    virtual ~ScreenTransition() { }
    virtual void Update(float) = 0;
    virtual void Render(GLView*) = 0;
    virtual bool IsFinished() = 0;
    virtual float Time() const = 0;
    virtual float CutTime() const { return 0.0f; }
    virtual void Reset() = 0;
    virtual void Cancel() = 0;
    virtual float GetTransitionLength() = 0;
    virtual void DoSanityCheck() { }
    virtual bool UnidentifiedVirtual30() { return false; }
};

class ScreenTransitionCallback
{
public:
    virtual inline void fn_80188764();
    virtual void TransitionFinished();
    virtual void TransitionProgressed(float);
    virtual inline void Cut();
    virtual inline void SequenceSwitch();
};

class ScreenTransitionManager : public nlSingleton<ScreenTransitionManager>
{
public:
    ScreenTransitionManager();
    virtual ~ScreenTransitionManager();

    void Update(float dt);
    void Render();
    void CancelAllTransitions();
    void AddTransitionToMap(char*, ScreenTransition*);
    void EnableRandomTransition(const char*);
    void SelectRandomTransition(const char*);
    float GetSelectedTransitionCutTime() const;
    void EnableSelectedTransition();
    void AddTransitions(char*, unsigned long);

    /* 0x04 */ ScreenTransition* m_pActiveTransition;
    /* 0x08 */ nlAVLTree<unsigned long, ScreenTransition*, DefaultKeyCompare<unsigned long> > m_TransitionMap;
    /* 0x18 */ GLView* m_eView;
    /* 0x1C */ ScreenTransitionCallback* m_pCallback;
    /* 0x20 */ ScreenTransition* m_SelectedTransition;
    /* 0x24 */ float m_fCurrentTime;
    /* 0x28 */ float m_fCurrentLength;
    /* 0x2C */ Vector<BasicString<char, Detail::TempStringAllocator>, DefaultAllocator> m_Transitions;
    /* 0x38 */ bool m_Cut;
}; // total size: 0x3C

#endif // GAME_TRANSITIONS_SCREEN_TRANSITION_MANAGER_H
