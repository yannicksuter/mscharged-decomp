#ifndef _LOADFRAME_H_
#define _LOADFRAME_H_

class LoadFrame
{
public:
    template <int N, typename T>
    void Replayable(T& current);

    template <int N, typename T>
    void Replayable(T& current, ReplayablePod);

    template <int N, typename T>
    void Replayable(T& current, NotReplayablePod);

    /* 0x0 */ int mInterval;
    /* 0x4 */ ReadByteStream mStream;
    /* 0xC */ ReplayNonBlendables mReplayNonBlendables;
    /* 0x10 */ float mNonBlendableAheadOfFrame;
}; // total size: 0x14

template <int N, typename T>
inline void LoadFrame::Replayable(T& current)
{
    typename ReplayableCategory<T>::Type category = ReplayableCategoryOf(current);
    Replayable<N>(current, category);
}

template <int N, typename T>
inline void LoadFrame::Replayable(T& current, ReplayablePod)
{
    if (N == 0 || mInterval == N)
    {
        memcpy(&current, mStream.mStorage, sizeof(T));
        mStream.mStorage += sizeof(T);
    }
}

template <int N, typename T>
inline void LoadFrame::Replayable(T& current, NotReplayablePod)
{
    if (N == 0 || mInterval == N)
    {
        current.Replay(*this);
    }
}

#endif // _LOADFRAME_H_
