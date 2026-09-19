#ifndef GAME_RENDER_BULLET_BILL_H
#define GAME_RENDER_BULLET_BILL_H

#include "NL/nlMath.h"
#include "types.h"

class PhysicsObject;
class DrawableObject;
class cFielder;
class EmissionController;

typedef DrawableObject RenderObject;

// DrawableBulletBill reads this object through const pointers. R4QE01 keeps
// its drawable load below Render's callee saves, which GC/3.0a5 emits only
// when the pointed-to type has a mutable non-pointer member. The stripped DOL
// cannot identify which member carried the qualifier; active is the state the
// const snapshot readers consume.
struct BulletBillObject
{
    BulletBillObject(RenderObject* pDrawable, u32 nIndex, float radius, float parameter);
    ~BulletBillObject();

    void Update(float deltaTime);
    void Show(cFielder* fielder);
    void Hide(bool destroyEffect);
    void Reset();

    /* 0x00 */ nlQuaternion orientation;
    /* 0x10 */ nlVector3 position;
    /* 0x1C */ nlVector3 velocity;
    /* 0x28 */ u32 index;
    /* 0x2C */ float scale;
    /* 0x30 */ float targetScale;
    /* 0x34 */ float scaleTimer;
    /* 0x38 */ mutable bool active;
    /* 0x39 */ u8 padding_39[3];
    /* 0x3C */ PhysicsObject* physics;
    /* 0x40 */ RenderObject* drawable;
    /* 0x44 */ cFielder* target;
}; // total size: 0x48

void UpdateBulletBillEmitter(EmissionController& controller);

#endif // GAME_RENDER_BULLET_BILL_H
