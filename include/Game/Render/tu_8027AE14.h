#ifndef GAME_RENDER_TU_8027AE14_H
#define GAME_RENDER_TU_8027AE14_H

#include "NL/nlMath.h"
#include "Game/World/WorldDrawable.h"
#include "Game/Render/TimedObject.h"

class UnidentifiedObject_8027AE14 : public TimedObject
{
public:
    UnidentifiedObject_8027AE14(const nlVector3& param1);
    virtual ~UnidentifiedObject_8027AE14();
    virtual void Update(float param1);

    /* 0x10 */ nlVector3 mUnidentified010;
    /* 0x1C */ float mUnidentified01C;
    /* 0x20 */ bool mUnidentified020;
}; // size: 0x24

class StadiumDrawable_8027ADC0 : public WorldDrawable
{
public:
    virtual ~StadiumDrawable_8027ADC0();
    virtual void ReleaseResources();
    virtual void Draw();
    virtual void Initialize(WorldObjectLoadContext* context);

    /* 0x70 */ unsigned long m_Unknown70;
};

extern bool lbl_806E19B8;
extern StadiumDrawable_8027ADC0* lbl_806E19BC;


#endif // GAME_RENDER_TU_8027AE14_H
