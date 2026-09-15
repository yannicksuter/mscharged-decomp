#ifndef _LOADFRAME_H_
#define _LOADFRAME_H_

class LoadFrame
{
public:
    template <int N, typename T>
    void ReplayablePolymorphicPtr(T*& current);
    int GetInterval() const;
    float fn_801948B0() const;
    void fn_80191504();
    bool fn_801919D0() const;
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
    typename ReplayableCategory<T>::Type category;
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

template <int N, typename T>
inline void LoadFrame::ReplayablePolymorphicPtr(T*& current)
{
    if (N == 0 || mInterval == N)
    {
        unsigned char notNull = 1;
        memcpy(&notNull, mStream.mStorage, 1);
        mStream.mStorage++;
        if (notNull)
        {
            char typeId = 0;
            memcpy(&typeId, mStream.mStorage, 1);
            mStream.mStorage++;
            if (typeId < 0 || typeId > 4)
                nlBreak();
            ::Replayable<N>(*this, typeId, current);
        }
        else
        {
            current = 0;
        }
    }
}

#endif // _LOADFRAME_H_
