#ifndef _REPLAY_H_
#define _REPLAY_H_

#include "types.h"

class Replay
{
public:
    Replay(char* memory, int memorySize, int maxFrameSize);
    ~Replay();

    float BeginTime() const;
    float EndTime() const;

    template <typename T>
    void Record(float time, T& snapshot, unsigned int events, unsigned int unidentifiedState);

    template <typename T>
    void Play(float time, T& previous, T& current, float* blend) const;
};

#endif // _REPLAY_H_
