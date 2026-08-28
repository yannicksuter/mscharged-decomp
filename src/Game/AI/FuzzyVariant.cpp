#include "Game/AI/FuzzyVariant.h"

char lbl_806DC418[] = "???";
char lbl_806DC420[] = "{0} {1}";
char lbl_806DC428[] = "Home";
char lbl_806DC430[] = "Away";
char lbl_806DC438[] = "Game";
char lbl_806DC440[] = "Ball";
char lbl_80503F70[] = "Team={0}";

unsigned long FuzzyVariant::GetHash() const
{
    unsigned long hash = 0;

    if (mType != FT_UNSPECIFIED && mType >= (eVariantType)9)
    {
        switch (GetType())
        {
        case (eVariantType)9:
            hash = (unsigned long)mData.pointer;
            break;
        case (eVariantType)10:
            hash = mData.u;
            break;
        case (eVariantType)11:
            hash = (unsigned long)mData.pointer;
            break;
        case (eVariantType)12:
            hash = mData.u;
            break;
        }
    }

    return hash;
}

FuzzyVariant lbl_8056DBD4;
