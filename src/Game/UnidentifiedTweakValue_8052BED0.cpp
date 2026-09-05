#include "Game/UnidentifiedTweakValue_8052BED0.h"
#include "NL/nlList.h"

static bool lbl_806DF360 = true;
static UnidentifiedTweakValue_8052BED0* lbl_806E1E80;
static UnidentifiedTweakValue_8052BED0* lbl_806E1E84;

UnidentifiedTweakValue_8052BED0::~UnidentifiedTweakValue_8052BED0()
{
    nlListRemoveElement(&lbl_806E1E80, this, &lbl_806E1E84);
}

void UnidentifiedTweakValue_8052BED0::fn_802C4E78()
{
    if (lbl_806DF360)
    {
        lbl_806E1E80 = 0;
        lbl_806E1E84 = 0;
        lbl_806DF360 = false;
    }

    nlListAddEnd(&lbl_806E1E80, &lbl_806E1E84, this);
}

int UnidentifiedTweakValue_8052BED0::UnidentifiedVirtual0C()
{
    int result = 6;
    if (mUnidentified010)
    {
        result = 7;
    }
    return result;
}
