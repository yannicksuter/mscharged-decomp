#ifndef GAME_AUDIO_AUDIO_SCRIPT_INTERPRETER_H
#define GAME_AUDIO_AUDIO_SCRIPT_INTERPRETER_H

#include "Game/InterpreterCore.h"

// Bytecode interpreter embedded by the runtime.
class AudioScriptInterpreter : public InterpreterCore
{
public:
    AudioScriptInterpreter(unsigned int size)
        : InterpreterCore(size)
    {
    }

    virtual inline void DoFunctionCall(unsigned int index);
};

#endif // GAME_AUDIO_AUDIO_SCRIPT_INTERPRETER_H
