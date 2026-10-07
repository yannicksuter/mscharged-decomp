#ifndef RVL_SDK_TPL_TYPES_H
#define RVL_SDK_TPL_TYPES_H

#include <revolution/gx/GXTextureTypes.h>

typedef struct TPLHeader {
    unsigned short height;            // at 0x0
    unsigned short width;             // at 0x2
    unsigned long format;            // at 0x4
    char* data;            // at 0x8
    GXTexWrapMode wrapS;   // at 0xC
    GXTexWrapMode wrapT;   // at 0x10
    GXTexFilter minFilter; // at 0x14
    GXTexFilter magFilter; // at 0x18
    float LODBias;           // at 0x1C
    unsigned char edgeLODEnable;      // at 0x20
    unsigned char minLOD;             // at 0x21
    unsigned char maxLOD;             // at 0x22
    unsigned char unpacked;           // at 0x23
} TPLHeader;

typedef struct TPLClutHeader {
    unsigned short numEntries;   // at 0x0
    unsigned char unpacked;      // at 0x2
    unsigned char pad8;          // at 0x3
    GXTlutFmt format; // at 0x4
    char* data;       // at 0x8
} TPLClutHeader;

typedef struct TPLDescriptor {
    TPLHeader* textureHeader;  // at 0x0
    TPLClutHeader* CLUTHeader; // at 0x4
} TPLDescriptor;

typedef struct TPLPalette {
    unsigned long versionNumber;              // at 0x0
    unsigned long numDescriptors;             // at 0x4
    TPLDescriptor* descriptorArray; // at 0x8
} TPLPalette;

#endif // RVL_SDK_TPL_TYPES_H
