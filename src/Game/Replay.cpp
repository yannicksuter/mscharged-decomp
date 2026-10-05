#include "Game/Replay.h"

#include "Game/TweakRegistry.h"
#include "NL/nlMemory.h"

namespace
{
const int kFrameCount = 2000;
bool renderMemoryLayout;
} // namespace

ReplayFramePool* g_pReplayFramePool;

Replay::Replay(char* memory, int memorySize, int maxFrameSize)
    : mReels()
    , mReelIdx(0)
    , mTick(0)
    , mMemorySize(memorySize)
    , mMaxFrameSize(maxFrameSize)
    , mActualMaxFrameSize(0)
{
    g_pReplayFramePool
        = new (nlMalloc(sizeof(ReplayFramePool), 8, false))
            ReplayFramePool(memory);

    Frame* frame = new (g_pReplayFramePool->Allocate())
        Frame(memory + kFrameCount * sizeof(Frame),
            memorySize - kFrameCount * sizeof(Frame),
            0);
    mFree = frame;
    mFree->mNext = mFree;
    mReels[0].mBegin = mReels[0].mLast = mFree;

    renderMemoryLayout
        = GetTweakBool("/user/draw_replay_bar", false);
}

Replay::~Replay()
{
    g_pReplayFramePool->Reset();
    delete g_pReplayFramePool;
}

Replay::Frame::Frame(char* begin, int size, Frame* next)
{
    mTime = 0.0f;
    mBegin = begin;
    mSize = size;
    mInterval = 0;
    mEvents = 0;
    mExcitement = 0;
    mReelIdx = -1;
    mNext = next;
}

Replay::Frame* Replay::Next(Frame* frame, int reelIdx) const
{
    Frame* current = frame->mNext;

    while (current != mReels[reelIdx].mBegin)
    {
        if (current->mReelIdx == reelIdx)
        {
            return current;
        }

        if (reelIdx == 0 && current->mReelIdx > 0)
        {
            if (frame->mTime <= current->mTime)
            {
                return current;
            }
        }

        current = current->mNext;
    }

    return 0;
}

inline Replay::Frame* GetFrame(const Replay::Reel* reels, int reelIdx)
{
    return reels[reelIdx].mBegin;
}

float Replay::TimeOfLastOccurence(unsigned int events) const
{
    Frame* frame = GetFrame(mReels, mReelIdx);
    float time = frame->mTime;

    while (frame != 0 && frame != mReels[mReelIdx].mLast)
    {
        if ((frame->mEvents & events) != 0)
        {
            time = frame->mTime;
        }
        frame = Next(frame, mReelIdx);
    }

    return time;
}

void Replay::NewFrame()
{
    while (mFree->mSize < mMaxFrameSize)
    {
        Frame* next = mFree->mNext;

        if (next->mReelIdx > 0)
        {
            do
            {
                if (mReels[0].mBegin == mFree->mNext)
                {
                    mReels[0].mBegin = Next(mReels[0].mBegin, 0);
                }
                mFree = mFree->mNext;
            } while (mFree->mReelIdx > 0);
            mFree->mReelIdx = -1;
        }
        else
        {
            if (mReels[0].mBegin == next)
            {
                mReels[0].mBegin = Next(mReels[0].mBegin, 0);
            }

            if (mFree->End() == mFree->mNext->mBegin)
            {
                Frame* nextFrame = mFree->mNext;
                mFree->mSize += nextFrame->mSize;
                mFree->mNext = nextFrame->mNext;
                mFree->mReelIdx = -1;
                g_pReplayFramePool->Free(nextFrame);
            }
            else
            {
                mFree = mFree->mNext;
                mFree->mReelIdx = -1;
            }
        }
    }
}

bool Replay::DidOccurInLastNumSeconds(
    unsigned int events, float seconds) const
{
    float timeThreshold = mReels[mReelIdx].mLast->mTime - seconds;
    Frame* frame = GetFrame(mReels, mReelIdx);

    while (frame != 0)
    {
        if (frame->mTime >= timeThreshold)
        {
            if ((frame->mEvents & events) != 0)
            {
                return true;
            }
        }

        frame = Next(frame, mReelIdx);
    }

    return false;
}

float Replay::BeginTime() const
{
    return mReels[mReelIdx].mBegin->mTime;
}

float Replay::EndTime() const
{
    return mReels[mReelIdx].mLast->mTime;
}

void GetRecordingFrames(Replay* replay,
    Replay::Frame** begin, Replay::Frame** last, Replay::Frame** free)
{
    *begin = replay->mReels[0].mBegin;
    *last = replay->mReels[0].mLast;
    *free = replay->mFree;
}

void SetRecordingFrames(Replay* replay,
    Replay::Frame* begin, Replay::Frame* last, Replay::Frame* free)
{
    replay->mReels[0].mBegin = begin;
    replay->mReels[0].mLast = last;
    replay->mFree = free;
}
