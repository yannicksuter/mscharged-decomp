#ifndef GAME_SCRIPT_TUNING_H
#define GAME_SCRIPT_TUNING_H

#include "Game/TweaksBase.h"
#include "Game/TweakValue.h"

class FuzzyTweaks : public TweaksBase
{
public:
    FuzzyTweaks(const char* name, const char* category);
    virtual ~FuzzyTweaks();
    virtual void Init();

public:
    void fn_800E1C98(bool registerTweaks);

    /* 0x044 */ TweakFloatBinding mUnidentified044;
    /* 0x054 */ TweakFloatBinding mUnidentified054;
    /* 0x064 */ TweakFloatBinding mUnidentified064;
    /* 0x074 */ TweakFloatBinding mUnidentified074;
    /* 0x084 */ TweakFloatBinding mUnidentified084;
    /* 0x094 */ TweakFloatBinding mUnidentified094;
    /* 0x0A4 */ TweakFloatBinding mUnidentified0A4;
    /* 0x0B4 */ TweakFloatBinding mUnidentified0B4;
    /* 0x0C4 */ TweakFloatBinding mUnidentified0C4;
    /* 0x0D4 */ TweakFloatBinding mUnidentified0D4;
    /* 0x0E4 */ TweakFloatBinding mUnidentified0E4;
    /* 0x0F4 */ TweakFloatBinding mUnidentified0F4;
    /* 0x104 */ TweakFloatBinding mUnidentified104;
    /* 0x114 */ TweakFloatBinding mUnidentified114;
    /* 0x124 */ TweakFloatBinding mUnidentified124;
    /* 0x134 */ TweakFloatBinding mUnidentified134;
    /* 0x144 */ TweakFloatBinding mUnidentified144;
    /* 0x154 */ TweakFloatBinding mUnidentified154;
    /* 0x164 */ TweakFloatBinding mUnidentified164;
    /* 0x174 */ TweakFloatBinding mUnidentified174;
    /* 0x184 */ TweakFloatBinding mUnidentified184;
    /* 0x194 */ TweakFloatBinding mUnidentified194;
    /* 0x1A4 */ TweakFloatBinding mUnidentified1A4;
    /* 0x1B4 */ TweakFloatBinding mUnidentified1B4;
    /* 0x1C4 */ TweakFloatBinding mUnidentified1C4;
    /* 0x1D4 */ TweakFloatBinding mUnidentified1D4;
    /* 0x1E4 */ TweakFloatBinding mUnidentified1E4;
    /* 0x1F4 */ TweakFloatBinding mUnidentified1F4;
    /* 0x204 */ TweakFloatBinding mUnidentified204;
    /* 0x214 */ TweakFloatBinding mUnidentified214;
    /* 0x224 */ TweakFloatBinding mUnidentified224;
    /* 0x234 */ TweakFloatBinding mUnidentified234;
    /* 0x244 */ TweakFloatBinding mUnidentified244;
    /* 0x254 */ TweakFloatBinding mUnidentified254;
    /* 0x264 */ TweakFloatBinding mUnidentified264;
    /* 0x274 */ TweakFloatBinding mUnidentified274;
    /* 0x284 */ TweakFloatBinding mUnidentified284;
    /* 0x294 */ TweakFloatBinding mUnidentified294;
    /* 0x2A4 */ TweakFloatBinding mUnidentified2A4;
    /* 0x2B4 */ TweakFloatBinding mUnidentified2B4;
    /* 0x2C4 */ TweakFloatBinding mUnidentified2C4;
    /* 0x2D4 */ TweakFloatBinding mUnidentified2D4;
    /* 0x2E4 */ TweakFloatBinding mUnidentified2E4;
    /* 0x2F4 */ TweakFloatBinding mUnidentified2F4;
    /* 0x304 */ TweakFloatBinding mUnidentified304;
    /* 0x314 */ TweakFloatBinding mUnidentified314;
    /* 0x324 */ TweakFloatBinding mUnidentified324;
    /* 0x334 */ TweakFloatBinding mUnidentified334;
    /* 0x344 */ TweakFloatBinding mUnidentified344;
    /* 0x354 */ TweakFloatBinding mUnidentified354;
    /* 0x364 */ TweakFloatBinding mUnidentified364;
    /* 0x374 */ TweakFloatBinding mUnidentified374;
    /* 0x384 */ TweakFloatBinding mUnidentified384;
    /* 0x394 */ TweakFloatBinding mUnidentified394;
    /* 0x3A4 */ TweakIntBinding nFacingFullConfidenceAngle;
    /* 0x3B4 */ TweakIntBinding nFacingNoConfidenceAngle;
    /* 0x3C4 */ TweakFloatBinding mUnidentified3C4;
    /* 0x3D4 */ TweakFloatBinding mUnidentified3D4;
    /* 0x3E4 */ TweakFloatBinding mUnidentified3E4;
    /* 0x3F4 */ TweakFloatBinding mUnidentified3F4;
    /* 0x404 */ TweakFloatBinding mUnidentified404;
    /* 0x414 */ TweakFloatBinding mUnidentified414;
    /* 0x424 */ TweakFloatBinding mUnidentified424;
    /* 0x434 */ TweakFloatBinding mUnidentified434;
    /* 0x444 */ TweakFloatBinding mUnidentified444;
    /* 0x454 */ TweakFloatBinding mUnidentified454;
    /* 0x464 */ TweakFloatBinding mUnidentified464;
    /* 0x474 */ TweakFloatBinding mUnidentified474;
    /* 0x484 */ TweakFloatBinding mUnidentified484;
    /* 0x494 */ TweakFloatBinding mUnidentified494;
    /* 0x4A4 */ TweakFloatBinding mUnidentified4A4;
    /* 0x4B4 */ TweakFloatBinding mUnidentified4B4;
    /* 0x4C4 */ TweakFloatBinding mUnidentified4C4;
    /* 0x4D4 */ TweakFloatBinding mUnidentified4D4;
    /* 0x4E4 */ TweakFloatBinding mUnidentified4E4;
    /* 0x4F4 */ TweakFloatBinding mUnidentified4F4;
    /* 0x504 */ TweakFloatBinding mUnidentified504;
    /* 0x514 */ TweakFloatBinding mUnidentified514;
    /* 0x524 */ TweakFloatBinding mUnidentified524;
    /* 0x534 */ TweakFloatBinding mUnidentified534;
    /* 0x544 */ TweakFloatBinding mUnidentified544;
    /* 0x554 */ TweakFloatBinding mUnidentified554;
    /* 0x564 */ TweakFloatBinding mUnidentified564;
    /* 0x574 */ TweakFloatBinding mUnidentified574;
    /* 0x584 */ TweakFloatBinding mUnidentified584;
    /* 0x594 */ TweakFloatBinding mUnidentified594;
    /* 0x5A4 */ TweakFloatBinding mUnidentified5A4;
    /* 0x5B4 */ TweakFloatBinding mUnidentified5B4;
    /* 0x5C4 */ TweakFloatBinding mUnidentified5C4;
    /* 0x5D4 */ TweakFloatBinding mUnidentified5D4;
    /* 0x5E4 */ TweakFloatBinding mUnidentified5E4;
    /* 0x5F4 */ TweakFloatBinding mUnidentified5F4;
    /* 0x604 */ TweakFloatBinding mUnidentified604;
    /* 0x614 */ TweakFloatBinding mUnidentified614;
    /* 0x624 */ TweakFloatBinding mUnidentified624;
    /* 0x634 */ TweakFloatBinding mUnidentified634;
    /* 0x644 */ TweakFloatBinding mUnidentified644;
    /* 0x654 */ TweakFloatBinding mUnidentified654;
    /* 0x664 */ TweakFloatBinding mUnidentified664;
    /* 0x674 */ TweakFloatBinding mUnidentified674;
    /* 0x684 */ TweakFloatBinding mUnidentified684;
    /* 0x694 */ TweakFloatBinding mUnidentified694;
    /* 0x6A4 */ TweakFloatBinding mUnidentified6A4;
    /* 0x6B4 */ TweakFloatBinding mUnidentified6B4;
    /* 0x6C4 */ TweakFloatBinding mUnidentified6C4;
    /* 0x6D4 */ TweakFloatBinding mUnidentified6D4;
    /* 0x6E4 */ TweakFloatBinding mUnidentified6E4;
    /* 0x6F4 */ TweakFloatBinding mUnidentified6F4;
    /* 0x704 */ TweakFloatBinding mUnidentified704;
    /* 0x714 */ TweakFloatBinding mUnidentified714;
    /* 0x724 */ TweakFloatBinding mUnidentified724;
    /* 0x734 */ TweakFloatBinding mUnidentified734;
    /* 0x744 */ TweakFloatBinding mUnidentified744;
    /* 0x754 */ TweakFloatBinding mUnidentified754;
    /* 0x764 */ TweakFloatBinding mUnidentified764;
    /* 0x774 */ TweakFloatBinding mUnidentified774;
    /* 0x784 */ TweakFloatBinding mUnidentified784;
    /* 0x794 */ TweakFloatBinding mUnidentified794;
    /* 0x7A4 */ TweakFloatBinding mUnidentified7A4;

    /* 0x7B4 */ const char* mUnidentified7B4;
}; // total size: 0x7B8

#endif // GAME_SCRIPT_TUNING_H
