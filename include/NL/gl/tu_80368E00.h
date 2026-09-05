#ifndef NL_GL_TU_80368E00_H
#define NL_GL_TU_80368E00_H

struct UnidentifiedMemoryRequirement_80376664;

struct UnidentifiedConfiguration_80368E00
{
    unsigned long mUnidentified00;
    unsigned long mUnidentified04;
    const UnidentifiedMemoryRequirement_80376664* mUnidentified08;
    int mUnidentified0C;
    unsigned long mUnidentified10;
}; // size 0x14

extern "C" bool fn_80368E00(const UnidentifiedConfiguration_80368E00* config);

#endif // NL_GL_TU_80368E00_H
