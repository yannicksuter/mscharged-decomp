#ifndef GAME_WORLD_WORLD_DRAWABLE_H
#define GAME_WORLD_WORLD_DRAWABLE_H

#include "Game/Drawable/DrawableObj.h"

// A drawable the world stream places and the world animation drives: it
// reads its world matrix from the animation node it is bound to, culls
// against the frustum with the bounding sphere its model gave it, and
// hands the model's packets to the world's views. The shared bodies live
// with the world animation objects; the empty model slot sits with the
// stadium objects. Only the slots are shared here, every derived class
// keeps its own data.
class WorldDrawable : public DrawableObject
{
public:
    virtual nlMatrix4* GetWorldMatrix();
    virtual bool V6(const nlVector4* planes) const;
    virtual void V7(glModel* model);
    virtual void V8(GLView* view);
};

#endif // GAME_WORLD_WORLD_DRAWABLE_H
