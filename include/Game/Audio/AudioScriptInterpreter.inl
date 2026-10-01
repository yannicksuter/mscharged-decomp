#ifndef GAME_AUDIO_AUDIO_SCRIPT_INTERPRETER_INL
#define GAME_AUDIO_AUDIO_SCRIPT_INTERPRETER_INL

#include "Game/Audio/AudioScriptInterpreter.h"

inline void AudioScriptInterpreter::DoFunctionCall(unsigned int index)
{
    u32 value = m_SP[-1];
    ((u8*)m_SP)[-1] = value != 0;
    if (m_RunState == 3)
        m_SP[-1] = value;
}

#endif // GAME_AUDIO_AUDIO_SCRIPT_INTERPRETER_INL
