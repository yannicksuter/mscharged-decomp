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


void StadiumDrawable_8027ADC0::Initialize(WorldObjectLoadContext* context)
{
    WorldDrawable::Initialize(context);
    lbl_806E19BC = this;
}

void StadiumDrawable_8027ADC0::ReleaseResources()
{
}

void StadiumDrawable_8027ADC0::Draw()
{
    if (m_Unknown70 != 0 || lbl_806E19B8)
        WorldDrawable::Draw();
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
