#ifndef UNCLASSIFIED_TU_801AE530_H
#define UNCLASSIFIED_TU_801AE530_H

#include "NL/nlMath.h"
#include "types.h"

struct glModel;

struct UnidentifiedNumberModel_801AE530
{
    u8 mUnidentified000[0x64];
    glModel* mModel;
    u8 mUnidentified068[0x08];
    u32 mUnidentified070;
    u8 mUnidentified074[0x04];
}; // total size: 0x78

class UnidentifiedNumberDisplay_801AE530
{
public:
    UnidentifiedNumberDisplay_801AE530();
    ~UnidentifiedNumberDisplay_801AE530();

    void fn_801AE71C();
    void fn_801AE728(float deltaTime);
    void fn_801AE960();
    void fn_801AEA90();
    void fn_801AEE80(
        int modelIndex, float scale, float opacity, const nlVector2& position);
    void fn_801AF250(int firstScore, int secondScore);
    void fn_801AF25C();
    void fn_801AF294();
    void fn_801AF2B4();
    void fn_801AF2E0();
    void fn_801AF2EC();
    void fn_801AF30C();

    /* 0x00 */ UnidentifiedNumberModel_801AE530** mModels;
    /* 0x04 */ bool mUnidentified004;
    /* 0x05 */ bool mUnidentified005;
    /* 0x06 */ u8 mUnidentified006[0x02];
    /* 0x08 */ int mUnidentified008;
    /* 0x0C */ int mUnidentified00C;
    /* 0x10 */ bool mUnidentified010;
    /* 0x11 */ u8 mUnidentified011[0x03];
    /* 0x14 */ float mUnidentified014;
    /* 0x18 */ float mUnidentified018;
    /* 0x1C */ float mUnidentified01C;
    /* 0x20 */ int mUnidentified020;
    /* 0x24 */ bool mUnidentified024;
    /* 0x25 */ bool mUnidentified025;
    /* 0x26 */ u8 mUnidentified026[0x02];
}; // total size: 0x28

extern UnidentifiedNumberDisplay_801AE530* lbl_806E1628;

#endif // UNCLASSIFIED_TU_801AE530_H
