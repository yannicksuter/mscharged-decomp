#include "unclassified/tu_80264E98.h"

#include <string.h>

extern "C" u16 RFLGetAvailableOfficialDataNum();

TU80264E98Scene::TU80264E98Scene()
    : mUnidentified30(false)
    , mUnidentified38(0)
    , mUnidentified3C(false)
    , mUnidentified200()
    , mNavigation()
{
    mUnidentified1C = RFLGetAvailableOfficialDataNum();
    mUnidentified34 = mUnidentified1C / 10 + (mUnidentified1C % 10 != 0);
    if (mUnidentified34 == 0)
    {
        mUnidentified34 = 1;
    }

    mUnidentified20[0] = 0;
    mUnidentified20[1] = 0;
    mUnidentified20[2] = 0;
    mUnidentified20[3] = 0;

    for (int i = 0; i < 10; ++i)
    {
        mUnidentified200[i].mContext = (void*)i;
    }

    memset(mUnidentified40, -1, sizeof(mUnidentified40));
    mNavigation.fn_801D2BE8(false);
}

TU80264E98Scene::~TU80264E98Scene()
{
}
