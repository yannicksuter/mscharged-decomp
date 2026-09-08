#ifndef _FEAUDIO_H_
#define _FEAUDIO_H_

class FEAudio
{
public:
    static void EnableSounds(bool enable);
    static void PlaySound(int slotId, unsigned long cueId, const void* debugName, void* context);
    static void PlayTrackedSound(int slotId, unsigned long cueId, const void* debugName, void* context);
    static void PlayAnimAudioEvent(unsigned long cueId, const void* debugName, void* context, bool restartable);
    static void StopAnimAudioEvent(unsigned long cueId, void* context);
    static void PauseSound(unsigned long cueId, void* context);
    static void ResumeSound(unsigned long cueId, void* context);
    static void SetSoundCategory(int category);
    static bool IsSoundFinished(unsigned long cueId, void* context);
};

#endif // _FEAUDIO_H_
