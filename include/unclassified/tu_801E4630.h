#ifndef UNCLASSIFIED_TU_801E4630_H
#define UNCLASSIFIED_TU_801E4630_H

#include "Game/BaseSceneHandler.h"
#include "Game/EventConnection.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feBackButton.h"
#include "Game/FE/fePageControls.h"

class TLComponentInstance;
class TLImageInstance;
class TLInstance;
class FEScrollText;

class TU801E68F0Control
{
public:
    virtual bool fn_801E45C0();
    void Initialize(const char* filename);
    void fn_801E68F0(const char* filename);
    void fn_801E6BA4(float deltaTime);
    void Stop();

    /* 0x04 */ bool mUnidentified04;
    /* 0x05 */ bool mUnidentified05;
    /* 0x06 */ u8 mUnidentified06[2];
    /* 0x08 */ TLImageInstance* mUnidentified08;
    /* 0x0C */ char mUnidentified0C[128];
    /* 0x8C */ bool mUnidentified8C;
    /* 0x8D */ bool mUnidentified8D;
    /* 0x8E */ u8 mUnidentified8E[2];
    /* 0x90 */ int mUnidentified90;
}; // size 0x94

class TU801E4630Scene : public BaseSceneHandler
{
public:
    TU801E4630Scene();
    virtual ~TU801E4630Scene();
    virtual void Update(float deltaTime);
    virtual void SceneCreated();

    void fn_801E68DC();
    void fn_801E4F40();
    void fn_801E6170();
    void fn_801E6504(int index, void* context);
    void fn_801E6578(int index, void* context);
    void fn_801E65D4(int index, void* context);

    /* 0x01C */ TU801E68F0Control mUnidentified01C;
    /* 0x0B0 */ FEScrollText* mUnidentifiedB0;
    /* 0x0B4 */ bool mUnidentifiedB4;
    /* 0x0B5 */ bool mUnidentifiedB5;
    /* 0x0B6 */ bool mUnidentifiedB6;
    /* 0x0B7 */ u8 mUnidentifiedB7;
    /* 0x0B8 */ UnidentifiedEventConnectionOwner mUnidentifiedB8;
    /* 0x0BC */ int mUnidentifiedBC;
    /* 0x0C0 */ int mUnidentifiedC0;
    /* 0x0C4 */ int mUnidentifiedC4;
    /* 0x0C8 */ int mUnidentifiedC8[17];
    /* 0x10C */ u8 mUnidentified10C[4];
    /* 0x110 */ FEPointerButton mUnidentified110;
    /* 0x1C4 */ FEBackButton mUnidentified1C4;
    /* 0x29C */ FEPageControls mUnidentified29C;
    /* 0x424 */ unsigned short mUnidentified424[16];
    /* 0x444 */ u8 mUnidentified444[4];
    /* 0x448 */ TLComponentInstance* mUnidentified448;
    /* 0x44C */ TLComponentInstance* mUnidentified44C;
    /* 0x450 */ TLInstance* mUnidentified450;
}; // size 0x454

#endif // UNCLASSIFIED_TU_801E4630_H
