#include <revolution/kpad/KPAD.h>

#include "NL/plat/DPDData.h"

DPDData::DPDData()
{
    mValidFlag = 0;
    nlVec2Set(mRawPosition, 0.0f, 0.0f);
    mAngle = 0;
    nlVec2Set(mVector, 0.0f, 0.0f);
    nlVec2Set(mPosition, 0.0f, 0.0f);
}

int DPDData::GetPosition(nlVector2* position, unsigned short* angle)
{
    *position = mPosition;
    *angle = mAngle;
    return mValidFlag;
}

void DPDData::Update(KPADStatus* status)
{
    mValidFlag = status->dpd_valid_fg;
    mRawPosition.x = -status->pos.x;
    mRawPosition.y = -status->pos.y;
    float horizonX = status->horizon.x;
    float horizonY = status->horizon.y;
    mAngle
        = (unsigned short)(int)(10430.378f * nlATan2f(horizonY, horizonX));
    mVector.x = status->vec.x;
    mVector.y = status->vec.y;
    mPosition = mRawPosition;
}
