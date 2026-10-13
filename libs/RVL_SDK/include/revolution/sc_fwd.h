#ifndef REVOLUTION_SC_FWD_H
#define REVOLUTION_SC_FWD_H

#ifdef __cplusplus
extern "C" {
#endif

void SCInit(void);
unsigned long SCCheckStatus(void);
unsigned char SCGetProgressiveMode(void);
unsigned char SCGetEuRgb60Mode(void);

unsigned char SCGetAspectRatio(void);
unsigned char SCGetLanguage(void);
unsigned char SCGetSoundMode(void);
unsigned long SCGetSimpleAddressID(void);
int SCCheckPCMessageRestriction(void);

#ifdef __cplusplus
}
#endif

#endif // REVOLUTION_SC_FWD_H
