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

    /* 0x044 */ TweakValueImpl_804F4DC8 mUnidentified044;
    /* 0x054 */ TweakValueImpl_804F4DC8 mUnidentified054;
    /* 0x064 */ TweakValueImpl_804F4DC8 mUnidentified064;
    /* 0x074 */ TweakValueImpl_804F4DC8 mUnidentified074;
    /* 0x084 */ TweakValueImpl_804F4DC8 mUnidentified084;
    /* 0x094 */ TweakValueImpl_804F4DC8 mUnidentified094;
    /* 0x0A4 */ TweakValueImpl_804F4DC8 mUnidentified0A4;
    /* 0x0B4 */ TweakValueImpl_804F4DC8 mUnidentified0B4;
    /* 0x0C4 */ TweakValueImpl_804F4DC8 mUnidentified0C4;
    /* 0x0D4 */ TweakValueImpl_804F4DC8 mUnidentified0D4;
    /* 0x0E4 */ TweakValueImpl_804F4DC8 mUnidentified0E4;
    /* 0x0F4 */ TweakValueImpl_804F4DC8 mUnidentified0F4;
    /* 0x104 */ TweakValueImpl_804F4DC8 mUnidentified104;
    /* 0x114 */ TweakValueImpl_804F4DC8 mUnidentified114;
    /* 0x124 */ TweakValueImpl_804F4DC8 mUnidentified124;
    /* 0x134 */ TweakValueImpl_804F4DC8 mUnidentified134;
    /* 0x144 */ TweakValueImpl_804F4DC8 mUnidentified144;
    /* 0x154 */ TweakValueImpl_804F4DC8 mUnidentified154;
    /* 0x164 */ TweakValueImpl_804F4DC8 mUnidentified164;
    /* 0x174 */ TweakValueImpl_804F4DC8 mUnidentified174;
    /* 0x184 */ TweakValueImpl_804F4DC8 mUnidentified184;
    /* 0x194 */ TweakValueImpl_804F4DC8 mUnidentified194;
    /* 0x1A4 */ TweakValueImpl_804F4DC8 mUnidentified1A4;
    /* 0x1B4 */ TweakValueImpl_804F4DC8 mUnidentified1B4;
    /* 0x1C4 */ TweakValueImpl_804F4DC8 mUnidentified1C4;
    /* 0x1D4 */ TweakValueImpl_804F4DC8 mUnidentified1D4;
    /* 0x1E4 */ TweakValueImpl_804F4DC8 mUnidentified1E4;
    /* 0x1F4 */ TweakValueImpl_804F4DC8 mUnidentified1F4;
    /* 0x204 */ TweakValueImpl_804F4DC8 mUnidentified204;
    /* 0x214 */ TweakValueImpl_804F4DC8 mUnidentified214;
    /* 0x224 */ TweakValueImpl_804F4DC8 mUnidentified224;
    /* 0x234 */ TweakValueImpl_804F4DC8 mUnidentified234;
    /* 0x244 */ TweakValueImpl_804F4DC8 mUnidentified244;
    /* 0x254 */ TweakValueImpl_804F4DC8 mUnidentified254;
    /* 0x264 */ TweakValueImpl_804F4DC8 mUnidentified264;
    /* 0x274 */ TweakValueImpl_804F4DC8 mUnidentified274;
    /* 0x284 */ TweakValueImpl_804F4DC8 mUnidentified284;
    /* 0x294 */ TweakValueImpl_804F4DC8 mUnidentified294;
    /* 0x2A4 */ TweakValueImpl_804F4DC8 mUnidentified2A4;
    /* 0x2B4 */ TweakValueImpl_804F4DC8 mUnidentified2B4;
    /* 0x2C4 */ TweakValueImpl_804F4DC8 mUnidentified2C4;
    /* 0x2D4 */ TweakValueImpl_804F4DC8 mUnidentified2D4;
    /* 0x2E4 */ TweakValueImpl_804F4DC8 mUnidentified2E4;
    /* 0x2F4 */ TweakValueImpl_804F4DC8 mUnidentified2F4;
    /* 0x304 */ TweakValueImpl_804F4DC8 mUnidentified304;
    /* 0x314 */ TweakValueImpl_804F4DC8 mUnidentified314;
    /* 0x324 */ TweakValueImpl_804F4DC8 mUnidentified324;
    /* 0x334 */ TweakValueImpl_804F4DC8 mUnidentified334;
    /* 0x344 */ TweakValueImpl_804F4DC8 mUnidentified344;
    /* 0x354 */ TweakValueImpl_804F4DC8 mUnidentified354;
    /* 0x364 */ TweakValueImpl_804F4DC8 mUnidentified364;
    /* 0x374 */ TweakValueImpl_804F4DC8 mUnidentified374;
    /* 0x384 */ TweakValueImpl_804F4DC8 mUnidentified384;
    /* 0x394 */ TweakValueImpl_804F4DC8 mUnidentified394;
    /* 0x3A4 */ TweakValueIntImpl_804FD898 nFacingFullConfidenceAngle;
    /* 0x3B4 */ TweakValueIntImpl_804FD898 nFacingNoConfidenceAngle;
    /* 0x3C4 */ TweakValueImpl_804F4DC8 mUnidentified3C4;
    /* 0x3D4 */ TweakValueImpl_804F4DC8 mUnidentified3D4;
    /* 0x3E4 */ TweakValueImpl_804F4DC8 mUnidentified3E4;
    /* 0x3F4 */ TweakValueImpl_804F4DC8 mUnidentified3F4;
    /* 0x404 */ TweakValueImpl_804F4DC8 mUnidentified404;
    /* 0x414 */ TweakValueImpl_804F4DC8 mUnidentified414;
    /* 0x424 */ TweakValueImpl_804F4DC8 mUnidentified424;
    /* 0x434 */ TweakValueImpl_804F4DC8 mUnidentified434;
    /* 0x444 */ TweakValueImpl_804F4DC8 mUnidentified444;
    /* 0x454 */ TweakValueImpl_804F4DC8 mUnidentified454;
    /* 0x464 */ TweakValueImpl_804F4DC8 mUnidentified464;
    /* 0x474 */ TweakValueImpl_804F4DC8 mUnidentified474;
    /* 0x484 */ TweakValueImpl_804F4DC8 mUnidentified484;
    /* 0x494 */ TweakValueImpl_804F4DC8 mUnidentified494;
    /* 0x4A4 */ TweakValueImpl_804F4DC8 mUnidentified4A4;
    /* 0x4B4 */ TweakValueImpl_804F4DC8 mUnidentified4B4;
    /* 0x4C4 */ TweakValueImpl_804F4DC8 mUnidentified4C4;
    /* 0x4D4 */ TweakValueImpl_804F4DC8 mUnidentified4D4;
    /* 0x4E4 */ TweakValueImpl_804F4DC8 mUnidentified4E4;
    /* 0x4F4 */ TweakValueImpl_804F4DC8 mUnidentified4F4;
    /* 0x504 */ TweakValueImpl_804F4DC8 mUnidentified504;
    /* 0x514 */ TweakValueImpl_804F4DC8 mUnidentified514;
    /* 0x524 */ TweakValueImpl_804F4DC8 mUnidentified524;
    /* 0x534 */ TweakValueImpl_804F4DC8 mUnidentified534;
    /* 0x544 */ TweakValueImpl_804F4DC8 mUnidentified544;
    /* 0x554 */ TweakValueImpl_804F4DC8 mUnidentified554;
    /* 0x564 */ TweakValueImpl_804F4DC8 mUnidentified564;
    /* 0x574 */ TweakValueImpl_804F4DC8 mUnidentified574;
    /* 0x584 */ TweakValueImpl_804F4DC8 mUnidentified584;
    /* 0x594 */ TweakValueImpl_804F4DC8 mUnidentified594;
    /* 0x5A4 */ TweakValueImpl_804F4DC8 mUnidentified5A4;
    /* 0x5B4 */ TweakValueImpl_804F4DC8 mUnidentified5B4;
    /* 0x5C4 */ TweakValueImpl_804F4DC8 mUnidentified5C4;
    /* 0x5D4 */ TweakValueImpl_804F4DC8 mUnidentified5D4;
    /* 0x5E4 */ TweakValueImpl_804F4DC8 mUnidentified5E4;
    /* 0x5F4 */ TweakValueImpl_804F4DC8 mUnidentified5F4;
    /* 0x604 */ TweakValueImpl_804F4DC8 mUnidentified604;
    /* 0x614 */ TweakValueImpl_804F4DC8 mUnidentified614;
    /* 0x624 */ TweakValueImpl_804F4DC8 mUnidentified624;
    /* 0x634 */ TweakValueImpl_804F4DC8 mUnidentified634;
    /* 0x644 */ TweakValueImpl_804F4DC8 mUnidentified644;
    /* 0x654 */ TweakValueImpl_804F4DC8 mUnidentified654;
    /* 0x664 */ TweakValueImpl_804F4DC8 mUnidentified664;
    /* 0x674 */ TweakValueImpl_804F4DC8 mUnidentified674;
    /* 0x684 */ TweakValueImpl_804F4DC8 mUnidentified684;
    /* 0x694 */ TweakValueImpl_804F4DC8 mUnidentified694;
    /* 0x6A4 */ TweakValueImpl_804F4DC8 mUnidentified6A4;
    /* 0x6B4 */ TweakValueImpl_804F4DC8 mUnidentified6B4;
    /* 0x6C4 */ TweakValueImpl_804F4DC8 mUnidentified6C4;
    /* 0x6D4 */ TweakValueImpl_804F4DC8 mUnidentified6D4;
    /* 0x6E4 */ TweakValueImpl_804F4DC8 mUnidentified6E4;
    /* 0x6F4 */ TweakValueImpl_804F4DC8 mUnidentified6F4;
    /* 0x704 */ TweakValueImpl_804F4DC8 mUnidentified704;
    /* 0x714 */ TweakValueImpl_804F4DC8 mUnidentified714;
    /* 0x724 */ TweakValueImpl_804F4DC8 mUnidentified724;
    /* 0x734 */ TweakValueImpl_804F4DC8 mUnidentified734;
    /* 0x744 */ TweakValueImpl_804F4DC8 mUnidentified744;
    /* 0x754 */ TweakValueImpl_804F4DC8 mUnidentified754;
    /* 0x764 */ TweakValueImpl_804F4DC8 mUnidentified764;
    /* 0x774 */ TweakValueImpl_804F4DC8 mUnidentified774;
    /* 0x784 */ TweakValueImpl_804F4DC8 mUnidentified784;
    /* 0x794 */ TweakValueImpl_804F4DC8 mUnidentified794;
    /* 0x7A4 */ TweakValueImpl_804F4DC8 mUnidentified7A4;

    /* 0x7B4 */ const char* mUnidentified7B4;
}; // total size: 0x7B8

#endif // GAME_SCRIPT_TUNING_H
