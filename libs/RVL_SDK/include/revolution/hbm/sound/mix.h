#ifndef REVOLUTION_HBM_SOUND_MIX_H
#define REVOLUTION_HBM_SOUND_MIX_H

#include <revolution/types.h>

typedef u32 HBMMIXSoundMode;
enum HBMMIXSoundMode_et
{
    HBMMIX_SOUND_MODE_MONO,
    HBMMIX_SOUND_MODE_STEREO,
    HBMMIX_SOUND_MODE_DPL2,
};

#ifdef __cplusplus
extern "C"
{
#endif

    void HBMMIXInit(void);
    void HBMMIXQuit(void);
    void HBMMIXSetSoundMode(HBMMIXSoundMode mode);
    void HBMMIXUpdateSettings(void);

#ifdef __cplusplus
}
#endif

#endif
