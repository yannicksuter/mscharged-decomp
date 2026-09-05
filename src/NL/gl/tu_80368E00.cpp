#include "NL/gl/tu_80368E00.h"

#include "NL/gl/glMemory.h"
#include "NL/glx/glxTexture.h"

extern "C" bool fn_80368E00(const UnidentifiedConfiguration_80368E00* config)
{
    if (!glxInitMemory(config->mUnidentified00, config->mUnidentified04))
    {
        return false;
    }

    fn_802CC08C(fn_802CBFD8(config->mUnidentified08, config->mUnidentified0C, "Global"));
    fn_802CDEC0(config->mUnidentified10);
    return true;
}
