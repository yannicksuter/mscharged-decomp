#ifndef _REPLAY_H_
#define _REPLAY_H_

#include <string.h>

#include "types.h"

class LoadFrame;

struct ReplayablePod
{
};

struct NotReplayablePod
{
};

template <typename T>
struct ReplayableCategory
{
    typedef NotReplayablePod Type;
};

template <>
struct ReplayableCategory<int>
{
    typedef ReplayablePod Type;
};

template <typename T>
inline typename ReplayableCategory<T>::Type ReplayableCategoryOf(const T& current)
{
    typename ReplayableCategory<T>::Type category;
    return category;
}

class WriteByteStream
{
public:
    /* 0x0 */ char mCount;
    /* 0x4 */ char* mStorage;
}; // total size: 0x8

class ReadByteStream
{
public:
    /* 0x0 */ char mCount;
    /* 0x4 */ const char* mStorage;
}; // total size: 0x8

enum ReplayNonBlendables
{
    REPLAY_NON_BLENDABLES = 0,
    DO_NOT_REPLAY_NON_BLENDABLES = 1,
};

class SaveFrame
{
public:
    template <int N, typename T>
    void Replayable(T& current);

    template <int N, typename T>
    void Replayable(T& current, ReplayablePod);

    template <int N, typename T>
    void Replayable(T& current, NotReplayablePod);

    /* 0x0 */ int mInterval;
    /* 0x4 */ WriteByteStream mStream;
}; // total size: 0xC

template <int N, typename T>
inline void SaveFrame::Replayable(T& current)
{
    typename ReplayableCategory<T>::Type category = ReplayableCategoryOf(current);
    Replayable<N>(current, category);
}

template <int N, typename T>
inline void SaveFrame::Replayable(T& current, ReplayablePod)
{
    if (N == 0 || mInterval == N)
    {
        memcpy(mStream.mStorage, &current, sizeof(T));
        mStream.mStorage += sizeof(T);
    }
}

template <int N, typename T>
inline void SaveFrame::Replayable(T& current, NotReplayablePod)
{
    if (N == 0 || mInterval == N)
    {
        current.Replay(*this);
    }
}

template <int N, typename FrameType, typename T>
void Replayable(FrameType& frame, T& current);

#include "Game/LoadFrame.h"

template <int N, typename FrameType, typename T>
inline void Replayable(FrameType& frame, T& current)
{
    if (N == 0 || frame.mInterval == N)
    {
        frame.template Replayable<N>(current);
    }
}

class Replay
{
public:
    struct Frame
    {
        Frame(char* begin, int size, Frame* next);

        char* End() const { return mBegin + mSize; }

        /* 0x00 */ float mTime;
        /* 0x04 */ char* mBegin;
        /* 0x08 */ int mSize;
        /* 0x0C */ int mInterval;
        /* 0x10 */ unsigned int mEvents;
        /* 0x14 */ unsigned int mUnidentifiedState;
        /* 0x18 */ int mReelIdx;
        /* 0x1C */ Frame* mNext;
    }; // total size: 0x20

    struct Reel
    {
        Reel()
            : mBegin(0)
            , mLast(0)
            , mAge(0)
        {
        }

        /* 0x0 */ Frame* mBegin;
        /* 0x4 */ Frame* mLast;
        /* 0x8 */ int mAge;
    }; // total size: 0xC

    Replay(char* memory, int memorySize, int maxFrameSize);
    ~Replay();

    Frame* Next(Frame* frame, int reelIdx) const;
    float TimeOfLastOccurence(unsigned int events) const;
    void NewFrame();
    bool DidOccurInLastNumSeconds(
        unsigned int events, float seconds) const;
    float BeginTime() const;
    float EndTime() const;

    template <typename T>
    void Record(float time, T& snapshot, unsigned int events, unsigned int unidentifiedState);

    template <typename T>
    void Play(float time, T& previous, T& current, float* blend) const;

    /* 0x00 */ Frame* mFree;
    /* 0x04 */ Reel mReels[4];
    /* 0x34 */ int mReelIdx;
    /* 0x38 */ int mTick;
    /* 0x3C */ int mMemorySize;
    /* 0x40 */ int mMaxFrameSize;
    /* 0x44 */ int mActualMaxFrameSize;
};

extern "C" void fn_802C7FA4(Replay*, Replay::Frame**, Replay::Frame**,
    Replay::Frame**);
extern "C" void fn_802C7FC0(
    Replay*, Replay::Frame*, Replay::Frame*, Replay::Frame*);

#endif // _REPLAY_H_
