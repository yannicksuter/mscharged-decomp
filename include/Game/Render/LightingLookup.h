#ifndef GAME_RENDER_LIGHTING_LOOKUP_H
#define GAME_RENDER_LIGHTING_LOOKUP_H

#include "NL/nlColour.h"

class PlatTexture;

class LightingLookup
{
public:
    LightingLookup();
    ~LightingLookup();
    void LoadTexture(unsigned int textureHandle);
    nlColour SampleColour(int x, int y, bool tint) const;
    nlColour SampleFilteredColour(float x, float y, bool tint) const;
    u8 ReadTextureIntensity(const PlatTexture* texture, int x, int y) const;

    /* 0x00 */ u8* mValues;
    /* 0x04 */ int mWidth;
    /* 0x08 */ int mHeight;
}; // size: 0x0C

#endif // GAME_RENDER_LIGHTING_LOOKUP_H
