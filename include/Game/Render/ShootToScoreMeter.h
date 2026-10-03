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
    void TurnOffMeter();
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
    void SetYellowRegionWidth(float width);
    void SetSegment1Position(float position);
    void SetSegment1Width(float width);
    void SetSegment2Position(float position);
    void SetSegment2Width(float width);
    void SetSegment3Position(float position);
    void SetSegment3Width(float width);
    void SetSegment4Position(float position);
    void SetSegment4Width(float width);

    float GetWhiteBarAngle() const { return m_fWhiteBarAngle; }

    /* 0x00 */ nlVector3 m_v3MeterPosition;
    /* 0x0C */ nlVector3 m_v3OriginalMeterPosition;
    /* 0x18 */ float mfRumbleAmount;
    /* 0x1C */ bool m_bMeterVisible;
    /* 0x1D */ u8 mPad1D[3];
    /* 0x20 */ float m_fWhiteBarAngle;
    /* 0x24 */ float m_fWhiteBarPreviousAngle;
    /* 0x28 */ float m_fSavedWhiteBarAngle;
    /* 0x2C */ bool mbSecondButtonPressed;
    /* 0x2D */ bool mbShowSavedWhiteBar;
    /* 0x2E */ bool mbShowGreenRegion;
    /* 0x2F */ u8 mPad2F;
    /* 0x30 */ float m_fGreenBarAngle;
    /* 0x34 */ float m_fGreenRegionWidth;
    /* 0x38 */ float m_fYellowRegionWidth;
    /* 0x3C */ float m_fSegment1Angle;
    /* 0x40 */ float m_fSegment1Width;
    /* 0x44 */ float m_fSegment2Angle;
    /* 0x48 */ float m_fSegment2Width;
    /* 0x4C */ float m_fSegment3Angle;
    /* 0x50 */ float m_fSegment3Width;
    /* 0x54 */ float m_fSegment4Angle;
    /* 0x58 */ float m_fSegment4Width;
    /* 0x5C */ float m_fSegment5Angle;
    /* 0x60 */ float m_fSegment5Width;

    static ShootToScoreMeter instance;
    static float MeterWidth;
}; // size: 0x64

#endif // GAME_RENDER_SHOOT_TO_SCORE_METER_H
