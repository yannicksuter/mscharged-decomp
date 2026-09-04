#ifndef GAME_RENDER_PRESENTATION_H
#define GAME_RENDER_PRESENTATION_H

#include "Game/InterpreterCore.h"

class Presentation : public InterpreterCore
{
public:
    Presentation();
    virtual ~Presentation();
    virtual void DoFunctionCall(unsigned int function);

    static Presentation& Instance();

    void Update(float deltaTime);
    void Call(const char* functionName);

    /* 0x028 */ char mEmissionName[64];
    /* 0x068 */ char mCurrentFunction[64];
    /* 0x0A8 */ float mWaitTime;
    /* 0x0AC */ float mDeltaTime;
    /* 0x0B0 */ bool mCameraFinished;
    /* 0x0B1 */ bool mPresentationFinished;
}; // size: 0xB4

// Lazily constructed singleton accessor; the retail name is not recoverable.
extern "C" Presentation* fn_801FEEAC();

#endif // GAME_RENDER_PRESENTATION_H
