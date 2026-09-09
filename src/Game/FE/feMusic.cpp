#include "Game/FE/feMusic.h"
#include "Game/FE/FEAudio.h"

const unsigned long streamcues[14][2] = {
    { 0, 0xE326F931 },
    { 0, 0x445ABF3A },
    { 0, 0x445ABF3A },
    { 0, 0x89992DA1 },
    { 0, 0xAE597F5E },
    { 0, 0x2447F290 },
    { 0, 0x2341D569 },
    { 0, 0x244A44AE },
    { 0, 0x244C90C8 },
    { 0, 0x244C90C8 },
    { 0, 0x11EAB39F },
    { 0, 0x11EAB39F },
    { 0, 0x96384A12 },
    { 0, 0x11EAB39F },
};

namespace FEMusic
{
bool mEnabled = true;
unsigned long mCurrentMusicCue;
unsigned long mCurrentSoundCue;
bool mInGame;
} // namespace FEMusic

bool FEMusic::IsPlayingCupResultStream()
{
    switch (FEMusic::mCurrentMusicCue)
    {
    case (int)0xAE597F5E:
    case 0x2341D569:
    case 0x2447F290:
    case 0x244A44AE:
        return true;
    default:
        return false;
    }
}

void FEMusic::ResumeStream()
{
    FEAudio::ResumeSound(FEMusic::mCurrentMusicCue, (void*)FEMusic::StartStreamIfDifferent);
}

void FEMusic::PauseStream()
{
    FEAudio::PauseSound(FEMusic::mCurrentMusicCue, (void*)FEMusic::StartStreamIfDifferent);
}

void FEMusic::StopStream()
{
    FEAudio::StopAnimAudioEvent(mCurrentSoundCue, (void*)StartStreamIfDifferent);
    mCurrentSoundCue = 0;
    FEAudio::StopAnimAudioEvent(mCurrentMusicCue, (void*)StartStreamIfDifferent);
    mCurrentMusicCue = 0;
}

void FEMusic::StartStreamIfDifferent(int idx)
{
    unsigned long SoundCue = streamcues[idx][0];
    if (mCurrentSoundCue != SoundCue)
    {
        FEAudio::StopAnimAudioEvent(mCurrentSoundCue, (void*)StartStreamIfDifferent);
        mCurrentSoundCue = 0;
        FEAudio::PlaySound(mInGame ? 15 : 21, SoundCue,
            "FEMusic::SoundCue", (void*)StartStreamIfDifferent);
        mCurrentSoundCue = SoundCue;
    }

    unsigned long MusicCue = streamcues[idx][1];
    if (mCurrentMusicCue != MusicCue)
    {
        FEAudio::StopAnimAudioEvent(mCurrentMusicCue, (void*)StartStreamIfDifferent);
        mCurrentMusicCue = 0;
        if (mEnabled)
        {
            FEAudio::PlayTrackedSound(mInGame ? 18 : 22, MusicCue,
                "FEMusic::MusicCue", (void*)StartStreamIfDifferent);
        }
        mCurrentMusicCue = MusicCue;
    }
    else if (FEAudio::IsSoundFinished(mCurrentMusicCue, (void*)StartStreamIfDifferent))
    {
        FEAudio::StopAnimAudioEvent(mCurrentMusicCue, (void*)StartStreamIfDifferent);
        mCurrentMusicCue = 0;
        StartStreamIfDifferent(idx);
    }
}

bool FEMusic::IsEnabled()
{
    return mEnabled;
}

void FEMusic::SetEnabled(bool value)
{
    mEnabled = value;
}

void FEMusic::SetInGame(bool value)
{
    mInGame = value;
}
