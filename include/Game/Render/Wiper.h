#ifndef GAME_RENDER_WIPER_H
#define GAME_RENDER_WIPER_H

#include "Game/Transitions/ScreenTransitionManager.h"
#include "NL/nlMemory.h"
#include "NL/nlTask.h"

class Wiper : public nlTask
{
public:
    Wiper()
    {
        if (ScreenTransitionManager::s_pInstance == 0)
        {
            ScreenTransitionManager::s_pInstance = new (nlMalloc(sizeof(ScreenTransitionManager), 8, false)) ScreenTransitionManager();
        }
        mTimeDilated = false;
    }

    void Reset();
    void Initialize();
    bool WipeInProgress() const;
    bool CutHasOccured() const;
    static Wiper& Instance();
    void DoWipe(const char* wipe);
    virtual void Run(float dt);
    void Render();
    virtual const char* GetName()
    {
        return "NIS Transition Wiper";
    }
};

struct WiperCallback : public ScreenTransitionCallback
{
    WiperCallback()
        : mTransitionActive(false)
    {
    }

    virtual void TransitionFinished();
    virtual void TransitionProgressed(float fDeltaT);

    /* 0x04 */ bool mTransitionActive;
}; // total size: 0x8

#endif // GAME_RENDER_WIPER_H
