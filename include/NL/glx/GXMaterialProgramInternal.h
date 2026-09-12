#ifndef NL_GLX_GX_MATERIAL_PROGRAM_INTERNAL_H
#define NL_GLX_GX_MATERIAL_PROGRAM_INTERNAL_H

#include <revolution/gx/GXTypes.h>

inline GXPrimitive glxGetPrimitiveType(unsigned char primType)
{
    static GXPrimitive primitives[6] = {
        GX_TRIANGLES, GX_TRIANGLESTRIP, GX_TRIANGLEFAN, GX_QUADS, GX_LINES, GX_LINESTRIP
    };
    return primitives[primType];
}

#endif // NL_GLX_GX_MATERIAL_PROGRAM_INTERNAL_H
