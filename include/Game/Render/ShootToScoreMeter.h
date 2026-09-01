#ifndef GAME_RENDER_SHOOT_TO_SCORE_METER_H
#define GAME_RENDER_SHOOT_TO_SCORE_METER_H

#include "NL/nlColour.h"
#include "NL/nlMath.h"
#include "types.h"

class ShootToScoreMeter
{
public:
    ShootToScoreMeter();

    void TurnOnMeter();
    void fn_801AF97C();
    void RumbleMeter(u16 angle);
    void DrawMeter();
    void DrawIndicatorBar(float angle, const nlColour& colour,
        const nlMatrix4& meterMatrix, float scale);
    void DrawColouredRegion(float startAngle, float endAngle,
        const nlColour& startColour, const nlColour& endColour,
        nlMatrix4 meterMatrix, float scale);
    void UpdateAndRender(float fDeltaT);
    void SetWhiteBarPosition(float position);
    void SetSavedWhiteBarPosition(float position);
    void SetGreenBarPosition(float position);
    void SetGreenRegionWidth(float width);
    void fn_801B0FE4(float value);
    void fn_801B1004(float value);
    void fn_801B1024(float value);
    void fn_801B1044(float value);
    void fn_801B1064(float value);
    void fn_801B1084(float value);
    void fn_801B10A4(float value);
    void fn_801B10C4(float value);
    void fn_801B10E4(float value);

    /* 0x00 */ nlVector3 m_v3MeterPosition;
    /* 0x0C */ nlVector3 m_v3OriginalMeterPosition;
    /* 0x18 */ float mfRumbleAmount;
    /* 0x1C */ bool m_bMeterVisible;
    /* 0x1D */ u8 mPad1D[3];
    /* 0x20 */ float m_fWhiteBarAngle;
    /* 0x24 */ float m_fWhiteBarPreviousAngle;
    /* 0x28 */ float m_fSavedWhiteBarAngle;
    /* 0x2C */ bool mUnidentified2C;
    /* 0x2D */ bool mbShowSavedWhiteBar;
    /* 0x2E */ bool mUnidentified2E;
    /* 0x2F */ u8 mPad2F;
    /* 0x30 */ float m_fGreenBarAngle;
    /* 0x34 */ float m_fGreenRegionWidth;
    /* 0x38 */ float m_fYellowRegionWidth;
    /* 0x3C */ float mUnidentified3C;
    /* 0x40 */ float mUnidentified40;
    /* 0x44 */ float mUnidentified44;
    /* 0x48 */ float mUnidentified48;
    /* 0x4C */ float mUnidentified4C;
    /* 0x50 */ float mUnidentified50;
    /* 0x54 */ float mUnidentified54;
    /* 0x58 */ float mUnidentified58;
    /* 0x5C */ float mUnidentified5C;
    /* 0x60 */ float mUnidentified60;

    static ShootToScoreMeter instance;
    static float MeterWidth;
}; // size: 0x64

#endif // GAME_RENDER_SHOOT_TO_SCORE_METER_H
