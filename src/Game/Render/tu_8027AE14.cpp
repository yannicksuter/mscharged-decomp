#include "Game/Render/tu_8027AE14.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/World/WorldDrawable.h"

extern "C"
{
    float lbl_806DEE88 = 0.5f;
    float lbl_806DEE8C = 0.1f;
    float lbl_806DEE90 = 0.3f;
    float lbl_806DEE94 = 0.2f;
}

bool lbl_806E19B8;
StadiumDrawable_8027ADC0* lbl_806E19BC;


extern "C" void fn_8027ADC0(StadiumDrawable_8027ADC0* object, void* context)
{
    ((WorldDrawable*)object)->Initialize((WorldObjectLoadContext*)context);
    lbl_806E19BC = object;
}

extern "C" void fn_8027ADF0(StadiumDrawable_8027ADC0*)
{
}

extern "C" void fn_8027ADF4(StadiumDrawable_8027ADC0* object)
{
    if (object->m_Unknown70 != 0 || lbl_806E19B8)
        ((WorldDrawable*)object)->WorldDrawable::Draw();
}

UnidentifiedObject_8027AE14::UnidentifiedObject_8027AE14(const nlVector3& param1)
    : TimedObject(lbl_806DEE88
          + nlRandomf(-lbl_806DEE8C, lbl_806DEE8C, &nlDefaultSeed))
    , mUnidentified010(param1)
    , mUnidentified020(false)
{
    mUnidentified01C = lbl_806DEE90
        + nlRandomf(-lbl_806DEE94, lbl_806DEE94, &nlDefaultSeed);
}

UnidentifiedObject_8027AE14::~UnidentifiedObject_8027AE14()
{
}

StadiumDrawable_8027ADC0::~StadiumDrawable_8027ADC0()
{
}
