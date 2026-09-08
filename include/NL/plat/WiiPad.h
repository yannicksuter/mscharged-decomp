#ifndef NL_PLAT_WII_PAD_H
#define NL_PLAT_WII_PAD_H

void ClampWiiStick(signed char* px, signed char* py);
int GetWiiButtonIndex(int button);
int GetWiiButtonMask(int buttonIndex);
unsigned short MapWiiStickToDPad(float normalizedX, float normalizedY,
    unsigned short left, unsigned short right, unsigned short down, unsigned short up);

extern int* gWiiRemoteButtonRemap;
extern int* gWiiFreestyleButtonRemap;
extern int* gWiiClassicButtonRemap;
extern int gWiiRemotePadClassID;
extern int gWiiFreestylePadClassID;
extern int gWiiClassicPadClassID;

#endif // NL_PLAT_WII_PAD_H
