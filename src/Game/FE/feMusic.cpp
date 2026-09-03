#include "Game/FE/feMusic.h"

const unsigned long lbl_804E8438[14][2] = {
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

extern "C" void fn_801CBC54(int slotId, unsigned long cueId, const void* debugName, void* context);
extern "C" void fn_801CBC78(int slotId, unsigned long cueId, const void* debugName, void* context);
extern "C" void fn_801CBCE4(unsigned long cueId, void* context);
extern "C" bool fn_801CBD00(unsigned long cueId, void* context);

namespace FEMusic
{
bool mEnabled_806DDA98 = true;
unsigned long mCurrentMusicCue_806E1878;
unsigned long mCurrentSoundCue_806E187C;
bool mUnidentifiedMode_806E1880;
} // namespace FEMusic

void FEMusic::StopStream()
{
    fn_801CBCE4(mCurrentSoundCue_806E187C, (void*)StartStreamIfDifferent);
    mCurrentSoundCue_806E187C = 0;
    fn_801CBCE4(mCurrentMusicCue_806E1878, (void*)StartStreamIfDifferent);
    mCurrentMusicCue_806E1878 = 0;
}

void FEMusic::StartStreamIfDifferent(int idx)
{
    unsigned long SoundCue = lbl_804E8438[idx][0];
    if (mCurrentSoundCue_806E187C != SoundCue)
    {
        fn_801CBCE4(mCurrentSoundCue_806E187C, (void*)StartStreamIfDifferent);
        mCurrentSoundCue_806E187C = 0;
        fn_801CBC54(mUnidentifiedMode_806E1880 ? 15 : 21, SoundCue,
            "FEMusic::SoundCue", (void*)StartStreamIfDifferent);
        mCurrentSoundCue_806E187C = SoundCue;
    }

    unsigned long MusicCue = lbl_804E8438[idx][1];
    if (mCurrentMusicCue_806E1878 != MusicCue)
    {
        fn_801CBCE4(mCurrentMusicCue_806E1878, (void*)StartStreamIfDifferent);
        mCurrentMusicCue_806E1878 = 0;
        if (mEnabled_806DDA98)
        {
            fn_801CBC78(mUnidentifiedMode_806E1880 ? 18 : 22, MusicCue,
                "FEMusic::MusicCue", (void*)StartStreamIfDifferent);
        }
        mCurrentMusicCue_806E1878 = MusicCue;
    }
    else if (fn_801CBD00(mCurrentMusicCue_806E1878, (void*)StartStreamIfDifferent))
    {
        fn_801CBCE4(mCurrentMusicCue_806E1878, (void*)StartStreamIfDifferent);
        mCurrentMusicCue_806E1878 = 0;
        StartStreamIfDifferent(idx);
    }
}

bool FEMusic::IsEnabled_801FC2AC()
{
    return mEnabled_806DDA98;
}

void FEMusic::SetEnabled_801FC2A4(bool value)
{
    mEnabled_806DDA98 = value;
}

void FEMusic::SetUnidentifiedMode_801FC29C(bool value)
{
    mUnidentifiedMode_806E1880 = value;
}
