#ifndef _NOISEFILTER_H_
#define _NOISEFILTER_H_

#include "Game/Camera/rumblefilter.h"

class cNoiseFilter : public cCameraFilter
{
public:
    struct Sample
    {
        Sample()
            : mTime(-1)
        {
        }

        /* 0x00 */ int mTime;
        /* 0x04 */ nlVector3 mValue;
    }; // total size: 0x10

    cNoiseFilter();
    inline void UpdateSample(int index, int sample);
    virtual void Update(float dt);
    virtual void Filter(const nlMatrix4& matViewIn, nlMatrix4& matViewOut);
    virtual void Reset();
    virtual int GetFilterIndex();
    virtual void Start(nlVector3 amplitude, float frequency, float duration);

    /* 0x04 */ bool mActive;
    /* 0x05 */ u8 mPadding05[3];
    /* 0x08 */ int mSampleIndex;
    /* 0x0C */ float mFrequency;
    /* 0x10 */ float mSampleInterval;
    /* 0x14 */ float mElapsedTime;
    /* 0x18 */ float mTimeRemaining;
    /* 0x1C */ nlVector3 mAmplitude;
    /* 0x28 */ nlVector3 mSeed;
    /* 0x34 */ nlVector3 mDisplacement;
    /* 0x40 */ Sample mSamples[2];
}; // total size: 0x60

#endif // _NOISEFILTER_H_
