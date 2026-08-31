#ifndef GAME_RENDER_FLARE_HANDLER_H
#define GAME_RENDER_FLARE_HANDLER_H

#include "Game/GL/GLMeshWriter.h"
#include "NL/nlColour.h"
#include "NL/nlList.h"
#include "NL/nlMath.h"

class GLView;

enum eFlareType
{
    Flare_Halo = 0,
    Flare_Glow = 1,
    Flare_Num = 2,
};

class FlareStruct
{
public:
    FlareStruct() { }

    /* 0x00 */ eFlareType type;
    /* 0x04 */ float size;
    /* 0x08 */ nlColour colour;
    /* 0x0C */ nlVector3 worldPosition;
    /* 0x18 */ FlareStruct* next;
}; // size 0x1C

class FlareHandler
{
public:
    static FlareHandler instance;

    FlareHandler()
        : halos(0, 0)
        , glows(0, 0)
        , mpView(0)
    {
    }

    void Initialize(GLView* view);
    void Cleanup();
    void AddFace(const FlareStruct* pFlare, GLMeshWriter* pMeshWriter);
    void Render();

    /* 0x00 */ nlMatrix4 viewMatrix;
    /* 0x40 */ nlVector3 viewRight;
    /* 0x4C */ nlVector3 viewUp;
    /* 0x58 */ nlList<FlareStruct> halos;
    /* 0x60 */ nlList<FlareStruct> glows;
    /* 0x68 */ GLView* mpView;
}; // size 0x6C

#endif // GAME_RENDER_FLARE_HANDLER_H
