#ifndef NL_GLX_TU_8036A800_H
#define NL_GLX_TU_8036A800_H

class nlVector3;
struct nlFloatColour;

extern "C"
{
    void fn_8036A800(unsigned int index, const nlVector3* vector, const nlFloatColour* pColour);
    void fn_8036A938(const nlFloatColour* pColour);
    void fn_8036A9C4(unsigned int index, const nlVector3* vector, const nlFloatColour* pColour, float value);
    void fn_8036AB40(unsigned int index, const nlVector3* vector, const nlFloatColour* pColour, float value);
}

#endif
