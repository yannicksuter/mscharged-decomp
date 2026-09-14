#ifndef NL_GLX_GLXLIGHT_H
#define NL_GLX_GLXLIGHT_H

class nlVector3;
struct nlFloatColour;

void glx_LoadDirectionalLight(unsigned int index, const nlVector3* vector, nlFloatColour* pColour);
void glx_SetAmbientColour(nlFloatColour* pColour);
void glx_LoadPointLight(unsigned int index, const nlVector3* vector, nlFloatColour* pColour, float value);
void glx_LoadSpecular(unsigned int index, const nlVector3* vector, nlFloatColour* pColour, float value);

#endif
