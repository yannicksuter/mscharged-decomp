#ifndef GAME_RENDER_PRESENTATION_H
#define GAME_RENDER_PRESENTATION_H

#include "Game/FE/feCamera.h"
#include "Game/InterpreterCore.h"

class FEModelHandle;

void OnPresentationModelAnimationFinished(FEModelHandle* object);

class Presentation : public InterpreterCore
{
public:
    Presentation();
    virtual ~Presentation();
    virtual void DoFunctionCall(unsigned int function);

    static Presentation& Instance();
    static Presentation* GetInstance();

    void Update(float deltaTime);
    void Call(const char* functionName);
    bool IsActive() const;

    /* 0x028 */ char mEmissionName[64];
    /* 0x068 */ char mCurrentFunction[64];
    /* 0x0A8 */ float mWaitTime;
    /* 0x0AC */ float mDeltaTime;
    /* 0x0B0 */ bool mCameraFinished;
    /* 0x0B1 */ bool mCameraTransitionFinished;
}; // size: 0xB4

#endif // GAME_RENDER_PRESENTATION_H
