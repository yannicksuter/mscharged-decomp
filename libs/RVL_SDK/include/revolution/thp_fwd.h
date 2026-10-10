#ifndef REVOLUTION_THP_FWD_H
#define REVOLUTION_THP_FWD_H

#ifdef __cplusplus
extern "C" {
#endif

int THPInit(void);
long THPVideoDecode(void* file, void* tileY, void* tileU, void* tileV, void* work);
unsigned long THPAudioDecode(short* buffer, unsigned char* audioFrame, long flag);

int THPSimpleSetVolume(long volume, long time);

#ifdef __cplusplus
}
#endif

#endif // REVOLUTION_THP_FWD_H
