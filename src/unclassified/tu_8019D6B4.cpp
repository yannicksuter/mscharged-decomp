#include "Game/AI/AiUtil.h"
#include "Game/Character.h"
#include "Game/Drawable/Drawable_8017FFCC.h"

class PhysicsSphere_80175F8C;

extern "C" RenderObject* fn_80276360(int type, int index);
extern "C" PhysicsSphere_80175F8C* fn_80176C18(
    const nlVector3* position, void* owner);

static float lbl_806DCDB8 = 0.5f;
static float lbl_806DCDBC = 1.25f;
static float lbl_806DCDC0 = 0.22f;
static float lbl_806DCDC4 = 2.5f;
static float lbl_806DCDC8 = 3.2f;
static float lbl_806DCDCC = -3.5f;
static float lbl_806DCDD0 = -0.6f;
static float lbl_806DCDD4 = -2.0f;
static float lbl_806DCDD8 = 0.3f;
static float lbl_806DCDDC = 0.15f;

static const nlVector3 lbl_804DCDF8 = { 0.0f, -20.0f, -18.0f };

Object_8017FFF4::Object_8017FFF4(int index)
{
    _010 = lbl_806DCDC0;
    _014 = lbl_806DCDC0;
    _018 = 0.0f;
    _01C = 0.0f;
    visible = false;
    _028 = 0;
    _02C = 0.0f;
    position = lbl_804DCDF8;
    orientation = 0;
    _024 = fn_80276360(6, index);
}

Object_8017FFF4::~Object_8017FFF4()
{
}

void Object_8017FFF4::fn_8019D778(float dt)
{
    if (!visible)
    {
        return;
    }

    if (_02C > 0.0f)
    {
        _02C -= dt;
        if (!(_02C <= 0.0f))
        {
            return;
        }
    }

    if (_01C > 0.0f)
    {
        float oldZ = position.z;
        _01C -= dt;
        position.z += dt * (lbl_806DCDD0 - lbl_806DCDCC) / lbl_806DCDD8;
        if (oldZ < lbl_806DCDD4 && position.z >= lbl_806DCDD4)
        {
            nlVector3 effectPosition = position;
            effectPosition.z += lbl_806DCDC4;
            fn_80176C18(&effectPosition, _028);
        }
    }
    else if (position.z > lbl_806DCDCC)
    {
        position.z -= 0.5f * dt * (lbl_806DCDD0 - lbl_806DCDCC) / lbl_806DCDD8;
    }
    else
    {
        if (visible)
        {
            visible = false;
            position = lbl_804DCDF8;
        }
        _018 = 0.0f;
        _01C = 0.0f;
        _02C = 0.0f;
    }

    if (_018 > 0.0f)
    {
        _018 -= dt;
        if (_018 <= 0.0f)
        {
            _018 = 0.0f;
            _010 = _014;
            return;
        }

        float percent = dt / _018;
        if (percent > 1.0f)
        {
            percent = 1.0f;
        }
        _010 = Interpolate(_010, _014, percent);
    }
}

void Object_8017FFF4::fn_8019D940(cCharacter* owner, u16 angle)
{
    _028 = owner;

    float targetScale = lbl_806DCDB8;
    _014 = targetScale;
    _018 = 0.0f;
    _010 = targetScale;

    targetScale = lbl_806DCDBC;
    float scaleTime = lbl_806DCDDC;
    _014 = targetScale;
    _018 = scaleTime;
    if (scaleTime <= 0.0f)
    {
        _010 = targetScale;
    }

    _01C = lbl_806DCDD8;
    orientation = angle;

    nlVector3 local = { lbl_806DCDC8, 0.0f, lbl_806DCDCC };
    nlVector3 world;
    GetWorldPoint(world, local, owner->m_v3Position, angle);
    position = world;
    visible = true;
}

float Object_8017FFF4::fn_8019DA04() const
{
    return _010 / lbl_806DCDC0;
}

void Object_8017FFF4::fn_8019DA14()
{
    if (visible)
    {
        visible = false;
        position = lbl_804DCDF8;
    }

    _018 = 0.0f;
    _01C = 0.0f;
    _02C = 0.0f;
    position = lbl_804DCDF8;
    orientation = 0;
    _010 = 1.0f;
    _014 = 1.0f;
    _018 = 0.0f;
    _028 = 0;
    _02C = 0.0f;
    _01C = 0.0f;
}
