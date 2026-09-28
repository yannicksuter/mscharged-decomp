#ifndef GAME_DRAWABLE_DRAWABLE_OBJ_H
#define GAME_DRAWABLE_DRAWABLE_OBJ_H

#include "Game/World/WorldDrawable.h"
#include "NL/gl/glModel.h"

// Model drawables add their render flags and translucency to the world prefix.
class DrawableObject : public WorldDrawable
{
public:
    virtual ~DrawableObject() { }
    virtual DrawableObject* Clone(unsigned long hash);

    float GetTranslucency() const { return m_fTranslucency; }

    /* 0x70 */ unsigned long m_uObjectFlags;
    union
    {
        /* 0x74 */ float m_fTranslucency;
        /* 0x74 */ int mUnidentified074;
    };
}; // size: 0x78

#endif // GAME_DRAWABLE_DRAWABLE_OBJ_H
