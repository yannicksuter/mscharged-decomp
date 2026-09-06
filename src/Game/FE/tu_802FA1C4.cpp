#include "Game/FE/tu_802FA1C4.h"

#include "Game/TweakValue.h"

TU802FA1C4::TU802FA1C4()
    : mUnidentified01C(0)
    , mUnidentified050(0)
    , mUnidentified054(-1)
{
    for (int i = 0; i < 4; ++i)
    {
        mUnidentified00C[i] = 0;
        mUnidentified020[i][0] = 0.0f;
        mUnidentified020[i][1] = 0.0f;
        mUnidentified040[i] = 0;
        mUnidentified048[i] = false;
        mUnidentified04C[i] = true;
    }
}

TU802FA1C4::~TU802FA1C4()
{
}

void TU802FA1C4::fn_802FA2FC(TU80300104Base* listener)
{
    mUnidentified000.AddEnd(listener);
    ++mUnidentified050;
}

void TU802FA1C4::fn_802FA388(TU80300104Base* listener)
{
    mUnidentified000.RemoveEntry(listener);
    --mUnidentified050;
}

static float lbl_806DF4F8 = 0.02f;
static float lbl_806DF4FC = 0.95f;
static TweakValueImpl_804F4DC8 lbl_8057FC00("Pos Radius", "FE", &lbl_806DF4F8);
static TweakValueImpl_804F4DC8 lbl_8057FC20("Pos Sensitivity", "FE", &lbl_806DF4FC);
