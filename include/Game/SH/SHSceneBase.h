#ifndef GAME_SH_SH_SCENE_BASE_H
#define GAME_SH_SH_SCENE_BASE_H

#include "Game/BaseSceneHandler.h"
#include "NL/nlBasicString.h"
#include "NL/nlFunction.h"
#include "unclassified/tu_80219248.h"

class TLComponentInstance;
class TLInstance;
class TLTextInstance;

// Text crossfade helper owned by the 0x801E2xxx translation unit.
struct UnidentifiedTextFader
{
    UnidentifiedTextFader(int value);

    /* 0x00 */ u8 mUnidentified00[0x4];
    /* 0x04 */ BasicString<char, Detail::TempStringAllocator> mText;
    /* 0x08 */ u8 mUnidentified08[0x1C];
    /* 0x24 */ float mUnidentified24;
    /* 0x28 */ float mUnidentified28;
    /* 0x2C */ float mUnidentified2C;
    /* 0x30 */ Function<FnVoidVoid> mUnidentified30;
    /* 0x38 */ u8 mUnidentified38[0x8];
}; // size 0x40

// StrikerTimes texture streamer owned by the 0x801BFxxx translation unit.
struct UnidentifiedStrikerTimesImage
{
    UnidentifiedStrikerTimesImage(const char* resource, int value);
    ~UnidentifiedStrikerTimesImage();

    /* 0x00 */ u8 mUnidentified00[0x8];
    /* 0x08 */ TLInstance* mUnidentified08;
    /* 0x0C */ u8 mUnidentified0C[0x94];
}; // size 0xA0

// Scroll widget owned by the 0x8022Fxxx/0x80230xxx translation units.
struct UnidentifiedScrollWidget
{
    UnidentifiedScrollWidget();
    ~UnidentifiedScrollWidget();

    /* 0x000 */ u8 mUnidentified00[0x1B4];
}; // size 0x1B4

class UnidentifiedSHSceneBase : public BaseSceneHandler
{
public:
    UnidentifiedSHSceneBase();
    virtual ~UnidentifiedSHSceneBase();
    virtual void Update(float dt);
    virtual void SceneCreated();
    virtual void SHSceneVirtual2C(unsigned int transition);
    virtual void SHSceneVirtual30();
    virtual void SHSceneVirtual34() { }
    virtual void SHSceneVirtual38(int captain, int mood, int special);

    void fn_8026932C();
    void fn_8026942C();
    void fn_80269524();
    void fn_8026A63C();
    void fn_8026ABF0(int index, void* context);
    void fn_8026AD50(int index, void* context);
    void fn_8026AE98(int index, void* context);

    /* 0x01C */ int mUnidentified1C;
    /* 0x020 */ int mUnidentified20;
    /* 0x024 */ u8 mUnidentified24;
    /* 0x028 */ int mUnidentified28;
    /* 0x02C */ int mUnidentified2C;
    /* 0x030 */ bool mUnidentified30;
    /* 0x031 */ char mUnidentified31[0x40];
    /* 0x071 */ char mUnidentified71[0x40];
    /* 0x0B1 */ char mUnidentifiedB1[0x43];
    /* 0x0F4 */ BasicString<char, Detail::TempStringAllocator> mUnidentifiedF4;
    /* 0x0F8 */ BasicString<unsigned short, Detail::TempStringAllocator> mUnidentifiedF8;
    /* 0x0FC */ bool mUnidentifiedFC;
    /* 0x100 */ int mUnidentified100;
    /* 0x104 */ bool mUnidentified104;
    /* 0x105 */ bool mUnidentified105;
    /* 0x106 */ bool mUnidentified106;
    /* 0x107 */ bool mUnidentified107;
    /* 0x108 */ bool mUnidentified108;
    /* 0x109 */ bool mUnidentified109;
    /* 0x10C */ TU80219248Component mComponent;
    /* 0x1C0 */ UnidentifiedTextFader mUnidentified1C0;
    /* 0x200 */ UnidentifiedTextFader mUnidentified200;
    /* 0x240 */ UnidentifiedStrikerTimesImage mUnidentified240;
    /* 0x2E0 */ UnidentifiedStrikerTimesImage mUnidentified2E0;
    /* 0x380 */ UnidentifiedStrikerTimesImage mUnidentified380;
    /* 0x420 */ UnidentifiedScrollWidget mUnidentified420;

private:
    inline TLInstance* FindCurrentInstance(const char* item);
    inline TLComponentInstance* FindCurrentComponent(const char* item);
}; // size 0x5D4

#endif // GAME_SH_SH_SCENE_BASE_H
