#ifndef REVOLUTION_HBM_SOUND_SYN_H
#define REVOLUTION_HBM_SOUND_SYN_H

#include <revolution/types.h>

#define NUM_MIDI_CHANNELS        16
#define NUM_MIDI_NOTES           128
#define HBMSYN_INPUT_BUFFER_SIZE 256

typedef struct WTINST WTINST;
typedef struct WTREGION WTREGION;
typedef struct WTART WTART;
typedef struct WTSAMPLE WTSAMPLE;
typedef struct WTADPCM WTADPCM;
typedef struct HBMSYNVOICE HBMSYNVOICE;

typedef struct HBMSYNSYNTH
{
    struct HBMSYNSYNTH* next;
    WTINST* percussiveInst;
    WTINST* melodicInst;
    WTREGION* region;
    WTART* art;
    WTSAMPLE* sample;
    WTADPCM* adpcm;
    u32 samplesBaseWord;
    u32 samplesBaseByte;
    u32 samplesBaseNibble;
    WTINST* inst[NUM_MIDI_CHANNELS];
    s32 masterVolume;
    s32 volAttn[NUM_MIDI_CHANNELS];
    s32 auxAAttn[NUM_MIDI_CHANNELS];
    u8 pan[NUM_MIDI_CHANNELS];
    u8 input[HBMSYN_INPUT_BUFFER_SIZE][3];
    u8* inputPosition;
    u32 inputCounter;
    u32 notes;
    HBMSYNVOICE* voice[NUM_MIDI_CHANNELS][NUM_MIDI_NOTES];
} HBMSYNSYNTH;

#ifdef __cplusplus
extern "C"
{
#endif

    void HBMSYNInit(void);
    void HBMSYNQuit(void);
    void HBMSYNRunAudioFrame(void);

#ifdef __cplusplus
}
#endif

#endif
