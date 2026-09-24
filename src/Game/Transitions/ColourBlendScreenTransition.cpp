#include <stddef.h>

#include "Game/Transitions/ColourBlendScreenTransition.h"

#include "NL/nlColour.h"
#include "NL/gl/gl.h"
#include "NL/gl/glDraw2.h"
#include "NL/gl/glState.h"

#include <stdlib.h>

ColourBlendScreenTransition::ColourBlendScreenTransition(float length, const nlVector4& startColour, const nlVector4& endColour, u32 texture)
{
    m_fLength = length;
    m_fCurrentTime = 0.0f;
    m_RGBAstart = startColour;
    m_RGBAend = endColour;
    m_nTexture = texture;
}

/**
 * Offset/Address/Size: 0x3E0 | 0x80339B84 | size: 0x10
 */
void ColourBlendScreenTransition::Update(float dt)
{
    m_fCurrentTime += dt;
}

/**
 * Offset/Address/Size: 0x260 | 0x80339B94 | size: 0x180
 */
void ColourBlendScreenTransition::Render(GLView* view)
{
    struct glPoly2 poly;
    struct nlColour colour;
    class nlVector4 v4Clr;

    float startWeight;
    float endWeight;
    nlVector4 tmp = { 0.0f, 0.0f, 0.0f, 0.0f };

    if (m_fLength > 0.0001f)
    {
        endWeight = m_fCurrentTime / m_fLength;
        startWeight = 1.0f - endWeight;
        tmp.x = (startWeight * m_RGBAstart.e[0]) + (endWeight * m_RGBAend.e[0]);
        tmp.y = (startWeight * m_RGBAstart.e[1]) + (endWeight * m_RGBAend.e[1]);
        tmp.z = (startWeight * m_RGBAstart.e[2]) + (endWeight * m_RGBAend.e[2]);
        tmp.w = (startWeight * m_RGBAstart.e[3]) + (endWeight * m_RGBAend.e[3]);
    }

    v4Clr = tmp;
    nlColourSet(colour,
        (u8)(255.0f * v4Clr.x),
        (u8)(255.0f * v4Clr.y),
        (u8)(255.0f * v4Clr.z),
        (u8)(255.0f * v4Clr.w));

    glSetDefaultState(false);
    glSetCurrentTexture(m_nTexture, GLTT_Diffuse);
    glSetRasterState(GLS_AlphaBlend, 1);
    glSetCurrentRasterState(glHandleizeRasterState());
    glSetCurrentTextureState(glHandleizeTextureState());

    poly.FullCoverage(colour, 0.0f);
    poly.Attach(view, 0, NULL);
}

/**
 * Offset/Address/Size: 0x94 | 0x80339D14 | size: 0x1CC
 */
ColourBlendScreenTransition* ColourBlendScreenTransition::GetFromParser(SimpleParser* parser)
{
    class nlVector4 v4StartColour;
    class nlVector4 v4EndColour;
    unsigned long texture;
    char* pToken;

    static u32 defaultTexture = glHash("global/white");

    texture = defaultTexture;

    float fLength = atof(parser->NextTokenOnLine(true));

    v4StartColour.x = atof(parser->NextTokenOnLine(true));
    v4StartColour.y = atof(parser->NextTokenOnLine(true));
    v4StartColour.z = atof(parser->NextTokenOnLine(true));
    v4StartColour.w = atof(parser->NextTokenOnLine(true));

    v4EndColour.x = atof(parser->NextTokenOnLine(true));
    v4EndColour.y = atof(parser->NextTokenOnLine(true));
    v4EndColour.z = atof(parser->NextTokenOnLine(true));
    v4EndColour.w = atof(parser->NextTokenOnLine(true));

    pToken = parser->NextTokenOnLine(true);
    if (pToken != NULL)
    {
        texture = glHash(pToken);
    }

    ColourBlendScreenTransition* transition = new (nlMalloc(sizeof(ColourBlendScreenTransition), 8, 0)) ColourBlendScreenTransition(fLength, v4StartColour, v4EndColour, texture);
    return transition;
}
