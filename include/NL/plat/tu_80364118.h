#ifndef NL_PLAT_TU_80364118_H
#define NL_PLAT_TU_80364118_H

extern "C"
{
    void fn_80364118(signed char* px, signed char* py);
    int fn_80364298(int button);
    int fn_803643A8(int buttonIndex);
    unsigned short fn_80364434(float normalizedX, float normalizedY,
        unsigned short left, unsigned short right, unsigned short down, unsigned short up);
}

#endif // NL_PLAT_TU_80364118_H
